#include "aurora_draw.h"
#include "aurora_style.h"
#include "aurora_types.h"

#include "support.h"

#include <cairo.h>

#define M_PI 3.14159265358979323846
#define M_SQRT3OVER2  0.8660254037844386467637
// Static Aurora Functions



static void
clearlooks_rounded_rectangle (cairo_t * cr,
			      double x, double y, double w, double h,
			      double radius, uint8 corners)
{
				
    if (radius < 0.01 || (corners == AUR_CORNER_NONE)) {
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

static void
aurora_tab_rectangle (cairo_t * cr,
		      double x, double y, double w, double h,
		      double radius, boolean first, boolean last, boolean active)
{  
  
	if (active) {
		clearlooks_rounded_rectangle (cr, x, y, w, h, radius, AUR_CORNER_TOPLEFT | AUR_CORNER_TOPRIGHT);
	}
	else {
		if (first && last) {
			//Due to clipping issues on the frame this is for filling in clipped pixels
			cairo_move_to (cr, x, y + h + 1);
			cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
			cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
			cairo_line_to (cr, x + w, y + h + 1);
		}
		else if (first) {
			cairo_move_to (cr, x, y + h + 1);
			cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
			cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
			cairo_arc_negative (cr, x + w + radius, y + h - radius, radius, M_PI, M_PI * 0.5);
		}
		else if (last) {
			cairo_move_to (cr, x - radius, y + h);
			cairo_arc_negative (cr, x - radius, y + h - radius, radius, M_PI * 0.5, 0);
			cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
			cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
			cairo_line_to (cr, x + w, y + h);
			//cairo_line_to (cr, x + w + 2, y + h);
		}
		else {
			/*
			Draws bottom left corner topleft topright bottomright*/
			cairo_move_to (cr, x - radius, y + h);
			cairo_arc_negative (cr, x - radius, y + h - radius, radius, M_PI * 0.5, 0);
			cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
			cairo_arc (cr, x + w - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
			cairo_arc_negative (cr, x + w + radius, y + h - radius, radius, M_PI, M_PI * 0.5);
		}
	}
		
}


static void
rotate_mirror_translate (cairo_t * cr, double radius, double x, double y,
			 boolean mirror_horizontally,
			 boolean mirror_vertically)
{

	cairo_matrix_t matrix_result;

	double r_cos = cos (radius);
	double r_sin = sin (radius);

	cairo_matrix_init (&matrix_result, mirror_horizontally ? -r_cos : r_cos,
		     mirror_horizontally ? -r_sin : r_sin,
		     mirror_vertically ? -r_sin : r_sin,
		     mirror_vertically ? -r_cos : r_cos, x, y);

	cairo_set_matrix (cr, &matrix_result);
}

static void
aurora_draw_etched_shadow (cairo_t * cr, double x, double y, double width,
			    double height, double radius, uint8 corners,
			    const AuroraRGB*  bg_color)
{
	cairo_pattern_t *pattern;
	AuroraRGB shadow;
	AuroraRGB highlight;

	
	AuroraHSB bg;
	aurora_hsb_from_color(bg_color, &bg);
	aurora_shade_hsb (&bg, &shadow, 0.78);
	aurora_shade_hsb (&bg, &highlight, 1.16);	

	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	cairo_pattern_add_color_stop_rgba (pattern, 0.70, bg_color->r, bg_color->g, bg_color->b, 0.0);
	cairo_pattern_add_color_stop_rgba (pattern, 1.00, highlight.r, highlight.g, highlight.b, 0.20);	
	cairo_set_source (cr, pattern);

	//the highlight 
	clearlooks_rounded_rectangle (cr, x, y, width, height, radius, corners);
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);
		
	/*pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, shadow.r, shadow.g, shadow.b, 0.20);
	cairo_pattern_add_color_stop_rgba (pattern, 0.75, bg_color->r, bg_color->g, bg_color->b, 0.0);
	cairo_set_source (cr, pattern);

	//the shadow 
	clearlooks_rounded_rectangle (cr, x+0.5, y, width-1, height, radius, corners);
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);*/
		
		
		
}

//simple drop shadow
static void
aurora_draw_shadow (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const AuroraRGB*  bg_color, double alpha)
{
	AuroraRGB shadow;
	cairo_pattern_t *pattern;
	clearlooks_rounded_rectangle (cr, x+0.5, y+1, width-1, height-1, radius, corners);

	aurora_shade (bg_color, &shadow, 0.885);
	pattern = cairo_pattern_create_linear (0, y, 0, y + height+1);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, shadow.r, shadow.g, shadow.b, 0.0);
	cairo_pattern_add_color_stop_rgba (pattern, 0.4, shadow.r, shadow.g, shadow.b, 0.75*alpha);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0 , shadow.r, shadow.g, shadow.b, alpha);
	cairo_set_source (cr, pattern);

	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);
}


//shaded border
static void
aurora_draw_border (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const AuroraRGB*  border, double lighter, double darker)
{
	cairo_pattern_t *pattern;
	AuroraRGB border_darker, border_lighter;
	
	AuroraHSB border_hsb;
	aurora_hsb_from_color(border, &border_hsb);
	aurora_shade_shift_hsb (&border_hsb, &border_darker, darker);
	aurora_shade_shift_hsb (&border_hsb, &border_lighter, lighter);

	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, border_lighter.r, border_lighter.g, border_lighter.b); 
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, border->r, border->g, border->b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, border_darker.r, border_darker.g, border_darker.b); 

	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, x, y, width, height, radius, corners);
	
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);

}

static void
aurora_draw_etched_border (cairo_t * cr, double x, double y, double width,
		    double height, double radius, uint8 corners,
		    const AuroraRGB*  border, double lighter, double darker)
{
	cairo_pattern_t *pattern;
	AuroraRGB border_darker, border_lighter;
	AuroraHSB border_hsb;
	aurora_hsb_from_color(border, &border_hsb);
	aurora_shade_shift_hsb (&border_hsb, &border_darker, darker);
	aurora_shade_shift_hsb (&border_hsb, &border_lighter, lighter);

	pattern = cairo_pattern_create_linear (0, y, 0, y + height);
	cairo_pattern_add_color_stop_rgb (pattern, (radius/2+1)/height, border_darker.r, border_darker.g, border_darker.b);
	cairo_pattern_add_color_stop_rgb (pattern, (radius/2+1)/height, border->r, border->g, border->b);
	cairo_pattern_add_color_stop_rgba (pattern, (height-radius/2-1)/height, border->r, border->g, border->b, 0.90);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, border_lighter.r, border_lighter.g, border_lighter.b, 0.65); 
	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, x, y, width, height, radius, corners);
	
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);

}

inline static void list_menu_gradient(cairo_t * cr, const AuroraRGB* bg, double x, double y, double width, double height)
{
	AuroraHSB bg_hsb;
	AuroraRGB fill_shade, fill_light;
	AuroraRGB border;
	cairo_pattern_t *pattern;

	aurora_hsb_from_color(bg, &bg_hsb);
	aurora_shade_shift_hsb (&bg_hsb, &fill_shade, 0.85);
	aurora_shade_shift_hsb (&bg_hsb, &fill_light, 1.05);
	aurora_shade_shift_hsb (&bg_hsb, &border, 0.82);
	
	cairo_translate (cr, x, y);
	
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, fill_light.r, fill_light.g, fill_light.b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill_shade.r, fill_shade.g, fill_shade.b);
	cairo_set_source (cr, pattern);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);
	
	cairo_set_source_rgb (cr, bg->r, bg->g, bg->b); 
	cairo_move_to(cr, 0.5, 0.5);
	cairo_line_to(cr, width-1, 0.5);
	cairo_stroke (cr);
	
	cairo_set_source_rgb (cr, border.r, border.g, border.b); 
	cairo_move_to(cr, 0.5, height - 0.5);
	cairo_line_to(cr, width-1, height - 0.5);
	cairo_stroke (cr);
}


// Widget Functions

/*
  Draws the buttons, toolbar buttons and toggle buttons
  Varries based on disabled, active, prelight and focus states
*/
void
aurora_draw_button (cairo_t * cr,
		    const AuroraColors * colors,
		    const WidgetParameters * widget,
		    int x, int y, int width, int height)
{
	

	//Fill color, highlight color and border color
	AuroraRGB fill;
	AuroraHSB fill_hsb;
	AuroraRGB fill_darker, mid_highlight;
	AuroraRGB highlight;
	AuroraRGB border;
	cairo_pattern_t *pattern;
	cairo_translate (cr, x, y);
	
	fill = colors->bg[widget->state_type];
#ifdef HAVE_ANIMATION
	AuroraRGB fill_old;
	fill_old = colors->bg[widget->prev_state_type];
#endif		

	if (widget->active && widget->prelight)
		aurora_shade (&fill, &fill, 0.78);
	
#ifdef HAVE_ANIMATION
	if (widget->active && widget->prev_state_type == GTK_STATE_PRELIGHT)
		aurora_shade (&fill_old, &fill_old, 0.78);
	aurora_mix_color (&fill_old, &fill, widget->trans, &fill);
#endif		

	aurora_hsb_from_color(&fill, &fill_hsb);
	//Highlight
	if (widget->disabled) {
		aurora_shade_shift_hsb (&fill_hsb, &highlight, 1.02);
		aurora_shade_shift_hsb (&fill_hsb, &fill_darker, 0.95);
	}
	else if (widget->active) {
		aurora_shade_shift_hsb (&fill_hsb, &highlight, 1.09);
		aurora_shade_shift_hsb (&fill_hsb, &fill_darker, 0.93);
	}
	else {
		aurora_shade_shift_hsb (&fill_hsb, &highlight, 1.12);
		aurora_shade_shift_hsb (&fill_hsb, &fill_darker, 0.88);		
	}
	
	//border color a blend of bg color and button color
	if (widget->disabled) {
		aurora_shade (&colors->bg[GTK_STATE_INSENSITIVE], &border, 0.81);
	}
	else  {
		aurora_shade (&colors->bg[widget->state_type], &border, 0.60);
		aurora_mix_color (&colors->shade[5], &border, 0.65, &border);
		aurora_scale_saturation(&border, 0.4);
	}
	//aurora_tweak_saturation (&fill_hsb, &border);
		
	//Draw the fill
	aurora_mix_color (&fill, &highlight, 0.10, &mid_highlight);
	pattern = cairo_pattern_create_linear (2, 2, 2, height-2);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, mid_highlight.r, mid_highlight.g, mid_highlight.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, fill.r, fill.g, fill.b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill_darker.r, fill_darker.g, fill_darker.b);

	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, 2, 2, width-4, height-4, widget->curvature - 1, widget->corners);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);
		
	//Check if pressed...
	if (widget->active) {
		/* "pressed" add an inner shadow
		 * 0.5pixel missalignments for x values is intentional.  The shadow on the side edges of the button
		 * are meant to be softer and doing this reaches that goal without apply a gradient pattern.
		 */
		AuroraRGB shadow;
		aurora_shade (&border, &shadow, 0.75);

		cairo_save (cr);
		clearlooks_rounded_rectangle (cr, 2, 2, width-4, height-4, widget->curvature - 1, widget->corners);
		cairo_clip (cr);

		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.25);
		clearlooks_rounded_rectangle (cr, 1, 2.5, width - 2, height - 3, widget->curvature-0.5, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.15);
		clearlooks_rounded_rectangle (cr, 2, 3.5, width - 4 , height -3, widget->curvature-1.0, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.07);
		clearlooks_rounded_rectangle (cr, 3, 4.5, width - 6, height - 3, widget->curvature-2.0, widget->corners);
		cairo_stroke (cr);

		cairo_restore (cr);

	}
	else {
		aurora_shade_shift (&highlight, &highlight, widget->disabled? 1.030 : 1.08);

		pattern = cairo_pattern_create_linear (2, 2, 2, height-2);
		cairo_pattern_add_color_stop_rgb  (pattern, 0.0, highlight.r, highlight.g, highlight.b); 
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
		cairo_set_source (cr, pattern);
		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width-5, height-6, widget->curvature - 1, widget->corners);
		cairo_stroke (cr);

		cairo_pattern_destroy (pattern);
  }

	//the etching/shadow and border
	if (widget->disabled) {
		aurora_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners, &colors->bg[GTK_STATE_INSENSITIVE]); //GTK_STATE_INSENSITIVE to approx normal bg colour
		aurora_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, 1.00, 0.93);
	}
	else if (widget->active)  {
		aurora_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners, &colors->bg[GTK_STATE_INSENSITIVE]); //GTK_STATE_INSENSITIVE to approx normal bg colour
		aurora_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, 0.95, 0.93);
	}
	else {
		if (widget->corners == AUR_CORNER_ALL) //if not entry combo
			aurora_draw_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, 1.08, 0.86);
		else //make top edge darker to fit in with combo entry better
			aurora_draw_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, 1.0, 0.86);
		aurora_draw_shadow (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners, &border, 0.27);
	}
	
	//Draw the focused border 
	if (widget->focus || widget->is_default) {

		AuroraRGB focus_color = colors->spot[1];

		if (widget->is_default && !widget->focus) {	
			aurora_mix_color (&colors->shade[8], &focus_color, 0.65, &focus_color);
			aurora_shade (&focus_color, &focus_color, 0.80);
		}
		
		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.45);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.25);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);
	}
	
	//Draw the mouse over glow
	if (widget->prelight) {
		AuroraRGB focus_color = colors->spot[1];

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.05);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width-1, height-1, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);

	}

}

void
aurora_draw_entry (cairo_t * cr,
		   const AuroraColors * colors,
		   const WidgetParameters * widget,
		   const EntryParameters * entry,
		   int x, int y, int width, int height)
{
	
	//Fill color, border color
	AuroraRGB *fill;
	AuroraRGB border;

	fill = (AuroraRGB*)&colors->base[widget->state_type];

	//Prepare border colors based on the state of the entry
	if (widget->disabled) {
		aurora_shade (&colors->bg[GTK_STATE_INSENSITIVE], &border, 0.81);
	}
	else  {
		aurora_shade (&colors->bg[widget->state_type], &border, 0.60);
		aurora_mix_color (&colors->shade[5], &border, 0.65, &border);
		aurora_scale_saturation(&border, 0.4);
	}
	
	cairo_translate (cr, x, y);

	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);
	clearlooks_rounded_rectangle (cr, 2, 2, width - 4, height - 4, widget->curvature - 1, widget->corners);
	cairo_fill (cr);

	// Draw the border  and etching
	if (widget->disabled)
		aurora_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners,&colors->bg[GTK_STATE_INSENSITIVE]);
	
	aurora_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, widget->disabled ? 1.00 : 1.12, 0.86);


	// Draw the focused border
	if (widget->focus) {
		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.35);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.25);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);
	}

	//if not a combo widget add inner shadow
	AuroraRGB bg_darker;
	clearlooks_rounded_rectangle (cr, 1, 1, width - 3, height - 3, widget->curvature, widget->corners);
	cairo_clip (cr);

	//Inner shadow
	aurora_shade (&colors->bg[widget->state_type], &bg_darker, widget->disabled ? 0.75 : 0.65);
	
	cairo_set_source_rgba (cr, bg_darker.r, bg_darker.g, bg_darker.b, 0.38);
	clearlooks_rounded_rectangle (cr, 2, 2.5, width-4- (entry->isCombo ? 1 : 0), height, widget->curvature - 1, widget->corners);
	cairo_stroke (cr);
}

void
aurora_draw_scrolled (cairo_t * cr,
		   const AuroraColors * colors,
		   const WidgetParameters * widget,
		   int x, int y, int width, int height)
{

	AuroraRGB border;
	border = colors->shade[5];

	cairo_translate (cr, x, y);
	aurora_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature, widget->corners, &border, 1.28, 0.93);

}

void
aurora_draw_scale_trough (cairo_t * cr,
			  const AuroraColors * colors,
			  const WidgetParameters * widget,
			  const SliderParameters * slider,
			  int x, int y, int width, int height)
{
  
  int fill_x, fill_y, fill_width, fill_height;	/* Fill x,y,w,h   */      
  int trough_width, trough_height;
  double translate_x, translate_y;
  int fill_size = slider->fill_size;
  const int TROUGH_SIZE = 6;
  const double radius = 2.5;

  if (slider->horizontal) {
    if (fill_size > width - 3)
      fill_size = width - 3;

    fill_x = slider->inverted ? width - fill_size - 3 : 0;
    fill_y = 0;
    fill_width = fill_size;
    fill_height = TROUGH_SIZE - 2;

    trough_width = width - 3;
    trough_height = TROUGH_SIZE - 2;

    translate_x = x + 0.5;
    translate_y = y + 0.5 + (height / 2) - (TROUGH_SIZE / 2);

  }
  else {
    if (fill_size > height - 3)
      fill_size = height - 3;

    fill_x = 0;
    fill_y = slider->inverted ? height - fill_size - 3 : 0;
    fill_width = TROUGH_SIZE - 2;
    fill_height = fill_size;

    trough_width = TROUGH_SIZE - 2;
    trough_height = height - 3;

    translate_x = x + 0.5 + (width / 2) - (TROUGH_SIZE / 2);
    translate_y = y + 0.5;

  }

  cairo_translate (cr, translate_x + 0.5, translate_y + 0.5);

  AuroraRGB fill = colors->bg[widget->state_type];
  AuroraRGB lighter, darker, mid;
  AuroraRGB border;
  cairo_pattern_t *pattern;
		
  aurora_shade (&fill, &fill, 0.88);
  aurora_shade (&fill, &border, 0.35);

  //unfilled trough
  aurora_shade (&fill, &darker, 0.70);
  aurora_shade (&fill, &mid, 0.95);

  clearlooks_rounded_rectangle (cr, 0, 0, trough_width + 1, trough_height + 1, radius, AUR_CORNER_ALL);
  pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? 0 : trough_width, slider->horizontal ? trough_height : 0);
  cairo_pattern_add_color_stop_rgb (pattern, 0.00, border.r, border.g, border.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.08, darker.r, darker.g, darker.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.80, mid.r, mid.g, mid.b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.00, fill.r, fill.g, fill.b);
  cairo_set_source (cr, pattern);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);


  if (fill_size > 0) {

	  //filled trough
	  fill = colors->spot[1];
	  border = colors->spot[1];

	  aurora_shade_shift (&fill, &lighter, 1.50);
	  aurora_shade_shift (&border, &darker, 0.8125);

	  clearlooks_rounded_rectangle (cr, fill_x, fill_y, fill_width, fill_height+1, radius, AUR_CORNER_ALL);

	  if (slider->inverted)
	    pattern = cairo_pattern_create_linear (slider->horizontal ? fill_width : 0, slider->horizontal ? 0 : fill_height, 0, 0);
	  else
	    pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? fill_width : 0, slider->horizontal ? 0 : fill_height);

	  cairo_pattern_add_color_stop_rgb (pattern, 0.0, darker.r, darker.g, darker.b);
	  cairo_pattern_add_color_stop_rgb (pattern, 1.0, lighter.r, lighter.g, lighter.b);
	  cairo_set_source (cr, pattern);
	  cairo_fill (cr);
	  cairo_pattern_destroy (pattern);


	  /*shading*/
	  aurora_shade_shift (&fill, &fill, 1.25);
	  aurora_shade_shift (&colors->spot[2], &darker, 0.9);

	  clearlooks_rounded_rectangle (cr, fill_x, fill_y, fill_width+1, fill_height+1, radius, AUR_CORNER_ALL);

	  pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? 0 : trough_width, slider->horizontal ? trough_height : 0);
	  cairo_pattern_add_color_stop_rgba (pattern, 0.0, darker.r, darker.g, darker.b, 0.5);
	  cairo_pattern_add_color_stop_rgba (pattern, 1.0, fill.r, fill.g, fill.b, 0.5);
	  cairo_set_source (cr, pattern);
	  cairo_fill (cr);
	  cairo_pattern_destroy (pattern);
	  
	  //outline
	  clearlooks_rounded_rectangle (cr, fill_x+0.5, fill_y+0.5, fill_width, fill_height, radius, AUR_CORNER_ALL);
	  pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? 0 : trough_width, slider->horizontal ? trough_height : 0);
	  cairo_pattern_add_color_stop_rgba (pattern, 0.0, darker.r, darker.g, darker.b, 0.5);
	  cairo_pattern_add_color_stop_rgba (pattern, 0.0, darker.r, darker.g, darker.b, 0.08);
	  cairo_set_source (cr, pattern);
	  cairo_stroke (cr);
	  cairo_pattern_destroy (pattern);  
  }
		



  /* Draw the focused border */
  if (widget->focus) {
    cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.40);
    clearlooks_rounded_rectangle (cr, -0.5, -0.5, trough_width + 2, trough_height + 2, radius + 1, AUR_CORNER_ALL);
    cairo_stroke (cr);

    cairo_set_source_rgba (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.10);
    clearlooks_rounded_rectangle (cr, -1.5, -1.5, trough_width + 4, trough_height + 4, radius + 2, AUR_CORNER_ALL);
    cairo_stroke (cr);
  }
  else {
	fill = colors->bg[widget->state_type];
	aurora_shade (&fill, &darker, 0.616);
	  
	clearlooks_rounded_rectangle (cr, 0.5, 0.5, trough_width, trough_height, radius, AUR_CORNER_ALL);

	pattern = cairo_pattern_create_linear (0, 0, slider->horizontal ? 0 : trough_width, slider->horizontal ? trough_height : 0);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, darker.r, darker.g, darker.b, 0.3);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, darker.r, darker.g, darker.b, 0.18);
	cairo_set_source (cr, pattern);
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);

  }
	
}

/*
static void aurora_slider (cairo_t *cr, int x, int y, int width, int height, double radius) {
	
	cairo_translate(cr, x, y);
	
	clearlooks_rounded_rectangle (cr, 0, 0, width, height, radius, AUR_CORNER_ALL);
	
	cairo_move_to (cr, radius, 0);
	cairo_line_to (cr, width-radius, 0);
	cairo_arc     (cr, width-radius,radius,radius, 1.5*M_PI, 2*M_PI);
	cairo_line_to (cr, width, height - height/2);
	cairo_line_to (cr, width/2, height);
	cairo_line_to (cr, 0, height - height/2);
	cairo_arc     (cr, radius, radius, radius, M_PI, 1.5*M_PI);	
	
	cairo_translate(cr, -x, -y);
		
}*/


void
aurora_draw_slider_button (cairo_t * cr,
			   const AuroraColors * colors,
			   const WidgetParameters * widget,
			   const SliderParameters * slider,
			   int x, int y, int width, int height)
{
	cairo_save (cr);
		
	//inset used for drawing border, etching and focus
	int inset = 3;

	//Fill color, highlight color and border color
	AuroraHSB fill_hsb;
	AuroraRGB highlight;
	AuroraRGB border;
	cairo_pattern_t *pattern;

	//Move to x + 3, and y + 3 and draw interior of button
	cairo_translate (cr, x + inset, y + inset);
	width -= inset * 2;
	height -= inset * 2;
    

	aurora_hsb_from_color(&colors->bg[widget->state_type], &fill_hsb);	
	
	//Highlight
	aurora_shade_hsb (&fill_hsb, &highlight, 1.20);
    
	//border color a blend of bg color and button color
	aurora_mix_color (&colors->shade[8], &colors->bg[widget->state_type], 0.15, &border);
	aurora_tweak_saturation (&fill_hsb, &border);
	
	//Draw the fill
	AuroraRGB fill_darker, highlight_mid;

	aurora_shade_hsb (&fill_hsb, &fill_darker, 0.86);
	aurora_mix_color (&colors->bg[widget->state_type], &highlight, 0.3, &highlight_mid);
		
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill_darker.r, fill_darker.g, fill_darker.b);

	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, 0, 0, width, height, widget->curvature - 1, widget->corners);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);


	// add a 1px highlight along the left and top
	aurora_shade_hsb (&fill_hsb, &highlight, 1.3);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 1.0); 
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
	cairo_set_source (cr, pattern);

	clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature - 1, widget->corners);
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);

	//Restore to full button drawing bounds and draw border, shadow and focus
	cairo_translate (cr, -inset, -inset);
	width += inset * 2 - 1;
	height += inset * 2 - 1;

	//the shadow etching
	aurora_draw_shadow (cr, 1.5, 1.5, width - 2, height - 2, widget->curvature + 1, widget->corners , &border, 0.29);
	aurora_draw_shadow (cr, 0.5, 1.0, width, height - 0.5, widget->curvature + 2, widget->corners, &border, 0.09);
		
	//the border
	aurora_draw_border (cr, 2.5, 2.5, width - 4, height - 4, widget->curvature, widget->corners, &border, 1.20, 0.88);
	
	
	
	//Draw the focused border 
	if (widget->focus || widget->is_default) {
		AuroraRGB focus_color = colors->spot[1];

		if (widget->is_default && !widget->focus) {	
					aurora_mix_color (&colors->shade[3], &focus_color, 0.25, &focus_color);
		}
  
		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.35);
		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width - 4, height - 4, widget->curvature, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.40);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 2, height - 2, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.08);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width, height, widget->curvature + 2, widget->corners);
		cairo_stroke (cr);

	}
		
		//Draw the focused border 
	if (widget->prelight) {
		AuroraRGB focus_color = colors->spot[1];

		if (widget->is_default && !widget->focus) {	
			aurora_mix_color (&colors->shade[3], &focus_color, 0.25, &focus_color);
		}
  		
		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.08);
		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width - 4, height - 4, widget->curvature, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.05);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width-2, height-2, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);

	}		
		
		
  cairo_restore (cr);
  width ++;
  height++;

  aurora_shade_hsb (&fill_hsb, &highlight, 1.22);
		
  if (!slider->horizontal) {
    int tmp = height;

    height = width;
    width = tmp;

    rotate_mirror_translate (cr, M_PI / 2, x + .5, y + .5, FALSE, FALSE);
  }
  else {
    cairo_translate (cr, x + 0.5, y + 0.5);
  }


	
  int i;
  const unsigned int bar_spacing = 4;
  unsigned int bar_x = 6;
	
  int num_bars;
  int bar_height = MAX(height - bar_spacing - 4, 0);
  num_bars = (width - 2 * bar_x);
	
  //Adjust for inset
  cairo_translate (cr, 2, 2);
  height -= 4;
  width  -= 4;

  if (num_bars > 0) {
	num_bars /= (bar_spacing);

    for (i = 0; i < num_bars; i++) {
      cairo_move_to (cr, bar_x, bar_spacing - 1);
      cairo_line_to (cr, bar_x, bar_height);
	  cairo_set_source_rgba (cr, colors->shade[8].r, colors->shade[8].g, colors->shade[8].b, 0.80);
	  cairo_stroke (cr);

      cairo_move_to (cr, bar_x + 1, bar_spacing - 1);
      cairo_line_to (cr, bar_x + 1, bar_height);
      cairo_set_source_rgba (cr, highlight.r, highlight.g, highlight.b, 0.70);
      cairo_stroke (cr);
	  
      bar_x += bar_spacing;
    }
  }
}


void
aurora_draw_progressbar_trough (cairo_t * cr,
				const AuroraColors * colors,
				const WidgetParameters * widget,
				const ProgressBarParameters * progressbar,
				int x, int y, int width, int height)
{
  AuroraRGB bg_darker;
  cairo_pattern_t *pattern;

  aurora_mix_color (&colors->shade[8], &colors->spot[2], 0.20, &bg_darker);
	
  if (progressbar->orientation == AUR_ORIENTATION_LEFT_TO_RIGHT || progressbar->orientation == AUR_ORIENTATION_RIGHT_TO_LEFT) {
	  
	aurora_draw_shadow (cr, x+1.0, y+0.5, width - 2, height - 2, widget->curvature+2, widget->corners, &bg_darker, 0.22);
	//aurora_draw_shadow (cr, x+0.0, y+1.5, width, height - 2, widget->curvature+4, widget->corners, &bg_darker, 0.08);
	  
    pattern = cairo_pattern_create_linear (0, 0, 0, height);
    cairo_pattern_add_color_stop_rgba (pattern, 0.0, bg_darker.r, bg_darker.g, bg_darker.b, 0.28);
    cairo_pattern_add_color_stop_rgba (pattern, 1.0, bg_darker.r, bg_darker.g, bg_darker.b, 0.46);
    cairo_set_source (cr, pattern);
    cairo_rectangle (cr, x+1.5, y+2.5, width - 3, height - 5);
    cairo_stroke (cr);
    cairo_pattern_destroy (pattern);
  }
  else {
	aurora_draw_shadow (cr, x+2.0, y+1.5, width - 4, height - 2, widget->curvature+2, widget->corners, &bg_darker, 0.22);
	  
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
    cairo_pattern_add_color_stop_rgba (pattern, 0.0, bg_darker.r, bg_darker.g, bg_darker.b, 0.28);
    cairo_pattern_add_color_stop_rgba (pattern, 1.0, bg_darker.r, bg_darker.g, bg_darker.b, 0.46);
    cairo_set_source (cr, pattern);
    cairo_rectangle (cr, x+2.5, y+3.5, width - 5, height - 5);
    cairo_stroke (cr);
    cairo_pattern_destroy (pattern);
  }
	
  cairo_rectangle (cr, x+1.0, y+3.0, width-2, height-3);
	
  if (progressbar->orientation == AUR_ORIENTATION_LEFT_TO_RIGHT) {
    rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
  }
  else if (progressbar->orientation == AUR_ORIENTATION_RIGHT_TO_LEFT) {
    rotate_mirror_translate (cr, 0, x + width, y, TRUE, FALSE);
  }
  else {			//vertical swap width and height
    int tmp = height;

    height = width-1;
    width = tmp-2;
	y+=2;
	x++;
    if (progressbar->orientation == AUR_ORIENTATION_TOP_TO_BOTTOM) {
      rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
    }
    else {
      rotate_mirror_translate (cr, M_PI / 2, x, y + width, TRUE, FALSE);
    }
  }

  cairo_clip(cr);
	
  cairo_translate (cr, 1, 1);
  width -= 2;
  height -= 2;
	
  AuroraRGB highlight, highlight_mid;
  aurora_shade (&colors->bg[0], &bg_darker, 0.87);
  aurora_shade (&colors->bg[0], &highlight, 1.175);
	
  aurora_mix_color (&bg_darker, &highlight, 0.50, &highlight);		
  aurora_mix_color (&bg_darker, &highlight, 0.45, &highlight_mid);		

  pattern = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, highlight_mid.r, highlight_mid.g, highlight_mid.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, bg_darker.r, bg_darker.g, bg_darker.b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, colors->bg[0].r, colors->bg[0].g, colors->bg[0].b);

  cairo_set_source (cr, pattern);
  cairo_rectangle (cr, 1, 1, width - 2, height - 3);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);
	
}

void
aurora_draw_progressbar_fill (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const ProgressBarParameters * progressbar,
			      int x, int y, int width, int height,
			      gint offset)
{
  double tile_pos = 0;
  double stroke_width;
  int x_step;
	x++;
	y++;
	width-=2;
	if (progressbar->orientation == AUR_ORIENTATION_LEFT_TO_RIGHT || progressbar->orientation == AUR_ORIENTATION_RIGHT_TO_LEFT)
		height-=2;
	else
		height-=1;		
  cairo_rectangle (cr, x, y, width, height);


  if (progressbar->orientation == AUR_ORIENTATION_LEFT_TO_RIGHT) {
    rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
  }
  else if (progressbar->orientation == AUR_ORIENTATION_RIGHT_TO_LEFT) {
    rotate_mirror_translate (cr, 0, x + width, y, TRUE, FALSE);
  }
  else {			//vertical swap width and height
    int tmp = height;

    height = width;
    width = tmp;

    x = x + 1;
    height -= 2;

    if (progressbar->orientation == AUR_ORIENTATION_TOP_TO_BOTTOM) {
      rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
    }
    else {
      rotate_mirror_translate (cr, M_PI / 2, x, y + width, TRUE, FALSE);
    }
	y+=2;

  }
  //cairo_save (cr);
  
  cairo_clip (cr);

  stroke_width = height * 2;
  x_step = (stroke_width / 10.0) * offset;
  cairo_save (cr);

  AuroraRGB spot_darker, spot_lighter;

  aurora_shade_shift (&colors->spot[1], &spot_darker, 0.75);
  aurora_shade_shift (&colors->spot[1], &spot_lighter, 1.08);
  cairo_pattern_t *pattern;

  pattern = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, spot_darker.r, spot_darker.g, spot_darker.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.85, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, spot_lighter.r, spot_lighter.g, spot_lighter.b);
  cairo_rectangle (cr, 1, 1, width-2, height-2);
	
  cairo_set_source (cr, pattern);
  cairo_fill (cr);

  cairo_pattern_destroy (pattern);


  /* Draw strokes */
  while (tile_pos <= width + x_step - 2) {
    cairo_move_to (cr, stroke_width / 2 - x_step, 0);
    cairo_line_to (cr, stroke_width - x_step, 0);
    cairo_line_to (cr, stroke_width / 2 - x_step, height);
    cairo_line_to (cr, -x_step, height);

    cairo_translate (cr, stroke_width, 0);
    tile_pos += stroke_width;
  }
	
	cairo_set_source_rgb (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b);
	cairo_fill (cr);
	
  cairo_restore (cr);

  //Highlight
  AuroraRGB highlight;
  aurora_shade_shift (&colors->spot[0], &highlight, 1.7);

  pattern = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 0.55);
  cairo_pattern_add_color_stop_rgba (pattern, 0.5, highlight.r, highlight.g, highlight.b, 0.20);
  cairo_pattern_add_color_stop_rgba (pattern, 0.5, highlight.r, highlight.g, highlight.b, 0.0);
	
  cairo_set_source (cr, pattern);
  cairo_rectangle (cr, 1, 1, width-2, height-2);
  cairo_fill (cr);

  cairo_pattern_destroy (pattern);

  //aurora_draw_border (cr,  1.5, 0.5, width - 3, height - 1, widget->curvature+1, widget->corners, &colors->spot[2], 1.6, 1.25);
	  

	AuroraRGB border_darker, border_lighter;
	aurora_shade_shift (&colors->spot[2], &border_darker, 1.20);
	aurora_shade_shift (&colors->spot[2], &border_lighter, 1.8);
	aurora_scale_saturation(&border_lighter, 0.35);
	
	pattern = cairo_pattern_create_linear (0, 0.5, 0, height - 0.5);
	cairo_pattern_add_color_stop_rgb (pattern, 0.25, border_lighter.r, border_lighter.g, border_lighter.b); 
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, border_darker.r, border_darker.g, border_darker.b); 

	cairo_set_source (cr, pattern);
	cairo_rectangle (cr, 0.5, 0.5, width - 1, height - 1);
	
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);
	
	
	
}

void
aurora_draw_menubar (cairo_t * cr,
		     const AuroraColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height, int menubarstyle)
{
  cairo_translate (cr, x, y);
  cairo_rectangle (cr, 0, 0, width, height);

  //gradient menubar
  if (menubarstyle == 1) {
    cairo_pattern_t *pattern;
	AuroraRGB shade;
	aurora_shade_shift (&colors->bg[GTK_STATE_NORMAL], &shade, 0.96);
	  
    pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
    cairo_pattern_add_color_stop_rgb (pattern, 1.0, shade.r, shade.g, shade.b);
    cairo_set_source (cr, pattern);
    cairo_fill (cr);
    cairo_pattern_destroy (pattern);
  }
  else if (menubarstyle == 2) {
    cairo_pattern_t *pattern;
    pattern = cairo_pattern_create_linear (0, 0, 0, height);
    cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->shade[8].r, colors->shade[8].g, colors->shade[8].b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0 + 4.0/height, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, colors->shade[5].r, colors->shade[5].g, colors->shade[5].b);
    cairo_set_source (cr, pattern);
    cairo_fill (cr);
    cairo_pattern_destroy (pattern);	  
  }
  else {
    //flat menubar
    cairo_set_source_rgb (cr, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
    cairo_fill (cr);
  }

  /* Draw bottom line */

  cairo_move_to (cr, 0, height - 0.5);
  cairo_line_to (cr, width, height - 0.5);

	AuroraRGB shadow;
	aurora_mix_color (&colors->shade[3], &colors->shade[4], 0.5, &shadow);
	cairo_set_source_rgb (cr, shadow.r, shadow.g, shadow.b);
  cairo_stroke (cr);
}

static void
aurora_get_frame_gap_clip (int x, int y, int width, int height,
			   FrameParameters * frame,
			   AuroraRectangle * bevel, AuroraRectangle * border)
{
  if (frame->gap_side == AUR_GAP_TOP) {
    AURORA_RECTANGLE_SET ((*bevel),  2.5 + frame->gap_x, -0.5, frame->gap_width - 3, 4);
    AURORA_RECTANGLE_SET ((*border),       frame->gap_x, -0.5, frame->gap_width-3,     3);
  }
  else if (frame->gap_side == AUR_GAP_BOTTOM) {
    AURORA_RECTANGLE_SET ((*bevel),  1.5 + frame->gap_x - 3, height - 2.5, frame->gap_width - 3, 2.0);
    AURORA_RECTANGLE_SET ((*border), 0.5 + frame->gap_x - 1, height - 2.5, frame->gap_width,     2.0);
  }
  else if (frame->gap_side == AUR_GAP_LEFT) {
    AURORA_RECTANGLE_SET ((*bevel), -0.5,  1.5 + frame->gap_x, 2.0, frame->gap_width - 3);
    AURORA_RECTANGLE_SET ((*border), 0.5, -0.5 + frame->gap_x, 2.0, frame->gap_width);
  }
  else if (frame->gap_side == AUR_GAP_RIGHT) {
    AURORA_RECTANGLE_SET ((*bevel),  width - 2.5,  1.5 + frame->gap_x, 2.0, frame->gap_width - 3);
    AURORA_RECTANGLE_SET ((*border), width - 3.5, -0.5 + frame->gap_x, 2.0, frame->gap_width);
  }
}


void
aurora_draw_frame (cairo_t * cr,
		   const AuroraColors * colors,
		   const WidgetParameters * widget,
		   const FrameParameters * frame,
		   int x, int y, int width, int height)
{

	AuroraRGB *border = frame->border;
	AuroraRGB highlight, shadow;
	AuroraRectangle bevel_clip = {0, 0, 0, 0};
	AuroraRectangle frame_clip = {0, 0, 0, 0};
  
  
	double curvature = MIN (widget->curvature, (int)MIN ((width - 2.0) / 2.0, (height - 2.0) / 2.0)); 
	boolean shadow_in = frame->shadow == AUR_SHADOW_IN || frame->shadow == AUR_SHADOW_ETCHED_IN;
	
	if (frame->gap_x != -1)
		aurora_get_frame_gap_clip (x, y, width, height, (FrameParameters *) frame, &bevel_clip, &frame_clip);

	cairo_translate (cr, x + 0.5, y + 0.5);
	
	if (frame->fill_bg) {
		AuroraRGB bg = colors->bg[GTK_STATE_NORMAL];
		if (shadow_in)
			aurora_shade(&colors->bg[GTK_STATE_NORMAL], &bg, 0.97); 
		else {
			if (!frame->use_fill) 
				aurora_shade(&colors->bg[GTK_STATE_NORMAL], &bg, 1.04);
		}
		cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
		clearlooks_rounded_rectangle (cr, 1.5, 1.5, width-3, height-3, curvature-1, widget->corners);
		cairo_fill (cr);
	}
	
	// Set clip for the frame 
	cairo_reset_clip (cr);
	if (frame->gap_x != -1 && !frame->use_fill) {
		// Set clip for gap
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);		
		cairo_rectangle (cr, -0.5, -0.5, width, height);
		cairo_rectangle (cr, bevel_clip.x, bevel_clip.y, bevel_clip.width, bevel_clip.height); //for other frames
		cairo_clip (cr);
		cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);
	}

	// Draw frame
	if (shadow_in) {

		aurora_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.065);
		aurora_shade (&colors->shade[3], &shadow, 0.80);
	  
		aurora_draw_etched_border (cr, 1, 1, width - 3, height - 3, widget->curvature, widget->corners, &colors->shade[3], 1.20, 0.88);	  
    	
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.23);
		clearlooks_rounded_rectangle (cr, 1.5, 2, width - 4, height - 4, curvature - 0.5, widget->corners);
		cairo_stroke (cr);
    
		/*Bottom Highlight*/
		cairo_pattern_t *pattern;
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, highlight.r, highlight.g, highlight.b);
		cairo_set_source (cr, pattern);
    
		clearlooks_rounded_rectangle (cr, 0, 0, width - 1, height - 1, curvature + 1, widget->corners);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}
	else {//shadow out

		aurora_shade (border, &shadow, 0.85);
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.3);
		clearlooks_rounded_rectangle (cr, 0.5, 1, width - 2 , height - 2, curvature + 1, widget->corners);
		cairo_stroke (cr);

		aurora_draw_border (cr, 1, 1, width - 3, height - 3, widget->curvature, widget->corners, &colors->shade[4], frame->use_fill ? 1.00 : 1.08, 0.88);	  

		//Highlight
		aurora_shade (&colors->bg[GTK_STATE_NORMAL], &highlight, 1.07);
		
		cairo_pattern_t *pattern;
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 0.75);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
		cairo_set_source (cr, pattern);
		
		clearlooks_rounded_rectangle (cr, 2, 2, width - 5, height - 5, curvature - 1, widget->corners);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}
  
}


void
aurora_draw_tab_no_border (cairo_t * cr,
		 const AuroraColors * colors,
		 const WidgetParameters * widget,
		 const TabParameters * tab,
		 int x, int y, int width, int height)
{

	AuroraRGB *fill;
	AuroraRGB *border1;
	AuroraRGB highlight;
	
	AuroraCorners corners =  AUR_CORNER_ALL;
	cairo_pattern_t *pattern;
	if (tab->gap_side == AUR_GAP_TOP || widget->active)
		border1 = (AuroraRGB *) &colors->shade[5];
	else
		border1 = (AuroraRGB *) &colors->shade[4];
	
	fill = (AuroraRGB *) & colors->bg[widget->state_type];

	switch(tab->gap_side) {
		case GTK_POS_TOP:
			corners = AUR_CORNER_BOTTOMLEFT | AUR_CORNER_BOTTOMRIGHT;
		break;
		case GTK_POS_LEFT:
			corners = AUR_CORNER_BOTTOMRIGHT | AUR_CORNER_TOPRIGHT;
		break;
		case GTK_POS_RIGHT:
			corners = AUR_CORNER_BOTTOMLEFT | AUR_CORNER_TOPLEFT;
		break;
			case GTK_POS_BOTTOM:
			corners =  AUR_CORNER_TOPLEFT | AUR_CORNER_TOPRIGHT;
		break;
	}
	
	/* Make the tabs slightly bigger than they should be, to create a gap */
	if (tab->gap_side == AUR_GAP_TOP || tab->gap_side == AUR_GAP_BOTTOM) {
		x++;
		width -= 2;
		height--;

		if (tab->gap_side == AUR_GAP_TOP) {
			rotate_mirror_translate (cr, 0, x + 0.5, y + 0.5, FALSE, TRUE);
			cairo_translate (cr, 1.0, -height + 3);	/* gap at the other side */
		}
		else {
			cairo_translate (cr, x + 1.5, y + 1.5);
		}
  	}
	else {
		//width += 1;
		y++;
		height -= 2;
		width--;

		if (tab->gap_side == AUR_GAP_LEFT) {
			int tmp = width;

			width = height;
			height = tmp;

			rotate_mirror_translate (cr, M_PI * 1.5, x + 0.5, y + 0.5, TRUE, FALSE);
			cairo_translate (cr, 1, 3 - height);	/* gap at the other side */
		}
		else {
			int tmp = width;

			width = height;
			height = tmp;

			rotate_mirror_translate (cr, M_PI * 0.5, x + 0.5, y + 0.5, FALSE, FALSE);
			cairo_translate (cr, 1.0, 1.0);	/* gap at the other side */
		}
	}

	if (!widget->active)
		height++;
	
	clearlooks_rounded_rectangle (cr, -0.5, -0.5, width, height-1, widget->curvature, corners);
	cairo_clip(cr);

	
    // Draw Fill and shade
	if (widget->active) {
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 2, height-1, widget->curvature, corners);
	    aurora_shade (fill, &highlight, 1.06);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill->r, fill->g, fill->b);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
	
		cairo_fill (cr);
	}
	else {
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 2, height-1, widget->curvature, corners);
		
		AuroraRGB stripe_fill;
		aurora_shade (fill, &stripe_fill, 1.08);
		aurora_mix_color(&stripe_fill, &colors->spot[0], 0.10, &stripe_fill);

		pattern = cairo_pattern_create_linear (0, 0, 0, height-2);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, stripe_fill.r, stripe_fill.g, stripe_fill.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill->r, fill->g, fill->b);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
	  
		cairo_fill (cr);
		
		/*shadow */
		AuroraRGB shadow;
		aurora_shade (border1, &shadow, 0.88);

		//Draw shadow

		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.175);

		if (tab->gap_side == AUR_GAP_LEFT || tab->gap_side == AUR_GAP_RIGHT)
			clearlooks_rounded_rectangle (cr, 0, -1, width, height, widget->curvature + 1, corners);
		else
			clearlooks_rounded_rectangle (cr, -1.0, -1, width+1, height, widget->curvature + 1, corners);
		
		cairo_stroke (cr);
		
	}

	//border
	clearlooks_rounded_rectangle (cr, 0, 0, width - 1, height - 1, widget->curvature, corners);

	if (widget->active) {
		AuroraRGB shadowing;
		aurora_shade (border1, &shadowing, 0.82);
		
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, border1->r, border1->g, border1->b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, shadowing.r, shadowing.g, shadowing.b);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
	  	cairo_pattern_destroy (pattern);

	  		
		cairo_move_to (cr, 0.5, height-2);
		cairo_line_to (cr, width - 1, height-2);
		cairo_set_source_rgba (cr, shadowing.r, shadowing.g, shadowing.b, 0.12);
		cairo_stroke (cr);
	}
	else {
	  
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, border1->r, border1->g, border1->b);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);


		/*Highlight */
		aurora_shade (fill, &highlight,  widget->focus ? 1.2 : 1.1);
		
  		aurora_mix_color(&highlight, &colors->spot[0], 0.10, &highlight);
		
		clearlooks_rounded_rectangle (cr, 1, 1, width - 3, height-2, widget->curvature-1, corners);
	
		pattern = cairo_pattern_create_linear (0, 0, width, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 0.30);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, highlight.r, highlight.g, highlight.b, 0.60);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.30);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
		
		cairo_stroke (cr);

		if (widget->focus) {
			pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.30);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.0);
			cairo_set_source (cr, pattern);
			clearlooks_rounded_rectangle (cr, -1, -1, width + 1, height , widget->curvature + 1, corners);
			cairo_pattern_destroy (pattern);
			cairo_stroke (cr);
		}
	}
}


void
aurora_draw_tab (cairo_t * cr,
		 const AuroraColors * colors,
		 const WidgetParameters * widget,
		 const TabParameters * tab,
		 int x, int y, int width, int height)
{

	AuroraRGB *fill;
	AuroraRGB *border1;
	AuroraRGB highlight;
	
	cairo_pattern_t *pattern;
	if (tab->gap_side == AUR_GAP_TOP || widget->active)
		border1 = (AuroraRGB *) &colors->shade[5];
	else
		border1 = (AuroraRGB *) &colors->shade[4];
	
	fill = (AuroraRGB *) &colors->bg[widget->state_type];

	/* Make the tabs slightly bigger than they should be, to create a gap */
	if (tab->gap_side == AUR_GAP_TOP || tab->gap_side == AUR_GAP_BOTTOM) {
		height += 1;
		width -= 2;

		if (tab->gap_side == AUR_GAP_TOP) {
			rotate_mirror_translate (cr, 0, x + 0.5, y + 0.5, FALSE, TRUE);
			cairo_translate (cr, 1.0, -height + 3);	/* gap at the other side */
		}
		else {
			cairo_translate (cr, x + 1.5, y + 1.5);
		}
  	}
	else {
		width += 1;
		height -= 2;

		if (tab->gap_side == AUR_GAP_LEFT) {
			int tmp = width;

			width = height;
			height = tmp;

			rotate_mirror_translate (cr, M_PI * 1.5, x + 0.5, y + 0.5, TRUE, FALSE);
			cairo_translate (cr, 1, 3 - height);	/* gap at the other side */
		}
		else {
			int tmp = width;

			width = height;
			height = tmp;

			rotate_mirror_translate (cr, M_PI * 0.5, x + 0.5, y + 0.5, FALSE, FALSE);
			cairo_translate (cr, 1.0, 1.0);	/* gap at the other side */
		}
	}

	boolean first_tab = tab->first_tab;
	boolean last_tab = tab->last_tab;
	
    // Draw Fill and shade
	if (widget->active) {
		cairo_rectangle (cr, -0.5, -0.5, width, height-1);
		cairo_clip(cr);
		
		aurora_tab_rectangle (cr, 0.5, 0.5, width - 2, height, widget->curvature, first_tab, last_tab, widget->active);
	    aurora_shade (fill, &highlight, 1.06);

		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill->r, fill->g, fill->b);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
	
		cairo_fill (cr);
	}
	else {
		aurora_tab_rectangle (cr, 0.5, 0.5, width - 2, height, widget->curvature, first_tab, last_tab, widget->active);
		
		AuroraRGB stripe_fill;
		aurora_shade (fill, &stripe_fill, 1.08);
		aurora_mix_color(&stripe_fill, &colors->spot[0], 0.10, &stripe_fill);

		pattern = cairo_pattern_create_linear (0, 0, 0, height-2);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, stripe_fill.r, stripe_fill.g, stripe_fill.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill->r, fill->g, fill->b);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
	  
		cairo_fill (cr);
		
		/*shadow */
		AuroraRGB shadow;
		aurora_shade (border1, &shadow, 0.88);
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.175);

		if (tab->gap_side == AUR_GAP_LEFT || tab->gap_side == AUR_GAP_RIGHT)
			aurora_tab_rectangle (cr, 0, -1, width, height, widget->curvature + 1, first_tab, last_tab, widget->active);
		else
			aurora_tab_rectangle (cr, -1.0, -1, width+1, height, widget->curvature + 1, first_tab, last_tab, widget->active);
		
		cairo_stroke (cr);
		
	}

	//border
	aurora_tab_rectangle (cr, 0, 0, width - 1, height - 1, widget->curvature, first_tab, last_tab, widget->active);

	if (widget->active) {
		AuroraRGB shadowing;
		aurora_shade (border1, &shadowing, 0.82);
		
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, border1->r, border1->g, border1->b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, shadowing.r, shadowing.g, shadowing.b);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
	  	cairo_pattern_destroy (pattern);

	  		
		cairo_move_to (cr, 0.5, height-2);
		cairo_line_to (cr, width - 1, height-2);
		cairo_set_source_rgba (cr, shadowing.r, shadowing.g, shadowing.b, 0.12);
		cairo_stroke (cr);
	  
	}
	else {	  
		pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, border1->r, border1->g, border1->b);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);


		/*Highlight */
		aurora_shade (fill, &highlight,  widget->focus ? 1.2 : 1.1);
  		aurora_mix_color(&highlight, &colors->spot[0], 0.10, &highlight);
		
		aurora_tab_rectangle (cr, 1, 1, width - 3, height-2, widget->curvature-1, first_tab, last_tab, widget->active);
	
		pattern = cairo_pattern_create_linear (0, 0, width, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 0.30);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, highlight.r, highlight.g, highlight.b, 0.60);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.30);
		cairo_set_source (cr, pattern);
		cairo_pattern_destroy (pattern);
		
		cairo_stroke (cr);

		if (widget->focus) {
			pattern = cairo_pattern_create_linear (0, 0, 0, height-1);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.30);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b, 0.0);
			cairo_set_source (cr, pattern);
			aurora_tab_rectangle (cr, -1, -1, width + 1, height , widget->curvature + 1, first_tab, last_tab, widget->active);
			cairo_pattern_destroy (pattern);
			cairo_stroke (cr);
		}
	}
}

void
aurora_draw_separator (cairo_t * cr,
		       const AuroraColors * colors,
		       const WidgetParameters * widget,
		       const SeparatorParameters * separator,
		       int x, int y, int width, int height)
{

  AuroraRGB dark = colors->bg[widget->state_type];
  AuroraRGB light;

  aurora_shade (&dark, &light, 1.15);
  aurora_shade (&dark, &dark, 0.68);


  if (separator->horizontal) {
    cairo_translate (cr, x + 0.5, y + 0.5);

    cairo_move_to (cr, 0.0, 0.0);
    cairo_line_to (cr, width, 0.0);
    cairo_set_source_rgba (cr, dark.r, dark.g, dark.b, 0.6);
    cairo_stroke (cr);

    cairo_move_to (cr, 0.0, 1.0);
    cairo_line_to (cr, width, 1.0);
    cairo_set_source_rgba (cr, light.r, light.g, light.b, 0.6);
    cairo_stroke (cr);
  }
  else {
    cairo_translate (cr, x + 0.5, y + 0.5);
    
    cairo_move_to (cr, 0.0, 0.0);
    cairo_line_to (cr, 0.0, height);
    cairo_set_source_rgba (cr, dark.r, dark.g, dark.b, 0.6);
    cairo_stroke (cr);

    cairo_move_to (cr, 1.0, 0.0);
    cairo_line_to (cr, 1.0, height);
    cairo_set_source_rgba (cr, light.r, light.g, light.b, 0.6);
    cairo_stroke (cr);
  }
}

void
aurora_draw_list_view_header (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const ListViewHeaderParameters * header,
			      int x, int y, int width, int height)
{
	AuroraRGB fill;

	if (header->sorted) {
		fill = colors->bg[GTK_STATE_ACTIVE];
		//If prelight match the sorted row prelight brightness with that of normal prelight 
		if (widget->state_type == GTK_STATE_PRELIGHT) {
			//Increase lightness for sorted prelight by the difference between prelight and nomral lightness
			double prelight_lightness = aurora_get_lightness (&colors->bg[GTK_STATE_PRELIGHT]);
			double normal_lightness = aurora_get_lightness (&colors->bg[GTK_STATE_NORMAL]);
			aurora_set_lightness (&fill, aurora_get_lightness(&fill) + prelight_lightness - normal_lightness);
		}
	}
	else {
		fill = colors->bg[widget->state_type];
	}
#ifdef HAVE_ANIMATION
	AuroraRGB fill_old = colors->bg[widget->prev_state_type];

	if (header->sorted && widget->prev_state_type != GTK_STATE_PRELIGHT)
		fill_old = colors->bg[GTK_STATE_ACTIVE];

	if (header->sorted && widget->prev_state_type == GTK_STATE_PRELIGHT) {
		fill_old = colors->bg[GTK_STATE_ACTIVE];
		//Increase lightness for sorted prelight by the difference between prelight and nomral lightness
		double prelight_lightness = aurora_get_lightness (&colors->bg[GTK_STATE_PRELIGHT]);
		double normal_lightness = aurora_get_lightness (&colors->bg[GTK_STATE_NORMAL]);
		aurora_set_lightness (&fill_old, aurora_get_lightness(&fill_old) + prelight_lightness - normal_lightness);
		
		aurora_match_lightness (&colors->bg[GTK_STATE_PRELIGHT], &fill_old);
	}
	aurora_mix_color (&fill_old, &fill, widget->trans, &fill);
#endif
	AuroraRGB *border = (AuroraRGB *) & colors->shade[5];

	cairo_translate (cr, x, y);

	//Draw the fill
	AuroraRGB fill_darker, highlight, highlight_mid;

	aurora_shade_shift (&fill, &highlight, 1.25);
	aurora_shade_shift (&fill, &fill_darker, 0.95);
	aurora_mix_color (&fill_darker, &highlight, 0.35, &highlight_mid);

	
	cairo_pattern_t *pattern;
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, highlight_mid.r, highlight_mid.g, highlight_mid.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, fill.r, fill.g, fill.b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill_darker.r, fill_darker.g, fill_darker.b);
	cairo_set_source (cr, pattern);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);
				
	//Highlight
	cairo_move_to (cr, 0.0, 0.5);
	cairo_line_to (cr, width, 0.5);

	if (header->order == AUR_ORDER_LAST) {
		cairo_move_to (cr, width, 0.5);
		cairo_line_to (cr, width, height - 1);
	}
     
    aurora_shade_shift (&highlight, &highlight, 1.05);
	cairo_set_source_rgba (cr, highlight.r, highlight.g, highlight.b, 0.75);
	cairo_stroke (cr);

		
	/* Draw bottom border */
	cairo_move_to (cr, 0.0, height - 0.5);
	cairo_line_to (cr, width, height - 0.5);
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke (cr);
  
	// Draw separator and resize grip 
	AuroraRGB separator;

	if (((header->order == AUR_ORDER_LAST) ||
		(header->order == AUR_ORDER_FIRST_AND_LAST))
		&& header->resizable) {
		aurora_shade (border, &separator, 0.45);
		cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.62);
		cairo_move_to (cr, width -1.5, 3.5);
		cairo_line_to (cr, width -1.5,  height - 4.5);
		cairo_stroke (cr);
			
		aurora_shade (&fill, &separator, 1.35);
		cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.25);
		cairo_move_to (cr, 0.5, 0.5);
		cairo_line_to (cr, 0.5,  height - 1.5);
			
		cairo_move_to (cr, width -0.5, 0.5);
		cairo_line_to (cr, width -0.5,  height - 1.5);
		cairo_stroke (cr);	
	}
	else if (header->order == AUR_ORDER_FIRST_AND_LAST) {}
	else {
		if (header->order != AUR_ORDER_LAST) {
			aurora_shade (border, &separator, 0.42);

			pattern = cairo_pattern_create_linear (0, 0, 0, height);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, separator.r, separator.g, separator.b, 0.35);
			cairo_pattern_add_color_stop_rgba (pattern, 0.3, separator.r, separator.g, separator.b, 0.65);
			cairo_pattern_add_color_stop_rgba (pattern, 0.6, separator.r, separator.g, separator.b, 0.65);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, separator.r, separator.g, separator.b, 0.25);
			cairo_set_source (cr, pattern);
			//cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.52);     
			cairo_move_to (cr, width -0.5, 0.5);
			cairo_line_to (cr, width -0.5,  height - 1.5);
			cairo_stroke (cr);
			cairo_pattern_destroy (pattern);
			
			aurora_shade (&fill, &separator, 1.35);
			cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.25);
			cairo_move_to (cr, width -1.5, 0.5);
			cairo_line_to (cr, width -1.5,  height - 1.5);
			cairo_stroke (cr);
			
		}
		if (header->order != AUR_ORDER_FIRST) {
			aurora_shade (&fill, &separator, 1.35);
			cairo_set_source_rgba (cr, separator.r, separator.g, separator.b, 0.25);
			cairo_move_to (cr, 0.5, 0.5);
			cairo_line_to (cr, 0.5,  height - 1.5);
			cairo_stroke (cr);
		}
	}
}

/* We can't draw transparent things here, since it will be called on the same
 * surface multiple times, when placed on a handlebox_bin or dockitem_bin */
void
aurora_draw_toolbar (cairo_t * cr,
		     const AuroraColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height)
{
 AuroraRGB *fill = (AuroraRGB *) &colors->bg[0];
	AuroraRGB *highlight = (AuroraRGB *) &colors->shade[1];
	AuroraRGB shadow, highlight_mid;
	
  aurora_mix_color (fill, highlight, 0.35, &highlight_mid);
  aurora_mix_color (fill, &colors->shade[3], 0.205, &shadow);
		
  cairo_translate (cr, x, y);

  cairo_pattern_t *pattern;
  pattern = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight->r, highlight->g, highlight->b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, highlight_mid.r, highlight_mid.g, highlight_mid.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, fill->r, fill->g, fill->b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, shadow.r, shadow.g, shadow.b);	
  cairo_set_source (cr, pattern);
  cairo_rectangle (cr, 0, 0, width, height);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);


  AuroraRGB stroke_highlight;
  aurora_shade (fill, &stroke_highlight, 1.08);
  /* Draw highlight */
  cairo_move_to (cr, 0, 0.5);
  cairo_line_to (cr, width, 0.5);
  cairo_set_source_rgb (cr, stroke_highlight.r, stroke_highlight.g, stroke_highlight.b);
  cairo_stroke (cr);

  /* Draw shadow */
  cairo_move_to (cr, 0, height - 0.5);
  cairo_line_to (cr, width, height - 0.5);
  cairo_set_source_rgb (cr, colors->shade[4].r, colors->shade[4].g, colors->shade[4].b);
  cairo_stroke (cr);
	
}

void
aurora_draw_menubar_item (cairo_t * cr,
		      const AuroraColors * colors,
		      const WidgetParameters * widget,
		      int x, int y, int width, int height)
{
	AuroraRGB fill_shade, fill_light, fill_light_mid;
	AuroraRGB *fill = (AuroraRGB *) & colors->spot[1];  
	AuroraRGB border = colors->spot[1];
	cairo_pattern_t *pattern;

	aurora_shade_shift (&border, &border, 0.75);
	aurora_shade_shift (fill, &fill_shade, 0.88);
	aurora_shade_shift (fill, &fill_light, 1.1);
	aurora_mix_color (fill, &fill_light, 0.10, &fill_light_mid);
		
	cairo_translate (cr, x, y);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, fill_light.r, fill_light.g, fill_light.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, fill_light_mid.r, fill_light_mid.g, fill_light_mid.b);
	cairo_pattern_add_color_stop_rgb (pattern, 0.5, fill->r, fill->g, fill->b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, fill_shade.r, fill_shade.g, fill_shade.b);
	cairo_set_source (cr, pattern);

	clearlooks_rounded_rectangle (cr, 1, 1, width - 2, height - 2, widget->curvature - 1, widget->corners);
	cairo_fill (cr);
	cairo_pattern_destroy (pattern);

	aurora_shade_shift (fill, &fill_light, 1.2);

	AuroraRGB border_lighter;
	aurora_shade_shift (&border, &border_lighter, 1.15);

	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, border_lighter.r, border_lighter.g, border_lighter.b); 
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, border.r, border.g, border.b); 

	cairo_set_source (cr, pattern);
	clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature, widget->corners);
	
	cairo_stroke (cr);
	cairo_pattern_destroy (pattern);


	aurora_shade_shift (fill, &fill_light, 1.20);

	pattern = cairo_pattern_create_linear (0, 0, 0, height / 1.60);
	cairo_pattern_add_color_stop_rgb  (pattern, 0.0, fill_light.r, fill_light.g, fill_light.b);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, fill_light.r, fill_light.g, fill_light.b, 0.0);

	cairo_set_source (cr, pattern);

	clearlooks_rounded_rectangle (cr, 1.5, 1.5, width - 3, height - 3, widget->curvature - 1, widget->corners);
	cairo_stroke (cr);

	cairo_pattern_destroy (pattern);
}


void
aurora_draw_menuitem (cairo_t * cr,
		      const AuroraColors * colors,
		      const WidgetParameters * widget,
		      int x, int y, int width, int height)
{
	list_menu_gradient(cr, &colors->spot[1], x, y, width, height);
}



void
aurora_draw_scrollbar_trough (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const ScrollBarParameters * scrollbar,
			      int x, int y, int width, int height)
{
  AuroraRGB bg_shadow,bg_darker,bg_mid;
  AuroraHSB fill_hsb;
  aurora_hsb_from_color(&colors->bg[widget->state_type], &fill_hsb);
	x--;y--;height+=2;width+=2;
	
  if (scrollbar->horizontal) {
    int tmp = height;

    height = width;
    width = tmp;

    rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
  }
  else {
    cairo_translate (cr, x, y);
  }

  aurora_shade_hsb (&fill_hsb, &bg_darker, 0.97);
  aurora_shade_hsb (&fill_hsb, &bg_shadow, 0.78);
  aurora_mix_color (&bg_shadow, &bg_darker, 0.85, &bg_mid);
	
  //fill
  cairo_pattern_t *pattern;

  pattern = cairo_pattern_create_linear (1, 0, width-2, 0);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, bg_shadow.r, bg_shadow.g, bg_shadow.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.56, bg_mid.r, bg_mid.g, bg_mid.b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, bg_darker.r, bg_darker.g, bg_darker.b);
  cairo_set_source (cr, pattern);

  cairo_rectangle (cr, 1, 1, width - 2, height - 2);
  cairo_fill_preserve (cr);
  cairo_pattern_destroy (pattern);

    
  //upper shadow 
  pattern = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgb  (pattern, 0.0, bg_shadow.r, bg_shadow.g, bg_shadow.b);
  cairo_pattern_add_color_stop_rgba (pattern, width/(4.0*height), bg_darker.r, bg_darker.g, bg_darker.b, 0.0);
  cairo_pattern_add_color_stop_rgba (pattern, 1-width/(4.0*height), bg_darker.r, bg_darker.g, bg_darker.b, 0.0);
  cairo_pattern_add_color_stop_rgba (pattern, 1.0, bg_shadow.r, bg_shadow.g, bg_shadow.b, 1.0);
  cairo_set_source (cr, pattern);   

  cairo_fill(cr);
  cairo_pattern_destroy (pattern);
}


void
aurora_draw_scrollbar_slider (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const ScrollBarParameters * scrollbar,
			      int x, int y, int width, int height)
{

  AuroraHSB bg_hsb;
  AuroraRGB bg_lighter, bg_mid;
  AuroraRGB border;

  aurora_hsb_from_color(&colors->bg[widget->state_type], &bg_hsb);
  aurora_shade_hsb (&bg_hsb, &border, 0.64);
  aurora_shade_shift_hsb (&bg_hsb, &bg_lighter, 1.2);
	
  //begin drawing
  if (scrollbar->horizontal) {
    y++;
    height--;
    cairo_translate (cr, x, y);
  }
  else {
    x++;
    width--;

    int tmp = height;

    height = width;
    width = tmp;

    rotate_mirror_translate (cr, M_PI / 2, x, y, FALSE, FALSE);
	  
  }

	
  cairo_pattern_t *pattern;
  pattern = cairo_pattern_create_linear (0, 0, width, 0);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, bg_lighter.r, bg_lighter.g, bg_lighter.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, colors->bg[widget->state_type].r, colors->bg[widget->state_type].g, colors->bg[widget->state_type].b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, bg_lighter.r, bg_lighter.g, bg_lighter.b);
  cairo_set_source (cr, pattern);
  clearlooks_rounded_rectangle (cr, 1, 1, width - 2, height - 2, widget->curvature-1, widget->corners);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);

  aurora_shade_shift_hsb (&bg_hsb, &bg_lighter, 1.15);
  aurora_mix_color (&colors->bg[widget->state_type], &bg_lighter, 0.50, &bg_mid);
	
  pattern = cairo_pattern_create_linear (0, 0, 0, height+1);
  cairo_pattern_add_color_stop_rgb  (pattern, 0.0, bg_lighter.r, bg_lighter.g, bg_lighter.b);
  cairo_pattern_add_color_stop_rgba (pattern, 0.45, bg_mid.r, bg_mid.g, bg_mid.b, 0.3);
  cairo_pattern_add_color_stop_rgba (pattern, 0.50, colors->bg[widget->state_type].r, colors->bg[widget->state_type].g, colors->bg[widget->state_type].b, 0.42);
  cairo_pattern_add_color_stop_rgba (pattern, 1.0, border.r, border.g, border.b, 0.45);

  cairo_set_source (cr, pattern);
  clearlooks_rounded_rectangle (cr, 1, 1, width - 2, height - 2, widget->curvature-1, widget->corners);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);

  // Draw border 
  AuroraRGB border_lighter;
  aurora_shade_shift_hsb (&bg_hsb, &border, 0.44);
  aurora_shade_shift_hsb (&bg_hsb, &border_lighter, 0.60);
	
  pattern = cairo_pattern_create_linear (0, 0, width, 0);
  cairo_pattern_add_color_stop_rgb (pattern, 0.0, border_lighter.r, border_lighter.g, border_lighter.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.5, border.r, border.g, border.b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, border_lighter.r, border_lighter.g, border_lighter.b);
  cairo_set_source (cr, pattern);
  clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature, widget->corners);	
  cairo_stroke (cr);
  cairo_pattern_destroy (pattern);
	
	//Draw the prelight border 
	if (widget->prelight) {
		AuroraRGB focus_color = colors->spot[1];
  		
		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.08);
		clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, widget->curvature, widget->corners);
		cairo_stroke (cr);

		cairo_set_source_rgba (cr, focus_color.r, focus_color.g, focus_color.b, 0.05);
		clearlooks_rounded_rectangle (cr, -0.5, -0.5, width + 1, height + 1, widget->curvature + 1, widget->corners);
		cairo_stroke (cr);
	}	
	
  //grips

  const int NUM_OF_BARS = 3;
  int bar_x = width / 2 - (NUM_OF_BARS - 1) * 2;

  cairo_translate (cr, 0.5, 0.5);
  int i;
		
  aurora_shade_hsb (&bg_hsb, &border, 0.54);
		
  for (i = 0; i < NUM_OF_BARS; i++) {
    cairo_move_to (cr, bar_x, 4.5);
    cairo_line_to (cr, bar_x, height - 5.5);
    cairo_set_source_rgb (cr, border.r, border.g, border.b);
    cairo_stroke (cr);

    cairo_move_to (cr, bar_x + 1, 4.5);
    cairo_line_to (cr, bar_x + 1, height - 5.5);
    cairo_set_source_rgb (cr, bg_lighter.r, bg_lighter.g, bg_lighter.b);
    cairo_stroke (cr);

    bar_x += 4;
  }

}

void
aurora_draw_menu_frame (cairo_t * cr,
			const AuroraColors * colors,
			const WidgetParameters * widget,
			int x, int y, int width, int height)
{
  AuroraRGB border;
  aurora_shade (&colors->bg[GTK_STATE_NORMAL], &border, 0.68);
	
  cairo_translate (cr, x, y);
  cairo_rectangle (cr, 0.5, 0.5, width - 1, height - 1);
  cairo_set_source_rgb (cr, border.r, border.g, border.b);
  cairo_stroke (cr);
    	
}

void
aurora_draw_handle (cairo_t * cr,
		    const AuroraColors * colors,
		    const WidgetParameters * widget,
		    const HandleParameters * handle,
		    int x, int y, int width, int height)
{

  AuroraRGB dark;
  AuroraRGB light = colors->shade[2];
  aurora_mix_color (&colors->shade[5], &colors->shade[6], 0.6, &dark);
  int bar_height;
  int bar_width = 5;
  int i, bar_y = 1;
  int num_bars, bar_spacing;

  num_bars = 3;
  bar_spacing = 3;
  bar_height = num_bars * bar_spacing;

  aurora_shade (&light, &light, 1.15);
	
  if (handle->horizontal) {
    int tmp = height;

    rotate_mirror_translate (cr, M_PI / 2, x + width / 2 - bar_height / 2.0, y + height / 2 - bar_width / 2.0, FALSE, FALSE);
    height = width;
    width = tmp;
  }
  else {
    cairo_translate (cr, x + width / 2 - bar_width / 2.0, y + height / 2 - bar_height / 2.0);
  }

  //gradient arround center
  if (handle->type == AUR_HANDLE_SPLITTER) {

    AuroraRGB bg;

    if (widget->prelight) {
      bg = colors->bg[widget->state_type];
    }
    else {
      bg = colors->bg[GTK_STATE_NORMAL];
    }

    AuroraRGB bg_lighter;

    aurora_shade (&bg, &bg_lighter, 1.1);

    cairo_pattern_t *pattern;

    pattern = cairo_pattern_create_linear (0, -height / 2, 0, height / 2);

    cairo_pattern_add_color_stop_rgba (pattern, 0.0, bg.r, bg.g, bg.b, 0.0);
    cairo_pattern_add_color_stop_rgb  (pattern, 0.5, bg_lighter.r, bg_lighter.g, bg_lighter.b);
    cairo_pattern_add_color_stop_rgba (pattern, 1.0, bg.r, bg.g, bg.b, 0.0);

    cairo_set_source (cr, pattern);
    cairo_paint (cr);
    cairo_pattern_destroy (pattern);
    
    //Lighten bar highlights
    aurora_shade (&bg, &light, 1.25);
  }
	
  for (i = 0; i < num_bars; i++) {
    cairo_move_to (cr, 0, bar_y);
    cairo_line_to (cr, bar_width, bar_y);
    cairo_set_source_rgb (cr, dark.r, dark.g, dark.b);
    cairo_stroke (cr);

    cairo_move_to (cr, 0, bar_y + 1);
    cairo_line_to (cr, bar_width, bar_y + 1);
    cairo_set_source_rgb (cr, light.r, light.g, light.b);
    cairo_stroke (cr);

    bar_y += bar_spacing;
  }
}

static void
aurora_draw_normal_arrow (cairo_t * cr, AuroraRGB * color, float alpha,
			  double x, double y, double width, double height)
{
  int ARROW_WIDTH;
  int ARROW_HEIGHT;


  ARROW_WIDTH = (width >= height) ? height : width;

  ARROW_WIDTH = ((ARROW_WIDTH % 2) == 0) ? ARROW_WIDTH + 1 : ARROW_WIDTH;
  if (ARROW_WIDTH < 7)
    ARROW_WIDTH = 7;

  ARROW_HEIGHT = ARROW_WIDTH;

  cairo_translate (cr, -0.5, 0.5);

  cairo_move_to (cr, -ARROW_WIDTH / 2, -ARROW_HEIGHT / 2);
  cairo_line_to (cr, 0, ARROW_HEIGHT / 2);
  cairo_line_to (cr, ARROW_WIDTH / 2, -ARROW_HEIGHT / 2);

	if (alpha < 1) {
		cairo_set_source_rgba (cr, color->r, color->g, color->b, alpha);
		cairo_fill (cr);
	}
	else {
		AuroraRGB bg_lighter;
		aurora_shade (color, &bg_lighter, 1.15);

		cairo_pattern_t *pattern;
		pattern = cairo_pattern_create_linear (0, -height / 2, 0, height / 2);
		cairo_pattern_add_color_stop_rgba (pattern, 0.0, color->r, color->g, color->b, alpha);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, bg_lighter.r, bg_lighter.g, bg_lighter.b, alpha);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);
		cairo_pattern_destroy (pattern);
	}
}

static void
aurora_draw_combo_arrow (cairo_t * cr, AuroraRGB * fill_arrow,
			 AuroraRGB * fill_circle, double x, double y,
			 double width, double height, double min_size)
{
  int ARROW_WIDTH;
  int ARROW_HEIGHT;

  ARROW_WIDTH = (width >= height) ? height : width;

  ARROW_WIDTH = ((ARROW_WIDTH % 2) == 0) ? ARROW_WIDTH + 1 : ARROW_WIDTH;
  if (ARROW_WIDTH < min_size)
    ARROW_WIDTH = min_size;

  cairo_translate (cr, -0.5, 0.5);
	
  // Draw Circle background 
  cairo_move_to (cr, 0, -1);
  if (ARROW_WIDTH <= 13)
  	cairo_arc (cr, 0, -1, ARROW_WIDTH / 2.0, 0, 2 * M_PI);
  else
    cairo_arc (cr, 0, -1, ARROW_WIDTH / 2, 0, 2 * M_PI);
  cairo_set_source_rgb (cr, fill_circle->r, fill_circle->g, fill_circle->b);
  cairo_fill (cr);

  //Draw Arrow
  ARROW_WIDTH = (int)(ARROW_WIDTH * M_SQRT3OVER2); //reduce to max triangle that will fit inside circle.
  ARROW_WIDTH = (ARROW_WIDTH % 2 == 0) ? ARROW_WIDTH - 3 : ARROW_WIDTH - 2; //kep arrow size odd and add padding.
  ARROW_HEIGHT = (int)(ARROW_WIDTH * M_SQRT3OVER2 + 0.5); // Length*root(3)/2 is height for equilaterial triangle
           
  cairo_move_to (cr, -ARROW_WIDTH / 2.0, -ARROW_HEIGHT / 2.0);
  cairo_line_to (cr, 0, ARROW_HEIGHT / 2.0 );
  cairo_line_to (cr, ARROW_WIDTH / 2.0, -ARROW_HEIGHT / 2.0);
  cairo_set_source_rgb (cr, fill_arrow->r, fill_arrow->g, fill_arrow->b);
  cairo_fill (cr);

}

static void
aurora_draw_combo_arrow_2 (cairo_t * cr, AuroraRGB * fill_arrow, double x, double y,
			 double width, double height, double min_size)
{
	int ARROW_WIDTH;
	int ARROW_HEIGHT;

	ARROW_WIDTH = (width >= height) ? height : width;

	ARROW_WIDTH = ((ARROW_WIDTH % 2) == 0) ? ARROW_WIDTH + 1 : ARROW_WIDTH;
	if (ARROW_WIDTH < min_size)
		ARROW_WIDTH = min_size;

	cairo_translate (cr, -0.5, 0.5);
	
	//Draw Arrow
	ARROW_WIDTH = (int)(ARROW_WIDTH * M_SQRT3OVER2); //reduce to max triangle that will fit inside circle.
	ARROW_WIDTH = (ARROW_WIDTH % 2 == 0) ? ARROW_WIDTH - 3 : ARROW_WIDTH - 2; //kep arrow size odd and add padding.
	ARROW_HEIGHT = (int)(ARROW_WIDTH * M_SQRT3OVER2 + 0.5); // Length*root(3)/2 is height for equilaterial triangle
           
	cairo_move_to (cr, -ARROW_WIDTH / 2.0, -ARROW_HEIGHT / 2.0);
	cairo_line_to (cr, 0, ARROW_HEIGHT / 2.0 );
	cairo_line_to (cr, ARROW_WIDTH / 2.0, -ARROW_HEIGHT / 2.0);
	
	AuroraRGB bg_lighter;
	bg_lighter.r = bg_lighter.g = bg_lighter.b = (fill_arrow->r == 0) ? 0.25 : fill_arrow->r;

	aurora_shade (&bg_lighter, &bg_lighter, 1.05);

    cairo_pattern_t *pattern;
    pattern = cairo_pattern_create_linear (0, -height / 2, 0, height / 2);
    cairo_pattern_add_color_stop_rgb (pattern, 0.0, fill_arrow->r, fill_arrow->g, fill_arrow->b);
    cairo_pattern_add_color_stop_rgb (pattern, 1.0, bg_lighter.r, bg_lighter.g, bg_lighter.b);
    cairo_set_source (cr, pattern);
    cairo_fill (cr);
	cairo_pattern_destroy (pattern);

}

static void
_aurora_draw_arrow (cairo_t * cr, AuroraRGB * color, AuroraRGB * color2,
		    float alpha, AuroraDirection dir, AuroraArrowType type,
		    double x, double y, double width, double height, double min_size)
{
  double rotate;

  if (dir == AUR_DIRECTION_LEFT)
    rotate = M_PI * 1.5;
  else if (dir == AUR_DIRECTION_RIGHT)
    rotate = M_PI * 0.5;
  else if (dir == AUR_DIRECTION_UP)
    rotate = M_PI;
  else
    rotate = 0;

  if (type == AUR_ARROW_NORMAL) {
    rotate_mirror_translate (cr, rotate, x, y, FALSE, FALSE);
    aurora_draw_normal_arrow (cr, color, alpha, 0, 0, width, height);
  }
  else if (type == AUR_ARROW_COMBO) {
    rotate_mirror_translate (cr, rotate, x, y, FALSE, FALSE);
    aurora_draw_combo_arrow (cr, color, color2, 0, 0, width, height, min_size);
  }
	else if (type == AUR_ARROW_COMBO_2) {
    rotate_mirror_translate (cr, rotate, x, y, FALSE, FALSE);
    aurora_draw_combo_arrow_2 (cr, color, 0, 0, width, height, min_size);
  }
}

void
aurora_draw_arrow (cairo_t * cr,
		   const AuroraColors * colors,
		   const WidgetParameters * widget,
		   const ArrowParameters * arrow,
		   int x, int y, int width, int height)
{

  double min_size = 11 + 2*arrow->size;

  AuroraRGB color;

  if (arrow->type == AUR_ARROW_COMBO) {
    color = colors->text[GTK_STATE_SELECTED];
    if (widget->disabled) {
      aurora_shade (&color, &color, 0.90);
    }
  }
  else {
    color = colors->text[widget->state_type];
  }

  AuroraRGB color2;

  if (widget->state_type == GTK_STATE_ACTIVE) {
    color2 = colors->spot[1];
  }
  else {
		color2 = colors->bg[widget->state_type];
		aurora_shade (&color2, &color2, 0.74);
		if (widget->disabled) {
			aurora_shade (&color2, &color2, 1.16);
		}
  }

  gdouble tx, ty;

  if (arrow->direction == AUR_DIRECTION_DOWN) {
    tx = x + width / 2 + 1;
    ty = (y + height / 2) + 0.5;
  }
  else if (arrow->direction == AUR_DIRECTION_UP) {
    tx = x + width / 2;
    ty = (y + height / 2) + 0.5;
  }
  else if (arrow->direction == AUR_DIRECTION_LEFT) {
    tx = (x + width / 2) + 0.5;
    ty = y + height / 2;
  }
  else {
    tx = (x + width / 2) + 0.5;
    ty = y + height / 2 + 1;
  }

  if (arrow->type == AUR_ARROW_NORMAL && (aurora_get_lightness(&color) < 0.6) && widget->disabled) {
      	aurora_shade (&colors->bg[widget->state_type], &color2, 1.3);
      	_aurora_draw_arrow (cr, &color2, &color2, 0.55, arrow->direction, arrow->type, tx, ty + 1, width, height, min_size);
  }
	else if (arrow->type == AUR_ARROW_COMBO_2  && (aurora_get_lightness(&color) < 0.6) && widget->disabled) {
      	aurora_shade (&colors->bg[widget->state_type], &color2, 1.3);
      	_aurora_draw_arrow (cr, &color2, &color2, 0.80, arrow->direction, arrow->type, tx, ty + 1.25, width, height, min_size);
  }
  _aurora_draw_arrow (cr, &color, &color2, 1.0, arrow->direction, arrow->type, tx, ty, width, height, min_size);
}


void
aurora_draw_resize_grip (cairo_t * cr,
			 const AuroraColors * colors,
			 const WidgetParameters * widget,
			 const ResizeGripParameters * grip,
			 int x, int y, int width, int height)
{
  AuroraRGB dark = colors->shade[4];
  AuroraRGB shadow, light;

  int size = MAX(height-14,0);
    
  aurora_shade (&dark, &dark, 1.04);
  aurora_shade (&dark, &shadow, 0.76);
  aurora_shade (&dark, &light, 1.13);
	
  cairo_translate(cr,x-2,y-2);
	
  cairo_move_to(cr,size,height-1);
  cairo_line_to(cr,width-1,size);
  cairo_line_to(cr,width-1,height-1);
  cairo_line_to(cr,size,height-1);

	
  cairo_pattern_t *pattern;
  pattern = cairo_pattern_create_linear (0, 0, width, height);
  cairo_pattern_add_color_stop_rgb (pattern, 0.3, shadow.r, shadow.g, shadow.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.50, shadow.r, shadow.g, shadow.b);
  cairo_pattern_add_color_stop_rgb (pattern, 0.71, dark.r, dark.g, dark.b);
  cairo_pattern_add_color_stop_rgb (pattern, 1.0, light.r, light.g, light.b);

  cairo_set_source (cr, pattern);
  cairo_fill (cr);
  cairo_pattern_destroy (pattern);

}

void
aurora_draw_menu_checkbutton (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  width = height = 13;
  cairo_translate (cr, x, y);

  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_scale (cr, width / 17.0, height / 17.0);
			cairo_translate (cr, -2.0, 0.0);
			cairo_translate(cr, 0.75,1.75);
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			cairo_close_path (cr);
    }
    cairo_set_source_rgb (cr, colors->text[widget->state_type].r, colors->text[widget->state_type].g, colors->text[widget->state_type].b);
    cairo_fill (cr);
	}
}

void
aurora_draw_cell_checkbutton (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{
  AuroraRGB border;
  AuroraRGB bullet;
  AuroraRGB bg;
  width = height = 13;

  cairo_translate (cr, x, y);

  //selected text color for check mark as it goes over selected color
  bullet = colors->text[GTK_STATE_SELECTED];

  if (status->draw_bullet) {
	aurora_mix_color (&colors->spot[1], &colors->spot[2], 0.50, &bg);
	aurora_shade (&colors->spot[2], &border, 0.60);

  }
  else {
    bg = colors->base[widget->state_type];
    aurora_shade (&bg, &bg, 1.175);
    border = colors->text[GTK_STATE_NORMAL];
  }

  //Applies to all other states
  if (widget->disabled) {
	bg = colors->bg[widget->state_type];
    //lighten border
    border = colors->shade[4];
    //change bullet to insensitive color
    bullet = colors->text[widget->state_type];
  }

  // Draw the rectangle for the checkbox itself 
  clearlooks_rounded_rectangle (cr, 1.0, 1.0, width - 2, height - 2, 1.5, widget->corners);
  cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
  cairo_fill (cr);

  clearlooks_rounded_rectangle (cr, 0.5, 0.5, width - 1, height - 1, 2.5, widget->corners);
  cairo_set_source_rgb (cr, border.r, border.g, border.b);
  cairo_stroke (cr);


  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_scale (cr, width / 19.0, height / 20.0);
			cairo_translate(cr, 2.1, 4.75);		
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			cairo_close_path (cr);
    }
			
    cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
		cairo_fill (cr);
  }
}


void
aurora_draw_checkbutton (cairo_t * cr,
			 const AuroraColors * colors,
			 const WidgetParameters * widget,
			 const OptionParameters * status,
			 int x, int y, int width, int height)
{

	AuroraRGB border;
	AuroraRGB bullet;
	AuroraRGB bg;
	AuroraRGB shadow;
	AuroraRGB highlight; 
	cairo_pattern_t *pattern;
	
	width = 15;
	height = 15;

	cairo_translate (cr, x, y);

	//selected text color for check mark as it goes over selected color
	bullet = colors->text[GTK_STATE_SELECTED];

#ifdef HAVE_ANIMATION
	AuroraRGB bg_old, border_old;
#endif
	double lightness = aurora_get_lightness(&colors->bg[widget->state_type]);
	double border_scale = lightness < 0.5 ? (lightness*2.5) :1;
		
		
	if (status->draw_bullet) {
		bg  = colors->spot[1];
#ifdef HAVE_ANIMATION
		bg_old = colors->base[GTK_STATE_ACTIVE];
		aurora_mix_color (&colors->shade[8], &bg, 0.18, &border_old);
#endif
		aurora_shade_shift (&colors->spot[1], &border, 0.75*border_scale);
	}
	else {
		bg = colors->base[widget->state_type];
#ifdef HAVE_ANIMATION
		bg_old  = colors->spot[1];
		aurora_shade_shift (&colors->spot[2], &border_old, 0.90*border_scale);
#endif
		aurora_mix_color (&colors->shade[8], &bg, 0.18, &border);
	}
	
#ifdef HAVE_ANIMATION	
	aurora_mix_color (&bg_old, &bg, widget->trans, &bg);
	aurora_mix_color (&border_old, &border, widget->trans, &border);
#endif


	if (widget->prelight) {
		aurora_shade_shift (&bg, &bg, 1.09);
		aurora_shade_shift (&bg, &highlight, 1.2);
	}
	else if (widget->disabled) {
		bg = colors->bg[widget->state_type];
		border = colors->shade[5];
		bullet = colors->text[widget->state_type];
		aurora_shade_shift (&bg, &highlight, 1.10);
	}
	else {
		aurora_shade_shift (&bg, &highlight, 1.13);
	}

	aurora_shade (&border, &shadow, 0.875);
	
	//Fill
	if (status->draw_bullet) {

		AuroraRGB bg_darker, mid_highlight;

		aurora_shade_shift (&bg, &bg_darker, widget->disabled? 0.92 : 0.90);
		aurora_mix_color (&bg, &highlight, 0.50, &mid_highlight);

		pattern = cairo_pattern_create_linear (3, 3, 3, height-4);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, mid_highlight.r, mid_highlight.g, mid_highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, bg.r, bg.g, bg.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, bg_darker.r, bg_darker.g, bg_darker.b);

		cairo_set_source (cr, pattern);
		clearlooks_rounded_rectangle (cr, 2.0, 2.0, width - 4, height - 4, 1.5, widget->corners);
		cairo_fill (cr);
		cairo_pattern_destroy (pattern);
	  
		//stroke highlight
		aurora_shade_shift (&highlight, &highlight, widget->disabled ? 1.02 : 1.04);

		pattern = cairo_pattern_create_linear (0, 2.5, 0, height-2);
		cairo_pattern_add_color_stop_rgb  (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
		cairo_set_source (cr, pattern);

		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width - 5, height - 5, 1.5, widget->corners);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
		
	}
	else {
	  
		clearlooks_rounded_rectangle (cr, 2.0, 2.0, width - 4, height - 4, 1.5, widget->corners);
		cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
		cairo_fill (cr);	
	  
		//inner shadow
		cairo_save (cr);
		clearlooks_rounded_rectangle (cr, 2.5, 2.5, width - 5, height - 5, 1.5, widget->corners);
		cairo_clip (cr);

		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.32);
		cairo_rectangle (cr, 2.5, 2.5, width - 3, height - 3);
		cairo_stroke (cr);
		cairo_restore (cr);
		
  }

		// Draw the shadow inset for unchecked shadow for checked
	if (status->draw_bullet) {
		aurora_draw_shadow (cr, 0.5, 0.5, width - 1, height - 1, 3, widget->corners, &border, 0.24);
	}
	else {
		aurora_draw_etched_shadow (cr, 0.5, 0.5, width - 1, height - 1.5, 3.5, widget->corners, &colors->bg[widget->state_type]);
	}	
		
	
	//Border
	if (status->draw_bullet) {
		aurora_draw_border (cr, 1.5, 1.5, width - 3, height - 3, 2.5, widget->corners, &border, widget->disabled ? 1.12 : 1.30, 0.85);
	}
	else {
		aurora_draw_etched_border (cr, 1.5, 1.5, width - 3, height - 3, 2.5, widget->corners, &border,  widget->disabled ? 1.10 : 1.18, 0.85);
	}	
		

	cairo_save(cr);
	if (!widget->disabled) {
		if (status->inconsistent) {
			cairo_translate(cr, 0.0, 0.5);
			cairo_rectangle (cr, 4, 6, 7, 3);
		}
		else {
			cairo_scale (cr, width / 19.0, height / 20.0);
			cairo_translate(cr, 2.1, 5.25);		
			cairo_move_to (cr, 1.0, 8.0);
			cairo_rel_line_to (cr, 2.0, -2.50);
			cairo_rel_line_to (cr, 3.5, 2.75);
			cairo_rel_line_to (cr, 5.25, -8.5);
			cairo_rel_line_to (cr, 1.95, 0.0);
			cairo_rel_line_to (cr, -6.95, 12.5);
			  cairo_close_path (cr);
			}
    }
	if (status->draw_bullet)
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.55*widget->trans);
	else
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.55*(1- widget->trans));
	
	cairo_set_line_width(cr, 0.50);
	cairo_stroke (cr);
	cairo_restore(cr);
	
	if (status->inconsistent) {
		cairo_rectangle (cr, 4, 6, 7, 3);
	}
	else {
		cairo_scale (cr, width / 19.0, height / 20.0);
		cairo_translate(cr, 2.1, 4.75);		
		cairo_move_to (cr, 1.0, 8.0);
		cairo_rel_line_to (cr, 2.0, -2.50);
		cairo_rel_line_to (cr, 3.5, 2.75);
		cairo_rel_line_to (cr, 5.25, -8.5);
		cairo_rel_line_to (cr, 1.95, 0.0);
		cairo_rel_line_to (cr, -6.95, 12.5);
		cairo_close_path (cr);
	}
		
	if (status->draw_bullet)
    	cairo_set_source_rgba (cr, bullet.r, bullet.g, bullet.b, widget->trans);
    else
       cairo_set_source_rgba (cr, bullet.r, bullet.g, bullet.b, (1- widget->trans));

	cairo_fill (cr);
	
}

void
aurora_draw_menu_radiobutton (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  AuroraRGB bullet;

  width = height = 15;
  double center = width / 2.0 - 1.5;

  cairo_translate (cr, x-1.0, y);

  //selected text color for check mark as it goes over selected color
  bullet = colors->text[widget->state_type];

	/* Draw the rectangle for the checkbox itself */
  cairo_arc (cr, center + 1.5, center + 1.5, center, 0, M_PI * 2);

  cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
  cairo_set_line_width(cr, 1.0);
  cairo_stroke (cr);
		
  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 3, 5, 7, 3);
    }
    else {
      cairo_arc (cr, center + 1.5, center + 1.5, center - 2.5, 0, M_PI * 2);
    }

    cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
    cairo_fill (cr);
  }
}

void
aurora_draw_cell_radiobutton (cairo_t * cr,
			      const AuroraColors * colors,
			      const WidgetParameters * widget,
			      const OptionParameters * status,
			      int x, int y, int width, int height)
{

  AuroraRGB border;
  AuroraRGB bullet;
  AuroraRGB bg;

  width = height = 15;
  double center = width / 2.0 - 1.5;

  cairo_translate (cr, x, y);

  //selected text color for check mark as it goes over selected color
  bullet = colors->text[GTK_STATE_SELECTED];

  if (status->draw_bullet) {
    bg = colors->spot[1];
	aurora_mix_color (&colors->spot[1], &colors->spot[2], 0.50, &bg);
    border = colors->spot[2];
	aurora_shade (&border, &border, 0.60);

  }
  else {
    bg = colors->base[widget->state_type];
    aurora_shade (&bg, &bg, 1.175);
    border = colors->text[GTK_STATE_NORMAL];
  }

  //Applies to all other states
  if (widget->disabled) {
		aurora_mix_color (&colors->bg[widget->state_type], &bg, 0.10, &bg);
    //lighten border
    border = colors->shade[4];
    //change bullet to insensitive color
    bullet = colors->text[widget->state_type];
  }


  /* Draw the rectangle for the checkbox itself */
  cairo_arc (cr, center + 1.5, center + 1.5, center, 0, M_PI * 2);

  cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
  cairo_fill_preserve (cr);

  cairo_set_source_rgb (cr, border.r, border.g, border.b);
  cairo_stroke (cr);


  if (status->draw_bullet) {
    if (status->inconsistent) {
      cairo_rectangle (cr, 4, 6, 7, 3);
    }
    else {
      cairo_arc (cr, center + 1.5, center + 1.5, center - 2.75, 0, M_PI * 2);
    }

    cairo_set_source_rgb (cr, bullet.r, bullet.g, bullet.b);
    cairo_fill (cr);
  }
}



void
aurora_draw_radiobutton (cairo_t * cr,
			 const AuroraColors * colors,
			 const WidgetParameters * widget,
			 const OptionParameters * status,
			 int x, int y, int width, int height)
{

	AuroraRGB border;
	AuroraRGB bullet;
	AuroraRGB bg;
	AuroraRGB shadow;
	AuroraRGB highlight; 
	cairo_pattern_t *pattern;

	width = height = 15;
	double center = width / 2.0 - 1.5;

	cairo_translate (cr, x, y);

	//selected text color for check mark as it goes over selected color
	bullet = colors->text[GTK_STATE_SELECTED];

#ifdef HAVE_ANIMATION	
	AuroraRGB bg_old, border_old;
#endif
	double lightness = aurora_get_lightness(&colors->bg[widget->state_type]);
	double border_scale = lightness < 0.5 ? (lightness*2.5) :1;
		
		
	if (status->draw_bullet) {
		bg  = colors->spot[1];
#ifdef HAVE_ANIMATION	
		bg_old = colors->base[GTK_STATE_ACTIVE];
		aurora_mix_color (&colors->shade[8], &bg, 0.18, &border_old);
#endif
		aurora_shade_shift (&colors->spot[1], &border, 0.75*border_scale);
	}
	else {
		bg = colors->base[widget->state_type];
#ifdef HAVE_ANIMATION	
		bg_old  = colors->spot[1];
		aurora_shade_shift (&colors->spot[2], &border_old, 0.90*border_scale);
#endif
		aurora_mix_color (&colors->shade[8], &bg, 0.18, &border);
	}
	
#ifdef HAVE_ANIMATION		
	aurora_mix_color (&bg_old, &bg, widget->trans, &bg);
	aurora_mix_color (&border_old, &border, widget->trans, &border);
#endif

	if (widget->prelight) {
		aurora_shade_shift (&bg, &bg, 1.09);
		aurora_shade_shift (&bg, &highlight, 1.2);
	}
	else if (widget->disabled) {
		bg = colors->bg[widget->state_type];
		border = colors->shade[5];
		bullet = colors->text[widget->state_type];
		aurora_shade_shift (&bg, &highlight, 1.10);
	}
	else {
		aurora_shade_shift (&bg, &highlight, 1.13);
	}

	aurora_shade (&border, &shadow, 0.875);
	
	
	
	//Fill
	if (status->draw_bullet) {
		AuroraRGB bg_darker, mid_highlight;

		aurora_shade_shift (&bg, &bg_darker, widget->disabled? 0.92 : 0.90);
		aurora_mix_color (&bg, &highlight, 0.60, &mid_highlight);

		pattern = cairo_pattern_create_linear (3, 3, 3, height-4);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, mid_highlight.r, mid_highlight.g, mid_highlight.b);
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, bg.r, bg.g, bg.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, bg_darker.r, bg_darker.g, bg_darker.b);

		cairo_set_source (cr, pattern);
  		cairo_arc (cr, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2);
		cairo_fill (cr);
		cairo_pattern_destroy (pattern);
	  
		//stroke highlight
		aurora_shade_shift (&highlight, &highlight, widget->disabled ? 1.04 : 1.06);
			
		pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
		cairo_pattern_add_color_stop_rgb  (pattern, 0.0, highlight.r, highlight.g, highlight.b);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
		cairo_set_source (cr, pattern);

		cairo_arc (cr, center + 1.5, center + 1.5, center - 1, 0, M_PI * 2);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);

		/*Inner glow
		if (widget->prelight || widget->disabled) {
			aurora_shade_shift (&highlight, &highlight, 1.10);
			pattern = cairo_pattern_create_radial (width/2, height/1.1, 0, width/2, height/1.1, width/2);
			cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight.r, highlight.g, highlight.b, 0.85);
			cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight.r, highlight.g, highlight.b, 0.0);
			cairo_set_source (cr, pattern);
	  		cairo_arc (cr, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2);
			cairo_fill (cr);
			cairo_pattern_destroy (pattern);
		}*/
		
	}
	else {
		cairo_arc (cr, center + 1.5, center + 1.5, center -0.5, 0, M_PI * 2);
		cairo_set_source_rgb (cr, bg.r, bg.g, bg.b);
		cairo_fill (cr);
	  
		//inner shadow
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.32);
		cairo_arc (cr, center + 1.5, center + 1.5, center - 1, M_PI * 0.75, M_PI * 1.75);
		cairo_stroke (cr);

	}

	//Border
	cairo_arc (cr, center + 1.5, center + 1.5, center , 0, M_PI * 2);

	if (status->draw_bullet) {
		AuroraRGB border_darker, border_lighter;
		aurora_shade_shift (&border, &border_darker, 0.85);
		aurora_shade_shift (&border, &border_lighter, widget->disabled ? 1.25 : 1.45);
		  
		pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, border_lighter.r, border_lighter.g, border_lighter.b); 
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, border.r, border.g, border.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, border_darker.r, border_darker.g, border_darker.b); 

		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}
	else {
		AuroraRGB border_darker, border_lighter;
		aurora_shade_shift (&border, &border_darker, 0.78);
		aurora_shade_shift (&border, &border_lighter, widget->disabled ? 1.18 : 1.22);

		pattern = cairo_pattern_create_linear (0, 2, 0, height-2);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, border_darker.r, border_darker.g, border_darker.b); 
		cairo_pattern_add_color_stop_rgb (pattern, 0.5, border.r, border.g, border.b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, border_lighter.r, border_lighter.g, border_lighter.b); 

		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}
	
  /* Draw the shadow 
     inset for unchecked shadow for checked */

	if (status->draw_bullet) {
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.24);
		cairo_arc (cr, center + 1.5, center + 2.0, center + 0.5, 0, M_PI * 2);
		cairo_stroke (cr);
	}
	else {
		cairo_arc (cr, center + 1.5, center + 1.5, center + 1, 0, M_PI * 2);
		AuroraRGB shadow1;
		AuroraRGB highlight1;

		aurora_shade_shift (&colors->bg[widget->state_type], &shadow1, 0.83);
		aurora_shade_shift (&colors->bg[widget->state_type], &highlight1, 1.26);	

		pattern = cairo_pattern_create_linear (0, 0, 0, height);

		cairo_pattern_add_color_stop_rgba (pattern, 0.0, shadow1.r, shadow1.g, shadow1.b, 0.40);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, colors->bg[widget->state_type].r, colors->bg[widget->state_type].g, colors->bg[widget->state_type].b, 0.0);
		cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight1.r, highlight1.g, highlight1.b, 0.40);	

		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}

	
	cairo_save (cr);
	if (!widget->disabled) {
		cairo_translate (cr, 0.0, 0.2);
		if (status->inconsistent)
			clearlooks_rounded_rectangle(cr, 4, 6, 7, 3, 1.5, AUR_CORNER_ALL);
		else
			cairo_arc (cr, center + 1.5, center + 1.5, center - 2.75, 0, M_PI * 2);
	}
	if (status->draw_bullet)
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.55*widget->trans);
	else
		cairo_set_source_rgba (cr, shadow.r, shadow.g, shadow.b, 0.55*(1- widget->trans));
	
	cairo_set_line_width(cr, 0.50);
	cairo_stroke (cr);		
	cairo_restore (cr);		
		

	if (status->inconsistent) {
		clearlooks_rounded_rectangle(cr, 4, 6, 7, 3, 1.5, AUR_CORNER_ALL);
	}
	else {
		cairo_arc (cr, center + 1.5, center + 1.5, center - 2.75, 0, M_PI * 2);
	}
  
	if (status->draw_bullet)
		cairo_set_source_rgba (cr, bullet.r, bullet.g, bullet.b, widget->trans);
	else
		cairo_set_source_rgba (cr, bullet.r, bullet.g, bullet.b, (1- widget->trans));
  
	cairo_fill (cr);
}




void
aurora_draw_tooltip (cairo_t * cr,
		     const AuroraColors * colors,
		     const WidgetParameters * widget,
		     int x, int y, int width, int height)
{
	const AuroraRGB *highlight = &colors->text[GTK_STATE_SELECTED];
	AuroraRGB darker;
	cairo_pattern_t *pattern;
	
	cairo_translate (cr, x, y);
		

	cairo_rectangle(cr, 0, 0, width, height);
	
	aurora_shade_shift (&colors->bg[GTK_STATE_NORMAL], &darker, 0.90);
						
	pattern = cairo_pattern_create_linear (0, 0, width*0.75, height);
	cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->bg[GTK_STATE_NORMAL].r, colors->bg[GTK_STATE_NORMAL].g, colors->bg[GTK_STATE_NORMAL].b);
	cairo_pattern_add_color_stop_rgb (pattern, 1.0, darker.r, darker.g, darker.b);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_fill(cr);
	
	//Highlight
	pattern = cairo_pattern_create_linear (0, 0, width*0.08, height*0.75);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, highlight->r, highlight->g, highlight->b, 0.40);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, highlight->r, highlight->g, highlight->b, 0.0);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_move_to  (cr, 0, 0);
	cairo_line_to  (cr, 0, height);
	cairo_line_to  (cr, 5, height);
	cairo_curve_to (cr, 5, height, 0, 0, width, 0);
	cairo_fill(cr);
	
	//Stroke
	aurora_shade_shift (&colors->bg[GTK_STATE_NORMAL], &darker, 0.70);
	cairo_set_source_rgb (cr, darker.r, darker.g, darker.b);
	cairo_rectangle (cr, 0.5, 0.5, width - 1, height - 1);
	cairo_stroke (cr);

}

void
aurora_draw_list_selection (cairo_t * cr,
			    const AuroraColors * colors,
			    const WidgetParameters * widget,
			    int x, int y, int width, int height)
{
	AuroraRGB* fill_color;

	if (widget->focus)
		fill_color = (AuroraRGB*)&colors->base[widget->state_type];
	else
		fill_color = (AuroraRGB*)&colors->base[GTK_STATE_ACTIVE];
	
	list_menu_gradient(cr, fill_color, x, y, width, height);
	
}
