#ifndef SUPPORT_H
#define SUPPORT_H

#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

#include "aurora_types.h"

#define RADIO_SIZE 15
#define CHECK_SIZE 15


#define AURORA_IS_WIDGET(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkWidget"))
#define AURORA_IS_CONTAINER(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkContainer"))
#define AURORA_IS_BIN(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkBin"))

#define AURORA_IS_ARROW(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkArrow"))

#define AURORA_IS_SEPARATOR(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkSeparator"))
#define AURORA_IS_VSEPARATOR(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkVSeparator"))
#define AURORA_IS_HSEPARATOR(object) ((object)  && aurora_object_is_a ((GObject*)(object), "GtkHSeparator"))
 
#define AURORA_IS_HANDLE_BOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkHandleBox"))
#define AURORA_IS_HANDLE_BOX_ITEM(object) ((object) && AURORA_IS_HANDLE_BOX(object->parent))
#define AURORA_IS_BONOBO_DOCK_ITEM(object) ((object) && aurora_object_is_a ((GObject*)(object), "BonoboDockItem"))
#define AURORA_IS_BONOBO_DOCK_ITEM_GRIP(object) ((object) && aurora_object_is_a ((GObject*)(object), "BonoboDockItemGrip"))
#define AURORA_IS_BONOBO_TOOLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "BonoboUIToolbar"))
#define AURORA_IS_EGG_TOOLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "Toolbar"))
#define AURORA_IS_TOOLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkToolbar"))
#define AURORA_IS_PANEL_WIDGET(object) ((object) && (aurora_object_is_a ((GObject*)(object), "PanelWidget") || aurora_object_is_a ((GObject*)(object), "PanelApplet")))
#define AURORA_IS_CALENDAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCalendar"))

#define AURORA_IS_COMBO_BOX_ENTRY(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkComboBoxEntry"))
#define AURORA_IS_COMBO_BOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkComboBox"))
#define AURORA_IS_COMBO(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCombo"))
#define AURORA_IS_OPTION_MENU(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkOptionMenu"))
 
#define AURORA_IS_TOGGLE_BUTTON(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkToggleButton"))
#define AURORA_IS_CHECK_BUTTON(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCheckButton"))
#define AURORA_IS_SPIN_BUTTON(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkSpinButton"))
 
#define AURORA_IS_STATUSBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkStatusbar"))
#define AURORA_IS_PROGRESS_BAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkProgressBar"))
 
#define AURORA_IS_MENU_SHELL(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkMenuShell"))
#define AURORA_IS_MENU(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkMenu"))
#define AURORA_IS_MENU_BAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkMenuBar"))
#define AURORA_IS_MENU_ITEM(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkMenuItem"))

#define AURORA_IS_CHECK_MENU_ITEM(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCheckMenuItem"))

#define AURORA_IS_RANGE(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkRange"))
 
#define AURORA_IS_SCROLLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkScrollbar"))
#define AURORA_IS_VSCROLLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkVScrollbar"))
#define AURORA_IS_HSCROLLBAR(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkHScrollbar"))
 
#define AURORA_IS_SCALE(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkScale"))
#define AURORA_IS_VSCALE(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkVScale"))
#define AURORA_IS_HSCALE(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkHScale"))
  
#define AURORA_IS_PANED(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkPaned"))
#define AURORA_IS_VPANED(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkVPaned"))
#define AURORA_IS_HPANED(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkHPaned"))
 
#define AURORA_IS_BOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkBox"))
#define AURORA_IS_VBOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkVBox"))
#define AURORA_IS_HBOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkHBox"))

#define AURORA_IS_CLIST(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCList"))
#define AURORA_IS_TREE_VIEW(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkTreeView"))
#define AURORA_IS_ENTRY(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkEntry"))
#define AURORA_IS_BUTTON(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkButton"))
#define AURORA_IS_FIXED(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkFixed"))
#define AURORA_IS_VIEWPORT(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkViewport"))
#define AURORA_IS_SCROLLED_WINDOW(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkScrolledWindow"))

#define TOGGLE_BUTTON(object) (AURORA_IS_TOGGLE_BUTTON(object)?(GtkToggleButton *)object:NULL)
 
#define AURORA_IS_NOTEBOOK(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkNotebook"))
#define AURORA_IS_CELL_RENDERER_TOGGLE(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkCellRendererToggle"))

#define AURORA_WIDGET_HAS_DEFAULT(object) ((object) && AURORA_IS_WIDGET(object) && GTK_WIDGET_HAS_DEFAULT(object))

#define AURORA_IS_COLOR_BUTTON(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkColorButton"))
#define AURORA_IS_EVENT_BOX(object) ((object) && aurora_object_is_a ((GObject*)(object), "GtkEventBox"))









G_GNUC_INTERNAL GtkTextDirection aurora_get_direction     (GtkWidget *widget);

G_GNUC_INTERNAL GtkWidget *aurora_special_get_ancestor    (GtkWidget    *widget,
                                    GType         widget_type);
  
G_GNUC_INTERNAL void aurora_shade (const AuroraRGB *a, AuroraRGB *b, double k);
G_GNUC_INTERNAL void aurora_shade_shift (const AuroraRGB *a, AuroraRGB *b, double k);
G_GNUC_INTERNAL void aurora_hue_shift (const AuroraRGB *base, AuroraRGB *composite, double shift_amount);

G_GNUC_INTERNAL void aurora_match_lightness (const AuroraRGB * a, AuroraRGB * b);
G_GNUC_INTERNAL gdouble aurora_get_lightness (const AuroraRGB * a);
G_GNUC_INTERNAL void aurora_set_lightness (AuroraRGB * a, double new_lightness);
G_GNUC_INTERNAL void aurora_tweak_saturation (const AuroraHSB * a, AuroraRGB * b);
G_GNUC_INTERNAL void aurora_scale_saturation (AuroraRGB * a, double scale);
	
G_GNUC_INTERNAL void aurora_hsb_from_color (const AuroraRGB *base, AuroraHSB *composite);
G_GNUC_INTERNAL void aurora_shade_hsb (const AuroraHSB *base, AuroraRGB *composite, double shade_ratio);
G_GNUC_INTERNAL void aurora_shade_shift_hsb (const AuroraHSB *base, AuroraRGB *composite, double shade_ratio);
	
G_GNUC_INTERNAL GtkWidget*        aurora_get_parent_window       (GtkWidget    *widget);
G_GNUC_INTERNAL GdkColor*         aurora_get_parent_bgcolor      (GtkWidget    *widget);
G_GNUC_INTERNAL gboolean          mrn_is_combo_box         (GtkWidget    *widget);
G_GNUC_INTERNAL GtkWidget*        mrn_find_combo_box_widget(GtkWidget    *widget);

G_GNUC_INTERNAL void              aurora_gdk_color_to_rgb   (GdkColor     *c,
                                                 double       *r,
                                                 double       *g,
                                                 double       *b);

G_GNUC_INTERNAL void  aurora_gtk_treeview_get_header_index (GtkTreeView  *tv,
                                                 GtkWidget    *header,
                                                 gint         *column_index,
                                                 gint         *columns,
                                                 gboolean     *resizable,
                                                 gboolean     *sorted);

G_GNUC_INTERNAL void  aurora_gtk_clist_get_header_index    (GtkCList     *clist,
                                                 GtkWidget    *button,
                                                 gint         *column_index,
                                                 gint         *columns);

G_GNUC_INTERNAL void  aurora_get_parent_bg      (const GtkWidget *widget,
                                                 AuroraRGB      *color);

G_GNUC_INTERNAL void  aurora_option_menu_get_props         (GtkWidget       *widget,
                                                 GtkRequisition  *indicator_size,
                                                 GtkBorder       *indicator_spacing);

G_GNUC_INTERNAL gboolean          mrn_is_panel_widget            (GtkWidget *widget);


G_GNUC_INTERNAL gboolean  aurora_object_is_a (const GObject * object, const gchar * type_name);
G_GNUC_INTERNAL gboolean  aurora_is_combo_box_entry (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_combo_box (GtkWidget * widget, gboolean as_list);
G_GNUC_INTERNAL gboolean aurora_is_tree_column_header (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_combo (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_in_combo_box (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_toolbar_item (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_panel_widget_item (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_is_bonobo_dock_item (GtkWidget * widget);
G_GNUC_INTERNAL GtkWidget* aurora_find_combo_box_widget_parent (GtkWidget * widget);
G_GNUC_INTERNAL gboolean  aurora_widget_is_ltr (GtkWidget *widget);
G_GNUC_INTERNAL void  aurora_gdk_color_to_cairo (const GdkColor *c, AuroraRGB *cc);
G_GNUC_INTERNAL void  aurora_mix_color (const AuroraRGB * a, const AuroraRGB * b, gdouble mix_factor, AuroraRGB * c);

#endif /* SUPPORT_H */
