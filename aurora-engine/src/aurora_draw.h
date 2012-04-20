#ifndef AURORA_DRAW_H
#define AURORA_DRAW_H

#include "aurora_types.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <cairo.h>

G_GNUC_INTERNAL void aurora_draw_button           (cairo_t *cr,
                                       const AuroraColors      *colors,
                                       const WidgetParameters      *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_scale_trough     (cairo_t *cr,
                                       const AuroraColors      *colors,
                                       const WidgetParameters      *widget,
                                       const SliderParameters      *slider,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_progressbar_trough (cairo_t *cr,
                                       const AuroraColors      *colors,
                                       const WidgetParameters      *widget,
                                  		const ProgressBarParameters *progressbar,
                                       int x, int y, int width, int height);


G_GNUC_INTERNAL void aurora_draw_progressbar_fill (cairo_t *cr,
                                       const AuroraColors      *colors,
                                       const WidgetParameters      *widget,
                                       const ProgressBarParameters *progressbar,
                                       int x, int y, int width, int height, gint offset);

G_GNUC_INTERNAL void aurora_draw_slider_button    (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SliderParameters     *slider,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_entry            (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const EntryParameters      *entry,
                                       int x, int y, int width, int height);
                                       
G_GNUC_INTERNAL void aurora_draw_scrolled (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height);
						   
G_GNUC_INTERNAL void aurora_draw_menubar          (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height,
                                       int menubarstyle);

G_GNUC_INTERNAL void aurora_draw_tab_no_border              (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const TabParameters        *tab,
                                       int x, int y, int width, int height);


G_GNUC_INTERNAL void aurora_draw_tab              (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const TabParameters        *tab,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_frame            (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const FrameParameters      *frame,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_separator        (cairo_t *cr,
                                       const AuroraColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SeparatorParameters  *separator,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_list_view_header (cairo_t *cr,
                                       const AuroraColors         *colors,
                                       const WidgetParameters         *widget,
                                       const ListViewHeaderParameters *header,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_toolbar          (cairo_t *cr,
                                       const AuroraColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_menuitem         (cairo_t *cr,
                                       const AuroraColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_menubar_item     (cairo_t *cr,
                                       const AuroraColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_scrollbar_slider (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_scrollbar_trough (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_menu_frame       (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_handle           (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       const HandleParameters          *handle,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_arrow            (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ArrowParameters           *arrow,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_resize_grip      (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ResizeGripParameters      *grip,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void ubuntulooks_draw_tooltip      (cairo_t *cr,
                                       const AuroraColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_checkbutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_radiobutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_menu_checkbutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_cell_checkbutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);


G_GNUC_INTERNAL void aurora_draw_menu_radiobutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_cell_radiobutton 		(cairo_t *cr,
                             				const AuroraColors         *colors,
                             				const WidgetParameters      *widget,
                             				const OptionParameters      *status,
                             				int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_list_selection (cairo_t * cr,
                                    const AuroraColors * colors,
                                    const WidgetParameters * widget,
                                    int x, int y, int width, int height);

G_GNUC_INTERNAL void aurora_draw_tooltip (cairo_t * cr,
                                    const AuroraColors * colors,
                                    const WidgetParameters * widget,
                                    int x, int y, int width, int height);



#endif /* AURORA_DRAW_H */
