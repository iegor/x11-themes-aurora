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

#include <gtk/gtkrc.h>

typedef struct _AuroraRcStyle AuroraRcStyle;
typedef struct _AuroraRcStyleClass AuroraRcStyleClass;

G_GNUC_INTERNAL extern GType aurora_type_rc_style;

#define AURORA_TYPE_RC_STYLE              aurora_type_rc_style
#define AURORA_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), AURORA_TYPE_RC_STYLE, AuroraRcStyle))
#define AURORA_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), AURORA_TYPE_RC_STYLE, AuroraRcStyleClass))
#define AURORA_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), AURORA_TYPE_RC_STYLE))
#define AURORA_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), AURORA_TYPE_RC_STYLE))
#define AURORA_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), AURORA_TYPE_RC_STYLE, AuroraRcStyleClass))

/* XXX: needs fixing! */
typedef enum {
	AUR_FLAG_CONTRAST           = 1 <<  0,
	AUR_FLAG_MENUBARSTYLE       = 1 <<  1,
	AUR_FLAG_CURVATURE          = 1 <<  2,
	AUR_FLAG_ARROWSTYLE         = 1 <<  3,
	AUR_FLAG_OLD_ARROWSTYLE     = 1 <<  4,	
	AUR_FLAG_ANIMATION          = 1 <<  5

} AuroraRcFlags;


struct _AuroraRcStyle
{
  GtkRcStyle parent_instance;

  AuroraRcFlags flags;

  double contrast;
  guint8 menubarstyle;
  gdouble curvature;
  gdouble arrowsize;
  gboolean  old_arrowstyle;
  gboolean animation;
};

struct _AuroraRcStyleClass
{
  GtkRcStyleClass parent_class;
};

G_GNUC_INTERNAL void aurora_rc_style_register_type (GTypeModule *module);
