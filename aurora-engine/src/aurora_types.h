#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

typedef unsigned char boolean;
typedef unsigned char uint8;

typedef enum
{
	AUR_STATE_NORMAL,
	AUR_STATE_ACTIVE,
	AUR_STATE_SELECTED,
	AUR_STATE_INSENSITIVE
} AuroraStateType;

typedef enum
{
	AUR_CORNER_NONE        = 0,
	AUR_CORNER_TOPLEFT     = 1,
	AUR_CORNER_TOPRIGHT    = 2,
	AUR_CORNER_BOTTOMLEFT  = 4,
	AUR_CORNER_BOTTOMRIGHT = 8,
	AUR_CORNER_ALL         = 15
} AuroraCorners;

typedef enum
{
	AUR_JUNCTION_NONE      = 0,
	AUR_JUNCTION_BEGIN     = 1,
	AUR_JUNCTION_END       = 2
} AuroraJunction;

typedef enum
{
	AUR_STEPPER_UNKNOWN    = 0,
	AUR_STEPPER_A          = 1,
	AUR_STEPPER_B          = 2,
	AUR_STEPPER_C          = 4,
	AUR_STEPPER_D          = 8
} AuroraStepper;

typedef enum
{
	AUR_ORDER_FIRST,
	AUR_ORDER_MIDDLE,
	AUR_ORDER_LAST,
	AUR_ORDER_FIRST_AND_LAST
} AuroraOrder;

typedef enum
{
	AUR_ORIENTATION_LEFT_TO_RIGHT,
	AUR_ORIENTATION_RIGHT_TO_LEFT,
	AUR_ORIENTATION_BOTTOM_TO_TOP,
	AUR_ORIENTATION_TOP_TO_BOTTOM
} AuroraOrientation;

typedef enum
{
	AUR_GAP_LEFT,
	AUR_GAP_RIGHT,
	AUR_GAP_TOP,
	AUR_GAP_BOTTOM
} AuroraGapSide;

typedef enum
{
	AUR_SHADOW_NONE,
	AUR_SHADOW_IN,
	AUR_SHADOW_OUT,
	AUR_SHADOW_ETCHED_IN,
	AUR_SHADOW_ETCHED_OUT
} AuroraShadowType;

typedef enum
{
	AUR_HANDLE_TOOLBAR,
	AUR_HANDLE_SPLITTER
} AuroraHandleType;

typedef enum
{
	AUR_ARROW_NORMAL,
	AUR_ARROW_COMBO,
	AUR_ARROW_COMBO_2
} AuroraArrowType;

typedef enum
{
	AUR_DIRECTION_UP,
	AUR_DIRECTION_DOWN,
	AUR_DIRECTION_LEFT,
	AUR_DIRECTION_RIGHT
} AuroraDirection;


typedef enum
{
	AUR_PROGRESSBAR_CONTINUOUS,
	AUR_PROGRESSBAR_DISCRETE
} AuroraProgressBarStyle;

typedef enum
{
	AUR_WINDOW_EDGE_NORTH_WEST,
	AUR_WINDOW_EDGE_NORTH,
	AUR_WINDOW_EDGE_NORTH_EAST,
	AUR_WINDOW_EDGE_WEST,
	AUR_WINDOW_EDGE_EAST,
	AUR_WINDOW_EDGE_SOUTH_WEST,
	AUR_WINDOW_EDGE_SOUTH,
	AUR_WINDOW_EDGE_SOUTH_EAST  
} AuroraWindowEdge;

typedef struct
{
	double r;
	double g;
	double b;
} AuroraRGB;

typedef struct
{
	double h;
	double s;
	double b;
} AuroraHSB;

typedef struct
{
	double x;
	double y;
	double width;
	double height;
} AuroraRectangle;

typedef struct
{
	AuroraRGB bg[5];
	AuroraRGB base[5];
	AuroraRGB text[5];
	AuroraRGB shade[9];
		
	AuroraRGB highlight[6];
	AuroraRGB midtone[3];
	AuroraRGB shadow;
		
	AuroraRGB spot[3];
		
} AuroraColors;

typedef struct
{
	boolean active;
	boolean prelight;
	boolean disabled;
	boolean focus;
	boolean is_default;
	
	AuroraStateType state_type;
	
	double curvature;
	
	uint8 corners;
	uint8 xthickness;
	uint8 ythickness;

	AuroraStateType prev_state_type;
	double trans;
	boolean ltr;
	//boolean composited;

} WidgetParameters;

typedef struct
{
	boolean inverted;
	boolean horizontal;
	int fill_size;
} SliderParameters;

typedef struct
{
	AuroraOrientation orientation;
	AuroraProgressBarStyle style;
} ProgressBarParameters;

typedef struct
{
	int linepos;
} OptionMenuParameters;

typedef struct
{
	AuroraShadowType shadow;
	AuroraGapSide gap_side;
	int gap_x;
	int gap_width;
	AuroraRGB *border;
	boolean use_fill;
	boolean fill_bg;
} FrameParameters;

typedef struct
{
	AuroraGapSide gap_side;
	boolean first_tab;
	boolean last_tab;

} TabParameters;

typedef struct
{
	boolean inconsistent;
	boolean draw_bullet;
} OptionParameters;



typedef struct
{
	AuroraCorners    corners;
	AuroraShadowType shadow;	
} ShadowParameters;

typedef struct
{
	boolean horizontal;
} SeparatorParameters;

typedef struct
{
	AuroraOrder   order;
	boolean        resizable;
	boolean sorted;
	int 				   style;
} ListViewHeaderParameters;

typedef struct
{
	boolean            horizontal;
	int								 style;
} ScrollBarParameters;

typedef struct
{
	AuroraHandleType type;
	boolean              horizontal;
} HandleParameters;

typedef struct
{
	AuroraStepper stepper;         /* Which stepper to draw */
} ScrollBarStepperParameters;

typedef struct
{
	AuroraArrowType type;
	AuroraDirection direction;
	double size;
} ArrowParameters;


typedef struct
{
	boolean isCombo;
} EntryParameters;

typedef struct
{
	AuroraWindowEdge edge;
} ResizeGripParameters;

#define AURORA_RECTANGLE_SET(rect, _x, _y, _w, _h) rect.x      = _x; \
                                                       rect.y      = _y; \
                                                       rect.width  = _w; \
                                                       rect.height = _h;

#endif /* AURORA_TYPES_H */
