/* Aurora theme engine
 * Copyright (C) 2007 Eric Matthews.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Written by Owen Taylor <otaylor@redhat.com>
 * and by Alexander Larsson <alexl@redhat.com>
 * Modified by Richard Stellingwerff <remenic@gmail.com>
 * Modified by Kulyk Nazar <schamane@myeburg.net>
 * Modified by Andrea Cimitan <andrea.cimitan@gmail.com>
 * Modified by Eric Matthews <echm2007@gmail.com>
 */
#include <string.h>
#include "aurora_style.h"
#include "aurora_rc_style.h"

#include "animation.h"

static void      aurora_rc_style_init         (AuroraRcStyle      *style);
#ifdef HAVE_ANIMATION
static void      aurora_rc_style_finalize     (GObject                *object);
#endif
static void      aurora_rc_style_class_init   (AuroraRcStyleClass *klass);
static GtkStyle *aurora_rc_style_create_style (GtkRcStyle             *rc_style);
static guint     aurora_rc_style_parse        (GtkRcStyle             *rc_style,
						   GtkSettings            *settings,
						   GScanner               *scanner);
static void      aurora_rc_style_merge        (GtkRcStyle             *dest,
						   GtkRcStyle             *src);


static GtkRcStyleClass *parent_class;

GType aurora_type_rc_style = 0;

enum
{
  TOKEN_CONTRAST = G_TOKEN_LAST + 1,
  TOKEN_MENUBARSTYLE,
  TOKEN_CURVATURE,
  TOKEN_ANIMATION,
  TOKEN_ARROWSIZE,
  TOKEN_OLD_ARROWSTYLE,
		
  TOKEN_TRUE,
  TOKEN_FALSE,
		
  TOKEN_LAST
};

static gchar* aurora_rc_symbols =
	"contrast\0"
	"menubarstyle\0"
	"curvature\0"
	"animation\0"
	"arrowsize\0"
	"old_arrowstyle\0"

	"TRUE\0"
	"FALSE\0";


void
aurora_rc_style_register_type (GTypeModule *module)
{
  static const GTypeInfo object_info =
  {
    sizeof (AuroraRcStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) aurora_rc_style_class_init,
    NULL,           /* class_finalize */
    NULL,           /* class_data */
    sizeof (AuroraRcStyle),
    0,              /* n_preallocs */
    (GInstanceInitFunc) aurora_rc_style_init,
    NULL
  };
  
  aurora_type_rc_style = g_type_module_register_type (module,
						     GTK_TYPE_RC_STYLE,
						     "AuroraRcStyle",
						     &object_info, 0);
}

static void
aurora_rc_style_init (AuroraRcStyle *aurora_rc)
{

	aurora_rc->flags = 0;
  aurora_rc->contrast = 1.0;
  aurora_rc->menubarstyle = 1;
  aurora_rc->curvature = 5.0;
  aurora_rc->arrowsize = 1;
	aurora_rc->old_arrowstyle = TRUE;
  aurora_rc->animation = FALSE;
}

#ifdef HAVE_ANIMATION
static void
aurora_rc_style_finalize (GObject *object)
{
	/* cleanup all the animation stuff */
	aurora_animation_cleanup ();
	
	if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
		G_OBJECT_CLASS (parent_class)->finalize(object);
}
#endif


static void
aurora_rc_style_class_init (AuroraRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
#ifdef HAVE_ANIMATION
  GObjectClass    *g_object_class = G_OBJECT_CLASS (klass);
#endif

  parent_class = g_type_class_peek_parent (klass);

  rc_style_class->parse = aurora_rc_style_parse;
  rc_style_class->create_style = aurora_rc_style_create_style;
  rc_style_class->merge = aurora_rc_style_merge;

#ifdef HAVE_ANIMATION
  g_object_class->finalize = aurora_rc_style_finalize;
#endif
}

static guint
theme_parse_boolean (GtkSettings *settings, GScanner     *scanner,
                     gboolean *retval)
{
  guint token;
  token = g_scanner_get_next_token(scanner);
  
  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;
  
  token = g_scanner_get_next_token(scanner);
  if (token == TOKEN_TRUE)
    *retval = TRUE;
  else if (token == TOKEN_FALSE)
    *retval = FALSE;
  else
    return TOKEN_TRUE;

  return G_TOKEN_NONE;
}


static guint
theme_parse_double (GtkSettings *settings, GScanner     *scanner,
                    gdouble      *val)
{
  guint token;

  /* Skip '' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

	
  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_FLOAT) {
	if (token != G_TOKEN_INT) 
		return G_TOKEN_FLOAT;
	else
		  *val = scanner->value.v_int;
	return G_TOKEN_NONE;
  }
	
  *val = scanner->value.v_float;
  
  return G_TOKEN_NONE;
	
}

static guint
theme_parse_int (GtkSettings *settings, GScanner     *scanner,
                 guint8       *progressbarstyle)
{
	guint token;

	/* Skip option name */
	token = g_scanner_get_next_token(scanner);

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_EQUAL_SIGN)
	   return G_TOKEN_EQUAL_SIGN;

	token = g_scanner_get_next_token(scanner);
	if (token != G_TOKEN_INT)
	   return G_TOKEN_INT;

	*progressbarstyle = scanner->value.v_int;

	return G_TOKEN_NONE;
}

static guint
aurora_rc_style_parse (GtkRcStyle *rc_style,
			   GtkSettings  *settings,
			   GScanner   *scanner)
		     
{
  static GQuark scope_id = 0;
  AuroraRcStyle *aurora_style = AURORA_RC_STYLE (rc_style);

  guint old_scope;
  guint token;
  
  /* Set up a new scope in this scanner. */

  if (!scope_id)
    scope_id = g_quark_from_string("aurora_theme_engine");

  /* If we bail out due to errors, we *don't* reset the scope, so the
   * error messaging code can make sense of our tokens.
   */
  old_scope = g_scanner_set_scope(scanner, scope_id);

  /* Now check if we already added our symbols to this scope
   * (in some previous call to aurora_rc_style_parse for the
   * same scanner.
   */
		
	if (!g_scanner_lookup_symbol(scanner, aurora_rc_symbols)) {
		gchar *current_symbol = aurora_rc_symbols;
		gint i = G_TOKEN_LAST + 1;

		/* Add our symbols */
		while ((current_symbol[0] != '\0') && (i < TOKEN_LAST)) {
			g_scanner_scope_add_symbol(scanner, scope_id, current_symbol, GINT_TO_POINTER (i));

			current_symbol += strlen(current_symbol) + 1;
			i++;
		}
		g_assert (i == TOKEN_LAST && current_symbol[0] == '\0');
	}
	
	
  /* We're ready to go, now parse the top level */

  token = g_scanner_peek_next_token(scanner);
  while (token != G_TOKEN_RIGHT_CURLY)
    {
      switch (token)
	{
	case TOKEN_CONTRAST:
	  token = theme_parse_double (settings, scanner, &aurora_style->contrast);
	  aurora_style->flags |= AUR_FLAG_CONTRAST;
	  break;
	case TOKEN_MENUBARSTYLE:
	  token = theme_parse_int (settings, scanner, &aurora_style->menubarstyle);
	  aurora_style->flags |= AUR_FLAG_MENUBARSTYLE;
	  break;
	case TOKEN_CURVATURE:
	  token = theme_parse_double (settings, scanner, &aurora_style->curvature);
	  aurora_style->flags |= AUR_FLAG_CURVATURE;
	  break;
	case TOKEN_ANIMATION:
	  token = theme_parse_boolean (settings, scanner, &aurora_style->animation);
	  aurora_style->flags |= AUR_FLAG_ANIMATION;
	  break;
	case TOKEN_OLD_ARROWSTYLE:
	  token = theme_parse_boolean (settings, scanner, &aurora_style->old_arrowstyle);
	  aurora_style->flags |= AUR_FLAG_OLD_ARROWSTYLE;
	  break;
	case TOKEN_ARROWSIZE:
	  token = theme_parse_double (settings, scanner, &aurora_style->arrowsize);
	  aurora_style->flags |= AUR_FLAG_ARROWSTYLE;
	  break;
	default:
	  g_scanner_get_next_token(scanner);
	  token = G_TOKEN_RIGHT_CURLY;
	  break;
	}
		
      if (token != G_TOKEN_NONE)
	return token;

      token = g_scanner_peek_next_token(scanner);
    }

  g_scanner_get_next_token(scanner);

  g_scanner_set_scope(scanner, old_scope);

  return G_TOKEN_NONE;
}

static void
aurora_rc_style_merge (GtkRcStyle *dest,
			   GtkRcStyle *src)
{
	AuroraRcStyle *dest_w, *src_w;
	AuroraRcFlags flags;	
	
	parent_class->merge (dest, src);
	
	if (!AURORA_IS_RC_STYLE (src))
		return;
	
	src_w = AURORA_RC_STYLE (src);
	dest_w = AURORA_RC_STYLE (dest);
	
	flags = (~dest_w->flags) & src_w->flags;
	
	if (flags & AUR_FLAG_CONTRAST)
		dest_w->contrast = src_w->contrast;
	if (flags & AUR_FLAG_MENUBARSTYLE)
		dest_w->menubarstyle = src_w->menubarstyle;
	if (flags & AUR_FLAG_CURVATURE)
		dest_w->curvature = src_w->curvature;
	if (flags & AUR_FLAG_ARROWSTYLE)
		dest_w->arrowsize = src_w->arrowsize;
	if (flags & AUR_FLAG_OLD_ARROWSTYLE)
		dest_w->old_arrowstyle = src_w->old_arrowstyle;
	if (flags & AUR_FLAG_ANIMATION)
		dest_w->animation = src_w->animation;

	dest_w->flags |= src_w->flags;
	
}


/* Create an empty style suitable to this RC style
 */
static GtkStyle *
aurora_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (AURORA_TYPE_STYLE, NULL));
}
