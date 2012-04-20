#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "aurora_style.h"
#include "aurora_rc_style.h"
#include "aurora_draw.h"
#include "support.h"

static AuroraStyleClass *aurora_style_class;
static GtkStyleClass *aurora_parent_class;


#define DETAIL(xx)   ((detail) && (!strcmp(xx, detail)))

#define DRAW_ARGS    GtkStyle       *style, \
                     GdkWindow      *window, \
                     GtkStateType    state_type, \
                     GtkShadowType   shadow_type, \
                     GdkRectangle   *area, \
                     GtkWidget      *widget, \
                     const gchar    *detail, \
                     gint            x, \
                     gint            y, \
                     gint            width, \
                     gint            height

#define CHECK_ARGS					\
  g_return_if_fail (window != NULL);			\
  g_return_if_fail (style != NULL);

#define SANITIZE_SIZE					\
  g_return_if_fail (width  >= -1);			\
  g_return_if_fail (height >= -1);			\
                                                        \
  if ((width == -1) && (height == -1))			\
    gdk_drawable_get_size (window, &width, &height);	\
  else if (width == -1)					\
    gdk_drawable_get_size (window, &width, NULL);	\
  else if (height == -1)				\
    gdk_drawable_get_size (window, NULL, &height);


#ifdef HAVE_ANIMATION
#include "animation.h"
#endif

static const int FRAME_SHADE = 4;

static void
clearlooks_rounded_rectangle (cairo_t * cr,
			      double x, double y, double w, double h,
			      double radius, uint8 corners)
{
  if (radius < 0.01) {
    cairo_rectangle (cr, x, y, w, h);
    return;
  }

  if (corners & AUR_CORNER_TOPLEFT)
    cairo_move_to (cr, x + radius, y);
  else
    cairo_move_to (cr, x, y);

  if (corners & AUR_CORNER_TOPRIGHT)
    cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
  else
    cairo_line_to (cr, x + w, y);

  if (corners & AUR_CORNER_BOTTOMRIGHT)
    cairo_arc (cr, x + w - radius, y + h - radius, radius, 0, M_PI * 0.5);
  else
    cairo_line_to (cr, x + w, y + h);

  if (corners & AUR_CORNER_BOTTOMLEFT)
    cairo_arc (cr, x + radius, y + h - radius, radius, M_PI * 0.5, M_PI);
  else
    cairo_line_to (cr, x, y + h);

  if (corners & AUR_CORNER_TOPLEFT)
    cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
  else
    cairo_line_to (cr, x, y);
}

static cairo_t *
aurora_begin_paint (GdkDrawable * window, GdkRectangle * area)
{
  cairo_t *cr;

  g_return_val_if_fail (window != NULL, NULL);

  cr = (cairo_t *) gdk_cairo_create (window);
  cairo_set_line_width (cr, 1.0);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);

  if (area) {
    cairo_rectangle (cr, area->x, area->y, area->width, area->height);
    cairo_clip (cr);
  }

  return cr;
}


static void
aurora_set_widget_parameters (const GtkWidget * widget,
			      const GtkStyle * style,
			      GtkStateType state_type,
			      WidgetParameters * params)
{
	
  params->active     = (state_type == GTK_STATE_ACTIVE);
  params->prelight   = (state_type == GTK_STATE_PRELIGHT);
  params->disabled   = (state_type == GTK_STATE_INSENSITIVE);
  params->state_type = (AuroraStateType) state_type;
  params->corners    = AUR_CORNER_ALL;
  params->curvature  = AURORA_STYLE (style)->curvature;
  params->focus      = widget && GTK_WIDGET_HAS_FOCUS (widget);
  params->is_default = widget && GTK_WIDGET_HAS_DEFAULT (widget);
  params->trans      = 1.0;
  params->prev_state_type = state_type;
  params->ltr        = aurora_widget_is_ltr ((GtkWidget*)widget);
  //params->composited = gtk_widget_is_composited ((GtkWidget*)widget);
	
  params->xthickness = style->xthickness;
  params->ythickness = style->ythickness;


}

static void
aurora_style_draw_flat_box (DRAW_ARGS)
{
	if (DETAIL ("expander") || DETAIL ("checkbutton")) {
		WidgetParameters params;
		AuroraStyle *aurora_style;
		AuroraColors *colors;
		cairo_t *cr;
		AuroraRGB focus_color;
		
		CHECK_ARGS
		SANITIZE_SIZE
		aurora_style = AURORA_STYLE (style);

		aurora_set_widget_parameters (widget, style, state_type, &params);
		colors = &aurora_style->colors;
		cr = aurora_begin_paint (window, area);

		cairo_translate (cr, x, y);
		
		focus_color = colors->spot[1];
	    aurora_scale_saturation(&focus_color, 0.75);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.25);
		clearlooks_rounded_rectangle (cr, 0, 0, width, height, params.curvature + 1, AUR_CORNER_ALL);
		cairo_fill (cr);
	  
	  	cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.20);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, params.curvature, AUR_CORNER_ALL);
		cairo_stroke (cr);

		cairo_destroy (cr);
	}
	else if (DETAIL ("tooltip")) {
		WidgetParameters params;
		AuroraStyle *aurora_style;
		AuroraColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
					
		aurora_style = AURORA_STYLE (style);
		aurora_set_widget_parameters (widget, style, state_type, &params);
		colors = &aurora_style->colors;
		cr = aurora_begin_paint (window, area);

		aurora_draw_tooltip (cr, colors, &params, x, y, width, height);
		cairo_destroy (cr);
	}
	//else if (DETAIL ("entry_bg")) { }
	//else if (DETAIL ("text")) { }
	else if (detail && state_type == GTK_STATE_SELECTED && (!strncmp ("cell_even", detail, 9) || !strncmp ("cell_odd", detail, 8))) {
		WidgetParameters params;
		AuroraStyle *aurora_style;
		AuroraColors *colors;
		cairo_t *cr;

		CHECK_ARGS
		SANITIZE_SIZE
		aurora_style = AURORA_STYLE (style);

		aurora_set_widget_parameters (widget, style, state_type, &params);
		colors = &aurora_style->colors;
		cr = aurora_begin_paint (window, area);

		aurora_draw_list_selection (cr, colors, &params, x, y, width, height);
		cairo_destroy (cr);
	}
	else {
		aurora_parent_class->draw_flat_box (style, window, state_type,
				shadow_type,
				area, widget, detail,
				x, y, width, height);
	}
}

static void
aurora_style_draw_shadow (DRAW_ARGS)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);
		
	
	if ((DETAIL ("entry") && !(widget && AURORA_IS_TREE_VIEW (widget->parent))) ||
	    (shadow_type != GTK_SHADOW_NONE && (DETAIL ("frame") && aurora_is_in_combo_box (widget))))
	{
		WidgetParameters params;
		EntryParameters entry;
		entry.isCombo = FALSE;

		aurora_set_widget_parameters (widget, style, AURORA_IS_ENTRY (widget) ? GTK_WIDGET_STATE (widget) :state_type, &params);	
					
		if (widget && (AURORA_IS_COMBO (widget->parent) ||
		               AURORA_IS_COMBO_BOX_ENTRY (widget->parent) ||
		               AURORA_IS_SPIN_BUTTON (widget))) {
			//As the enter key for a GtkCombo makes the list appear, have the focus drawn for the button to.
			if (AURORA_IS_COMBO (widget->parent))
				gtk_widget_queue_draw (((GtkCombo*)widget->parent)->button);

			width += style->xthickness;
			if (!params.ltr)
				x -= style->xthickness;
			
			if (params.ltr)
				params.corners = AUR_CORNER_TOPLEFT | AUR_CORNER_BOTTOMLEFT;
			else
				params.corners = AUR_CORNER_TOPRIGHT | AUR_CORNER_BOTTOMRIGHT;

			entry.isCombo = TRUE;
		}

		/* Fill the background as it is initilized to base[NORMAL].
		* Relevant GTK+ bug: http://bugzilla.gnome.org/show_bug.cgi?id=513471
		* The fill only happens if no hint has been added by some application
		* that is faking GTK+ widgets. */
		if (widget && !g_object_get_data(G_OBJECT (widget), "transparent-bg-hint"))
		{
			AuroraRGB parentbg;
			aurora_get_parent_bg (widget, &parentbg);
			cairo_rectangle (cr, x, y, width, height);
			cairo_set_source_rgb (cr, parentbg.r, parentbg.g, parentbg.b);
			cairo_fill (cr);
		} 
		
		
		aurora_draw_entry (cr, colors, &params, &entry, x, y, width, height);
	}
	else if ((DETAIL ("scrolled_window") || DETAIL ("viewport")) && shadow_type != GTK_SHADOW_NONE) {
		if (widget && !g_str_equal ("XfcePanelWindow", gtk_widget_get_name (gtk_widget_get_toplevel (widget)))) {
			WidgetParameters params;
			aurora_set_widget_parameters (widget, style, state_type, &params);	
			params.curvature = 0;
	
			aurora_draw_scrolled (cr, colors, &params, x-1, y-1, width+2, height+2);
		}
	}
	else if (DETAIL ("frame") && widget && AURORA_IS_STATUSBAR (widget->parent) && shadow_type != GTK_SHADOW_NONE)
	{
		WidgetParameters params;
		SeparatorParameters separator;
		AuroraRGB dark;
		cairo_pattern_t *pattern;
		
		aurora_set_widget_parameters (widget, style, state_type, &params);
		separator.horizontal = TRUE;
			
		cairo_save(cr);
		aurora_mix_color (&colors->shade[8], &colors->bg[GTK_STATE_NORMAL], 0.9, &dark);
			
		cairo_translate (cr, x, y);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, dark.r, dark.g, dark.b);
		cairo_set_source (cr, pattern);
			
		cairo_rectangle (cr, 0, 0, width, height);
		cairo_fill (cr);
		cairo_pattern_destroy (pattern);
			
		cairo_restore(cr);
		aurora_draw_separator (cr, colors, &params, &separator, x, y, width, 2);
	}
	else if (widget && widget->parent && AURORA_IS_COLOR_BUTTON (widget->parent->parent)) {
		AuroraRGB border;
		GdkColor button_color;

		gtk_color_button_get_color ((GtkColorButton *) widget->parent->parent, &button_color);

		border.r = colors->shade[8].r * (0.85) +(button_color.red / (double) 65535) * 0.15;
		border.g = colors->shade[8].g * (0.85) +(button_color.green / (double) 65535) * 0.15;
		border.b = colors->shade[8].b * (0.85) +(button_color.blue / (double) 65535) * 0.15;

		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		clearlooks_rounded_rectangle (cr, x + 1.5, y + 1.5, width - 3, height - 3, 2, AUR_CORNER_ALL);
		cairo_stroke (cr);

		aurora_shade (&border, &border, 0.875);
		cairo_set_source_rgba (cr, border.r, border.g, border.b, 0.2);
		clearlooks_rounded_rectangle (cr, x + 2.5, y + 2.5, width - 3, height - 3, 3, AUR_CORNER_ALL);
		cairo_stroke (cr);
	}
	else if (DETAIL ("pager") || DETAIL ("pager-frame")) {
		AuroraRGB border;
		aurora_mix_color (&colors->shade[7], &colors->bg[GTK_STATE_SELECTED], 0.15, &border);
		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		clearlooks_rounded_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1, 2, AUR_CORNER_ALL);
		cairo_stroke (cr);
	}
	else if (shadow_type != GTK_SHADOW_NONE) {
		cairo_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1);
		cairo_set_source_rgb (cr, colors->shade[FRAME_SHADE].r, colors->shade[FRAME_SHADE].g, colors->shade[FRAME_SHADE].b);
		cairo_stroke (cr);
	}
	cairo_destroy (cr);
}

static void
aurora_style_draw_box_gap (DRAW_ARGS,
			   GtkPositionType gap_side,
			   gint gap_x, gint gap_width)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	if (DETAIL ("notebook")) {
		WidgetParameters params;
		FrameParameters frame;
		boolean start = FALSE;
		boolean	end = FALSE;
		
		//Don't round top corner when the border tab is selected.
		int current_page = gtk_notebook_get_current_page ((GtkNotebook *) widget);
		int num_pages = gtk_notebook_get_n_pages ((GtkNotebook *) widget);
				
		frame.shadow = shadow_type;
		frame.gap_side = gap_side;
		frame.gap_x = gap_x;
		frame.gap_width = gap_width;
		frame.border = &colors->shade[5];
		frame.use_fill = TRUE;
		frame.fill_bg = TRUE;
		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN(params.curvature, MIN(params.xthickness, params.ythickness) + 1.5);
    


		frame.gap_width -= (current_page == num_pages - 1) ? 2 : 0; 
		if (current_page == 0)
			start = TRUE;
		
		if(current_page == num_pages - 1)
			end = TRUE;
		
		params.corners = AUR_CORNER_ALL;
		switch (gap_side)
		{
			case GTK_POS_TOP:
				if (params.ltr)
					params.corners ^= AUR_CORNER_TOPLEFT;
				else
					params.corners ^= AUR_CORNER_TOPRIGHT;
			break;
			case GTK_POS_BOTTOM:
				if (params.ltr)
					params.corners ^= AUR_CORNER_BOTTOMLEFT;
				else
					params.corners ^= AUR_CORNER_BOTTOMRIGHT;
			break;
			case GTK_POS_LEFT:
					params.corners ^= AUR_CORNER_TOPLEFT;
			break;
			case GTK_POS_RIGHT:
					params.corners ^= AUR_CORNER_TOPRIGHT;
			break;
		}

		if (gtk_notebook_get_show_border ((GtkNotebook*)widget)) {
			aurora_draw_frame (cr, colors, &params, &frame, x, y, width, height);
		}
		else {
			//Shaded background
			/*AuroraRGB dark;
			aurora_mix_color (&colors->shade[8], &colors->bg[GTK_STATE_NORMAL], 0.2, &dark);
			double y2 = widget->allocation.y + GTK_CONTAINER (widget)->border_width;

			cairo_pattern_t *pattern;
			pattern = cairo_pattern_create_linear (0, y2, 0, y);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, dark.r, dark.g, dark.b, 0.90);
			cairo_pattern_add_color_stop_rgba (pattern, 0.90, dark.r, dark.g, dark.b, 0.10);
			cairo_set_source (cr, pattern);
		
			cairo_rectangle (cr, x, y2, width, y-y2);
			cairo_fill(cr);
			cairo_pattern_destroy (pattern);
			cairo_move_to(cr, x-0.5, y2+0.5);
			cairo_line_to(cr, x+width+1, y2+0.5);
			cairo_set_source_rgb (cr,  dark.r, dark.g, dark.b);
			cairo_stroke(cr);*/
				
			cairo_rectangle (cr, x, y-1, width, 4);
			cairo_set_source_rgb (cr, colors->bg[0].r, colors->bg[0].g, colors->bg[0].b);
			cairo_fill (cr);
			
			cairo_move_to(cr, x-0.5, y-0.5);
			cairo_line_to(cr, x+width+1, y-0.5);
			cairo_set_source_rgb (cr,  colors->shade[FRAME_SHADE].r, colors->shade[FRAME_SHADE].g, colors->shade[FRAME_SHADE].b);
			cairo_stroke(cr);
			
			cairo_move_to(cr, x-0.5, y+3.5);
			cairo_line_to(cr, x+width+1, y+3.5);
			cairo_set_source_rgb (cr,  colors->shade[FRAME_SHADE+1].r, colors->shade[FRAME_SHADE+1].g, colors->shade[FRAME_SHADE+1].b);
			cairo_stroke(cr);
		}
		/*else {
			aurora_parent_class->draw_box_gap (style, window, state_type, shadow_type,
				       area, widget, detail,
				       x, y, width, height,
				       gap_side, gap_x, gap_width);
		}*/
	}
	cairo_destroy (cr);
}

static void
aurora_style_draw_extension (DRAW_ARGS, GtkPositionType gap_side)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	if (DETAIL ("tab"))
	{
		WidgetParameters params;
		TabParameters tab;
		
		int current_page = gtk_notebook_get_current_page ((GtkNotebook *) widget);
		int num_of_pages = gtk_notebook_get_n_pages ((GtkNotebook *) widget);
		
		aurora_set_widget_parameters (widget, style, state_type, &params);
		tab.gap_side = (AuroraGapSide) gap_side;

		if (current_page == 0)
			tab.first_tab = (params.ltr || (tab.gap_side == AUR_GAP_LEFT || tab.gap_side == AUR_GAP_RIGHT)) ? TRUE : FALSE;
		else
			tab.first_tab = (params.ltr || (tab.gap_side == AUR_GAP_LEFT || tab.gap_side == AUR_GAP_RIGHT)) ? FALSE : TRUE;
		if (num_of_pages - 1 == current_page)
			tab.last_tab = (params.ltr || (tab.gap_side == AUR_GAP_LEFT || tab.gap_side == AUR_GAP_RIGHT)) ? TRUE : FALSE;
		else
			tab.last_tab = (params.ltr || (tab.gap_side == AUR_GAP_LEFT || tab.gap_side == AUR_GAP_RIGHT)) ? FALSE : TRUE;

		if (num_of_pages == 1)
			tab.last_tab = tab.first_tab = TRUE;
			
		if(params.active)
			params.curvature = 1.0;
		else
			params.curvature = 2.0;
    
		if (gtk_notebook_get_show_tabs((GtkNotebook*)widget)) {
			if (gtk_notebook_get_show_border((GtkNotebook*)widget)) {
				aurora_draw_tab (cr, colors, &params, &tab, x, y, width, height);
			}
			else {
				params.corners = AUR_CORNER_NONE;
				aurora_draw_tab_no_border (cr, colors, &params, &tab, x, y, width, height);
			}
		}

	}
	else {
		aurora_parent_class->draw_extension (style, window, state_type,
					shadow_type, area, widget, detail, x,
					y, width, height, gap_side);

	}
	cairo_destroy (cr);
}

static void
aurora_style_draw_handle (DRAW_ARGS, GtkOrientation orientation)
{

	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	gboolean is_horizontal;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	// Evil hack to work around broken orientation for toolbars
	is_horizontal = (width > height);

	if (DETAIL ("handlebox"))
	{
		WidgetParameters params;
		HandleParameters handle;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		handle.type = AUR_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;

    // Is this ever true? -Daniel
		if (AURORA_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE) {
			cairo_save (cr);
			aurora_draw_toolbar (cr, colors, &params, x, y, width, height);
			cairo_restore (cr);
		}

		aurora_draw_handle (cr, colors, &params, &handle, x, y, width, height);
	}
	else if (DETAIL ("paned"))
	{
		WidgetParameters params;
		HandleParameters handle;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		handle.type = AUR_HANDLE_SPLITTER;
		handle.horizontal = orientation == GTK_ORIENTATION_HORIZONTAL;
        
		aurora_draw_handle (cr, colors, &params, &handle, x, y, width, height);
	}
	else {
    WidgetParameters params;
    HandleParameters handle;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		handle.type = AUR_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;

		if (AURORA_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE) {
			cairo_save (cr);
			aurora_draw_toolbar (cr, colors, &params, x, y, width, height);
			cairo_restore (cr);
		}

		aurora_draw_handle (cr, colors, &params, &handle, x, y, width, height);
  }
	cairo_destroy (cr);
}

static void
aurora_style_draw_box (DRAW_ARGS)
{

	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);


	if (DETAIL ("button") && widget && widget->parent &&
                  (AURORA_IS_TREE_VIEW(widget->parent) ||
                   AURORA_IS_CLIST (widget->parent)  ||
				   aurora_object_is_a (G_OBJECT (widget->parent), "ETreeView") 
				   )) 
	{
		WidgetParameters params;
		ListViewHeaderParameters header;

		gint columns, column_index;
		gboolean resizable = TRUE;
		gboolean sorted = FALSE;
		columns = 3;
		column_index = 1;
		
		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.corners = AUR_CORNER_NONE;

		if (AURORA_IS_TREE_VIEW (widget->parent)) {
			aurora_gtk_treeview_get_header_index (GTK_TREE_VIEW (widget->parent), widget, &column_index, &columns, &resizable, &sorted);
		}
		else if (AURORA_IS_CLIST (widget->parent)) {
			aurora_gtk_clist_get_header_index (GTK_CLIST (widget->parent), widget, &column_index, &columns);
		}

		header.resizable = resizable;
		header.sorted = sorted;

		if (column_index == 0) {
			if (column_index == columns - 1) 
				header.order = AUR_ORDER_FIRST_AND_LAST;
			else
				header.order = AUR_ORDER_FIRST;
		}
		else if (column_index == columns - 1)
			header.order = AUR_ORDER_LAST;
		else
			header.order = AUR_ORDER_MIDDLE;


#ifdef HAVE_ANIMATION
		if (aurora_style->animation)
		{
			aurora_animation_connect_button (widget);
			params.prev_state_type = aurora_animation_starting_state (widget);
		}

		if (aurora_style->animation && GTK_IS_BUTTON (widget) && aurora_animation_is_animated (widget))
		{
			gfloat elapsed = aurora_animation_elapsed (widget);
			params.prev_state_type = aurora_animation_starting_state (widget);
			params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));	
		}
#endif

		aurora_draw_list_view_header (cr, colors, &params, &header, x, y, width, height);
			
	}
  else if (DETAIL ("button")) {
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.active = (shadow_type == GTK_SHADOW_IN);
	  
#ifdef HAVE_ANIMATION
		if (aurora_style->animation && !(aurora_is_panel_widget_item(widget)))
		{
			aurora_animation_connect_button (widget);
			params.prev_state_type = aurora_animation_starting_state (widget);
		}

		if (aurora_style->animation && GTK_IS_BUTTON (widget) && aurora_animation_is_animated (widget))
		{
			gfloat elapsed = aurora_animation_elapsed (widget);
			params.prev_state_type = aurora_animation_starting_state (widget);
			params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));	
		}
#endif
    
		if ((widget && (AURORA_IS_COMBO_BOX_ENTRY (widget->parent) || AURORA_IS_COMBO (widget->parent)))) {
			
			//As the enter key for a GtkCombo makes the list appear, have the focus drawn for the button too.
			if (AURORA_IS_COMBO (widget->parent))
				params.focus |= GTK_WIDGET_HAS_FOCUS (((GtkCombo*)widget->parent)->entry);
			
			if (params.ltr) {
				params.corners = AUR_CORNER_TOPRIGHT | AUR_CORNER_BOTTOMRIGHT;
				x -= 1;
				width += 1;					
			}
			else {
				params.corners = AUR_CORNER_TOPLEFT | AUR_CORNER_BOTTOMLEFT;
				width += 1;					
			}
		}


		if (aurora_is_panel_widget_item(widget)) {	//for task list
			params.corners = AUR_CORNER_TOPLEFT | AUR_CORNER_TOPRIGHT;
			height += 3;
		}
	  
		//For special buttons like tab close buttons, reduce curvature to sane amount.
		//Increase width/height as well as focus won't be drawn
		//Haven't confirmed if some apps use x/ythickness = 0 for other reasons and this could backfire.
		if (params.xthickness <= 0 || params.ythickness <= 0) {
			x--;
			y--;
			width+=2;
		  	height+=2;
	  		params.curvature = MIN (params.curvature, 2.5);
		}
	  
	  	aurora_draw_button (cr, &aurora_style->colors, &params, x, y, width, height);
	} 
	else if (DETAIL ("trough")  && (AURORA_IS_VSCROLLBAR (widget) || AURORA_IS_HSCROLLBAR (widget))) {
		WidgetParameters params;
		ScrollBarParameters scrollbar;

		aurora_set_widget_parameters (widget, style, state_type, &params);
				params.curvature = 0;
		scrollbar.horizontal = TRUE;
		if (AURORA_IS_RANGE (widget))
			scrollbar.horizontal = GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL;
		aurora_draw_scrollbar_trough (cr, colors, &params, &scrollbar, x, y, width, height);
	}
	else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar") || DETAIL ("slider") || DETAIL ("stepper")) {
		WidgetParameters params;
		ScrollBarParameters scrollbar;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		scrollbar.horizontal = TRUE;

		if (AURORA_IS_RANGE (widget))
			scrollbar.horizontal = GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL;
			
		if (DETAIL ("slider")) {  
			aurora_draw_scrollbar_slider (cr, colors, &params, &scrollbar, x, y, width, height);
		}
	}
	else if (DETAIL ("menu")) {
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);
		aurora_draw_menu_frame (cr, colors, &params, x, y, width, height);
	}
	else if (DETAIL ("menubar") && !aurora_is_panel_widget_item(widget))
	{
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);
		aurora_draw_menubar (cr, colors, &params, x, y, width, height, aurora_style->menubarstyle);
	}
	else if (DETAIL ("menuitem")) {
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);

		//If parent is menubar draw top corners rounded else draw normal
		if (widget && AURORA_IS_MENU_BAR (widget->parent)) {
			if (aurora_is_panel_widget_item(widget)) {
				params.corners = AUR_CORNER_NONE;
			}
			else {
				params.corners = AUR_CORNER_TOPRIGHT | AUR_CORNER_TOPLEFT;
			}
			params.curvature = (params.curvature > height*0.2) ? height*0.2 : params.curvature;
			height++;			//hide bottom border so we don't get a double border with menu
			aurora_draw_menubar_item (cr, colors, &params, x, y, width, height);
		}
		else {
			aurora_draw_menuitem (cr, colors, &params, x, y, width, height);
		}
	}
	else if (DETAIL ("spinbutton_up") || DETAIL ("spinbutton_down")) {
	  
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);
	  
	  	cairo_rectangle (cr, x, y, width, height);
		if (widget && !g_object_get_data(G_OBJECT (widget), "transparent-bg-hint"))
		{
			AuroraRGB parentbg;
			aurora_get_parent_bg (widget, &parentbg);
			cairo_set_source_rgb (cr, parentbg.r, parentbg.g, parentbg.b);
			cairo_fill_preserve(cr);
		} 
	  	cairo_clip(cr);
	  
	  	//Only draw button as disabled if entire widget is disabled
	  	if (params.disabled && !(GTK_WIDGET_STATE(widget) == GTK_STATE_INSENSITIVE) ) {
			params.disabled = FALSE;
			params.state_type = GTK_STATE_NORMAL;
		}
			
		if (params.ltr) {
			x -= 1;
			width += 1;
			params.corners = AUR_CORNER_TOPRIGHT | AUR_CORNER_BOTTOMRIGHT;
		}
		else {
			width += 1;
			params.corners = AUR_CORNER_TOPLEFT | AUR_CORNER_BOTTOMLEFT;
		}
	  
	  
	 	if (DETAIL ("spinbutton_up")) {
			aurora_draw_button (cr, &aurora_style->colors, &params, x, y, width, height*2);			
		}
		else {
			aurora_draw_button (cr, &aurora_style->colors, &params, x, y - height, width, height*2);			
		}
  }
  else if (DETAIL ("spinbutton")) {}
	else if (DETAIL ("trough")  &&  AURORA_IS_PROGRESS_BAR (widget)) {
		WidgetParameters params;
		ProgressBarParameters progressbar;
		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = 0;

		progressbar.orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));
	  
		if (widget && !g_object_get_data(G_OBJECT (widget), "transparent-bg-hint"))
		{
			AuroraRGB parentbg;
			aurora_get_parent_bg (widget, &parentbg);
			cairo_rectangle (cr, x, y, width, height);
			cairo_set_source_rgb (cr, parentbg.r, parentbg.g, parentbg.b);
			cairo_fill (cr);
		} 
		aurora_draw_progressbar_trough (cr, colors, &params, &progressbar, x, y, width, height);
	
	}
	else if (DETAIL ("bar")) {
		WidgetParameters params;
		ProgressBarParameters progressbar;
		gdouble elapsed = 0.0;

#ifdef HAVE_ANIMATION
		if (aurora_style->animation && AUR_IS_PROGRESS_BAR (widget)) {
			gboolean activity_mode = GTK_PROGRESS (widget)->activity_mode;
			if (!activity_mode)
				aurora_animation_progressbar_add ((gpointer) widget);
		}
		elapsed = aurora_animation_elapsed (widget);
#endif

		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = 0;

		if (widget && AURORA_IS_PROGRESS_BAR (widget))
			progressbar.orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));
		else
			progressbar.orientation = AUR_ORIENTATION_LEFT_TO_RIGHT;
			
		aurora_draw_progressbar_fill (cr, colors, &params, &progressbar, x - 1, y, width + 2, height, (int) (elapsed * 10.0) % 10);
	}
	else if (DETAIL ("toolbar") || DETAIL ("handlebox_bin") || DETAIL ("dockitem_bin")) {
		// Only draw the shadows on horizontal toolbars
		if (shadow_type != GTK_SHADOW_NONE && height < 2 * width) {
			aurora_draw_toolbar (cr, colors, NULL, x, y, width, height);
		}
	}
  else if (DETAIL ("trough") && GTK_IS_SCALE (widget)) {
    GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (widget));
    WidgetParameters params;
    SliderParameters slider;
    gint slider_length;

    gtk_widget_style_get (widget, "slider-length", &slider_length, NULL);
    aurora_set_widget_parameters (widget, style, state_type, &params);

    slider.inverted = gtk_range_get_inverted (GTK_RANGE (widget));
    slider.horizontal = (GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL);
    slider.fill_size = ((slider.horizontal ? width : height) - slider_length) * (1 / ((adjustment->upper - adjustment->lower) / (adjustment->value - adjustment->lower)));
    if (slider.horizontal)
      slider.inverted = slider.inverted != (aurora_get_direction (widget) == GTK_TEXT_DIR_RTL);

    if (slider.horizontal) {
      x += slider_length / 2;
      width -= slider_length;
    }
    else {
      y += slider_length / 2;
      height -= slider_length;
    }


    aurora_draw_scale_trough (cr, &aurora_style->colors, &params, &slider, x, y, width, height);
  }
	else if (DETAIL ("trough")) { }
	else if (DETAIL ("hseparator") || DETAIL ("vseparator"))
	{
		gchar *new_detail = (gchar*) detail;
		if (AURORA_IS_MENU_ITEM (widget))
			new_detail = "menuitem";

		if (DETAIL ("hseparator"))
		{
			gtk_paint_hline (style, window, state_type, area, widget, new_detail,
			                 x, x + width - 1, y + height/2);
		}
		else
			gtk_paint_vline (style, window, state_type, area, widget, new_detail,
			                 y, y + height - 1, x + width/2);
	}
	else if (DETAIL ("buttondefault")) {
  		/* We are already checking the default button with the
		* "clearlooks_set_widget_parameters" function, so we may occur
		* in drawing the button two times. Let's return then.
		*/
	}
	else if (DETAIL ("optionmenu")) {
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);
    
#ifdef HAVE_ANIMATION
	if (aurora_style->animation) {
		aurora_animation_connect_button (widget);
		params.prev_state_type = aurora_animation_starting_state (widget);
	}

	if (aurora_style->animation && GTK_IS_BUTTON (widget) && aurora_animation_is_animated (widget))
	{
		gfloat elapsed = aurora_animation_elapsed (widget);
		params.prev_state_type = aurora_animation_starting_state (widget);
		params.trans = sqrt (sqrt (MIN(elapsed / ANIMATION_TRANS_TIME, 1.0)));	
	}
#endif 
		aurora_draw_button (cr, colors, &params, x, y, width, height);
	}
	else {
		aurora_parent_class->draw_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
	}
	cairo_destroy (cr);
}

static void
aurora_style_draw_slider (DRAW_ARGS, GtkOrientation orientation)
{

	if (DETAIL ("hscale") || DETAIL ("vscale"))
	{
		AuroraStyle *aurora_style = AURORA_STYLE (style);
		AuroraColors *colors = &aurora_style->colors;
		cairo_t *cr;

		WidgetParameters params;
		SliderParameters slider;
		
		CHECK_ARGS
		SANITIZE_SIZE
		cr = aurora_begin_paint (window, area);
		
		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN (MIN(height,width)*0.147, params.curvature);

		slider.horizontal = (orientation == GTK_ORIENTATION_HORIZONTAL);

		if (!params.disabled)
			aurora_draw_slider_button (cr, colors, &params, &slider, x, y, width, height);

		cairo_destroy (cr);
	}
	else {
		aurora_parent_class->draw_slider (style, window, state_type, shadow_type,
				      area, widget, detail, x, y, width,
				      height, orientation);
	}
	
}

static void
aurora_style_draw_option (DRAW_ARGS)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	OptionParameters option;
	
	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);
  
	aurora_set_widget_parameters (widget, style, state_type, &params);

	option.draw_bullet = ((shadow_type == GTK_SHADOW_IN) || (shadow_type == GTK_SHADOW_ETCHED_IN));
	option.inconsistent = (shadow_type == GTK_SHADOW_ETCHED_IN);

#ifdef HAVE_ANIMATION
	if (aurora_style->animation)
		aurora_animation_connect_checkbox (widget);

	if (aurora_style->animation && GTK_IS_CHECK_BUTTON (widget) && aurora_animation_is_animated (widget) && !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = aurora_animation_elapsed (widget);
		params.trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
	}
#endif

	if (widget && AURORA_IS_MENU (widget->parent)) {
		x--;
		y--;
		aurora_draw_menu_radiobutton (cr, colors, &params, &option, x, y, width, height);
	}
	else if (DETAIL ("cellradio")) {
		x--;
		y--;
		aurora_draw_cell_radiobutton (cr, colors, &params, &option, x, y, width, height);
	}
	else {
		aurora_draw_radiobutton (cr, colors, &params, &option, x, y, width, height);
	}
  cairo_destroy (cr);
}

static void
aurora_style_draw_check (DRAW_ARGS)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	OptionParameters option;
	
	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	aurora_set_widget_parameters (widget, style, state_type, &params);

	option.draw_bullet = ((shadow_type == GTK_SHADOW_IN) || (shadow_type == GTK_SHADOW_ETCHED_IN));
	option.inconsistent = (shadow_type == GTK_SHADOW_ETCHED_IN);

#ifdef HAVE_ANIMATION
	if (aurora_style->animation)
		aurora_animation_connect_checkbox (widget);

	if (aurora_style->animation && GTK_IS_CHECK_BUTTON (widget) && aurora_animation_is_animated (widget) && !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = aurora_animation_elapsed (widget);
		params.trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
	}
#endif

	if (widget && AURORA_IS_MENU (widget->parent)) {
		aurora_draw_menu_checkbutton (cr, colors, &params, &option, x, y, width, height);
	}
	else if (DETAIL ("cellcheck")) {
		aurora_draw_cell_checkbutton (cr, colors, &params, &option, x, y, width, height);
	}
	else {
		aurora_draw_checkbutton (cr, colors, &params, &option, x, y, width, height);
	}
	cairo_destroy (cr);
}

static void
aurora_style_draw_tab (DRAW_ARGS)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	ArrowParameters arrow;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	aurora_set_widget_parameters (widget, style, state_type, &params);
	arrow.type = aurora_style->old_arrowstyle ? AUR_ARROW_COMBO : AUR_ARROW_COMBO_2;
	arrow.direction = AUR_DIRECTION_DOWN;
	arrow.size = aurora_style->arrowsize;

	aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);

	cairo_destroy (cr);
}

static void
aurora_style_draw_vline (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail, gint y1, gint y2, gint x)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	SeparatorParameters separator;
	WidgetParameters params;
	
	CHECK_ARGS
	cr = aurora_begin_paint (window, area);
	
	separator.horizontal = FALSE;	
	aurora_set_widget_parameters (widget, style, state_type, &params);
  
	if (widget && widget->parent && widget->parent->parent && widget->parent->parent->parent &&
      AURORA_IS_HBOX (widget->parent) &&
      AURORA_IS_TOGGLE_BUTTON (widget->parent->parent) &&
      AURORA_IS_COMBO_BOX (widget->parent->parent->parent)) {
	}
	else
		aurora_draw_separator (cr, colors, &params, &separator, x, y1, 2, y2 - y1);
		
	cairo_destroy (cr);
}

static void
aurora_style_draw_hline (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail, gint x1, gint x2, gint y)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	SeparatorParameters separator;
	
	CHECK_ARGS
	cr = aurora_begin_paint (window, area);

	aurora_set_widget_parameters (widget, style, state_type, &params);

	separator.horizontal = TRUE;
		
	if (!DETAIL ("menuitem"))
		aurora_draw_separator (cr, colors, &params, &separator, x1, y, x2 - x1, 2);
	else {
		AuroraRGB border;
		cairo_move_to(cr, x1+0.5,y+0.5);
		cairo_line_to(cr, x2+0.5,y+0.5);
		aurora_shade (&colors->bg[params.state_type], &border, 0.85);
		cairo_set_source_rgb (cr, border.r, border.g, border.b);
		cairo_stroke(cr);
	}

	cairo_destroy (cr);
}

static void
aurora_style_draw_shadow_gap (DRAW_ARGS,
			      GtkPositionType gap_side,
			      gint gap_x, gint gap_width)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	if (DETAIL ("frame") && shadow_type != GTK_SHADOW_NONE) {
		WidgetParameters params;
		FrameParameters frame;

		frame.shadow = shadow_type;
		frame.gap_side = gap_side;
		frame.gap_x = gap_x;
		frame.gap_width = gap_width;
		frame.border = (AuroraRGB *) & colors->shade[FRAME_SHADE];
		frame.use_fill = FALSE;
		frame.fill_bg = TRUE;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		params.curvature = MIN(params.curvature, MIN(params.xthickness, params.ythickness) + 1.5);
		
		aurora_draw_frame (cr, colors, &params, &frame, x-1, y-1, width+2, height+2);
	}
	
	/*else {
		aurora_parent_class->draw_shadow_gap (style, window, state_type,
					  shadow_type, area, widget, detail,
					  x, y, width, height, gap_side,
					  gap_x, gap_width);
	}*/
  cairo_destroy (cr);
}

static void
aurora_style_draw_resize_grip (GtkStyle * style,
			       GdkWindow * window,
			       GtkStateType state_type,
			       GdkRectangle * area,
			       GtkWidget * widget,
			       const gchar * detail,
			       GdkWindowEdge edge,
			       gint x, gint y, gint width, gint height)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;
	WidgetParameters params;
	ResizeGripParameters grip;
	
	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	aurora_set_widget_parameters (widget, style, state_type, &params);
	grip.edge = (AuroraWindowEdge) edge;

	if (edge != GDK_WINDOW_EDGE_SOUTH_EAST)
		return;			// sorry... need to work on this :P

	aurora_draw_resize_grip (cr, colors, &params, &grip, x+2*params.xthickness, y+2*params.ythickness, width, height);

	cairo_destroy (cr);
}

static void
aurora_style_draw_arrow (GtkStyle * style,
			 GdkWindow * window,
			 GtkStateType state_type,
			 GtkShadowType shadow,
			 GdkRectangle * area,
			 GtkWidget * widget,
			 const gchar * detail,
			 GtkArrowType arrow_type,
			 gboolean fill,
			 gint x, gint y, gint width, gint height)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = aurora_begin_paint (window, area);

	if (arrow_type == GTK_ARROW_NONE) {	
		cairo_destroy (cr);
		return;
	}

	if (DETAIL ("arrow"))
	{
		WidgetParameters params;
		ArrowParameters arrow;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		arrow.direction = (AuroraDirection) arrow_type;
		arrow.type = aurora_style->old_arrowstyle ? AUR_ARROW_COMBO : AUR_ARROW_COMBO_2;
		if (aurora_is_tree_column_header(widget))
			arrow.type = AUR_ARROW_COMBO;

		arrow.size = aurora_style->arrowsize;
		aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);
	}
	else if (DETAIL ("menuitem"))
	{
		WidgetParameters params;
		ArrowParameters arrow;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		arrow.type = AUR_ARROW_NORMAL;
		arrow.direction = (AuroraDirection) arrow_type;

		x -= 2;
		aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);
  }
  else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))
	{
		WidgetParameters params;
		ArrowParameters arrow;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		arrow.type = AUR_ARROW_NORMAL;
		arrow.direction = (AuroraDirection) arrow_type;

	
   /* AuroraRGB focus_color = colors->spot[1];
    cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.05);
    clearlooks_rounded_rectangle (cr, x-width, y-height, x+2*width, y+2*height, 0, AUR_CORNER_ALL);
    cairo_fill (cr);*/


		if (arrow.direction == AUR_DIRECTION_LEFT)
			x++;
		else if (arrow.direction == AUR_DIRECTION_RIGHT)
			x--;
		else if (arrow.direction == AUR_DIRECTION_UP)
			y++;
		else
			y--;

		aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);
	}
	else if (DETAIL ("spinbutton"))
	{
		WidgetParameters params;
		ArrowParameters arrow;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		arrow.type = AUR_ARROW_NORMAL;
		arrow.direction = (AuroraDirection) arrow_type;

		//width += 4;
		//x -= 2;
		//height += 3;

		if (arrow.direction == AUR_DIRECTION_DOWN)
			y -= 1;


		aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);
	}
  else
	{
		WidgetParameters params;
		ArrowParameters arrow;

		aurora_set_widget_parameters (widget, style, state_type, &params);
		arrow.type = AUR_ARROW_NORMAL;
		arrow.direction = (AuroraDirection) arrow_type;

		aurora_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);
	}
	cairo_destroy (cr);
}

static void
aurora_style_init_from_rc (GtkStyle * style, GtkRcStyle * rc_style)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);

	aurora_parent_class->init_from_rc (style, rc_style);
	
	aurora_style->curvature        = AURORA_RC_STYLE (rc_style)->curvature;
	aurora_style->menubarstyle     = AURORA_RC_STYLE (rc_style)->menubarstyle;
	aurora_style->animation        = AURORA_RC_STYLE (rc_style)->animation;
	aurora_style->arrowsize        = AURORA_RC_STYLE (rc_style)->arrowsize;
	aurora_style->old_arrowstyle   = AURORA_RC_STYLE (rc_style)->old_arrowstyle;
	
}

static void
aurora_style_realize (GtkStyle * style)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);

		//shade 1,2,6,7 not used.  0,3,4,5,8 used (something better could be done here
		//lots of variation is used for highlights but little in the mid and shadow tones...
		// highlights 1.175, 1.15, 1.1, 1.07, 1.03   and mids 0.82, 0.7, 0.665  and shadow 0.4 ???
		//more spot shades are desired too I think or maybe just adjust defaults...
	double shades[] = { 1.15, 1.04, 0.94, 0.80, 0.70, 0.64, 0.5, 0.45, 0.4 };
			
	AuroraRGB spot_color;
	AuroraRGB bg_normal;
	AuroraHSB bg_hsb;
	double contrast;
	int i;

	aurora_parent_class->realize (style);

	contrast = AURORA_RC_STYLE (style->rc_style)->contrast;

	aurora_gdk_color_to_cairo (&style->bg[GTK_STATE_NORMAL], &bg_normal);
	aurora_hsb_from_color(&bg_normal, &bg_hsb);

	/* Lighter to darker */
	for (i = 0; i < 9; i++)
	{
		aurora_shade_hsb(&bg_hsb, &aurora_style->colors.shade[i], (shades[i]-0.7) * contrast + 0.7);
	}
	
	aurora_gdk_color_to_cairo (&style->bg[GTK_STATE_SELECTED], &spot_color);
	
	aurora_shade(&spot_color, &aurora_style->colors.spot[0],1.42);
	aurora_style->colors.spot[1].r = spot_color.r;
	aurora_style->colors.spot[1].g = spot_color.g;
	aurora_style->colors.spot[1].b = spot_color.b;
	aurora_shade(&spot_color, &aurora_style->colors.spot[2], 0.65);
	
	for (i=0; i<5; i++)
	{
		aurora_gdk_color_to_cairo (&style->bg[i], &aurora_style->colors.bg[i]);
		aurora_gdk_color_to_cairo (&style->base[i], &aurora_style->colors.base[i]);
		aurora_gdk_color_to_cairo (&style->text[i], &aurora_style->colors.text[i]);
	}
}

static void
aurora_style_draw_focus (GtkStyle * style, GdkWindow * window,
			 GtkStateType state_type, GdkRectangle * area,
			 GtkWidget * widget, const gchar * detail, gint x,
			 gint y, gint width, gint height)
{
	AuroraStyle *aurora_style = AURORA_STYLE (style);
	AuroraColors *colors = &aurora_style->colors;
	cairo_t *cr;

	CHECK_ARGS
	SANITIZE_SIZE
	cr = gdk_cairo_create (window);

	//As all other focus drawing is taken care of by respective widgets...
	//GtkTextView

	if (DETAIL ("scrolled_window") || DETAIL ("viewport") || DETAIL ("checkbutton") || DETAIL ("expander") || detail == NULL
				|| (DETAIL ("button") && widget && (AURORA_IS_TREE_VIEW (widget->parent) || AURORA_IS_CLIST (widget->parent)))) {
		WidgetParameters params;
		aurora_set_widget_parameters (widget, style, state_type, &params);

		cairo_translate (cr, x, y);
		cairo_set_line_width (cr, 1.0);

		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.15);
		clearlooks_rounded_rectangle (cr, 0, 0, width - 1, height - 1, params.curvature, AUR_CORNER_ALL);
		cairo_fill (cr);
					
		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.45);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, params.curvature, AUR_CORNER_ALL);
		cairo_stroke (cr);
	}
	cairo_destroy (cr);
}

static void
aurora_style_copy (GtkStyle * style, GtkStyle * src)
{
  AuroraStyle *aur_style = AURORA_STYLE (style);
  AuroraStyle *aur_src = AURORA_STYLE (src);

  aur_style->colors         = aur_src->colors;
  aur_style->curvature      = aur_src->curvature;
  aur_style->menubarstyle   = aur_src->menubarstyle;
  aur_style->arrowsize      = aur_src->arrowsize;
  aur_style->old_arrowstyle = aur_src->old_arrowstyle;
  aur_style->animation      = aur_src->animation;

  aurora_parent_class->copy (style, src);
}

static void
aurora_style_unrealize (GtkStyle * style)
{
  aurora_parent_class->unrealize (style);
}

static GdkPixbuf *
set_transparency (const GdkPixbuf * pixbuf, gdouble alpha_percent)
{
  GdkPixbuf *target;
  guchar *data, *current;
  guint x, y, rowstride, height, width;

  g_return_val_if_fail (pixbuf != NULL, NULL);
  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

  /* Returns a copy of pixbuf with it's non-completely-transparent pixels to
     have an alpha level "alpha_percent" of their original value. */

  target = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

  if (alpha_percent == 1.0)
    return target;
  width = gdk_pixbuf_get_width (target);
  height = gdk_pixbuf_get_height (target);
  rowstride = gdk_pixbuf_get_rowstride (target);
  data = gdk_pixbuf_get_pixels (target);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      /* The "4" is the number of chars per pixel, in this case, RGBA,
         the 3 means "skip to the alpha" */
      current = data + (y * rowstride) + (x * 4) + 3;
      *(current) = (guchar) (*(current) * alpha_percent);
    }
  }

  return target;
}

static GdkPixbuf *
scale_or_ref (GdkPixbuf * src, int width, int height)
{
  if (width == gdk_pixbuf_get_width (src) && height == gdk_pixbuf_get_height (src)) {
    return g_object_ref (src);
  }
  else {
    return gdk_pixbuf_scale_simple (src, width, height, GDK_INTERP_BILINEAR);
  }
}


static void
icon_scale_brightness (const GdkPixbuf * src,
		       GdkPixbuf * dest, gfloat brightness)
{
  int i, j, t;
  int width, height, has_alpha, src_rowstride, dest_rowstride, bytes_per_pixel;
  guchar *src_line;
  guchar *dest_line;
  guchar *src_pixel;
  guchar *dest_pixel;

  g_return_if_fail (GDK_IS_PIXBUF (src));
  g_return_if_fail (GDK_IS_PIXBUF (dest));
  g_return_if_fail (gdk_pixbuf_get_height (src) == gdk_pixbuf_get_height (dest));
  g_return_if_fail (gdk_pixbuf_get_width (src) == gdk_pixbuf_get_width (dest));
  g_return_if_fail (gdk_pixbuf_get_has_alpha (src) == gdk_pixbuf_get_has_alpha (dest));
  g_return_if_fail (gdk_pixbuf_get_colorspace (src) == gdk_pixbuf_get_colorspace (dest));

  has_alpha = gdk_pixbuf_get_has_alpha (src);
  bytes_per_pixel = has_alpha ? 4 : 3;
  width = gdk_pixbuf_get_width (src);
  height = gdk_pixbuf_get_height (src);
  src_rowstride = gdk_pixbuf_get_rowstride (src);
  dest_rowstride = gdk_pixbuf_get_rowstride (dest);

  src_line = gdk_pixbuf_get_pixels (src);
  dest_line = gdk_pixbuf_get_pixels (dest);

#define CLAMP_UCHAR(v) (t = (v), CLAMP (t, 0, 255))
#define SCALE_BRIGHTNESS(v) ( brightness * (v))

  for (i = 0; i < height; i++) {
    src_pixel = src_line;
    src_line += src_rowstride;
    dest_pixel = dest_line;
    dest_line += dest_rowstride;

    for (j = 0; j < width; j++) {
      dest_pixel[0] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[0]));
      dest_pixel[1] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[1]));
      dest_pixel[2] = CLAMP_UCHAR (SCALE_BRIGHTNESS (src_pixel[2]));

      if (has_alpha)
	  dest_pixel[3] = src_pixel[3];

      src_pixel += bytes_per_pixel;
      dest_pixel += bytes_per_pixel;
    }
  }
}

static GdkPixbuf *
aurora_style_draw_render_icon (GtkStyle * style,
			       const GtkIconSource * source,
			       GtkTextDirection direction,
			       GtkStateType state,
			       GtkIconSize size,
			       GtkWidget * widget, const char *detail)
{
  int width = 1;
  int height = 1;
  GdkPixbuf *scaled;
  GdkPixbuf *stated;
  GdkPixbuf *base_pixbuf;
  GdkScreen *screen;
  GtkSettings *settings;

  /* Oddly, style can be NULL in this function, because
   * GtkIconSet can be used without a style and if so
   * it uses this function.
   */

  base_pixbuf = gtk_icon_source_get_pixbuf (source);

  g_return_val_if_fail (base_pixbuf != NULL, NULL);

  if (widget && gtk_widget_has_screen (widget)) {
    screen = gtk_widget_get_screen (widget);
    settings = gtk_settings_get_for_screen (screen);
  }
  else if (style->colormap) {
    screen = gdk_colormap_get_screen (style->colormap);
    settings = gtk_settings_get_for_screen (screen);
  }
  else {
    settings = gtk_settings_get_default ();
    GTK_NOTE (MULTIHEAD, g_warning("Using the default screen for gtk_default_aurora_style_draw_render_icon()"));
  }


  if (size != (GtkIconSize) - 1
      && !gtk_icon_size_lookup_for_settings (settings, size, &width, &height)) {
    g_warning (G_STRLOC ": invalid icon size '%d'", size);
    return NULL;
  }

  /* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
   * leave it alone.
   */
  if (size != (GtkIconSize) - 1
      && gtk_icon_source_get_size_wildcarded (source))
    scaled = scale_or_ref (base_pixbuf, width, height);
  else
    scaled = g_object_ref (base_pixbuf);

  /* If the state was wildcarded, then generate a state. */
  if (gtk_icon_source_get_state_wildcarded (source)) {
    if (state == GTK_STATE_INSENSITIVE) {
      stated = set_transparency (scaled, 0.3);
      gdk_pixbuf_saturate_and_pixelate (stated, stated, 0.125, FALSE);
      g_object_unref (scaled);
    }
    else if (state == GTK_STATE_PRELIGHT) {
      stated = gdk_pixbuf_copy (scaled);

      //Increase brightness
      icon_scale_brightness (scaled, stated, 1.125);
      //gdk_pixbuf_saturate_and_pixelate (stated, stated, 1.05, FALSE);

      g_object_unref (scaled);
    }
    else {
      stated = scaled;
    }
  }
  else
    stated = scaled;

  return stated;
}
static void
aurora_style_init (AuroraStyle * style) { }

static void
aurora_style_draw_layout (GtkStyle * style,
	     GdkWindow * window,
	     GtkStateType state_type,
	     gboolean use_text,
	     GdkRectangle * area,
	     GtkWidget * widget,
	     const gchar * detail, gint x, gint y, PangoLayout *layout)
{
	GdkGC *gc;

	g_return_if_fail (GTK_IS_STYLE (style));
	g_return_if_fail (window != NULL);

	gc = use_text ? style->text_gc[state_type] : style->fg_gc[state_type];

	if (area)
		gdk_gc_set_clip_rectangle (gc, area);


		
	if (state_type == GTK_STATE_INSENSITIVE ){ //&& !AURORA_IS_MENU_BAR(widget->parent->parent)) {
		AuroraStyle *aurora_style = AURORA_STYLE (style);
		AuroraColors *colors = &aurora_style->colors;
		double bg_value, text_value;

		WidgetParameters params;
		GdkColor etched;
		AuroraRGB temp;
		AuroraRGB parentbg;
		
		aurora_set_widget_parameters (widget, style, state_type, &params);

		aurora_get_parent_bg (widget, &parentbg);
		

		bg_value = MAX (MAX (parentbg.r, parentbg.g), parentbg.b);
		text_value = MAX (MAX (colors->text[widget->state].r, colors->text[widget->state].g), colors->text[widget->state].b);

		if(text_value < (1.20*bg_value) ) {	
		    if (widget && GTK_WIDGET_NO_WINDOW (widget))
		      aurora_shade (&parentbg, &temp, 1.15);
		    else
		      aurora_shade (&colors->bg[widget->state], &temp, 1.15);

		    etched.red = (int) (temp.r * 65535);
		    etched.green = (int) (temp.g * 65535);
		    etched.blue = (int) (temp.b * 65535);

		    gdk_draw_layout_with_colors (window, style->text_gc[state_type], x, y + 1, layout, &etched, NULL);
		}
		
    gdk_draw_layout (window, style->text_gc[state_type], x, y, layout);
	}
  else {
		gdk_draw_layout (window, gc, x, y, layout);
  }

  if (area)
    gdk_gc_set_clip_rectangle (gc, NULL);

}

static void
aurora_style_draw_expander (GtkStyle * style,
			    GdkWindow * window,
			    GtkStateType state_type,
			    GdkRectangle * area,
			    GtkWidget * widget,
			    const gchar * detail,
			    gint x, gint y, GtkExpanderStyle expander_style)
{
#define DEFAULT_EXPANDER_SIZE 11

  gint expander_size;
  gint line_width;
  double vertical_overshoot;
  int diameter;
  double radius;
  double interp;		/* interpolation factor for center position */
  double x_double_horz, y_double_horz;
  double x_double_vert, y_double_vert;
  double x_double, y_double;
  gint degrees = 0;

  cairo_t *cr;

  CHECK_ARGS cr = gdk_cairo_create (window);

  if (area) {
    gdk_cairo_rectangle (cr, area);
    cairo_clip (cr);
  }

  if (widget &&
      gtk_widget_class_find_style_property (GTK_WIDGET_GET_CLASS (widget),
					    "expander-size")) {
    gtk_widget_style_get (widget, "expander-size", &expander_size, NULL);
  }
  else
    expander_size = DEFAULT_EXPANDER_SIZE;

  line_width = 1;

  switch (expander_style) {
  case GTK_EXPANDER_COLLAPSED:
    degrees = (aurora_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 180 : 0;
    interp = 0.0;
    break;
  case GTK_EXPANDER_SEMI_COLLAPSED:
    degrees = (aurora_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 150 : 30;
    interp = 0.25;
    break;
  case GTK_EXPANDER_SEMI_EXPANDED:
    degrees = (aurora_get_direction (widget) == GTK_TEXT_DIR_RTL) ? 120 : 60;
    interp = 0.75;
    break;
  case GTK_EXPANDER_EXPANDED:
    degrees = 90;
    interp = 1.0;
    break;
  default:
    g_assert_not_reached ();
  }

  /* Compute distance that the stroke extends beyonds the end
   * of the triangle we draw.
   */
  vertical_overshoot = line_width / 2.0 * (1. / tan (G_PI / 8));

  /* For odd line widths, we end the vertical line of the triangle
   * at a half pixel, so we round differently.
   */
  if (line_width % 2 == 1)
    vertical_overshoot = ceil (0.5 + vertical_overshoot) - 0.5;
  else
    vertical_overshoot = ceil (vertical_overshoot);

  /* Adjust the size of the triangle we draw so that the entire stroke fits
   */
  diameter = MAX (3, expander_size - 2 * vertical_overshoot);

  /* If the line width is odd, we want the diameter to be even,
   * and vice versa, so force the sum to be odd. This relationship
   * makes the point of the triangle look right.
   */
  diameter -= (1 - (diameter + line_width) % 2);

  radius = diameter / 2. + 4;

  /* Adjust the center so that the stroke is properly aligned with
   * the pixel grid. The center adjustment is different for the
   * horizontal and vertical orientations. For intermediate positions
   * we interpolate between the two.
   */
  x_double_vert = floor (x - (radius + line_width) / 2.) + (radius + line_width) / 2. + ceil (radius / 8.0);
  y_double_vert = y - 0.5;

  x_double_horz = x - 0.5 + ceil (radius / 8.0);
  y_double_horz = floor (y - (radius + line_width) / 2.) + (radius + line_width) / 2.;

  x_double = x_double_vert * (1 - interp) + x_double_horz * interp;
  y_double = y_double_vert * (1 - interp) + y_double_horz * interp;

  cairo_translate (cr, x_double, y_double);
  cairo_rotate (cr, degrees * G_PI / 180);

  cairo_move_to (cr, -radius / 2., -radius / 2.0);
  cairo_line_to (cr,  radius / 2.,  0);
  cairo_line_to (cr, -radius / 2.,  radius / 2.0);
  cairo_close_path (cr);

  cairo_set_line_width (cr, line_width);

  if (state_type == GTK_STATE_INSENSITIVE)
    gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_INSENSITIVE]);
  else if (state_type == GTK_STATE_PRELIGHT)
    gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_NORMAL]);
  else if (state_type == GTK_STATE_ACTIVE)
    gdk_cairo_set_source_color (cr, &style->light[GTK_STATE_ACTIVE]);
  else
    gdk_cairo_set_source_color (cr, &style->fg[GTK_STATE_PRELIGHT]);

  cairo_fill_preserve (cr);

  gdk_cairo_set_source_color (cr, &style->fg[state_type]);
  cairo_stroke (cr);

  cairo_destroy (cr);

}



static void
aurora_style_class_init (AuroraStyleClass * klass)
{
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);

  aurora_style_class = AURORA_STYLE_CLASS (klass);
  aurora_parent_class = g_type_class_peek_parent (klass);

  style_class->copy		          = aurora_style_copy;
  style_class->realize          = aurora_style_realize;
  style_class->unrealize        = aurora_style_unrealize;
  style_class->init_from_rc     = aurora_style_init_from_rc;
  style_class->draw_handle      = aurora_style_draw_handle;
  style_class->draw_slider      = aurora_style_draw_slider;
  style_class->draw_shadow_gap  = aurora_style_draw_shadow_gap;
  style_class->draw_focus       = aurora_style_draw_focus;
  style_class->draw_box         = aurora_style_draw_box;
  style_class->draw_shadow      = aurora_style_draw_shadow;
  style_class->draw_box_gap     = aurora_style_draw_box_gap;
  style_class->draw_extension   = aurora_style_draw_extension;
  style_class->draw_option      = aurora_style_draw_option;
  style_class->draw_check       = aurora_style_draw_check;
  style_class->draw_flat_box    = aurora_style_draw_flat_box;
  style_class->draw_tab         = aurora_style_draw_tab;
  style_class->draw_vline       = aurora_style_draw_vline;
  style_class->draw_hline       = aurora_style_draw_hline;
  style_class->draw_resize_grip = aurora_style_draw_resize_grip;
  style_class->draw_arrow       = aurora_style_draw_arrow;
  style_class->render_icon      = aurora_style_draw_render_icon;
  style_class->draw_layout      = aurora_style_draw_layout;
  style_class->draw_expander    = aurora_style_draw_expander;
}

GType aurora_type_style = 0;

void
aurora_style_register_type (GTypeModule * module)
{
  static const GTypeInfo object_info = {
    sizeof (AuroraStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) aurora_style_class_init,
    NULL,			/* class_finalize */
    NULL,			/* class_data */
    sizeof (AuroraStyle),
    0,				/* n_preallocs */
    (GInstanceInitFunc) aurora_style_init,
    NULL
  };

  aurora_type_style = g_type_module_register_type (module,
						   GTK_TYPE_STYLE,
						   "AuroraStyle",
						   &object_info, 0);
}
