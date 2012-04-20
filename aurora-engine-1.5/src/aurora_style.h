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
 * Modified by Andrea Cimitan <andrea.cimitan@gmail.com>
 * Modified by Eric Matthews <echm2007@gmail.com>
 */
#include <gtk/gtkstyle.h>

#ifndef AURORA_STYLE_H
#define AURORA_STYLE_H

#include "animation.h"
#include "aurora_types.h"

typedef struct _AuroraStyle AuroraStyle;
typedef struct _AuroraStyleClass AuroraStyleClass;

G_GNUC_INTERNAL extern GType aurora_type_style;

#define AURORA_TYPE_STYLE              aurora_type_style
#define AURORA_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), AURORA_TYPE_STYLE, AuroraStyle))
#define AURORA_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), AURORA_TYPE_STYLE, AuroraStyleClass))
#define AURORA_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), AURORA_TYPE_STYLE))
#define AURORA_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), AURORA_TYPE_STYLE))
#define AURORA_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), AURORA_TYPE_STYLE, AuroraStyleClass))

struct _AuroraStyle
{
	GtkStyle parent_instance;

	AuroraColors colors;
	
	gdouble   curvature;
	guint8   menubarstyle;
	guint8   arrowsize;
	gboolean old_arrowstyle;
	gboolean animation;

};

struct _AuroraStyleClass
{
  GtkStyleClass parent_class;
};


G_GNUC_INTERNAL void aurora_style_register_type (GTypeModule *module);

#endif /* CLEARLOOKS_STYLE_H */







