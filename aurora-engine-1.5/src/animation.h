/* Aurora Engine
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
 */

#include "config.h"

#ifdef HAVE_ANIMATION
#include <gtk/gtk.h>

#define AUR_IS_PROGRESS_BAR(widget) GTK_IS_PROGRESS_BAR(widget) && widget->allocation.x != -1 && widget->allocation.y != -1
#define ANIMATION_DELAY 25
#define PROGRESSBAR_DELAY 4
#define CHECK_ANIMATION_TIME 0.5
#define ANIMATION_TRANS_TIME 0.05

G_GNUC_INTERNAL void     aurora_animation_progressbar_add (GtkWidget *progressbar);
G_GNUC_INTERNAL void     aurora_animation_connect_checkbox (GtkWidget *widget);
G_GNUC_INTERNAL void     aurora_animation_connect_button (GtkWidget *widget);
G_GNUC_INTERNAL void     aurora_animation_connect_widget (GtkWidget *widget);
G_GNUC_INTERNAL gboolean aurora_animation_is_animated (GtkWidget *widget);
G_GNUC_INTERNAL gdouble  aurora_animation_elapsed (gpointer data);
G_GNUC_INTERNAL GtkStateType  aurora_animation_starting_state (gpointer data);
G_GNUC_INTERNAL void     aurora_animation_cleanup ();
#endif /* HAVE_ANIMATION */
