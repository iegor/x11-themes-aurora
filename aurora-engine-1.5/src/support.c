#include "support.h"

GtkTextDirection
aurora_get_direction (GtkWidget *widget)
{
	GtkTextDirection dir;
	
	if (widget)
		dir = gtk_widget_get_direction (widget);
	else
		dir = GTK_TEXT_DIR_LTR;
	
	return dir;
}


/* Widget Type Lookups/Macros
   
   Based on/modified from functions in
   Smooth-Engine.
*/ 
gboolean
aurora_object_is_a (const GObject * object, const gchar * type_name)
{
  gboolean result = FALSE;
 
  if ((object))
    {
      GType tmp = g_type_from_name (type_name);

      if (tmp)
	result = g_type_check_instance_is_a ((GTypeInstance *) object, tmp);
    }
 
  return result;
}

 
gboolean
aurora_is_combo_box_entry (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent))
    {
      if (AURORA_IS_COMBO_BOX_ENTRY (widget->parent))
	result = TRUE;
      else
	result = aurora_is_combo_box_entry (widget->parent);
    }
  return result;
}
 
static gboolean
aurora_combo_box_is_using_list (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if (AURORA_IS_COMBO_BOX (widget))
    {
      gboolean *tmp = NULL;
 
      gtk_widget_style_get (widget, "appears-as-list", &result, NULL);

      if (tmp)
	result = *tmp;
    }
 
  return result;
}
 
gboolean
aurora_is_combo_box (GtkWidget * widget, gboolean as_list)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent))
    {
      if (AURORA_IS_COMBO_BOX (widget->parent))
        {
          if (as_list)
            result = (aurora_combo_box_is_using_list(widget->parent));
          else
            result = (!aurora_combo_box_is_using_list(widget->parent));
        }
      else
	result = aurora_is_combo_box (widget->parent, as_list);
    }
  return result;
}
 
gboolean
aurora_is_combo (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent))
    {
      if (AURORA_IS_COMBO (widget->parent))
	result = TRUE;
      else
	result = aurora_is_combo (widget->parent);
    }
  return result;
}
 
gboolean
aurora_is_in_combo_box (GtkWidget * widget)
{
  return ((aurora_is_combo (widget) || aurora_is_combo_box (widget, TRUE) || aurora_is_combo_box_entry (widget)));
}
 
gboolean
aurora_is_tree_column_header (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent))
    {
      if (AURORA_IS_TREE_VIEW (widget->parent))
	result = TRUE;
      else
	result = aurora_is_tree_column_header (widget->parent);
    }
  return result;
}

gboolean
aurora_is_toolbar_item (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent)) {
    if ((AURORA_IS_BONOBO_TOOLBAR (widget->parent))
	|| (AURORA_IS_BONOBO_DOCK_ITEM (widget->parent))
	|| (AURORA_IS_EGG_TOOLBAR (widget->parent))
	|| (AURORA_IS_TOOLBAR (widget->parent))
	|| (AURORA_IS_HANDLE_BOX (widget->parent)))
      result = TRUE;
    else
      result = aurora_is_toolbar_item (widget->parent);
  }
  return result;
}
 
gboolean
aurora_is_panel_widget_item (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget) && (widget->parent))
    {
      if (AURORA_IS_PANEL_WIDGET (widget->parent))
	result = TRUE;
      else
	result = aurora_is_panel_widget_item (widget->parent);
    }
  return result;
}

gboolean 
aurora_is_bonobo_dock_item (GtkWidget * widget)
{
  gboolean result = FALSE;
 
  if ((widget))
    {
      if (AURORA_IS_BONOBO_DOCK_ITEM(widget) || AURORA_IS_BONOBO_DOCK_ITEM (widget->parent))
	result = TRUE;
      else if (AURORA_IS_BOX(widget) || AURORA_IS_BOX(widget->parent))
        {
          GtkContainer *box = AURORA_IS_BOX(widget)?GTK_CONTAINER(widget):GTK_CONTAINER(widget->parent);
          GList *children = NULL, *child = NULL;
 
          children = gtk_container_get_children(box);
              
          for (child = g_list_first(children); child; child = g_list_next(child))
            {
	      if (AURORA_IS_BONOBO_DOCK_ITEM_GRIP(child->data))
	        {
	          result = TRUE;
	          child = NULL;
	        }
            }	            
         
          if (children)   
  	    g_list_free(children);
	}
    }
  return result;
}

static GtkWidget *
aurora_find_combo_box_entry_widget (GtkWidget * widget)
{
  GtkWidget *result = NULL;

  if (widget)
    {
      if (AURORA_IS_COMBO_BOX_ENTRY (widget))
	result = widget;
      else
	result = aurora_find_combo_box_entry_widget (widget->parent);
    }

  return result;
}

static GtkWidget *
aurora_find_combo_box_widget (GtkWidget * widget, gboolean as_list)
{
  GtkWidget *result = NULL;
 
  if (widget)
    {
      if (AURORA_IS_COMBO_BOX (widget))
        {
          if (as_list)
            result = (aurora_combo_box_is_using_list(widget))?widget:NULL;
          else
            result = (!aurora_combo_box_is_using_list(widget))?widget:NULL;
        }
      else
	result = aurora_find_combo_box_widget (widget->parent, as_list);
    }
  return result;
}
 
static GtkWidget *
aurora_find_combo_widget (GtkWidget * widget)
{
  GtkWidget *result = NULL;
 
  if (widget)
    {
      if (AURORA_IS_COMBO (widget))
	result = widget;
      else
	result = aurora_find_combo_widget(widget->parent);
    }
  return result;
}

GtkWidget*
aurora_find_combo_box_widget_parent (GtkWidget * widget)
{
   GtkWidget *result = NULL;
   
   if (!result)
     result = aurora_find_combo_widget(widget);
  
   if (!result)
     result = aurora_find_combo_box_widget(widget, TRUE);

   if (!result)
     result = aurora_find_combo_box_entry_widget(widget);

  return result;
}


gboolean
aurora_widget_is_ltr (GtkWidget *widget)
{
	GtkTextDirection dir = GTK_TEXT_DIR_NONE;
	
	if (AURORA_IS_WIDGET (widget))
		dir = gtk_widget_get_direction (widget);

	if (dir == GTK_TEXT_DIR_NONE)
		dir = gtk_widget_get_default_direction ();

	if (dir == GTK_TEXT_DIR_RTL)
		return FALSE;
	else
		return TRUE;
}




/***********************************************
 * ge_hsb_from_color -
 *  
 *   Get HSB values from RGB values.
 *
 *   Modified from Smooth but originated in GTK+
 ***********************************************/
void
aurora_hsb_from_color (const AuroraRGB *rgb, AuroraHSB *hsb) 
{
	gdouble min, max, delta;
  
	if (rgb->r > rgb->g)
	{
		max = MAX(rgb->r, rgb->b);
		min = MIN(rgb->g, rgb->b);
	}
	else
	{
		max = MAX(rgb->g, rgb->b);
		min = MIN(rgb->r, rgb->b);
	}
  
	hsb->b = (max + min) / 2.0;
 	
	delta = max -min;
	
	if (fabs(delta) < 0.0001)
	{
		hsb->h = 0;
		hsb->s = 0;
	}	
	else
	{
		if (hsb->b <= 0.5)
			hsb->s = (max - min) / (max + min);
		else
			hsb->s = (max - min) / (2.0 - max - min);
        
		if (rgb->r == max)
			hsb->h = (rgb->g - rgb->b) / delta;
		else if (rgb->g == max)
			hsb->h = 2.0 + (rgb->b - rgb->r) / delta;
		else if (rgb->b == max)
			hsb->h = 4.0 + (rgb->r - rgb->g) / delta;
 
		hsb->h /= 6.0;
		if (hsb->h < 0.0)
			hsb->h += 1.0;
	}
}
 
/***********************************************
 * ge_color_from_hsb -
 *  
 *   Get RGB values from HSB values.
 *
 *   Modified from Smooth but originated in GTK+
 ***********************************************/
#define MODULA(number, divisor) (((gint)number % divisor) + (number - (gint)number))

static inline gdouble
	aurora_hsl_value (gdouble n1,
	gdouble n2,
	gdouble hue)
	{
	gdouble val;
	
	if (hue > 6.0)
		hue -= 6.0;
	else if (hue < 0.0)
		hue += 6.0;

	if (hue < 1.0)
		val = n1 + (n2 - n1) * hue;
	else if (hue < 3.0)
		val = n2;
	else if (hue < 4.0)
		val = n1 + (n2 - n1) * (4.0 - hue);
	else
		val = n1;
		
	return val;
} 

static void
aurora_color_from_hsb (const AuroraHSB * hsb, AuroraRGB *rgb)
{

	g_return_if_fail (hsb != NULL);
	g_return_if_fail (rgb != NULL);
	
	if (hsb->s == 0)
	{
	/* achromatic case */
		rgb->r = hsb->b;
		rgb->g = hsb->b;
		rgb->b = hsb->b;
	}
	else
	{
		gdouble m1, m2;
		
		if (hsb->b <= 0.5)
			m2 = hsb->b * (1.0 + hsb->s);
		else
			m2 = hsb->b + hsb->s - hsb->b * hsb->s;
	
		m1 = 2.0 * hsb->b - m2;
	
		rgb->r = aurora_hsl_value (m1, m2, hsb->h * 6.0 + 2.0);
		rgb->g = aurora_hsl_value (m1, m2, hsb->h * 6.0);
		rgb->b = aurora_hsl_value (m1, m2, hsb->h * 6.0 - 2.0);
	}
}


void
aurora_shade (const AuroraRGB *base, AuroraRGB *composite, double shade_ratio)
{
	AuroraHSB hsb;
	g_return_if_fail (base && composite);

	aurora_hsb_from_color (base, &hsb);
 
	hsb.b = MIN(hsb.b*shade_ratio, 1.0);
	hsb.b = MAX(hsb.b, 0.0);
	
	aurora_color_from_hsb (&hsb, composite);
}

void
aurora_shade_shift (const AuroraRGB *base, AuroraRGB *composite, double shade_ratio)
{
	AuroraHSB hsb;
	g_return_if_fail (base && composite);

	aurora_hsb_from_color (base, &hsb);
 
	hsb.b = MIN(hsb.b*shade_ratio, 1.0);
	hsb.b = MAX(hsb.b, 0.0);
	hsb.h += (shade_ratio >= 1.0) ? -0.0097222*shade_ratio : -0.033333*shade_ratio + 0.047222;
	
	aurora_color_from_hsb (&hsb, composite);
}


void
aurora_shade_hsb (const AuroraHSB *base, AuroraRGB *composite, double shade_ratio)
{
	AuroraHSB hsb;
	g_return_if_fail (base && composite);

	hsb.h = base->h;
	hsb.s = base->s;
	
	hsb.b = MIN(base->b*shade_ratio, 1.0);
	hsb.b = MAX(hsb.b, 0.0);
	
	aurora_color_from_hsb (&hsb, composite);
}

void
aurora_shade_shift_hsb (const AuroraHSB *base, AuroraRGB *composite, double shade_ratio)
{
	AuroraHSB hsb;
	g_return_if_fail (base && composite);
	
	hsb.h = base->h;
	hsb.s = base->s;
	
	hsb.b = MIN(base->b*shade_ratio, 1.0);
	hsb.b = MAX(hsb.b, 0.0);
	
	hsb.h += (shade_ratio >= 1.0) ? -0.0097222*shade_ratio : -0.033333*shade_ratio + 0.047222;
	aurora_color_from_hsb (&hsb, composite);
}

void
aurora_hue_shift (const AuroraRGB *base, AuroraRGB *composite, double shift_amount)
{
	AuroraHSB hsb;
	g_return_if_fail (base && composite);

	aurora_hsb_from_color (base, &hsb);

	hsb.h += shift_amount;
	
	aurora_color_from_hsb (&hsb, composite);
}

void
aurora_match_lightness (const AuroraRGB * a, AuroraRGB * b)
{
	AuroraHSB hsb;
	
	aurora_hsb_from_color (b, &hsb);
	hsb.b = aurora_get_lightness(a);
	aurora_color_from_hsb (&hsb, b);
		
}

void
aurora_set_lightness (AuroraRGB * a, double new_lightness)
{
		
	AuroraHSB hsb;
 
	aurora_hsb_from_color (a, &hsb);
	hsb.b = new_lightness;
	aurora_color_from_hsb (&hsb, a);
		
}

void
aurora_scale_saturation (AuroraRGB * a, double scale)
{
	AuroraHSB hsb;
 
	aurora_hsb_from_color (a, &hsb);
	hsb.s *= scale;
	aurora_color_from_hsb (&hsb, a);
		
}

void
aurora_tweak_saturation (const AuroraHSB * a, AuroraRGB * b)
{
	AuroraHSB hsb;

 	aurora_hsb_from_color (b, &hsb);
	hsb.h = a->h;
	hsb.s = (a->s + hsb.s)/2.0;
	aurora_color_from_hsb (&hsb, b);
		
}


gdouble
aurora_get_lightness (const AuroraRGB * a)
{

	gdouble min, max;
 
	if (a->r > a->g)
	{
		max = MAX(a->r, a->b);
		min = MIN(a->g, a->b);
	}
	else
	{
		max = MAX(a->g, a->b);
		min = MIN(a->r, a->b);
	}
  	
	return (max + min) / 2.0;
}


void
aurora_mix_color (const AuroraRGB * a, const AuroraRGB * b, gdouble mix_factor, AuroraRGB * c)
{
	g_return_if_fail (a && b && c);

	c->r = a->r * (1-mix_factor) + b->r * mix_factor;
	c->g = a->g * (1-mix_factor) + b->g * mix_factor;
	c->b = a->b * (1-mix_factor) + b->b * mix_factor;
}


void aurora_gtk_treeview_get_header_index (GtkTreeView *tv, GtkWidget *header,
                                    gint *column_index, gint *columns,
                                    gboolean *resizable, gboolean *sorted)
{
	GList *list, *list_start;
	*column_index = *columns = 0;
	list_start = list = gtk_tree_view_get_columns (tv);

	do
	{
		GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(list->data);
		if ( column->button == header )
		{
			*column_index = *columns;
			*resizable = column->resizable;
			*sorted = column->show_sort_indicator;
		}
		if ( column->visible )
			(*columns)++;
	} while ((list = g_list_next(list)));

	g_list_free (list_start);
}


void aurora_gtk_clist_get_header_index (GtkCList *clist, GtkWidget *button,
                                 gint *column_index, gint *columns)
{
	int i;
	*columns = clist->columns;
	
	for (i=0; i<*columns; i++)
	{
		if (clist->column[i].button == button)
		{
			*column_index = i;
			break;
		}
	}
}


void
aurora_gdk_color_to_cairo (const GdkColor *c, AuroraRGB *cc)
{
	gdouble r, g, b;

	g_return_if_fail (c && cc);

	r = c->red / 65535.0;
	g = c->green / 65535.0;
	b = c->blue / 65535.0;

	cc->r = r;
	cc->g = g;
	cc->b = b;
}

void
aurora_gdk_color_to_rgb (GdkColor *c, double *r, double *g, double *b)
{
	*r = (double)c->red   /  (double)65535;
	*g = (double)c->green /  (double)65535;
	*b = (double)c->blue  /  (double)65535;
}


void
aurora_get_parent_bg (const GtkWidget *widget, AuroraRGB *color)
{
	GtkStateType state_type;
	const GtkWidget *parent;
	GdkColor *gcolor;
	gboolean stop;
	GtkShadowType shadow = GTK_SHADOW_NONE;
	
	if (widget == NULL)
		return;
	
	parent = widget->parent;
	stop = FALSE;
	
	while (parent && !stop)
	{
		stop = FALSE;

		stop |= !GTK_WIDGET_NO_WINDOW (parent);
		stop |= GTK_IS_NOTEBOOK (parent) &&
		        gtk_notebook_get_show_tabs (GTK_NOTEBOOK (parent)) &&
		        gtk_notebook_get_show_border (GTK_NOTEBOOK (parent));

		if (GTK_IS_FRAME(parent))
		{
			shadow = gtk_frame_get_shadow_type(GTK_FRAME(parent));
			stop |= (shadow != GTK_SHADOW_NONE);
		}
		else if (GTK_IS_TOOLBAR (parent))
		{
			gtk_widget_style_get (GTK_WIDGET (parent), "shadow-type", &shadow, NULL);
			
			stop |= (shadow != GTK_SHADOW_NONE);
		}

		if (!stop)
			parent = parent->parent;
	}

	if (parent == NULL)
		return;
	
	state_type = GTK_WIDGET_STATE (parent);
	
	gcolor = &parent->style->bg[state_type];
	
	aurora_gdk_color_to_rgb (gcolor, &color->r, &color->g, &color->b);
    
    if (GTK_IS_FRAME (parent) && shadow != GTK_SHADOW_NONE) {
         if (shadow == (GTK_SHADOW_IN || GTK_SHADOW_ETCHED_IN))
         	aurora_shade (color, color, 0.97);
         else
            aurora_shade (color, color, 1.04);
    }
}

