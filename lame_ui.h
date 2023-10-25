/*
 *  Created on: 02-Apr-2020
 */
/**
 * @file lame_ui.h
 * @author Avra Mitra
 * @brief One and only header file for LameUI library.
 * @version 2.0
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2020-2023
 *
 */

#ifndef INC_LAME_UI_H_
#define INC_LAME_UI_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*--------------------------------------------
 *				User Config
 *--------------------------------------------
 */

/**
 * @defgroup LUI_USER_CONFIG User Configuration
 * @brief User may change configuration using these macros. All these macros
 * can be found in `lame_ui.h` file.
 * @{
 */

#define LUI_MAX_OBJECTS 200 ///< Set number of maximum objects that can be created

// #define LUI_USE_DARK_THEME 0 ///< Comment out OR set value to 0 for using light theme

/**
 * @defgroup LUI_USE_OBJECT Widgets to use
 * @brief Defines which widgets to use. Only if the relevant macro is defined,
 * that widget will be compiled. Comment out the object that you don't want to
 * use. This helps to save Flash.
 *
 * @note Some widgets, that are not in this list, are always compiled and can
 * not be disabled.
 *
 * Also see: @ref LUI_OBJ
 * @{
 */
#define LUI_USE_LINECHART  ///< Compile Linechart widget
#define LUI_USE_SWITCH	   ///< Compile Switch widget
#define LUI_USE_CHECKBOX   ///< Compile Checkbox widget
#define LUI_USE_SLIDER	   ///< Compile Slider widget
#define LUI_USE_LIST	   ///< Compile List widget
#define LUI_USE_PANEL	   ///< Compile Panel widget
#define LUI_USE_TEXTBOX	   ///< Compile Textbox widget
#define LUI_USE_BUTTONGRID ///< Compile Buttongrid widget
#if defined(LUI_USE_BUTTONGRID)
	#define LUI_USE_KEYBOARD ///< Compile Keyboard  widget. (Note: To use keyboard, buttongrid must be used)
#endif
/**@} */

/**@} */

/*--------------------------------------------
 *					End Config
 *--------------------------------------------
 */

/*----------------------------------------------------
 * ------- !!! Don't edit anything below !!! ---------
 *----------------------------------------------------
 */

/*--------------------------------------------
 *				Macro Definitions
 *--------------------------------------------
 */

#define LUI_INPUT_TYPE_TOUCH 1
// #define LUI_INPUT_TYPE_ENCODER				2

/**
 * @defgroup LUI_STATE LameUI input states
 * @brief All input states of LameUI
 *
 * Also see: @ref LUI_EVENT
 * @{
 */
#define LUI_STATE_IDLE        0  ///< Idle state. Object is not under the pointing device
#define LUI_STATE_SELECTED    1  ///< Object is under the pointing device
#define LUI_STATE_PRESSED     2  ///< Object is under the pointing device and the pointing device button is pressed
#define LUI_STATE_ENTERED     3  ///< Object is in entered state. Example: Text box is clicked
/**@} */

/**
 * @defgroup LUI_EVENT LameUI input events
 * @brief All input events of LameUI. Events depend on previous and current state
 *
 * Also see: @ref LUI_STATE
 * @{
 */
#define LUI_EVENT_NONE           0 ///< No event occurred
#define LUI_EVENT_SELECTED       1 ///< Object is selected (under the pointing device)
#define LUI_EVENT_SELECTION_LOST 2 ///< Selection of object is lost (no more under the pointing device)
#define LUI_EVENT_PRESSED        3 ///< Object is pressed (object under pointing device and pointing device button is low)
#define LUI_EVENT_RELEASED       4 ///< Object is released (object under pointing device but pointing device button is released)
#define LUI_EVENT_ENTERED        5 ///< Object is entered to edit/input mode. Example: Clicking on a text box
#define LUI_EVENT_EXITED         6 ///< Opposite of above. Example: Exiting edit mode of a text box
#define LUI_EVENT_VALUE_CHANGED  7 ///< Value of a object is changed. Example: a slider is dragged
#define LUI_EVENT_CHECK_CHANGED  7 ///< Check status of an object is changed. Example: checkbox is checked/unchecked
/**@} */

/**
 * @defgroup LUI_OBJ LameUI object types
 * @brief All types of objects (widgets) in LameUI
 * @{
 */
#define LUI_OBJ_NONE 0			///< No type
#define LUI_OBJ_LABEL 1			///< Label widget. See: \ref lui_label
#define LUI_OBJ_BUTTON 2		///< Button widget. See: \ref lui_button
#define LUI_OBJ_SWITCH 3		///< Switch widget. See: \ref lui_switch
#define LUI_OBJ_LINECHART 4 	///< Line Chart widget. See: \ref lui_linechart
#define LUI_OBJ_PANEL 5			///< Panel widget. See: \ref lui_panel
#define LUI_OBJ_SCENE 6			///< Scene widget. See: \ref lui_scene
#define LUI_OBJ_GENERIC 7		///< Generic widget (user defined widget)
#define LUI_OBJ_LIST 8			///< List widget. See: \ref lui_list
#define LUI_OBJ_CHECKBOX 9		///< Checkbox widget. See: \ref lui_checkbox
#define LUI_OBJ_SLIDER 10		///< Slider widget. See: \ref lui_slider
#define LUI_OBJ_BTNGRID 11		///< Buttongrid widget. See: \ref lui_btngrid
#define LUI_OBJ_TEXTBOX 12		///< Textbox widget. See: \ref lui_textbox
/**@} */

#define LUI_LAYER_POPUP 255
#define LUI_LAYER_SYSTEM 254

/**
 * @defgroup LUI_KEYBOARD_MODE Keyboard modes
 * @brief Keyboard modes of LameUI
 * @{
 */
#define LUI_KEYBOARD_MODE_TXT_LOWER 1 ///< lowercase text mode
#define LUI_KEYBOARD_MODE_TXT_UPPER 2 ///< UPPERCASE TEXT MODE
#define LUI_KEYBOARD_MODE_TXT_SPCL 3  ///< $pec!a1 charac+er m0d3;
/**@} */


/**
 * @defgroup LUI_ICONS Default built-in icons
 * @brief These are default icons used by LameUI. Not to be modified by user.
 *
 * Icons can be used with strings like this:
 * `char* str = LUI_ICON_ARROW_FORWARD "My Home" LUI_ICON_HOME`.
 * Range of default icon index is 0x01 (1) to 0x1D (29). This MUST not be changed by user.
 * User may add custom icons when creating font.
 * Range of user defined icons is 0x80 (128) to 0xFF (255).
 *
 * Note: Range 0x20 (32) to 0x7F (127) must be kept free for ASCII characters.
 *
 * @{
 */
#define LUI_ICON_HOME "\x01"
#define LUI_ICON_RELOAD "\x02"
#define LUI_ICON_CARET_BACK "\x03"
#define LUI_ICON_LOCATION "\x04"
#define LUI_ICON_BATTERY_FULL "\x05"
#define LUI_ICON_CHECKMARK "\x06"
#define LUI_ICON_RETURN_DOWN_BACK "\x07"
#define LUI_ICON_ARROW_DOWN "\x08"
#define LUI_ICON_BACKSPACE "\x09"
#define LUI_ICON_PAUSE "\x0A"
#define LUI_ICON_REMOVE "\x0B"
#define LUI_ICON_POWER "\x0C"
#define LUI_ICON_CARET_UP "\x0D"
#define LUI_ICON_VOLUME_MEDIUM "\x0E"
#define LUI_ICON_ARROW_BACK "\x0F"
#define LUI_ICON_CLOSE "\x10"
#define LUI_ICON_BLUETOOTH "\x11"
#define LUI_ICON_CARET_FORWARD "\x12"
#define LUI_ICON_VOLUME_OFF "\x13"
#define LUI_ICON_ARROW_FORWARD "\x14"
#define LUI_ICON_BATTERY_CHARGING "\x15"
#define LUI_ICON_ARROW_UP "\x16"
#define LUI_ICON_WIFI "\x17"
#define LUI_ICON_SEARCH "\x18"
#define LUI_ICON_WARNING "\x19"
#define LUI_ICON_SETTINGS "\x1A"
#define LUI_ICON_ADD "\x1B"
#define LUI_ICON_BATTERY_DEAED "\x1C"
#define LUI_ICON_CARET_DOWN "\x1D"
/**@} */

/**
 * @defgroup LUI_BTNGRID_MASKS Buttongrid button property masks
 * @brief Property byte masks of a buttongrid. Masks set different properties of
 * a button in a buttongrid.
 * @{
 */
#define LUI_BTNGRID_MASK_BTN_DISABLED 0x40	 ///< Button disabled mask
#define LUI_BTNGRID_MASK_BTN_HIDDEN 0x20	 ///< Button hidden mask
#define LUI_BTNGRID_MASK_BTN_CHECKABLE 0x10	 ///< Button checkable mask
#define LUI_BTNGRID_MASK_BTN_CHECKED 0x80	 ///< Button checked mask
#define LUI_BTNGRID_MASK_BTN_WIDTH_UNIT 0x0F ///< Button width mask
/**@} */

/**
 * @defgroup LUI_ALIGNMENT Content alignment flags
 * @brief Content alignment flags
 * @{
 */
#define LUI_ALIGN_LEFT 0		///< Align content to left
#define LUI_ALIGN_CENTER 1		///< Align content to center
#define LUI_ALIGN_RIGHT 2		///< Align content to right
/**@} */

/**
 * @defgroup LUI_LAYOUT Widget layout flags
 * @brief Widget layout flags to use with panels and scenes only.
 * @{
 */
#define LUI_LAYOUT_NONE        0   ///< No layout
#define LUI_LAYOUT_HORIZONTAL  1   ///< Horizontal layout
#define LUI_LAYOUT_VERTICAL    2   ///< Vertical layout
/**@} */

/**
 * @defgroup LUI_SLIDER_KNOB_TYPE Slider widget knob type flag
 * @brief Knob type flag for slider widget. It defines how the knob of a slider 
 * is rendered.
 * @{
 */
#define LUI_SLIDER_KNOB_TYPE_NONE		0	///< No knob will be rendered
#define LUI_SLIDER_KNOB_TYPE_DEFAULT	1	///< Default knob (a rectangle) will be rendered
#define LUI_SLIDER_KNOB_TYPE_TEXT		2	///< Text (value and/or custom text) will be rendered
/**@} */

/**
 * @defgroup LUI_LINECHART_DRAW_MODE Linechart widget draw mode
 * @brief Draw mode flag for linechart widget. It defines how the line is drawn.
 * These flags can be ORed.
 * @{
 */
#define LUI_LINECHART_DRAW_MODE_LINE	(1 << 0)	///< Draw the lines by connection points
#define LUI_LINECHART_DRAW_MODE_POINT	(1 << 1)	///< Draw the chart points/markers
/**@} */

/**
 * @defgroup LUI_DEFAULT_FONT	LameUI default font
 * @brief This is default font of LameUI. User can access default font using this macro.
 * @{
 */
#define LUI_DEFAULT_FONT		FONT_lui_default
/**@} */

/*--------------------------------------------
 *				End Macro Definitions
 *--------------------------------------------
 */

/*--------------------------------------------
 *				Typedefs and Structs
 *--------------------------------------------
 */

#pragma pack(push, 1)

/** 
 * @defgroup PublicTypedefs Custom public datatypes. 
 * @brief Custom public datatypes
 */

typedef struct
{
	uint8_t* mem_block;
	uint32_t block_max_sz;
	uint32_t mem_allocated;
} _lui_mem_block_t;


/**
 * @ingroup PublicTypedefs
 * @brief bitmap datatype. Used to render images. Generated using LameUI font tools
 * 
 * Bitmap must be generated with LameUI tools (see docs). Supported 
 * bits-per-pixel (bpp) are 1, 8, and 16.
 * @{
 */
typedef struct _lui_bitmap_s
{
	const uint8_t* const payload;	///< Bitmap payload
	const uint16_t size_x;			///< Bitmap size in x axis
	const uint16_t size_y;			///< Bitmap size in y axis
	const uint8_t bpp;				///< Bits-per-pixels (1, 8, 16)
} lui_bitmap_t;
/**@} */

/* Color palette for 1-bpp mono bitmap image.  */
/**
 * @ingroup PublicTypedefs
 * @brief Color palette for 1-bpp mono bitmap image.
 * 
 * This color palette has NO effect on 8-bpp grayscale and 16-vpp rgb565 bitmaps.
 * @{
 */
typedef struct _lui_bitmap_mono_pal_s
{
	uint16_t fore_color;	///< foreground color of 1-bpp bitmap
	uint16_t back_color;	///< background color of 1-bpp bitmap
	uint8_t is_backgrnd;	///< 0: Don't draw any background color, 1: Draw background color
} lui_bitmap_mono_pal_t;
/**@} */

/* This is a font glyph description - for now it does not support kerning */
/* See: https://freetype.org/freetype2/docs/glyphs/glyphs-3.html */
typedef struct
{
	const uint16_t payload_index;
	const char character; // ASCII code
	const uint8_t width;  // glyph bbox width
	const uint8_t x_adv;  // glyph advance x
	const uint8_t x_off;  // glyph left bearing x
} _lui_glyph_t;

/**
 * @ingroup PublicTypedefs
 * @brief font datatype. Generated using LameUI font tools.
 * 
 * Users must NOT read/write members directly. All actions are done using 
 * getter and setter functions
 * @{
 */
typedef struct _lui_font_s
{
	const lui_bitmap_t* const bitmap;
	const uint8_t glyph_count;
	const _lui_glyph_t* glyphs; // pointer to array of glyph_t elements
} lui_font_t;
/**@} */

extern const lui_font_t LUI_DEFAULT_FONT;

/* !! For internal private use only. This was an old design
 * choice and should be replaced with `lui_area_t` in future.
 */
typedef struct _lui_area_priv_s
{
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} _lui_area_priv_t;

/**
 * @ingroup PublicTypedefs
 * @brief Area datatype.
 *
 * lui_area_t is used to define area or size of an item.
 * @{
 */
typedef struct _lui_area_s
{
	uint16_t x;			///< Start position in X axis
	uint16_t y;			///< Start position in Y axis
	uint16_t w;			///< Width of item
	uint16_t h;			///< Height of item
} lui_area_t;
/**@} */

struct _lui_layout_s
{
	uint8_t type;
	uint8_t pad_x;
	uint8_t pad_y;
	uint16_t dim;
// 	uint8_t align;
};

struct _lui_common_style_s
{
	uint16_t bg_color;
	uint16_t border_color;
	uint16_t width;
	uint16_t height;
	uint8_t border_width;
};

struct _lui_label_style_s
{
	uint16_t text_color;
	uint8_t is_transparent_bg;
};
struct _lui_button_style_s
{
	uint16_t label_color;
	uint16_t label_pressed_color;
	uint16_t pressed_color;
	uint16_t selection_color;
	uint8_t is_transparent_bg;
};

#if defined(LUI_USE_SWITCH)
struct _lui_switch_style_s
{
	uint16_t knob_on_color;
	uint16_t knob_off_color;
	uint16_t selection_color;
};
#endif

#if defined(LUI_USE_CHECKBOX)
struct _lui_checkbox_style_s
{
	uint16_t tick_color;
	uint16_t bg_checked_color;
	uint16_t selection_color;
};
#endif

#if defined(LUI_USE_SLIDER)
struct _lui_slider_style_s
{
	uint16_t knob_color;
	uint16_t bg_filled_color;
	uint16_t selection_color;
	uint8_t knob_width;
};
#endif

#if defined(LUI_USE_LINECHART)
struct _lui_linechart_style_s
{
	uint16_t line_color;
	uint16_t grid_color;
	uint16_t point_color;
	uint8_t grid_visible;
	uint8_t line_width;
	uint8_t point_width;
	uint8_t draw_mode;
};
#endif

#if defined(LUI_USE_LIST)
struct _lui_list_style_s
{
	uint16_t item_label_color;
	uint8_t item_has_border;
	uint16_t item_border_color;
};
#endif

#if defined(LUI_USE_BUTTONGRID)
struct _lui_btngrid_style_s
{
	uint8_t btn_margin_hor;
	uint8_t btn_margin_vert;
	uint16_t btn_label_color;
	uint16_t btn_pressed_color;
	uint16_t btn_bg_color;
};
#endif

#if defined(LUI_USE_TEXTBOX)
struct _lui_textbox_style_s
{
	uint16_t text_color;
	uint16_t bg_filled_color;
};
#endif

/**
 * @ingroup PublicTypedefs
 * @brief Generic object datatype. Accessed only through getter and setter functions.
 *
 * lui_obj_t contains all the common data of objects along with address of
 * the extended data as per the type of the object
 * @{
 */
typedef struct _lui_obj_s
{
	uint16_t x;									  ///< Horizontal position (px)
	uint16_t y;									  ///< Vertical position (px)
	uint8_t layer;								  ///< Rendering layer (0 - 128)
	struct _lui_common_style_s common_style;	  ///< Common style properties of objects
	uint8_t state;								  ///< Input state
	uint8_t event;								  ///< Input event
	int32_t value;								  ///< User defined value
	void (*obj_event_cb)(struct _lui_obj_s* obj); ///< Input event callback function

	uint8_t needs_refresh;			 ///< Object refresh flag
	uint8_t visible;				 ///< Object visibility flag
	uint8_t enabled;				 ///< Object input enable flag
	uint8_t obj_type;				 ///< Object type
	struct _lui_obj_s* parent;		 ///< Parent of object
	uint8_t children_count;			 ///< Object's children count
	struct _lui_obj_s* first_child;	 ///< First child of object
	struct _lui_obj_s* next_sibling; ///< Next sibling of object

	void *obj_main_data; ///< Main (extended) data of the object
} lui_obj_t;
/**@} */

#if defined(LUI_USE_LINECHART)
typedef struct _lui_linechart_s
{
	struct
	{
		uint8_t hor_count;
		uint8_t vert_count;
		// uint16_t color;
		// uint8_t is_grid;
	} grid;

	struct _lui_linechart_style_s style;

	struct
	{
		double* source;
		uint16_t points;
		uint8_t auto_scale;
		double y_max_value;
		double y_min_value;
	} data;

	// uint16_t color;
	const lui_font_t* font;
} lui_chart_t;
#endif

typedef struct _lui_label_s
{
	char* text;
	const lui_font_t* font;
	struct _lui_label_style_s style;
} lui_label_t;

typedef struct _lui_button_s
{
	struct
	{
		const lui_font_t* font;
		const char* text;
		const char* text_pressed;
		uint8_t text_align;
	} label;
	struct _lui_button_style_s style;
	const lui_bitmap_t* img_idle;
	const lui_bitmap_t* img_pressed;
	/* Only when images are mono (1-bpp) */
	lui_bitmap_mono_pal_t img_idle_pal;
	lui_bitmap_mono_pal_t img_press_pal;

	uint8_t is_checkable;
} lui_button_t;

#if defined(LUI_USE_SWITCH)
typedef struct _lui_switch_s
{

	struct _lui_switch_style_s style;
} lui_switch_t;
#endif

#if defined(LUI_USE_CHECKBOX)
typedef struct _lui_checkbox_s
{
	struct _lui_label_s label;
	struct _lui_checkbox_style_s style;
} lui_checkbox_t;
#endif

#if defined(LUI_USE_SLIDER)
typedef struct _lui_slider_s
{
	struct _lui_slider_style_s style;
	int16_t range_min;
	int16_t range_max;
	uint8_t knob_type;
	// knob's center's distance relative to slider's start position. Measured along x axis for horizontal slider (and y for vertical).
	uint16_t knob_center_rel_d;
	const char* custom_text;
	uint8_t show_value;
	const lui_font_t* font;
	uint8_t is_progress_bar;
} lui_slider_t;
#endif

#if defined(LUI_USE_LIST)
struct _lui_list_item
{
	const char* text;
	_lui_area_priv_t area;
};
typedef struct _lui_list_s
{
	struct _lui_list_item** items;
	struct _lui_list_style_s style;
	const lui_font_t* font;
	int16_t selected_item_index;
	uint8_t page_count;
	uint8_t current_page_index;
	uint8_t items_per_page;
	uint8_t page_first_item_index;
	uint8_t item_min_height;
	uint8_t text_align;
	uint8_t max_items;
	uint8_t items_cnt;
	uint8_t is_dropdown;
	uint8_t is_expanded;
} lui_list_t;
#endif

#if defined(LUI_USE_KEYBOARD)
typedef struct _lui_keyboard_s
{
	lui_obj_t* target_txtbox;
	uint8_t keyboard_mode;

} lui_keyboard_t;
#endif

#if defined(LUI_USE_BUTTONGRID)
typedef struct _lui_btngrid_s
{
	const char* *texts;
	uint8_t* btn_properties;
	_lui_area_priv_t* btn_area;
	uint8_t btn_cnt;
	uint8_t row_cnt;
	const lui_font_t* font;
	int16_t active_btn_index;
	uint8_t needs_full_render;
	struct _lui_btngrid_style_s style;
#if defined(LUI_USE_KEYBOARD)
	struct _lui_keyboard_s *kb_data;
#endif
} lui_btngrid_t;
#endif

#if defined(LUI_USE_TEXTBOX)
typedef struct _lui_textbox_s
{
	char* text_buffer;
	const lui_font_t* font;
	uint16_t max_len;
	uint16_t caret_index;
	uint16_t used_chars;
	uint8_t needs_full_render;
	struct _lui_textbox_style_s style;
} lui_textbox_t;
#endif

#if defined(LUI_USE_PANEL)
typedef struct _lui_panel_s
{
	struct _lui_layout_s layout;
	const lui_bitmap_t* bg_image;
	/* Color palette for only when image is 1-bpp mono bitmap */
	lui_bitmap_mono_pal_t img_pal;
} lui_panel_t;
#endif

typedef struct _lui_scene_s
{
	struct _lui_layout_s layout;
	const lui_bitmap_t* bg_image;
	/* Color palette for only when image is 1-bpp mono bitmap */
	lui_bitmap_mono_pal_t img_pal;
	const lui_font_t* font;

} lui_scene_t;

/**
 * @ingroup PublicTypedefs
 * @brief Touch Input data
 * 
 * touch input device data.
 * 
 * @{
 */
typedef struct _lui_touch_input_data_s
{
	uint8_t is_pressed; ///< 0: NOT pressed, 1: Pressed
	int16_t x;			///< X position of press/touch. -1 if not Pressed/touched
	int16_t y;			///< Y position of press/touch. -1 if not Pressed/touched
} lui_touch_input_data_t;
/**@} */

/**
 * @ingroup PublicTypedefs
 * @brief display driver object.
 * 
 * @{
 */
typedef struct _lui_disp_drv_s
{
	void (*draw_pixels_buff_cb)(uint16_t* disp_buff, lui_area_t* area);	///< draw pixels buffer callback function.
	uint16_t* disp_buff;			///< display buffer. It is an array of uint16_t type.
	uint16_t disp_buff_sz_px;		///< size of display buffer in pixel count (NOT in bytes)
	uint16_t display_hor_res;		///< display horizontal resolution (along x axis)
	uint16_t display_vert_res;		///< display vertical resolution (along y axis)
} lui_dispdrv_t;
/**@} */

/**
 * @ingroup PublicTypedefs
 * @brief touch input device object
 * 
 * The callback function to read input should be implemented by user.
 * 
 * @{
 */
typedef struct _lui_touch_input_dev_s
{
	void (*read_touch_input_cb)(lui_touch_input_data_t* input);	///< callback function to read input data.
	// lui_touch_input_data_t touch_data;
} lui_touch_input_dev_t;
/**@} */

typedef struct _lui_main_s
{
	// lui_obj_t* scenes[LUI_MAX_SCENES];
	const lui_font_t* default_font;
	lui_obj_t* active_scene;
	lui_obj_t* active_obj;
	lui_dispdrv_t* disp_drv;
	lui_touch_input_dev_t* touch_input_dev;
	lui_touch_input_data_t last_touch_data;
	uint8_t input_state_pressed;
	uint8_t input_event_clicked;
	uint8_t total_scenes;
	uint8_t total_created_objects; // increases as new objs are created. It never decreases
} _lui_main_t;

#pragma pack(pop)

/*--------------------------------------------
 *				End Typedefs/Structs
 *--------------------------------------------
 */

/*--------------------------------------------
 *				Function Prototypes
 *--------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				Init and Update functions
 *-------------------------------------------------------------------------------
 */

/**
 * @defgroup lui_core Initialization and UI Update API
 * @brief These functions are used to initialize the core and to update it.
 * - lui_init() is only called once at the very beginning.
 * - lui_update() is called periodically in an infinite loop.
 * @{
 */

/**
 * @brief Initialize the LameUI core. Here user provides LameUI some RAM to
 * create widgets. If and when RAM is not sufficient, widget creation will fail.
 * 
 * NOTE: This memory does NOT include display buffer memory. Display buffer is 
 * set seperately using `lui_dispdrv_set_disp_buff()`.
 * 
 * If init fails due to memory allocation problem, returns -1, else returns 0.
 *
 * @param mem_block an array of uint8_t. this will be used as work area
 * @param size size of the alloted memory
 * @return 0: Success, -1: Failure
 */
int8_t lui_init(uint8_t mem_block[], uint32_t size);

/**
 * @brief This function updates the UI by reading input and rendering widgets
 *  that need rendering.
 *
 * User must call this function periodically (e.g. every 20ms) in a loop so
 * no input event is missed. Also, LameUI renders only when widgets need
 * rendering. So, draw calls are not wasted.
 *
 */
void lui_update();

/**
 * @brief Sets default font of LameUI. 
 * 
 * Objects that are created after calling this, will inherit the new default font. 
 * But objects created earlier won't update their font automatically.
 * 
 * @param font font
 * @return int8_t 0: Success, -1: Fail
 */
int8_t lui_set_default_font(const lui_font_t* font);
/**@}*/

/*-------------------------------------------------------------------------------
 * 				LUI_OBJECT (generic) related functions
 *-------------------------------------------------------------------------------
 */

/**
 * @defgroup lui_object Generic object related API. Works with all widgets.
 * @brief These functions are applicable for all types of objects.
 * Only for a few particular types of objects, one or more of these functions have no effect.
 * @{
 */

/**
 * @brief Add a child object to a parent object, thus grouping them together
 *
 * @code
 * // Creating a scene which will be a parent object
 * lui_obj_t* parent_scene = lui_scene_create();
 * lui_scene_set_active(parent_scene);
 *
 * // Creating a label which will be a child of the parent scene
 * lui_obj_t* child_label = lui_label_create();
 * lui_label_set_text(child_label, "I am child of a scene");
 * lui_object_add_to_parent(child_label, parent_scene);		// Add child to parent
 * @endcode
 *
 * @param obj child object
 * @param parent_obj parent object
 */
void lui_object_add_to_parent(lui_obj_t* obj, lui_obj_t* parent_obj);

/**
 * @brief Remove an object from its current parent
 *
 * @param obj Child object
 */
void lui_object_remove_from_parent(lui_obj_t* obj);

/**
 * @brief Get a particular child by index from a parent
 * 
 * @param obj_parent parent object
 * @param child_index index of child
 * @return lui_obj_t* pointer to child object. Returns NULL if not found
 */
lui_obj_t* lui_object_get_child(lui_obj_t* obj_parent, uint16_t child_index);

/**
 * @brief Set drawing area of an object
 *
 * @param obj target object
 * @param width width
 * @param height height
 */
void lui_object_set_area(lui_obj_t* obj, uint16_t width, uint16_t height);

/**
 * @brief Set width of an object
 *
 * @param obj target object
 * @param width width
 */
void lui_object_set_width(lui_obj_t* obj, uint16_t width);

/**
 * @brief Set height of an object
 *
 * @param obj target object
 * @param height height
 */
void lui_object_set_height(lui_obj_t* obj, uint16_t height);

/**
 * @brief Get object's position relative to its parent
 *
 * @param obj target object
 * @param pos pos array of 2 items to return position ({x, y})
 */
void lui_object_get_position_rel(lui_obj_t* obj, uint16_t pos[2]);

/**
 * @brief Get object's absolute position on the screen
 *
 * @param obj target object
 * @param pos pos array of 2 items to return position ({x, y})
 */
void lui_object_get_position_abs(lui_obj_t* obj, uint16_t pos[2]);

/**
 * @brief Set position of an object. This is the relative position of the
 * object to its parent.
 *
 * @param obj target object
 * @param x x position (relative)
 * @param y y position (relative)
 */
void lui_object_set_position(lui_obj_t* obj, uint16_t x, uint16_t y);

/**
 * @brief Set only x position of an object
 *
 * @param obj target object
 * @param x x position
 */
void lui_object_set_x_pos(lui_obj_t* obj, uint16_t x);

/**
 * @brief Set only y position of an object
 *
 * @param obj target object
 * @param y y position
 */
void lui_object_set_y_pos(lui_obj_t* obj, uint16_t y);

/**
 * @brief Set border color of an object
 *
 * @param obj target object
 * @param border_color border color
 */
void lui_object_set_border_color(lui_obj_t* obj, uint16_t border_color);

/**
 * @brief Deprecated!! Use `lui_object_set_border_width()` instead
 * Set border's visibility of an object
 *
 * @param obj target object
 * @param is_visible 1: visible; 0: invisible
 */
void lui_object_set_border_visibility(lui_obj_t* obj, uint8_t is_visible) __attribute__ ((deprecated));

/**
 * @brief Set border's width or thickness. Set width to 0 to remove border
 *
 * @param obj target object
 * @param width width/thickness value. 0 means invisible
 */
void lui_object_set_border_width(lui_obj_t* obj, uint8_t width);

/**
 * @brief Set background color of an object
 *
 * @param obj target object
 * @param bg_color background color
 */
void lui_object_set_bg_color(lui_obj_t* obj, uint16_t bg_color);

/**
 * @brief Set event call back function for input handling
 *
 * This function is called when an event occurs against this object
 *
 * <b>Example:</b>
 * @code
 * lui_obj_t* my_button;
 * void button_callback(lui_obj_t* btn_obj)
 * {
 *     // Do something
 * }
 * my_button = lui_button_create();
 * lui_object_set_callback(my_button, button_callback);
 * @endcode
 *
 * @param obj target object
 * @param obj_event_cb function pointer of the callback function
 */
void lui_object_set_callback(lui_obj_t* obj, void (*obj_event_cb)(lui_obj_t* ));

/**
 * @brief Get the input state of an object
 *
 * @param obj target object
 * @return int8_t state ID
 */
int8_t lui_object_get_state(lui_obj_t* obj);

/**
 * @brief Get input event of an object
 *
 * @param obj target object
 * @return int8_t event ID
 */
int8_t lui_object_get_event(lui_obj_t* obj);

/**
 * @brief Set visibility of an object
 *
 * @param obj target object
 * @param is_visible 1: visible; 0: hidden
 */
void lui_object_set_visibility(lui_obj_t* obj, uint8_t is_visible);

/**
 * @brief Set rendering layer index of an object
 *
 * Objects in higher layer will be rendered over the objects in lower layer.
 * If 2 or more objects are in same layer, they're rendered based on the sequence
 * they're added to parent.
 *
 * @param obj target object
 * @param layer_index layer index (0 - 128)
 */
void lui_object_set_layer(lui_obj_t* obj, uint8_t layer_index);

/**
 * @brief Get rendering layer index of an object
 *
 * @param obj target object
 * @return int16_t layer index. Returns -1 if object is NULL
 */
int16_t lui_object_get_layer(lui_obj_t* obj);

/**
 * @brief Get the type of the object
 * 
 * @param obj object
 * @return int16_t type of object. -1 if object is NULL
 */
int16_t lui_object_get_type(lui_obj_t* obj);

/**
 * @brief Enable or disble input handling of an object. If disabled,
 * object won't cause input event callback
 *
 * @param obj target object
 * @param is_enabled 1: input enabled; 0: input disabled
 * @return uint8_t 1: success; 0: failed
 */
uint8_t lui_object_set_enable_input(lui_obj_t* obj, uint8_t is_enabled);

/* Private functions (User must not call them) */

/**
 * @private
 * @brief Create a generic object with default values
 *
 * @return lui_obj_t* created object
 */
lui_obj_t* _lui_object_create(void);

/**
 * @private
 * @brief Set needs_refresh flag for an object. This flag determines if onject will be redrawn
 *
 * When setting this flag for an object, flags of children and/or parent might also be set,
 * depending on the requirement.
 *
 * @param obj target object
 */
void _lui_object_set_need_refresh(lui_obj_t* obj);

/**
 * @private
 * @brief Render an object along with all its children (if any)
 *
 * @param obj target object
 */
void _lui_object_render_parent_with_children(lui_obj_t* obj);

/**
 * @private
 * @brief Render a single object (and NOT its children)
 *
 * @param obj target object
 */
void _lui_object_render(lui_obj_t* obj);

/**
 * @private
 * @brief Compares two objects' layers and returns (layer1 - layer2)
 *
 * @param p1 pointer to object 1
 * @param p2 pointer to object 2
 * @return int (layer1 - layer2)
 */
int _lui_obj_layer_cmprtr(const void *p1, const void *p2);
/* Private functions end */

/**@}*/

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_LABEL related functions
 *-------------------------------------------------------------------------------
 */

/**
 * @defgroup lui_label Label widget API
 * @brief API for <tt><b>label</b></tt> widgets
 *
 * @section label_example Example
 * @image html docs/widgets_images/label.png "Dark Theme"
 * @image html docs/widgets_images/label_2.png "Light Theme"
 * @code
 * const char* text = "Hi Universe";
 * lui_obj_t* my_label = lui_label_create();
 * lui_label_set_text(my_label, "Hello World");
 * lui_label_set_text_color(my_label, lui_rgb(255, 20, 80));	// Setting text color
 * lui_object_set_bg_color(my_label, lui_rgb(10, 10, 10));		// Setting background color
 * 
 * lui_obj_t* my_label2 = lui_label_create_and_add(parent_panel); // Creating and adding to a parent
 * lui_label_set_text(my_label2, text);
 * lui_label_set_font(my_label2, &FONT_montserrat_32);
 * lui_object_set_position(my_label2, 0, 30);
 * @endcode
 * @{
 */

/**
 * @brief Create a label with initial values
 *
 * @return lui_obj_t* Created label object
 */
lui_obj_t* lui_label_create(void);

/**
 * @brief Create a label with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created label object
 */
lui_obj_t* lui_label_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a label widget
 *
 * @param obj_lbl label object
 */
void lui_label_draw(lui_obj_t* obj_lbl);

/**
 * @brief Set font of a label.
 *
 * @param obj_lbl label object
 * @param font font object. If NULL is passed, default font will be used
 */
void lui_label_set_font(lui_obj_t* obj_lbl, const lui_font_t* font);

/**
 * @brief Set text of a label
 *
 * @param obj_lbl label object
 * @param text char array of text
 */
void lui_label_set_text(lui_obj_t* obj_lbl, const char* text);

/**
 * @brief Set forecolor of a label.
 * @note this only changes text color and NOT background color
 *
 * @param obj_lbl label object
 * @param text_color 16-bit color
 */
void lui_label_set_text_color(lui_obj_t* obj_lbl, uint16_t text_color);

/**
 * @brief Set whether the label background is transparent or not.
 * When background is set to transparent, parent's background color 
 * or bitmap is used as background to simulate transparency.
 * 
 * @param obj label object
 * @param is_transparent 0: NOT transparent, 1: Transparent
 */
void lui_label_set_bg_transparent(lui_obj_t* obj, uint8_t is_transparent);
/**@}*/
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_LINECHART related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_LINECHART)
/**
 * @defgroup lui_linechart Line Chart widget API
 * @brief API for <b><tt>linechart</tt></b> widget
 *
 * @section linechart_example Example
 * @image html docs/widgets_images/linechart.png "Dark Theme"
 * @image html docs/widgets_images/linechart_2.png "Light Theme"
 * @code
 * double fib_data[40] = {0,0, 1,1, 2,1, 3,2, 4,3, 5,5, 6,8, 7,13, 8,21, 9,34};
 * lui_obj_t* my_chart = lui_linechart_create();
 * lui_linechart_set_data_source(my_chart, fib_data, 10);
 * lui_linechart_set_grid_count(my_chart, 4, 2);
 * lui_linechart_set_line_color(my_chart, lui_rgb(255, 0, 0));
 * // Setting line chart's area is important!
 * lui_object_set_area(my_chart, 120, 80);
 * @endcode
 * @{
 */

/**
 * @brief Create a linechart with initial values
 *
 * @return lui_obj_t* Created linechart object
 */
lui_obj_t* lui_linechart_create(void);

/**
 * @brief Create a linechart with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created linechart object
 */
lui_obj_t* lui_linechart_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a linechart
 *
 * @param obj_linechart linechart object
 */
void lui_linechart_draw(lui_obj_t* obj_linechart);

/**
 * @brief Set number of horizontal and vertical grid lines in a linechart
 *
 * @param obj_linechart linechart object
 * @param hor_lines number of horizontal grid lines
 * @param vert_lines number of vertical grid lines
 */
void lui_linechart_set_grid_count(lui_obj_t* obj_linechart, uint16_t hor_lines, uint16_t vert_lines);

/**
 * @brief Set color of grid lines
 *
 * @param obj_linechart linechart object
 * @param color 16-bit color
 */
void lui_linechart_set_grid_color(lui_obj_t* obj_linechart, uint16_t color);

/**
 * @brief Set visibility of grid lines
 *
 * @param obj_linechart linechart object
 * @param state 0: hidden, 1: visible (default)
 */
void lui_linechart_set_grid_visible(lui_obj_t* obj_linechart, uint8_t state);

/**
 * @brief Set color of plot line
 *
 * @param obj_linechart linechart object
 * @param line_color 16-bit color
 */
void lui_linechart_set_line_color(lui_obj_t* obj_linechart, uint16_t line_color);

/**
 * @brief Set width of plot line
 *
 * @param obj_linechart linechart object
 * @param line_width width of line
 */
void lui_linechart_set_line_width(lui_obj_t* obj_linechart, uint8_t line_width);

/**
 * @brief Set color of points/markers of the plot
 *
 * @param obj_linechart linechart object
 * @param point_color 16-bit color
 */
void lui_linechart_set_point_color(lui_obj_t* obj_linechart, uint16_t point_color);

/**
 * @brief Set width of points/markers of plot
 *
 * @param obj_linechart linechart object
 * @param point_width width of points/markers
 */
void lui_linechart_set_point_width(lui_obj_t* obj_linechart, uint8_t point_width);

/**
 * @brief Set draw mode of line chart's plot line
 * Sets whether to draw line segments, or only the points/markers, or both.
 *
 * flags:
 * LUI_LINECHART_DRAW_MODE_LINE  -> draw line segments by connecting points
 * LUI_LINECHART_DRAW_MODE_POINT -> draw points/markers
 *
 * Flags can be ORed to enable both of them.
 *
 * @param obj_linechart linechart object
 * @param mode_flag ORed drawmode flags.
 */
void lui_linechart_set_draw_mode(lui_obj_t* obj_linechart, uint8_t mode_flag);

/**
 * @brief Set whether to apply automatic scaling on the data source or not.
 * If auto scale is enabled, graph will always be in the boundary of drawing area.
 * If auto scale is disabled, user must provide manual range
 * @param obj_linechart linechart object
 * @param auto_scale 1: enabled (default), 0: disabled
 */
void lui_linechart_set_data_auto_scale(lui_obj_t* obj_linechart, uint8_t auto_scale);

/**
 * @brief Set data range of line chart when auto scaling is disabled
 *
 * @param obj_linechart linechart object
 * @param y_min minimum value of y axis data
 * @param y_max maximum value of y axis data
 */
void lui_linechart_set_data_range(lui_obj_t* obj_linechart, double y_min, double y_max);

/**
 * @brief Set data source of the line chart
 * 
 * Data source contains both x and y data. Odd index: X, Even index: 1. That 
 * means, data source array looks like: {x0, y0, x1, y1, ..., xn, yn}
 *
 * @param obj_linechart linechart object
 * @param source array of data points (in x,y format)
 * @param points number of data points
 */
void lui_linechart_set_data_source(lui_obj_t* obj_linechart, double* source, uint16_t points);
/**@}*/
#endif
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/**
 * @defgroup lui_button Button widget API
 *
 * @brief API for <b><tt>button</tt></b> widget
 * 
 * Buttons have 3 important visual properties:
 * 1. Background color (bg color)
 * 2. Bitmap image
 * 3. Text
 * 
 * Each of them can be configured for both pressed state and idle state.
 * 
 * Rendering order is: 1.bg color -> 2.bitmap -> 3.text.
 * 
 * If button is set as transparent bg, then bg color is not rendered, rather parent's 
 * bg color or bitmap ius rendered as the step 1.
 *
 * @section button_example1 Example
 * @image html docs/widgets_images/button.png "Dark Theme"
 * @image html docs/widgets_images/button_2.png "Light Theme"
 * @code
 * void button_callback(lui_obj_t* btn_obj)
 * {
 *     static uint8_t light_state = 0;
 *     // Only turn on/off light when button is pressed. For any other event, return.
 *     if (lui_object_get_event(btn_obj) != LUI_EVENT_PRESSED)
 *         return;
 *     light_state = !light_state;
 *     if (light_state)
 *     {
 *         lui_button_set_label_text(my_button, "Turn OFF Light");
 *         // turn_on_light();
 *     }
 *     else
 *     {
 *         lui_button_set_label_text(my_button, "Turn ON Light");
 *         // turn_off_light();
 *     }
 *
 *     // Do more stuffs...
 * }
 * lui_obj_t* my_button = lui_button_create();
 * lui_button_set_label_text(my_button, "Turn ON Light");
 * // Optionally set button area and position
 * lui_object_set_area(my_button, 140, 60);
 * lui_object_set_position(my_button, 20, 10);
 * // set button call back function
 * lui_object_set_callback(my_button, button_callback);
 * @endcode
 * 
 * @section button_example2 Example of checkable button
 * This code creates a button and sets it as checkable. Also, this button changes its 
 * text and color when pressed.
 * @code
 * lui_obj_t* check_btn = lui_button_create();
 * 
 * // Set is as checkable
 * lui_button_set_checkable(check_btn, 1);
 * // This label text is for both idle and pressed state
 * lui_button_set_label_text(check_btn, "Turn ON Light");
 * // Green text fo idle (normal) state
 * lui_button_set_label_color(check_btn, lui_rgb(0, 255, 0));
 * // This label text is for pressed state
 * lui_button_set_label_text_pressed(check_btn, "Turn OFF Light");
 * // Red text when pressed state 
 * lui_button_set_label_color(check_btn, lui_rgb(255, 0, 0));
 * 
 * // Optionally set button area and position
 * lui_object_set_area(check_btn, 140, 60);
 * lui_object_set_position(check_btn, 20, 10);
 * @endcode
 * 
 * @section button_example3 Example of setting background image
 * Example of setting a background bitmap image for button
 * @code
 * #include "warning_symbol.h"	// include the image bitmap
 * lui_obj_t* img_btn = lui_button_create();
 * // Set area and position of button.
 * lui_object_set_area(img_btn, 140, 60);
 * lui_object_set_position(img_btn, 20, 10);
 * // Now set the bitmap image as background.
 * // WE are only setting bitmap for button idle state. No extra bitmap is set for pressed state (NULL passed)
 * lui_button_set_bitmap_images(img_btn, &BITMAP_warning_symbol, NULL);
 * // NOTE: if we need to set a different bitmap for pressed state too, do this:
 * // lui_button_set_bitmap_images(img_btn, &BITMAP_idle, &BITMAP_pressed);
 * @endcode
 * 
 * @section button_example4 Example of setting 1-bpp mono background image
 * Example of setting a 1-bpp background bitmap image for button. When we set 1-bpp images, 
 * we should call another function `lui_button_set_bitmap_images_mono_palette()` to set 
 * rendering colors of bitmaps too.
 * 
 * The following code creates a square shaped button and sets it as checkable. Then 
 * sets play symbol bitmap for idle state and pause symbol bitmap for pressed state. 
 * These bitmaps are 1-bpp.
 * 
 * Play button's color set to red(ish) and pause button;s color set to green(ish). 
 * background color for both bitmaps are ignored due to `is_bg` flag is set to 0.
 * 
 * @code
 * #include "play_symbol.h"		// 1-bpp mono bitmap
 * #include "pause_symbol.h"	// 1-bpp mono bitmap
 * lui_obj_t* img_btn = lui_button_create();
 * // Make it checkable since it's a play/pause button
 * lui_button_set_checkable(img_btn, 1);
 * // Set area
 * lui_object_set_area(img_btn, 80, 80);
 * // Now set 1-bpp mono bitmap images for idle and pressed states
 * lui_button_set_bitmap_images(img_btn, &BITMAP_play_symbol, &BITMAP_pause_symbol);
 * // Prepare rendering color palettes of both 'idle' and 'pressed' state images
 * lui_bitmap_mono_pal_t idle_palette = {
 *     .fore_color = lui_rgb(255, 10, 10),
 *     .back_color = 0,
 *     .is_backgrnd = 0 // don't want to draw background (sets transparent background)
 * };
 * lui_bitmap_mono_pal_t pressed_palette = {
 *     .fore_color = lui_rgb(5, 250, 10),
 *     .back_color = 0,
 *     .is_backgrnd = 0 // don't want to draw background (sets transparent background)
 * };
 * // Now set those palettes
 * lui_button_set_bitmap_images_mono_palette(img_btn, &idle_palette, &pressed_palette);
 * @endcode
 * 
 * @{
 */

/**
 * @brief Create a button with initial values
 *
 * @return lui_obj_t* created button object
 */
lui_obj_t* lui_button_create(void);

/**
 * @brief Create a button with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created button object
 */
lui_obj_t* lui_button_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a button object
 *
 * @param obj_btn button object
 */
void lui_button_draw(lui_obj_t* obj_btn);

/**
 * @brief Set the text of the button's label.
 * 
 * Same text is set for idle (normal) state and pressed state.
 *
 * @param obj_btn button object
 * @param text char array of text
 */
void lui_button_set_label_text(lui_obj_t* obj_btn, const char* text);

/**
 * @brief Set text of button's label only for the pressed state
 * 
 * @param obj_btn button object
 * @param pressed_text text for pressed state
 */
void lui_button_set_label_text_pressed(lui_obj_t* obj_btn, const char* pressed_text);

/**
 * @brief Set different label texts for idle and pressed states.
 * 
 * Calling this function is same as first calling `lui_button_set_label_text()` 
 * and then `lui_button_set_label_text_pressed()`.
 * 
 * @param obj_btn button object
 * @param idle_text label text when button is idle
 * @param pressed_text label text when button is pressed
 */
void lui_button_set_label_texts(lui_obj_t* obj_btn, const char* idle_text, const char* pressed_text);

/**
 * @brief Set alignment of the label text. See: @ref LUI_ALIGNMENT
 * 
 * @param obj_btn button object
 * @param alignment Alignment flags. Allowed values: `LUI_ALIGN_LEFT`, `LUI_ALIGN_CENTER`, `LUI_ALIGN_RIGHT`
 */
void lui_button_set_label_align(lui_obj_t* obj_btn, uint8_t alignment);

/**
 * @brief Set text color of button's label.
 * 
 * Same color is set for idle (normal) state and pressed state.
 *
 * @param obj_btn button object
 * @param color 16-bit text color
 */
void lui_button_set_label_color(lui_obj_t* obj_btn, uint16_t color);

/**
 * @brief Set text color of button's label only for the pressed state
 * 
 * @param obj_btn button object
 * @param pressed_color 16-bit text color
 */
void lui_button_set_label_color_pressed(lui_obj_t* obj_btn, uint16_t pressed_color);

/**
 * @brief Set different label text-colors for idle and pressed states.
 * 
 * Calling this function is same as first calling `lui_button_set_label_color()` 
 * and then `lui_button_set_label_color_pressed()`.
 * 
 * @param obj_btn button object
 * @param idle_color text color for idle state
 * @param pressed_color text color for pressed state
 */
void lui_button_set_label_colors(lui_obj_t* obj_btn, uint16_t idle_color, uint16_t pressed_color);

/**
 * @brief Set font of button's label
 *
 * @param obj_btn button object
 * @param font font object. If NULL is passed, default font will be used
 */
void lui_button_set_label_font(lui_obj_t* obj_btn, const lui_font_t* font);

/**
 * @brief Set background bitmap image for idle (normal) and pressed states of button.
 * 
 * Images are rendered after rendering bg color. If button is set as transparent, then 
 * button's bg color is not rendered nut images are rendered.
 * 
 * NOTE: Only if the image is 1-bpp monochrome, call `lui_button_set_bitmap_images_mono_palette()` 
 * too to set colors of the bitmap. Else, default color will be used to render 1-bpp bitmaps.
 * 
 * @param obj_btn button object
 * @param bitmap bitmap image object when button is idle (normal situation). Can be NULL
 */
void lui_button_set_bitmap_images(lui_obj_t* obj_btn, const lui_bitmap_t* idle_bitmap, const lui_bitmap_t* pressed_bitmap);

/**
 * @brief Set color palette for 1-bpp mono bitmap images. Has no effect if images are not 1-bpp.
 * 
 * When idle and/or pressed bitmap image is/are 1-bpp mono, this function sets rendering 
 * colors. Has no effect for 8-bpp grayscale and 16-bpp rgb565 bitmaps.
 * 
 * Set `idle_palette->is_backgrnd` and/or `press_palette->is_backgrnd` to 0 for only drawing the foreground and no background. 
 * This enables transparent background. `idle_palette->back_color` and/or `press_palette->back_color` is/are ignored in this case.
 * 
 * @param obj_btn button object
 * @param idle_palette color palette for button idle state image
 * @param press_palette color palette for button pressed state image
 */
void lui_button_set_bitmap_images_mono_palette(
	lui_obj_t* obj_btn, 
	lui_bitmap_mono_pal_t* idle_palette, 
	lui_bitmap_mono_pal_t* press_palette);

/**
 * @brief Set check value of button. Works only if button is set as checkable
 * 
 * @param obj_btn button object
 * @param value 0: Not checked, 1: Checked
 */
void lui_button_set_value(lui_obj_t* obj_btn, uint8_t value);

/**
 * @brief Set button as checked. Only works for checkable button
 * 
 * @param obj_btn button object
 */
void lui_button_set_checked(lui_obj_t* obj_btn);

/**
 * @brief Set button as unchecked. Only works for checkable button
 * 
 * @param obj_btn button object
 */
void lui_button_set_unchecked(lui_obj_t* obj_btn);

/**
 * @brief Set a button as checkable or not. Checkable buttons can be toggled.
 * 
 * @param obj_btn button object
 * @param is_checkable 0: NOT checkable, 1: Checkable
 */
void lui_button_set_checkable(lui_obj_t* obj_btn, uint8_t is_checkable);

/**
 * @brief Get checkable property of a button
 * 
 * @param obj_btn button object
 * @return uint8_t Returns 0 or 1. 0: NOT checkable, 1: Checkable
 */
uint8_t lui_button_get_checkable(lui_obj_t* obj_btn);

/**
 * @brief Get the check value (status) of a checkable button.
 * 
 * A checkable button fires LUI_EVENET_VALUE CHANGED event on click. Call this 
 * function to get the current check value of a checkable button.
 * 
 * If a button is not set to checkable using lui_button_set_checkable(), it will 
 * return 0 always.
 * 
 * @param obj_btn button object
 * @return uint8_t Returns 0 or 1. 0: Unchecked, 1: Checked
 */
uint8_t lui_button_get_check_value(lui_obj_t* obj_btn);

/**
 * @brief Set other colors of button object
 *
 * @param obj_btn button object
 * @param pressed_color 16-bit color of button when it's pressed
 * @param selection_color 16-bit color of button when it's selected (hovering)
 */
void lui_button_set_extra_colors(lui_obj_t* obj_btn, uint16_t pressed_color, uint16_t selection_color);

/**
 * @brief Set background of the button as transparent. 
 * 
 * When background is transparent, button's own bg color is not drawn. 
 * Rather, the color or bitmap of the parent item is drawn as background to 
 * simulate transparency. 
 * 
 * @param obj_btn button object
 * @param is_transparent 0: Transparent background, 1: NOT transparent background
 */
void lui_button_set_bg_transparent(lui_obj_t* obj_btn, uint8_t is_transparent);


/**@}*/

#if defined(LUI_USE_SWITCH)
/**
 * @defgroup lui_switch Switch widget API
 *
 * @brief API for <b><tt>switch</tt></b> widget
 *
 * @section switch_example Example
 * @image html docs/widgets_images/switch.png "Dark Theme"
 * @image html docs/widgets_images/switch_2.png "Light Theme"
 * @code
 * void switch_callback(lui_obj_t* switch_obj)
 * {
 *     if (lui_object_get_event(switch_obj) != LUI_EVENT_VALUE_CHANGED)
 *         return;
 *     int8_t value = lui_switch_get_value(switch_obj);
 *     if (value == 0)
 *     {
 *         // Switch is turned off. Do something now.
 *     }
 *     else if (value == 1)
 *     {
 *         // Switch is turned on. Do something now.
 *     }
 * }
 * lui_obj_t* my_switch = lui_switch_create();
 * // Let's keep the switch ON by default
 * lui_switch_set_on(my_switch);
 * lui_object_set_position(my_switch, 10, 10);
 * // Set a callback function to do stuffs when switch is toggled
 * lui_object_set_callback(my_switch, switch_callback)
 * 
 * lui_obj_t* my_switch2 = lui_switch_create();
 * lui_object_set_position(my_switch2, 140, 10);
 * @endcode
 * @{
 */

/**
 * @brief Create a switch with initial values
 *
 * @return lui_obj_t* created switch object
 */
lui_obj_t* lui_switch_create(void);

/**
 * @brief Create a switch with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created switch object
 */
lui_obj_t* lui_switch_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a switch object
 *
 * @param obj_swtch switch object
 */
void lui_switch_draw(lui_obj_t* obj_swtch);

/**
 * @brief Set extra colors of switch object
 *
 * @param obj_swtch switch object
 * @param knob_off_color 16-bit color of knob when switch is off
 * @param knob_on_color 16-bit color of knob when switch is on
 * @param selection_color 16-bit color when switch is selected
 */
void lui_switch_set_extra_colors(lui_obj_t* obj_swtch, uint16_t knob_off_color, uint16_t knob_on_color, uint16_t selection_color);

/**
 * @brief Get value of a switch
 *
 * @param obj_swtch switch object
 * @return int8_t value of switch. 0: Off, 1: On, -1: Error.
 */
int8_t lui_switch_get_value(lui_obj_t* obj_swtch);

/**
 * @brief Set value of switch
 *
 * @param obj_swtch switch object
 * @param value 1: switch on, 0: switch off
 */
void lui_switch_set_value(lui_obj_t* obj_swtch, uint8_t value);

/**
 * @brief Set switch to ON (value: 1)
 *
 * @param obj_swtch switch object
 */
void lui_switch_set_on(lui_obj_t* obj_swtch);

/**
 * @brief Set switch to OFF (value: 0)
 *
 * @param obj_swtch switch object
 */
void lui_switch_set_off(lui_obj_t* obj_swtch);
/**@}*/
#endif

#if defined(LUI_USE_CHECKBOX)
/**
 * @defgroup lui_checkbox Checkbox widget API
 *
 * @brief API for <b><tt>checkbox</tt></b> widget
 *
 * @section checkbox_example Example
 * @image html docs/widgets_images/checkbox.png "Dark Theme"
 * @image html docs/widgets_images/checkbox_2.png "Light Theme"
 * @code
 * lui_obj_t* chkbox_cricket = lui_checkbox_create();
 * lui_checkbox_set_label_text(chkbox_cricket, "Cricket");
 * lui_object_set_position(chkbox_cricket, 0, 0);
 * lui_checkbox_set_value(chkbox_cricket, 1); // This checkbox is selected
 *
 * lui_obj_t* chkbox_hockey = lui_checkbox_create();
 * lui_checkbox_set_label_text(chkbox_hockey, "Hockey");
 * lui_object_set_position(chkbox_hockey, 0, 25);
 * 
 * lui_obj_t* chkbox_football = lui_checkbox_create();
 * lui_checkbox_set_label_text(chkbox_football, "Football");
 * lui_object_set_position(chkbox_football, 0, 50);
 * @endcode
 * @{
 */

/**
 * @brief Create a checkbox object with initial values
 *
 * @return lui_obj_t*
 */
lui_obj_t* lui_checkbox_create(void);

/**
 * @brief Create a checkbox with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created checkbox object
 */
lui_obj_t* lui_checkbox_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a checkbox object
 *
 * @param obj_checkbox checkbox object
 */
void lui_checkbox_draw(lui_obj_t* obj_checkbox);

/**
 * @brief Set label text of a checkbox
 * 
 * @param obj_checkbox checkbox object
 * @param text text
 */
void lui_checkbox_set_label_text(lui_obj_t* obj_checkbox, const char* text);

/**
 * @brief Set label font of a checkbox
 * 
 * @param obj_checkbox checkbox object
 * @param font font
 */
void lui_checkbox_set_label_font(lui_obj_t* obj_checkbox, const lui_font_t* font);

/**
 * @brief Set label text color of a checkbox
 * 
 * @param obj_checkbox checkbox object
 * @param color 16-bit color
 */
void lui_checkbox_set_label_color(lui_obj_t* obj_checkbox, uint16_t color);

/**
 * @brief Set extra colors of checkbox
 *
 * @param obj_checkbox checkbox object
 * @param bg_checked_color 16-bit background color of checkbox when in checked status
 * @param tick_color 16-bit color of the check mark (tick)
 * @param selection_color 16-bit color of checkbox when it is selected (hovered)
 */
void lui_checkbox_set_extra_colors(lui_obj_t* obj_checkbox, uint16_t bg_checked_color, uint16_t tick_color, uint16_t selection_color);

/**
 * @brief Get value of checkbox
 *
 * @param obj_checkbox checkbox object
 * @return int8_t Value of checkbox. 0: Unchecked, 1: Checked, -1: Error
 */
int8_t lui_checkbox_get_value(lui_obj_t* obj_checkbox);

/**
 * @brief Set checkbox value
 *
 * @param obj_checkbox checkbox object
 * @param value 0: Unchecked, 1: Checked
 */
void lui_checkbox_set_value(lui_obj_t* obj_checkbox, uint8_t value);

/**
 * @brief Set checkbox status to Checked (value: 1)
 *
 * @param obj_checkbox checkbox object
 */
void lui_checkbox_set_checked(lui_obj_t* obj_checkbox);

/**
 * @brief Set checkbox status to Unchecked (value: 0)
 *
 * @param obj_checkbox checkbox object
 */
void lui_checkbox_set_unchecked(lui_obj_t* obj_checkbox);
/**@}*/
#endif

#if defined(LUI_USE_SLIDER)
/**
 * @defgroup lui_slider Slider widget API
 *
 * @brief API for <b><tt>slider</tt></b> widget
 * 
 * Sliders can have 3 types of knob: None, Default, Text.
 * - `LUI_SLIDER_KNOB_TYPE_NONE`: No knob is rendered.
 * - `LUI_SLIDER_KNOB_TYPE_DEFAULT`: A rectangular knob.
 * - `LUI_SLIDER_KNOB_TYPE_TEXT`: Text will be rendered in place of knob. This 
 * text can be value of progress bar, custom text, or both.
 * 
 * Sliders can also act as progress bars. Set the progress bar mode using 
 * `lui_slider_set_progress_bar()` function. When slider is set as a progress 
 * bar, knob type automatically becomes Text(`LUI_SLIDER_KNOB_TYPE_TEXT`).
 * 
 * Whether to show slider/progress bar value, can be set using `lui_slider_set_show_value()` 
 * function. Custom text can be shown using `lui_slider_set_text()`.
 *
 * @section slider_example1 Example Slider
 * Example of sliders with different knob configuration
 * @image html docs/widgets_images/slider.png "Dark Theme"
 * @image html docs/widgets_images/slider_2.png "Light Theme"
 * @code
 * void slider_event_handler_cb(lui_obj_t* obj)
 * {
 *     if (lui_object_get_event(obj) == LUI_EVENT_VALUE_CHANGED)
 *     {
 *         int16_t val = lui_slider_get_value(obj);
 *         // Do something with this value..
 *     }
 * }
 *
 * // Create a slider object with default knob
 * slider_led_brightness = lui_slider_create();
 * // Setting slider's area is important
 * lui_object_set_area(slider_led_brightness, 160, 20);
 * lui_object_set_position(slider_led_brightness, 0, 5);
 * // Set range of slider 0-255
 * lui_slider_set_range(slider_led_brightness, 0, 255);
 * // Set default value of slider
 * lui_slider_set_value(slider_led_brightness, 50);
 * // Set callback function
 * lui_object_set_callback(slider_led_brightness, slider_event_handler_cb);
 * 
 * // Create a slider with text knob
 * slider_2 = lui_slider_create();
 * lui_object_set_area(slider_2, 160, 20);
 * lui_object_set_position(slider_2, 0, 50);
 * lui_slider_set_range(slider_2, 0, 100);
 * lui_slider_set_value(slider_2, 69);
 * // Set knob type as text
 * lui_slider_set_knob_type(slider_2, LUI_SLIDER_KNOB_TYPE_TEXT);
 * // We'll show slider's value on the knob, and also some custom text
 * lui_slider_set_show_value(slider_2, 1);
 * lui_slider_set_text(slider_2, " %");
 * @endcode
 * 
 * @section slider_example2 Example Progress Bar
 * Example of slider as progress bar
 * @image html docs/widgets_images/progress_bar.png "Dark Theme"
 * @image html docs/widgets_images/progress_bar_2.png "Light Theme"
 * @code
 * // Create a slider
 * progress_bar = lui_slider_create();
 * lui_object_set_area(progress_bar, 160, 40);
 * lui_object_set_position(progress_bar, 10, 20);
 * lui_slider_set_range(progress_bar, 0, 100);
 * lui_slider_set_value(progress_bar, 85);
 * // Set as progress bar
 * lui_slider_set_progress_bar(progress_bar, 1);
 * // We'll also show some custom text inside progress bar
 * lui_slider_set_text(progress_bar, " %");
 * 
 * // What if we only want to show custom text and no value?
 * // lui_slider_set_show_value(progress_bar, 0);
 * @endcode
 * 
 * @{
 */

/**
 * @brief Create a slider object with initial values
 *
 * @return lui_obj_t* created slider object
 */
lui_obj_t* lui_slider_create(void);

/**
 * @brief Create a slider with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created slider object
 */
lui_obj_t* lui_slider_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw slider object
 *
 * @param obj_slider slider object
 */
void lui_slider_draw(lui_obj_t* obj_slider);

/**
 * @brief Set extra colors of slider
 *
 * @param obj_slider slider object
 * @param knob_color 16-bit color of slider knob
 * @param bg_filled_color 16-bit background color of the filled section of slider
 * @param selection_color 16-bit color of slider when it's selected (hovered)
 */
void lui_slider_set_extra_colors(lui_obj_t* obj_slider, uint16_t knob_color, uint16_t bg_filled_color, uint16_t selection_color);

/**
 * @brief Set value of slider
 *
 * @param obj_slider slider object
 * @param value value of slider
 */
void lui_slider_set_value(lui_obj_t* obj_slider, int16_t value);

/**
 * @brief Set whether to show value on the slider or not.
 * 
 * When using as progress bar, this is very helpful.
 * 
 * @param obj_slider slider object
 * @param show_val 0: Do NOT show value, 1: Show value
 */
void lui_slider_set_show_value(lui_obj_t* obj_slider, uint8_t show_val);

/**
 * @brief Set minimum and maximum values of slider
 *
 * @param obj_slider slider object
 * @param range_min minimum value of slider
 * @param range_max maximum value of slider
 */
void lui_slider_set_range(lui_obj_t* obj_slider, int16_t range_min, int16_t range_max);

/**
 * @brief Get value of slider
 *
 * @param obj_slider slider object
 * @return int16_t value of slider
 */
int16_t lui_slider_get_value(lui_obj_t* obj_slider);

/**
 * @brief Get minimum value of slider's range
 *
 * @param obj_slider slider object
 * @return int16_t minimum value of slider
 */
int16_t lui_slider_get_min_value(lui_obj_t* obj_slider);

/**
 * @brief Get maximum value of slider's range
 *
 * @param obj_slider slider object
 * @return int16_t maximum value of slider
 */
int16_t lui_slider_get_max_value(lui_obj_t* obj_slider);

/**
 * @brief Set custom text to be rendered on the slider.
 * 
 * @param obj_slider slider object
 * @param custom_text custom text
 */
void lui_slider_set_text(lui_obj_t* obj_slider, const char* custom_text);

/**
 * @brief Configure the slider as a progress bar.
 * 
 * When a slider becomes progress bar, it does not take touch input. Value of 
 * the slider is rendered in the center, followed by the custom text (if any).
 * 
 * @param obj_slider slider object
 * @param is_progress_bar 1: Set as Progress Bar, 0: Set as normal slider
 */
void lui_slider_set_progress_bar(lui_obj_t* obj_slider, uint8_t is_progress_bar);

/**
 * @brief Set font of the slider
 * 
 * @param obj_slider slider object
 * @param font font
 */
void lui_slider_set_font(lui_obj_t* obj_slider, const lui_font_t* font);

/**
 * @brief Set knob type of the slider. See @ref LUI_SLIDER_KNOB_TYPE.
 * 
 * @param obj_slider slider object
 * @param knob_type Allowed values: `LUI_SLIDER_KNOB_TYPE_NONE`, `LUI_SLIDER_KNOB_TYPE_DEFAULT`, 
 * `LUI_SLIDER_KNOB_TYPE_TEXT`, 
 * @return int8_t 0: Success, -1: Fail
 */
int8_t lui_slider_set_knob_type(lui_obj_t* obj_slider, uint8_t knob_type);
/**@}*/
#endif

#if defined(LUI_USE_LIST)
/**
 * @defgroup lui_list List widget API
 *
 * @brief API for <b><tt>list</tt></b> widget
 *
 * <tt>list</tt> is a collection of items. List can be dropdown or static. A 
 * dropdown list can be expanded/contracted while a static list cannot be.
 * 
 * Some important points about list:
 * 1. Number of maximum items must be set by calling `lui_list_set_max_items_count()` 
 *    function before items can be added.
 * 2. Maximum items count cannot be changed anymore once set. LameUI does NOT 
 *    support memory reallocation.
 * 3. After changes are made to a list like changing area/position, addition/removal 
 *    of items etc., `lui_list_prepare()` must be called to prepare the list.
 *
 * @section list_example Example
 * @image html docs/widgets_images/list.png "Dark Theme"
 * @image html docs/widgets_images/list_2.png "Light Theme"
 * @code
 * void my_list_callback(lui_obj_t* list_obj)
 * {
 *     uint8_t event = lui_object_get_event(list_obj);
 *     if (event == LUI_EVENT_PRESSED)
 *     {
 *         int index = lui_list_get_selected_item_index(list_obj);
 *         char* txt = lui_list_get_item_text(list_obj, index);
 * 
 *         // We got both index and text of selected item.
 *         // Now do something with that information.
 *     }
 * }
 * 
 * lui_obj_t* my_list = lui_list_create();
 * 
 * // Setting list area is important. Else items won't be properly rendered
 * lui_object_set_area(my_list, 110, 160);
 * lui_object_set_position(my_list, 50, 5);
 * 
 * // Setting max items count is must. Otherwise we can't add items.
 * // Note: This is a one-time process. Max items count cannot be changed later
 * lui_list_set_max_items_count(my_list, 20);
 * 
 * // Now, let's add some items. We can not add more than 20 items
 * lui_list_add_item(my_list, "--Select--");
 * lui_list_add_item(my_list, "Algerian");
 * lui_list_add_item(my_list, "Amharic ");
 * lui_list_add_item(my_list, "Assamese");
 * lui_list_add_item(my_list, "Bavarian");
 * lui_list_add_item(my_list, "Bengali");
 * lui_list_add_item(my_list, "Czech");
 * lui_list_add_item(my_list, "Deccan");
 * lui_list_add_item(my_list, "Dutch");
 * lui_list_add_item(my_list, "English");
 * lui_list_add_item(my_list, "French");
 * lui_list_add_item(my_list, "German");
 * lui_list_add_item(my_list, "Hindi");
 * 
 * // Now make this list a dropdown list
 * lui_list_set_dropdown_mode(my_list, 1);
 * 
 * // Change the selected item to 6th (index = 5)
 * lui_list_set_selected_item_index(my_list, 5);
 * 
 * // Set a callback function
 * lui_object_set_callback(my_list, my_list_callback);
 *
 * // IMPORTANT! After everything is done, we must prepare the list.
 * // Else nothing will work
 * lui_list_prepare(my_list);
 * @endcode
 * @{
 */

/**
 * @brief Create a list object with initial values
 *
 * @return lui_obj_t* created list object
 */
lui_obj_t* lui_list_create(void);

/**
 * @brief Create a list with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created list object
 */
lui_obj_t* lui_list_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Set maximum item count for a list. The list cannot store items more 
 * than this number.
 * 
 * This function must be called once before adding items to the list. 
 * Trying to add items before setting max items count will fail.
 * Once max items count is set, it can not be changed anymore. Calling
 * this function again will fail and return -1.
 * 
 * @param obj list object
 * @param max_items_cnt maximum items count
 * @return int8_t error code. 0: Success, -1: Fail
 */
int8_t lui_list_set_max_items_count(lui_obj_t* obj, uint8_t max_items_cnt);

/**
 * @brief Draw list object
 *
 * @param obj_list list object
 */
void lui_list_draw(lui_obj_t* obj_list);

/**
 * @brief Add an item to the end of the list. 
 * 
 * Fails if added items exceeds `max items count` set by `lui_list_set_max_items_count()`
 * function
 * 
 * @param obj_list list object
 * @param text item text
 * @return int8_t error code. 0: Success, -1: Fail
 */
int8_t lui_list_add_item(lui_obj_t* obj_list, const char* text);

/**
 * @brief Remove an item from list at a specific index
 * 
 * @param obj list object
 * @param item_index item index
 * @return int8_t error code. 0: Success, -1: Fail
 */
int8_t lui_list_remove_item(lui_obj_t* obj, uint8_t item_index);

/**
 * @brief Remove all items from a list
 * 
 * @param obj list object
 * @return int8_t error code. 0: Success, -1: Fail
 */
int8_t lui_list_remove_all(lui_obj_t* obj);

/**
 * @brief Get index of selected item of a list
 * 
 * @param obj list object
 * @return int16_t item index. -1 if failed.
 */
int16_t lui_list_get_selected_item_index(lui_obj_t* obj);

/**
 * @brief Set selected item's index in a list
 * 
 * @param obj list object
 * @param item_index item index
 * @return int16_t 0: Success, -1: Fail
 */
int16_t lui_list_set_selected_item_index(lui_obj_t* obj, uint8_t item_index);

/**
 * @brief Get text of a list item by its index
 * 
 * @param obj list object
 * @param item_index item index
 * @return char* text
 */
const char* lui_list_get_item_text(lui_obj_t* obj, uint8_t item_index);

/**
 * @brief Set text of a list item by its index
 * 
 * @param obj list object
 * @param text item text
 * @param item_index item index
 * @return int8_t 0: Success, -1:Fail
 */
int8_t lui_list_set_item_text(lui_obj_t* obj, const char* text, uint8_t item_index);

/**
 * @brief Set if list is a dropdown or not.
 * 
 * When list is dropdown, it can be expanded/contracted by a button. When a list
 * is not a dropdown, it is always expanded.
 * 
 * @param obj list object
 * @param is_dropdown 0: NOT dropdown, 1: Dropdown
 * @return int8_t 0:Success, -1: Fail
 */
int8_t lui_list_set_dropdown_mode(lui_obj_t* obj, uint8_t is_dropdown);

/**
 * @brief Expand a dropdown list manually. Works only if list mode is set as
 * dropdown. Otherwise, has no effect.
 * 
 * @param obj list object
 * @param is_expanded 0: NOT expanded, 1: Expanded
 * @return int8_t 0: Success, -1: Fail
 */
int8_t lui_list_set_dropdown_expand(lui_obj_t* obj, uint8_t is_expanded);

/**
 * @brief Prepares a list for final render. This function must be called by
 * the user after creating a list/making changes in a list. Else, those
 * changes won't be reflected.
 *
 * @param obj_list list object
 */
void lui_list_prepare(lui_obj_t* obj_list);

/**
 * @brief Sets minimum height of each item in a list
 *
 * @param obj_list list object
 * @param height minimum height of an item
 */
void lui_list_set_item_min_height(lui_obj_t* obj_list, uint8_t height);

/**
 * @brief Set font of list. If none is set, default font is used
 *
 * @param obj_list list object
 * @param font font of list
 */
void lui_list_set_font(lui_obj_t* obj_list, const lui_font_t* font);

/**
 * @brief Set alignment of texts in a list. 
 * 
 * If alignment is changed after the list is already prepared, call `list_prepare()
 * function again so new alignment is applied to all the items.
 *
 * @param obj_list list object
 * @param alignment Alignment flag. Allowed values are: LUI_ALIGN_LEFT, LUI_ALIGN_CENTER, LUI_ALIGN_RIGHT
 */
void lui_list_set_text_align(lui_obj_t* obj_list, uint8_t alignment);

/**
 * @brief Set text color of list items
 * 
 * @param obj_list list object
 * @param color 16-bit color
 */
void lui_list_set_text_color(lui_obj_t* obj_list, uint16_t color);

/**
 * @brief Set whether list items have border or not
 * 
 * @param obj_list list object
 * @param has_border 0: Items have NO border, 1: Items have border
 * @param border_color 16-bit border color
 */
void lui_list_set_item_border(lui_obj_t* obj_list, uint8_t has_border, uint16_t border_color);

/**
 * @brief Sets label text color of navigation buttons of a list. Navigation buttons
 * are used to change current page of list
 *
 * @param obj_list list object
 * @param color 16-bit color
 */
void lui_list_set_nav_btn_label_color(lui_obj_t* obj_list, uint16_t color);

/**
 * @brief Sets label background color of navigation buttons of a list. Navigation buttons
 * are used to change current page of list
 *
 * @param obj_list list object
 * @param color 16-bit color
 */
void lui_list_set_nav_btn_bg_color(lui_obj_t* obj_list, uint16_t color);

/**
 * @brief Sets extra colors of list
 *
 * @param obj_list list object
 * @param pressed_color 16-bit color of list item when pressed
 * @param selection_color 16-bit color of list item when selected (hovered)
 */
void lui_list_set_nav_btn_extra_colors(lui_obj_t* obj_list, uint16_t pressed_color, uint16_t selection_color);

/**
 * @brief Sets current page index of a list. The current page is rendered.
 * Has no effect if index is out of bound.
 *
 * @param obj list object
 * @param index index of current page
 */
void lui_list_set_page_index(lui_obj_t* obj, uint8_t index);

/**
 * @private
 * @brief Adds navigation buttons to list. Only called by the library.
 *
 * @param obj_list list object
 */
void _lui_list_add_nav_buttons(lui_obj_t* obj_list);

/**
 * @private
 * @brief Sets callback function to handle nav button events. Only called by
 * the library.
 *
 * @param obj_list list object
 */
void _lui_list_nav_btn_cb(lui_obj_t* obj_list);

/**
 * @private
 * @brief Adds actual button objects that are created during `lui_list_add_item()` call.
 * Only called by the library.
 *
 * @param obj_list list object
 * @param obj_btn button object, which is a list item
 */
void _lui_list_add_button_obj(lui_obj_t* obj_list, lui_obj_t* obj_btn);
/**@}*/
#endif

#if defined(LUI_USE_BUTTONGRID)
/**
 * @defgroup lui_btngrid Buttongrid widget API
 *
 * @brief API for <b><tt>buttongrid</tt></b> widget
 *
 * <tt>buttongrid</tt> is a grid of buttons. It's way more effcient than adding
 * individual button objects to forma grid/matrix. For example, if we need 20
 * buttons in a 5x4 grid, creating 20 individual button objects will take lots of
 * RAM. Rather using a 5x4 buttongrid will only create a single object and saves
 * RAM significantly.
 *
 * Practical examples of buttongrid are qwerty keyboards, numpads etc..
 *
 * @section buttongrid_example1 Example 1
 * Let's create a simple numpad using buttongrid.
 * @image html docs/widgets_images/btngrid_simple.png "Dark Theme"
 * @image html docs/widgets_images/btngrid_simple_2.png "Light Theme"
 * @code
 * lui_obj_t* numpad = lui_btngrid_create();
 * // Setting buttongrid area is important. Else items won't be properly rendered
 * lui_object_set_area(numpad, 200, 200);
 *
 * // Text map of the numpad
 * const char* numpad_txt_map[] =
 * {
 *     "7", "8", "9", "\n",
 *     "4", "5", "6", "\n",
 *     "1", "2", "3", "\n",
 *     "0", "00", ".", "\n",
 *     "+", "-", "=", "\0"
 * };
 *
 * // Property map. Notice how the "=" button has twice the width of "+" and "-".
 * const uint8_t numpad_property_map[] =
 * {
 *     1, 1, 1,
 *     1, 1, 1,
 *     1, 1, 1,
 *     1, 1, 1,
 *     1, 1, 2
 * };
 *
 * lui_btngrid_set_textmap(numpad, numpad_txt_map);
 * lui_btngrid_set_propertymap(numpad, numpad_property_map);
 *
 * lui_btngrid_set_btn_margin(numpad, 5, 5);
 * @endcode
 *
 * @section buttongrid_example2 Example 2
 * Let's create a simple control panel and use most of the features of buttongrid.
 * Also we'll see how to use callback for buttongrid.
 * @image html docs/widgets_images/btngrid_advanced.png "Dark Theme"
 * @image html docs/widgets_images/btngrid_advanced_2.png "Light Theme"
 * @code
 * void controlpanel_callback(lui_obj_t* ctrlpanel_btngrid)
 * {
 *     // Get index of the pressed button
 *     int active_btn_index = lui_btngrid_get_acive_btn_index(ctrlpanel_btngrid);
 *     if (active_btn_index == -1)	return;
 *     // Get text of the button
 *     const char* btn_txt = lui_btngrid_get_btn_text(ctrlpanel_btngrid, active_btn_index);
 *     if (txt == NULL)	return;
 *
 *     // We can check which button is pressed either by its index or by its text.
 *     // In this example we'll use text of the button.
 *
 *     if (strcmp(txt, "Motor 1") == 0)
 *     {
 *         // Do something with motor 1
 *     }
 *     else if (strcmp(txt, "Fan 1") == 0)
 *     {
 *         // Do something with fan 1
 *     }
 *     else if (strcmp(txt, "Clr T1") == 0)
 *     {
 *         // This button is checkable. So, get its check status.
 *
 *         int8_t clr_T1_status = lui_btngrid_get_btn_check_status(ctrlpanel_btngrid, active_btn_index);
 *         if (clr_T1_status == 1)
 *         {
 *             // Checked, do something
 *         }
 *         else if (clr_T1_status == 0)
 *         {
 *             // Unchecked, do something else
 *         }
 *         else
 *         {
 *             // Error, return
 *             return;
 *         }
 *     }
 *
 *     //... Check other buttons too ....
 * }
 *
 * lui_obj_t* controlpanel = lui_btngrid_create();
 * // Setting buttongrid area is important. Else items won't be properly rendered
 * lui_object_set_area(controlpanel, 640, 240);
 *
 * // Text map of the controlpanel
 * const char* controlpanel_txt_map[] =
 * {
 *     "Motor 1", "Fan 1", "Heater L1", "Heater L2", "\n",
 *     "Clr T1", "Clr T2","Ring A", "Ring B", "\n",
 *     "X", "Y", "Z", "Stop", "\n",
 *     "-", "+", "\0"
 * };
 *
 * // Property map. 2 buttons are set checkable and 1 button is disabled
 * const uint8_t controlpanel_property_map[] =
 * {
 *     1, 1, 1, 1,
 *     1|LUI_BTNGRID_MASK_BTN_CHECKABLE, 1|LUI_BTNGRID_MASK_BTN_CHECKABLE, 1, 1,
 *     1, 1, 1,
 *     1, 1, 1, 3|LUI_BTNGRID_MASK_BTN_DISABLED,
 *     1, 1, 2
 * };
 *
 * lui_btngrid_set_textmap(controlpanel, controlpanel_txt_map);
 * lui_btngrid_set_propertymap(controlpanel,controlpanel_property_map);
 *
 * lui_btngrid_set_btn_margin(controlpanel, 2, 5);
 * lui_object_set_callback(controlpanel, controlpanel_callback);
 *
 * @endcode
 * @{
 */

/**
 * @brief Create a buttongrid with initial values
 *
 * @return lui_obj_t* created buttongrid object
 */
lui_obj_t* lui_btngrid_create(void);

/**
 * @brief Create a buttongrid with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created buttongrid object
 */
lui_obj_t* lui_btngrid_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a buttongrid
 *
 * @param obj buttongrid object
 */
void lui_btngrid_draw(lui_obj_t* obj);

/**
 * @brief Set texts of all buttons as a map. The grid is created based on this
 * text map. Each item in the array makes a column and `\n` (newline) cretaes
 * a row. The text map's last item must be a `\0`. This marks the end of the grid
 *
 * @param obj buttongrid object
 * @param texts array of strings to be used as text map.
 */
void lui_btngrid_set_textmap(lui_obj_t* obj, const char* texts[]);

/**
 * @brief Set properties of all buttons as a map.
 *
 * Each item in the array is a 8-bit value denoting various properties of a
 * button of the buttongrid. The size of array must be same as the number of
 * buttons in the button grid.The structure of a property byte is as follows:
 *
 * | Bit | Meaning              | Value                                            |
 * |-----|----------------------|--------------------------------------------------|
 * | 7   | Is button checked?   | 0: Unchecked, <br>1: Checked                     |
 * | 6   | Is button disabled?  | 0: NOT disabled (i.e., Enabled), <br>1: Disabled |
 * | 5   | Is button hidden?    | 0: NOT hidden (i.e., visible), <br>1: Hidden     |
 * | 4   | Is button checkable? | 0: NOT checkable, <br>1: Checkable               |
 * | 3:0 | Button width unit    | 1-15, <br>Setting it to 0 has no effect          |
 *
 * Example: To make a checkable button with 3 unit width and to set it to `checked`
 * status, property byte should be: (1 << 7) | (1 << 4) | 3
 *
 * @param obj buttongrid object
 * @param properties array of property bytes
 */
void lui_btngrid_set_propertymap(lui_obj_t* obj, const uint8_t properties[]);

/**
 * @brief Set property byte (8 bits) of a particular button in a buttongrid.
 *
 * Also see: @ref lui_btngrid_set_propertymap()
 *
 * @param obj buttongrid object
 * @param btn_index index of the button whose property is being set
 * @param property_byte 8-bit property value
 */
void lui_btngrid_set_btn_property_bits(lui_obj_t* obj, uint16_t btn_index, uint8_t property_byte);

/**
 * @brief Set text of a particular button in a buttongrid.
 *
 * @param obj buttongrid object
 * @param btn_index index of the button whose text is being set
 * @param text text of the button
 */
void lui_btngrid_set_btn_text(lui_obj_t* obj, uint8_t btn_index, char* text);

/**
 * @brief Get text of a particular button in a buttongrid
 *
 * @param obj buttongrid object
 * @param btn_index index of the button
 * @return const char* text of the button
 */
const char* lui_btngrid_get_btn_text(lui_obj_t* obj, uint16_t btn_index);

/**
 * @brief Set the width unit of a particular button in a buttongrid
 *
 * @param obj buttongrid object
 * @param btn_index index of the button whose width unit is being set
 * @param width_unit width unit. Range is 1-15
 */
void lui_btngrid_set_btn_width_unit(lui_obj_t* obj, uint16_t btn_index, uint8_t width_unit);

/**
 * @brief Hide/unhide a particular button in the buttongrid
 *
 * @param obj buttongrid object
 * @param btn_index index of the button
 * @param hidden 0: Visible, 1: Hidden
 */
void lui_btngrid_set_btn_hidden(lui_obj_t* obj, uint16_t btn_index, uint8_t hidden);

/**
 * @brief Set if a particular button is checkable or not
 *
 * @param obj buttongrid object
 * @param btn_index index of the button
 * @param checkable 0: NOT checkable, 1: Checkable
 */
void lui_btngrid_set_btn_checkable(lui_obj_t* obj, uint16_t btn_index, uint8_t checkable);

/**
 * @brief Set the check status of a particular button in button grid
 *
 * @param obj buttongrid object
 * @param btn_index index of the button
 * @param checked 0: NOT checked, 1: Checked
 */
void lui_btngrid_set_btn_checked(lui_obj_t* obj, uint16_t btn_index, uint8_t checked);

/**
 * @brief Get the index of currently active button, i.e., the button which has
 * the input.
 *
 * @param obj buttongrid object
 * @return int16_t Index of active button. -1 if error.
 */
int16_t lui_btngrid_get_acive_btn_index(lui_obj_t* obj);

/**
 * @brief Get the check status of a particular button in button grid
 *
 * @param obj buttongrid object
 * @param btn_index index of the button
 * @return int8_t Check status. 0: NOT checked, 1: Checked, -1: Error
 */
int8_t lui_btngrid_get_btn_check_status(lui_obj_t* obj, uint8_t btn_index);

/**
 * @brief Set font of button grid text
 *
 * @param obj buttongrid object
 * @param font font object
 */
void lui_btngrid_set_font(lui_obj_t* obj, const lui_font_t* font);

/**
 * @brief Set extra colors of button grid
 *
 * @param obj buttongrid object
 * @param btn_color 16-bit color of button
 * @param label_color 16-bit color of button texts
 * @param btn_pressed_color 16-bit color of pressed buttons
 */
void lui_btngrid_set_extra_colors(lui_obj_t* obj, uint16_t btn_color, uint16_t label_color, uint16_t btn_pressed_color);

/**
 * @brief Set margin of buttons in a button grid
 *
 * @param obj buttongrid object
 * @param margin_x margin in X axis
 * @param margin_y margin in Y axis
 */
void lui_btngrid_set_btn_margin(lui_obj_t* obj, uint8_t margin_x, uint16_t margin_y);

/**
 * @private
 * @brief Calculate buttongrid area
 *
 * @param obj buttongrid object
 */
void _lui_btngrid_calc_btn_area(lui_obj_t* obj);
/**@}*/
#endif

#if defined(LUI_USE_KEYBOARD) && defined(LUI_USE_BUTTONGRID)
/**
 * @defgroup lui_keyboard Keyboard widget API
 *
 * @brief API for <b><tt>Keyboard</tt></b> widget
 *
 * <tt>keyboard</tt> is a special typw of <tt>buttongrid</tt>. Keyboard has 3 modes.
 * See : @ref LUI_KEYBOARD_MODE.
 *
 * Usually keyboard is used along with a <tt>textbox</tt>. See: @ref lui_textbox
 *
 * Steps to use a keyboard with a textbox are:
 * 1. Create a keyboard object. Note: Keyboards are hidden by default.
 * 2. Create a textbox object.
 * 3. Create a callback function for the textbox
 *    3.1. If event is LUI_EVENT_ENTERED, set the target keyboard for this textbox.
 *         This will make the keyboard visible.
 *    3.2. If event is LUI_EVENT_EXITED, set target keyboard as NULL. This will hide
 *         the keyboard again.
 *
 * @section keyboard_example Example
 * This example only creates a keyboard and links it with an existing textbox.
 * To see a more complete example, see <tt>textbox</tt> example section.
 * See: @ref lui_textbox
 * @image html docs/widgets_images/keyboard.png "Dark Theme"
 * @image html docs/widgets_images/keyboard_2.png "Light Theme"
 * @code
 * lui_obj_t* my_keyboard = lui_keyboard_create();
 * // Keyboard is by default hidden. It is supposed to be made visible when a
 * // textbox is clicked on. But for this example, we are making it visible
 * // manually.
 * lui_object_set_visibility(my_keyboard, 1);
 *
 * // Set the target textbox
 * lui_keyboard_set_target_txtbox(my_keyboard. my_txtbox);
 * @endcode
 * @{
 */

/**
 * @brief Create a keyboard object with initial values
 *
 * @return lui_obj_t* Created keyboard (buttongrid) object
 */
lui_obj_t* lui_keyboard_create(void);

/**
 * @brief Create a keyboard with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created keyboard object
 */
lui_obj_t* lui_keyboard_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Get the text of a key against its index.
 *
 * @param obj keyboard (buttongrid) object
 * @param btn_index index of the key/button
 * @return const char* text of the key
 */
const char* lui_keyboard_get_key_text(lui_obj_t* obj, uint8_t btn_index);

/**
 * @brief Set the operation mode of keyboard.
 *
 * For allowed modes, see: @ref LUI_KEYBOARD_MODE
 *
 * @param obj keyboard (buttongrid) object
 * @param mode mode
 */
void lui_keyboard_set_mode(lui_obj_t* obj, uint8_t mode);

/**
 * @brief Set font of the keyboard
 *
 * @param obj keyboard (buttongrid) object
 * @param font font
 */
void lui_keyboard_set_font(lui_obj_t* obj, const lui_font_t* font);

/**
 * @brief Set the target textbox of the keyboard. When target textbox is set,
 * the keyboard becomes visible and only the target textbox receives input.
 * To hide the keyboard again, tap/click on the "check/ok" button on the keyboard.
 *
 * @param obj_kb keyboard (buttongrid) object
 * @param obj_txtbox textbox object
 */
void lui_keyboard_set_target_txtbox(lui_obj_t* obj_kb, lui_obj_t* obj_txtbox);

/**
 * @brief Keyboard callback function. This function handles all key presses.
 *
 * If user needs to write their own callback for keyboard, they must call this
 * function from their own callback function.
 *
 * <b>Example:</b>
 * @code
 * // User defined custom callback function for keyboard
 * void kb_user_callback(lui_obj_t* sender)
 * {
 *     // Keyboard key is pressed. Do something
 *     // ..........
 *     // [Mandatory] After all user stuffs are done, must call lui_keyboard_sys_cb()
 *     lui_keyboard_sys_cb(sender);
 * }
 *
 * lui_obj_t* keyboard = lui_keyboard_create();
 * lui_object_set_callback(keyboard, kb_user_callback);
 * @endcode
 *
 * @param obj_sender sender object
 */
void lui_keyboard_sys_cb(lui_obj_t* obj_sender);
/**@}*/
#endif

#if defined(LUI_USE_TEXTBOX)
/**
 * @defgroup lui_textbox Textbox widget API
 *
 * @brief API for <b><tt>textbox</tt></b> widget

 * Usually textbox is used along with a <tt>keyboard</tt>. See: @ref lui_keyboard
 *
 * Steps to use a keyboard with a textbox are:
 * 1. Create a keyboard object. Note: Keyboards are hidden by default.
 * 2. Create a textbox object.
 * 3. Create a callback function for the textbox
 *    3.1. If event is LUI_EVENT_ENTERED, set the target keyboard for this textbox.
 *         This will make the keyboard visible.
 *    3.2. If event is LUI_EVENT_EXITED, set target keyboard as NULL. This will hide
 *         the keyboard again.
 *
 * @section textbox_example Example
 * @image html docs/widgets_images/textbox_keyboard.png "Dark Theme"
 * @image html docs/widgets_images/textbox_keyboard_2.png "Light Theme"
 * @code
 * char txtbox_buff[50];
 * lui_obj_t* my_keyboard;
 * lui_obj_t* my_textbox;
 *
 * // This callback is fired when user enters/exists the textbox.
 * // Here, we set the target textbox of the keyboard when user enters the textbox.
 * // This unhides the keyboard and textbox will receive inputs from the textbox.
 * // When user exits the textbox by closing the keyboard, keyboard gets hidden.
 * void textbox_callback(lui_obj_t* obj_txtbox)
 * {
 *     int8_t event = lui_object_get_event(obj_txtbox);
 *     if (event == LUI_EVENT_ENTERED)
 *     {
 *         lui_keyboard_set_target_txtbox(my_keyboard, my_textbox);
 *     }
 *     else if (event == LUI_EVENT_EXITED)
 *     {
 *         lui_keyboard_set_target_txtbox(my_keyboard, NULL);
 *     }
 * }
 *
 * // Create a textbox object
 * my_textbox = lui_textbox_create();
 * // Important to set area of textbox.
 * lui_object_set_area(my_textbox, 200, 40);
 * //[Mandatory] Must set a buffer where the text will be stored
 * lui_textbox_set_text_buffer(my_textbox, txtbox_buff, 40);
 * // Let's set an initial string for the testbox.
 * // Note: the size of string does NOT include the null (\0) terminating char.
 * lui_textbox_insert_string(my_textbox, (char*)"Hello!!", 7);
 * //[Important] Set the callback for textbox. In this callback, we will
 * // link/unlink this textbox with a keyboard object. That will hide/unhide
 * // the keyboard.
 * lui_object_set_callback(my_textbox, textbox_callback);
 *
 * // Create a keyboard
 * my_keyboard = lui_keyboard_create();
 * // Keyboard is by default hidden. It is made visible when a
 * // textbox is clicked on.
 * @endcode
 * @{
 */

/**
 * @brief Create a textbox object with initial values
 *
 * @return lui_obj_t* created textbox object
 */
lui_obj_t* lui_textbox_create(void);

/**
 * @brief Create a textbox with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created textbox object
 */
lui_obj_t* lui_textbox_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw a textbox
 *
 */
void lui_textbox_draw();

/**
 * @brief Enter edit mode (input mode) of a textbox
 * 
 * @param obj Textbox object
 */
void lui_textbox_enter_edit_mode(lui_obj_t* obj);

/**
 * @brief Exit edit mode of a textbox
 * 
 * @param obj Textbox object
 */
void lui_textbox_exit_edit_mode(lui_obj_t* obj);

/**
 * @brief Set the index (position) of caret (cursor) in the textbox
 *
 * Text inputs are inserted at the position of caret.
 *
 * @param obj textbox object
 * @param caret_index caret index
 */
void lui_textbox_set_caret_index(lui_obj_t* obj, uint16_t caret_index);

/**
 * @brief Get the caret index
 *
 * @param obj textbox object
 * @return uint16_t caret index
 */
uint16_t lui_textbox_get_caret_index(lui_obj_t* obj);

/**
 * @brief Insert a character at the position of caret.
 *
 * This increments the caret index by 1 after the insert operation.
 *
 * @param obj textbox object
 * @param c character
 * @return int8_t 0: Success, -1: Error
 */
int8_t lui_textbox_insert_char(lui_obj_t* obj, char c);

/**
 * @brief Insert a string at the position of caret
 *
 * This increments the caret index by number of inserted chars after the insert operation.
 *
 * @param obj textbox object
 * @param str string (character array)
 * @param len length of the string NOT including the null character
 * @return int8_t 0: Success, -1: Error
 */
int8_t lui_textbox_insert_string(lui_obj_t* obj, char* str, uint16_t len);

/**
 * @brief Delete a character at the caret index. Does not work when caret is at 0
 *
 * @param obj textbox object
 * @return int8_t 0: Success, -1: Error
 */
int8_t lui_textbox_delete_char(lui_obj_t* obj);

/**
 * @brief Set the text buffer of a textbox. This is where the text is stored.
 *
 * @param obj textbox object
 * @param text_buffer buffer (character array)
 * @param buff_size buffer size
 */
void lui_textbox_set_text_buffer(lui_obj_t* obj, char* text_buffer, uint16_t buff_size);

/**
 * @brief Empty the text buffer and reset caret  position
 * 
 * @param obj textbox object
 */

void lui_textbox_clear_text_buffer(lui_obj_t* obj);
/**
 * @brief Set font of the textbox
 *
 * @param obj textbox object
 * @param font font object
 */
void lui_textbox_set_font(lui_obj_t* obj, const lui_font_t* font);
/**@}*/
#endif

#if defined(LUI_USE_PANEL)
/**
 * @defgroup lui_panel Panel widget API
 *
 * @brief API for <b><tt>panel</tt></b> widget
 *
 * A panel is just a container for multiple widgets (objects). This is used for
 * grouping widgets together. Panels can act as parent widget and must have a
 * scene as its parent.
 *
 * When widgets are added as children to a panel, their posions are relative to
 * the panel. For example, assume a panel is at (X=10, Y=20) position relative to its
 * parent scene. Now create a button and set the panel as its parent. Now, the position
 * of the button will be realtive to the panel. Setting the button's position to
 * (X=5, Y=3) means it global position actually is (X=15, Y=23).
 *
 * Moving a panel to a new position also moves all its children with it. Making a panel
 * hidden also hides all its children with it. Scaling a panel does NOT scales its
 * children. But if the new scale is too small to contain all its children, render
 * result will be unpredictable.
 *
 * @note A panel can have layout. All children under the panel will follow the layout. When layout
 * is enabled, children's position is set by that layout. Currently only horizontal and vertical layouts are supported.
 *
 * @section panel_example1 Example of panel
 * @code
 *
 * // Create a panel object
 * lui_obj_t* my_panel = lui_panel_create();
 * // Important to set area of panel.
 * lui_object_set_area(my_panel, 320, 300);
 * // Set position of the panel
 * lui_object_set_position(my_panel, 10, 20);
 * // Set panel background color
 * lui_object_set_bg_color(my_panel, lui_rgb(200, 128, 189))
 *
 * // Create a button
 * lui_obj_t* my_button_1 = lui_button_create();
 * // .... [ set buttons area, text, callback etc.] ...
 * // Now add it to the panel
 * lui_object_add_to_parent(my_button_1, my_panel);
 * // This position is relative to the panel
 * lui_object_set_position(my_button_1, 5, 5);
 *
 * // Create a label
 * lui_obj_t* my_label_1 = lui_label_create();
 * // .... [ set label text, color, area etc.] ...
 * // Now add it to the panel
 * lui_object_add_to_parent(my_label_1, my_panel);
 * // This position is relative to the panel
 * lui_object_set_position(my_label_1, 5, 35);
 *
 * // ... [Keep adding more widgets under the panel] ...
 * @endcode
 * 
 * @section panel_example2 Example of setting background image
 * Example of setting a background bitmap image
 * @code
 * #include "sunset_hill.h"	// include the image bitmap
 * lui_obj_t* img_panel = lui_panel_create();
 * // Important to set area of panel.
 * lui_object_set_area(img_panel, 320, 300);
 * // Set position of the panel
 * lui_object_set_position(img_panel, 10, 20);
 * // Now set the bitmap image as background
 * lui_panel_set_bitmap_image(img_panel, &BITMAP_sunset_hill);
 * @endcode
 * 
 * @section panel_example3 Example of setting color palette for 1-bpp mono bitmap image
 * Example of setting color palette for 1-bpp mono bitmap image. Color palettes are applicable 
 * only if the bitmap image is 1-bpp monochrome. If no palette is set by user, default palette 
 * is used.
 * @code
 * #include "some_logo.h"	// include the 1-bpp mono image bitmap
 * lui_obj_t* img_panel = lui_panel_create();
 * // Important to set area of panel.
 * lui_object_set_area(img_panel, 320, 300);
 * // Set position of the panel
 * lui_object_set_position(img_panel, 10, 20);
 * // Now set the bitmap image as background
 * lui_panel_set_bitmap_image(img_panel, &BITMAP_some_logo);
 * // Prepare the color palette for 1-bpp mono image, and set it
 * lui_bitmap_mono_pal_t palette = {
 *     .fore_color = lui_rgb(5, 250, 10),
 *     .back_color = lui_rgb(120, 50, 10),
 *     .is_backgrnd = 1 // Setting to 1 to draw the background with `back_color`. Set it to 0 to NOT draw background of bitmap
 * };
 * // Now set those palettes
 * lui_panel_set_bitmap_image_mono_palette(img_panel, &palette);
 * @endcode
 *
 * @section panel_example4 Example of using layout
 * Example of setting a layout for the panel
 * @code
 * lui_obj_t* panel1 = lui_panel_create();
 * // Important to set area of panel.
 * lui_object_set_area(panel1, 320, 300);
 * // Set position of the panel
 * lui_object_set_position(panel1, 10, 20);
 * // Now add a vertical layout to the panel
 * lui_panel_layout_set_properties(panel1, LUI_LAYOUT_VERTICAL, 5, 15);
 *
 * // Now add a few items. But don't set their positions.
 * // Because their position will be set by the layout
 *
 * lui_obj_t* my_button_1 = lui_button_create_and_add(panel1);
 * lui_button_set_label_text(my_button_1, "Button 1");
 *
 * lui_obj_t* my_button_2 = lui_button_create_and_add(panel1);
 * lui_button_set_label_text(my_button_2, "Button 2");
 *
 * lui_obj_t* my_button_3 = lui_button_create_and_add(panel1);
 * lui_button_set_label_text(my_button_3, "Button 3");
 *
 * lui_obj_t* my_button_4 = lui_button_create_and_add(panel1);
 * lui_button_set_label_text(my_button_4, "Button 4");
 *
 * lui_obj_t* my_button_5 = lui_button_create_and_add(panel1);
 * lui_button_set_label_text(my_button_5, "Button 5");
 *
 * // After adding all items to the panel, we'll calculate the layout
 * // Without calculating, actual layout won't take effect
 * lui_panel_layout_calculate(panel1);
 *
 * @endcode
 *
 * @{
 */

/**
 * @brief Create a panel object with initial values
 *
 * @return lui_obj_t* Created panel object
 */
lui_obj_t* lui_panel_create(void);

/**
 * @brief Create a panel with initial values and add it to a parent
 *
 * @param obj_parent parent object
 * @return lui_obj_t* Created panel object
 */
lui_obj_t* lui_panel_create_and_add(lui_obj_t* obj_parent);

/**
 * @brief Draw apanel
 *
 * @param obj_panel panel object
 */
void lui_panel_draw(lui_obj_t* obj_panel);

/**
 * @brief Set background bitmap image of the panel
 * 
 * @param obj_panel panel object
 * @param bitmap bitmap image object
 */
void lui_panel_set_bitmap_image(lui_obj_t* obj_panel, const lui_bitmap_t* bitmap);

/**
 * @brief Set color palette for 1-bpp mono bitmap image.
 * 
 * This function has no effect if the bitmap is not 1-bpp.
 * 
 * @param obj_panel panel object
 * @param palette color palette for 1-bpp mono bitmap 
 */
void lui_panel_set_bitmap_image_mono_palette(lui_obj_t* obj_panel, lui_bitmap_mono_pal_t* palette);

/**
 * @brief Set the layout properties of a panel.
 *
 * All items added as children to this panel will follow the specified layout
 * after calling `lui_panel_layout_calculate()`.
 *
 * Also see: @ref lui_panel_layout_calculate()
 *           @ref LUI_LAYOUT
 *
 * @param obj_panel panel object
 * @param type Type of layout. See `LUI_LAYOUT_` macros for supported values
 * @param pad_x Padding between children along x-axis
 * @param pad_y Padding between children along y-axis
 * @return int8_t 0: Success, -1: Failure
 */
int8_t lui_panel_layout_set_properties(lui_obj_t* obj_panel, uint8_t type, uint8_t pad_x, uint8_t pad_y);

/**
 * @brief Calculate the layout after adding all children to the parent panel.
 *
 * Call this function only after all children are added to the parent.
 * @note: Must call `lui_panel_layout_set_properties()` before calling this one.
 *
 *  Also see: @ref lui_panel_layout_set_properties()
 *
 * @param obj_panel panel object
 * @return int8_t int8_t 0: Success, < 0: Failure
 */
int8_t  lui_panel_layout_calculate(lui_obj_t* obj_panel);

/**@}*/
#endif

/**
 * @defgroup lui_scene Scene widget API
 *
 * @brief API for <b><tt>scene</tt></b> widget
 *
 * Scene is the main container of all widgets. It is at the top of hierarchy.
 * Scene has no parent and must have at least one child. All widgets must be
 * decedents of a scene.
 *
 * An application may have multiple scenes. Only the active scene is rendered
 * and it is set by `lui_scene_set_active(lui_obj_t* obj_scene)` function.
 *
 * @{
 */
/**
 * @brief Create a scene
 *
 * @return lui_obj_t* created scene
 */
lui_obj_t* lui_scene_create();

/**
 * @brief Draw scene
 *
 * @param obj_scene scene widget
 */
void lui_scene_draw(lui_obj_t* obj_scene);

/**
 * @brief Set the active scene. Only active scene is rendered.
 *
 * @param obj_scene scene widget
 */
void lui_scene_set_active(lui_obj_t* obj_scene);

/**
 * @brief Get the currently active scene
 *
 * @return lui_obj_t* active scene widget
 */
lui_obj_t* lui_scene_get_active();

/**
 * @brief Set background bitmap image of a scene.
 * 
 * See the example of panel background image setting. Same applies for scene too. 
 * @param obj_scene scene object
 * @param bitmap bitmap image object
 */
void lui_scene_set_bitmap_image(lui_obj_t* obj_scene, const lui_bitmap_t* bitmap);

/**
 * @brief Set color palette for 1-bpp mono bitmap image.
 * 
 * This function has no effect if the bitmap is not 1-bpp.
 * 
 * @param obj_scene scene object
 * @param palette color palette for 1-bpp mono bitmap 
 */
void lui_scene_set_bitmap_image_mono_palette(lui_obj_t* obj_scene, lui_bitmap_mono_pal_t* palette);

/**
 * @brief Set the layout properties of a scene.
 *
 * All items added as children to this scene will follow the specified layout
 * after calling `lui_scene_layout_calculate()`.
 *
 * Also see: @ref lui_scene_layout_calculate()
 *           @ref LUI_LAYOUT
 *
 * @param obj_scene scene object
 * @param layout_type Type of layout. See `LUI_LAYOUT_` macros for supported values
 * @param pad_x Padding between children along x-axis
 * @param pad_y Padding between children along y-axis
 * @return int8_t 0: Success, -1: Failure
 */
int8_t lui_scene_layout_set_properties(lui_obj_t* obj_scene, uint8_t type, uint8_t pad_x, uint8_t pad_y);

/**
 * @brief Calculate the layout after adding all children to the parent scene.
 *
 * Call this function only after all children are added to the parent.
 * @note: Must call `lui_scene_layout_set_properties()` before calling this one.
 *
 *  Also see: @ref lui_scene_layout_set_properties()
 *
 * @param obj_scene scene object
 * @return int8_t int8_t 0: Success, < 0: Failure
 */
int8_t  lui_scene_layout_calculate(lui_obj_t* obj_scene);

// /**
//  * @brief Set font of a scene
//  *
//  * @param obj_scene scene widget
//  * @param font font
//  */
// void lui_scene_set_font(lui_obj_t* obj_scene, const lui_font_t* font);
/**@}*/

/**
 * @defgroup lui_dispdrv Display Driver API
 *
 * @brief API for <b><tt>display driver</tt></b> object
 *
 * LameUI does NOT have its own display driver. User must supply their own functions
 * to draw pixels on a display. The displaydriver object is a virtual driver that
 * actually calls user supplied callback functions.
 *
 * The example below is using TFT_eSPI library on Arduino framework
 * Hardware: [MCU = ESP32, Display: ILI9341, Touch IC = XPT2046]
 *
 * @section displaydriver_touchinput_example Example
 * @code
 * // This example uses TFT_eSPI library on Arduino framework
 * // Hardware: [MCU = ESP32, Display: ILI9341, Touch IC = XPT2046]
 *
 * #include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
 * #include <SPI.h>
 * // ... [ Include LameUI library and other required headers too] ...
 * 
 * #define HOR_RES	240
 * #define VER_RES	320
 * 
 * #define DISP_BUFF_PX	(HOR_RES * 10)	// display buffer pixels count
 * #define LAMEUI_MEM_BYTES	4000		// LameUI working memory size in bytes
 * 
 * uint16_t disp_buffer[DISP_BUFF_PX];
 * uint8_t lui_memory[LAMEUI_MEM_BYTES];
 * 
 * TFT_eSPI tft = TFT_eSPI();  // Invoke library
 *
 * void draw_pixels_buff_cb(uint16_t* disp_buff, lui_area_t* area)
 * {
 *     tft.setAddrWindow(area->x, area->y, area->w, area->h);
 *     tft.pushColors(disp_buff, (area->w * area->h), true);	// swap_byte is true for ili9341
 * }
 * void read_touch_input_cb(lui_touch_input_data_t* inputdata)
 * {
 *     uint16_t x = 0, y = 0; // To store the touch coordinates
 *
 *     // Pressed will be set true is there is a valid touch on the screen
 *     bool pressed = tft.getTouch(&x, &y);
 *     inputdata->is_pressed = pressed;
 *     if (pressed)
 *     {
 *         inputdata->x = x;
 *         inputdata->y = y;
 *     }
 *     else
 *     {
 *         inputdata->x = -1;
 *         inputdata->y = -1;
 *     }
 * }
 *
 * void setup(void)
 * {
 *     // Initilaize tft
 *     tft.init();
 *     tft.setTouch(touch_cal_data);
 *
 *     // Initialize LameUI with some memory
 *     lui_init(lui_memory, sizeof(lui_memory));
 *
 *     // Create a display driver object
 *     lui_dispdrv_t* display_driver = lui_dispdrv_create();
 *     lui_dispdrv_register(display_driver);
 *     lui_dispdrv_set_resolution(display_driver, HOR_RES, VER_RES);
 *     lui_dispdrv_set_disp_buff(display_driver, disp_buffer, DISP_BUFF_PX);
 *     lui_dispdrv_set_draw_disp_buff_cb(display_driver, draw_pixels_buff_cb);
 *
 *     // Create touch input device
 *     lui_touch_input_dev_t* input_device = lui_touch_inputdev_create();
 *     lui_touch_inputdev_register(input_device);
 *     lui_touch_inputdev_set_read_input_cb(input_device, read_touch_input_cb);
 *
 *
 *     // ... [Add scene (mandatory) and other widgets] ...
 * }
 *
 * void loop()
 * {
 *     // Must update the UI periodically
 *     lui_update();
 *
 *     // ... [Do other stuffs] ...
 * }
 * @endcode
 *
 * @{
 */

/**
 * @brief Create display driver object
 *
 * @return lui_dispdrv_t* created display driver
 */
lui_dispdrv_t* lui_dispdrv_create(void);

/**
 * @brief Register display driver.
 *
 * @param dispdrv display driver object
 */
void lui_dispdrv_register(lui_dispdrv_t* dispdrv);

/**
 * @brief Create display driver object and register it
 *
 * @return lui_dispdrv_t* created display driver
 */
lui_dispdrv_t* lui_dispdrv_create_and_register(void);

/**
 * @brief Set horizontal and vertical resolution of display
 *
 * @param dispdrv display driver object
 * @param hor_res horizontal resolution
 * @param vert_res vertical resolution
 */
void lui_dispdrv_set_resolution(lui_dispdrv_t* dispdrv, uint16_t hor_res, uint16_t vert_res);

/**
 * @brief Set the display buffer. This buffer is provided by user and filled 
 * with colors by library. Then user passes this buffer to the display.
 * 
 * Display buffer is an array of uint16_t. Array size must be multiple of 
 * horizontal resolution of display. If display's horizontal res is 320px, 
 * minimum buffer size should be 320. It's recommended to use at least 10 times 
 * horizontal resolution as the buffer size.
 * 
 * @param dispdrv display driver object
 * @param disp_buff display buffer of type uint16_t
 * @param size_in_px buffer size (in pixel count)
 * 
 * @return int8_t -1: Failure, 0: Success
 */
int8_t lui_dispdrv_set_disp_buff(lui_dispdrv_t* dispdrv, uint16_t* disp_buff, uint32_t size_in_px);

/**
 * @brief Set callback function for drawing an area of pixels with a display buffer.
 * 
 * @param dispdrv display driver object
 * @param draw_pixels_buff_cb callback function pointer
 */
void lui_dispdrv_set_draw_disp_buff_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_buff_cb)(uint16_t* disp_buff, lui_area_t* area));

/**
 * @private
 * @brief Check if display driver and pixel drawing callback function are properly registered
 *
 * @return uint8_t 0: Not registered, 1: Registered
 */
uint8_t _lui_disp_drv_check();
/**@}*/

/**
 * @defgroup lui_input Touch Input Device API
 *
 * @brief API for <b><tt>touch input device</tt></b> object
 *
 * LameUI does NOT have its own Touch Input driver. User must supply their own functions
 * to read touch input. Touch input data is stored in `lui_touch_input_data_t`:
 * ```C
 * typedef struct _lui_touch_input_data_s
 * {
 * 	uint8_t is_pressed;
 * 	int16_t x;
 * 	int16_t y;
 * }lui_touch_input_data_t;
 * ```
 * @note When touch input is NOT pressed, x and y value must be -1.
 *
 * @section touchinput_example Example
 * Example code for touch input can be found inside the display driver example code.
 *
 * See: @ref displaydriver_touchinput_example
 *
 * @{
 */

/**
 * @brief Create touch input device object
 *
 * @return lui_touch_input_dev_t* Created touch input device object
 */
lui_touch_input_dev_t* lui_touch_inputdev_create(void);

/**
 * @brief Register a touch input device
 *
 * @param touch_inputdev touch input device object
 */
void lui_touch_inputdev_register(lui_touch_input_dev_t* touch_inputdev);

/**
 * @brief Create touch input device object and register it
 *
 * @return lui_touch_input_dev_t* Created touch input device object
 */
lui_touch_input_dev_t* lui_touch_inputdev_create_and_register(void);

/**
 * @brief
 *
 * @param touch_inputdev
 * @param read_touch_input_cb
 */
void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* touch_inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* touch_inputdata));
/**@}*/

//-------------------------------------------------------------------------------
//-------------------------------- HELPER FUNCTIONS -----------------------------
//-------------------------------------------------------------------------------
void _lui_mem_init(uint8_t mem_block[], uint32_t size);
void *_lui_mem_alloc(uint16_t element_size);
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
uint8_t _lui_clip_line(double* point_0, double* point_1, const lui_area_t* clip_win);
uint8_t _lui_calc_clip_region_code(double x, double y, const lui_area_t* clip_win);
lui_obj_t* _lui_process_input_of_act_scene();
lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj_root, lui_obj_t* obj_excluded);
lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj);
void _lui_set_obj_props_on_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
uint8_t _lui_check_if_active_obj_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
// const lui_font_t* _lui_get_font_from_active_scene();
uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state);
int8_t _lui_verify_obj(lui_obj_t* obj, uint8_t obj_type);
int8_t _lui_layout_set_properties(lui_obj_t* obj, uint8_t layout_type,  uint8_t pad_x, uint8_t pad_y);
int8_t _lui_layout_calculate(lui_obj_t* obj);

/**
 * @defgroup lui_gfx Graphics related API (for drawing shapes and text)
 *
 * @brief API for various graphics related functions like drawing line, rectangle, text etc.
 * User does NOT require to use them. But if needed for some special reason, user may
 * call these functions.
 *
 * Note: `lui_update()` function will overwrite any manual gfx calls made by user.
 * So, user must call gfx functions after the update function in a loop.
 * 
 * @{
 */

/**
 * @brief Advanced function to draw a string. For internal use only, but user 
 * may call it if needed. Most of the time `draw_string_simple` is enough for user.
 * 
 * This function lets user draw a string with specified fore color and background
 * color. The background can be a bitmap image instead of color.
 * 
 * `area` sets string bounding box. When the area of the string is not known, 
 * width and height of area (`area.w` and `area.h`) can be set to 0. In this case,
 * the function calculates the area. Width and height can be found using 
 * `lui_gfx_get_string_dimension()`.
 * 
 * When the `is_bg` arg is 0, background color/image is not rendered, creating a 
 * tranaparent background. But the downside is, if the text is cahnged later, it 
 * cannot clear the previous pixels when `is_bg` is 0.
 * 
 * When `is_bg` is 1 and `bg_bitmap` is not NULL, the bitmap is rendered as background 
 * instead of `bg_color`. Set bitmap to NULL for rendering `bg_color`.
 * 
 * `bitmap_crop` argument is useful when the backgrounf bitmap image is bigger than 
 * the text area.
 * 
 * @param str text string
 * @param area area of the string. It sets drawing start positions and the dimension.
 * @param fore_color text color
 * @param bg_color text background color
 * @param bg_bitmap text background bimap. 
 * @param palette color palette for 1-bpp mono bitmap. Has no effect if bitmap is NOT 1-bpp.
 * @param bitmap_crop crop area of the bitmap image.
 * @param is_bg flag decides whether to render background color/image or not
 * @param font font of the text
 */
void lui_gfx_draw_string_advanced(const char* str, lui_area_t* area, uint16_t fore_color, uint16_t bg_color, const lui_bitmap_t* bg_bitmap, lui_bitmap_mono_pal_t* palette, lui_area_t* bitmap_crop, uint8_t is_bg, const lui_font_t* font);

/**
 * @brief Simplified function to draw a string
 * 
 * Note: This function does not render the background
 * 
 * @param str text
 * @param x start X position
 * @param y start Y position
 * @param fore_color text color
 * @param font tetx font
 */
void lui_gfx_draw_string_simple(const char* str, uint16_t x, uint16_t y, uint16_t fore_color, const lui_font_t* font);

/**
 * @brief Draw a single character. 
 * 
 * @param c character
 * @param x postion X
 * @param y position Y
 * @param fore_color character color
 * @param bg_color background color
 * @param is_bg flag decides whether to draw bg_color or not
 * @param font character font
 */
void lui_gfx_draw_char(char c, uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font);

/**
 * @brief Get height of the font
 * 
 * @param font font
 * @return uint16_t height of font in pixels 
 */
uint16_t lui_gfx_get_font_height(const lui_font_t* font);

/**
 * @brief Get dimension of a string along x and y axis.
 * 
 * `max_w` (max width) sets the text's bounding box width. Past this width, text
 * is wrapped and goes to next line. This value must not be bigger than the width 
 * of the display.
 * 
 * It does not return any value, rather modifies the `str_dim` array.
 * 
 * @param str text
 * @param font_obj font 
 * @param max_w maximum allowed width
 * @param str_dim array of 2 items to return string dimension ({w, h})
 */
void lui_gfx_get_string_dimension(const char* str, const lui_font_t* font_obj, uint16_t max_w, uint16_t str_dim[2]);

/**
 * @brief Draw a line
 * 
 * @param x0 start X
 * @param y0 start Y
 * @param x1 end X
 * @param y1 end Y
 * @param line_width width in px 
 * @param color line color
 */
void lui_gfx_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);

/**
 * @brief Draw a line within a clipping area
 *
 * @param x0 start X
 * @param y0 start Y
 * @param x1 end X
 * @param y1 end Y
 * @param clip_area pointer to the clipping area
 * @param line_width width in px
 * @param color line color
 */
void lui_gfx_draw_line_clipped(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color);

/**
 * @brief Draw a rectangle
 * 
 * @param x start X
 * @param y start Y
 * @param w width
 * @param h height
 * @param line_width width of line in px 
 * @param color color of line
 */
void lui_gfx_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);

/**
 * @brief Draw a filled rectangle
 * 
 * @param x start X
 * @param y start Y
 * @param w width
 * @param h height
 * @param color fill color
 */
void lui_gfx_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Draw a filled rectangle within a clipping area/window
 *
 * @param x start X
 * @param y start Y
 * @param w width
 * @param h height
 * @param clip_area pointer to the clipping area
 * @param color fill color
 */
void lui_gfx_draw_rect_fill_clipped(uint16_t x, uint16_t y, uint16_t w, uint16_t h, lui_area_t* clip_area, uint16_t color);

/**
 * @brief Draws a bitmap at given x,y position. Crop area can be NULL if cropping 
 * is not needed. `palette` parameter is only applicable if image is 1-bpp mono bitmap.
 * 
 * Set `pallete` to NULL if the `bitmap` image is snot 1-bpp.
 * 
 * @param bitmap pointer to bitmap data
 * @param palette color palette for 1-bpp mono bitmap. Has no effect for 8-bpp and 16-bpp bitmaps.
 * @param x start X position
 * @param y start Y position
 * @param crop_area pointer to crop area. Set NULL for no cropping.
 */
void lui_gfx_draw_bitmap(const lui_bitmap_t* bitmap, lui_bitmap_mono_pal_t* palette, uint16_t x, uint16_t y, lui_area_t* crop_area);

/**
 * @brief Create 16-bit RGB565 color using R, G, and B values (each 8-bit)
 * 
 * Since LameUI uses RGB565 (16-bit) internally, this function is needed when 
 * when setting color for any item/object. 
 * 
 * @param red 8-bit red color
 * @param green 8-bit green color
 * @param blue 8-bit blue color
 * @return uint16_t 16-bit color
 */
uint16_t lui_rgb(uint8_t red, uint8_t green, uint8_t blue);
/**@}*/
const _lui_glyph_t* _lui_gfx_get_glyph_from_char(char c, const lui_font_t* font);
void _lui_gfx_render_char_glyph(uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const _lui_glyph_t* glyph, const lui_font_t* font);
void _lui_gfx_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color);
void _lui_gfx_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color);
/*--------------------------------------------
 *				End Function Prototypes
 *--------------------------------------------
 */

/*--------------------------------------------
 *				Themes (Dark/Light)
 *--------------------------------------------
 */

#define _LUI_R_POS_RGB 11 // Red last bit position for RGB display
#define _LUI_G_POS_RGB 5  // Green last bit position for RGB display
#define _LUI_B_POS_RGB 0  // Blue last bit position for RGB display

#define LUI_RGB(R, G, B) (((uint16_t)(R >> 3) << _LUI_R_POS_RGB) | \
						  ((uint16_t)(G >> 2) << _LUI_G_POS_RGB) | \
						  ((uint16_t)(B >> 3) << _LUI_B_POS_RGB))

/*------------------------------------------------------------------------------------
 *				Dark and Light Theme. User may modify here if needed
 *------------------------------------------------------------------------------------
 */
#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_BUTTON_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_BUTTON_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_BUTTON_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_BUTTON_BG_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_BUTTON_BORDER_COLOR LUI_RGB(75, 81, 92)
#else
#define LUI_STYLE_BUTTON_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_BUTTON_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_BUTTON_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_BUTTON_BG_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_BUTTON_BORDER_COLOR LUI_RGB(75, 81, 92)
#endif
#define LUI_STYLE_BUTTON_BORDER_THICKNESS 0
#define LUI_STYLE_BUTTON_WIDTH 40
#define LUI_STYLE_BUTTON_HEIGHT 30

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_LABEL_TEXT_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LABEL_BG_COLOR LUI_RGB(23, 33, 43)
#define LUI_STYLE_LABEL_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_LABEL_TEXT_COLOR LUI_RGB(0, 0, 0)
#define LUI_STYLE_LABEL_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_LABEL_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_LABEL_BORDER_THICKNESS 0
#define LUI_STYLE_LABEL_WIDTH 0	 /*40*/
#define LUI_STYLE_LABEL_HEIGHT 0 /*30*/

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_SWITCH_SELECTION_COLOR LUI_RGB(0, 170, 179)
#define LUI_STYLE_SWITCH_KNOB_OFF_COLOR LUI_RGB(57, 62, 70)
#define LUI_STYLE_SWITCH_KNOB_ON_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SWITCH_BG_COLOR LUI_RGB(23, 33, 43)
#define LUI_STYLE_SWITCH_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_SWITCH_SELECTION_COLOR LUI_RGB(0, 170, 179)
#define LUI_STYLE_SWITCH_KNOB_OFF_COLOR LUI_RGB(150, 150, 150)
#define LUI_STYLE_SWITCH_KNOB_ON_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SWITCH_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_SWITCH_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_SWITCH_BORDER_THICKNESS 1
#define LUI_STYLE_SWITCH_WIDTH 40
#define LUI_STYLE_SWITCH_HEIGHT 20

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_CHECKBOX_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_CHECKBOX_TICK_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_CHECKBOX_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_CHECKBOX_BG_COLOR LUI_RGB(23, 33, 43)
#define LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_CHECKBOX_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_CHECKBOX_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_CHECKBOX_TICK_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_CHECKBOX_LABEL_COLOR LUI_RGB(0, 0, 0)
#define LUI_STYLE_CHECKBOX_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_CHECKBOX_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_CHECKBOX_BORDER_THICKNESS 1
#define LUI_STYLE_CHECKBOX_WIDTH 20
#define LUI_STYLE_CHECKBOX_HEIGHT LUI_STYLE_CHECKBOX_WIDTH

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_SLIDER_SELECTION_COLOR LUI_RGB(0, 170, 179)
#define LUI_STYLE_SLIDER_KNOB_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_SLIDER_BG_COLOR LUI_RGB(57, 62, 70)
#define LUI_STYLE_SLIDER_BG_FILLED_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BORDER_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BORDER_THICKNESS 0
#else
// TODO: Improve light theme
#define LUI_STYLE_SLIDER_SELECTION_COLOR LUI_RGB(0, 170, 179)
#define LUI_STYLE_SLIDER_KNOB_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_SLIDER_BG_COLOR LUI_RGB(150, 150,150)
#define LUI_STYLE_SLIDER_BG_FILLED_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BORDER_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BORDER_THICKNESS 1
#endif
#define LUI_STYLE_SLIDER_KNOB_WIDTH 20
#define LUI_STYLE_SLIDER_WIDTH 80
#define LUI_STYLE_SLIDER_HEIGHT 20

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_LINECHART_LINE_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_LINECHART_POINT_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_LINECHART_GRID_COLOR LUI_RGB(75, 81, 92)
#define LUI_STYLE_LINECHART_BG_COLOR LUI_RGB(35, 46, 60)
#define LUI_STYLE_LINECHART_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_LINECHART_LINE_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_LINECHART_POINT_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_LINECHART_GRID_COLOR LUI_RGB(150, 150, 150)
#define LUI_STYLE_LINECHART_BG_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LINECHART_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_LINECHART_GRID_VISIBLE 1
#define LUI_STYLE_LINECHART_BORDER_THICKNESS 1
#define LUI_STYLE_LINECHART_WIDTH 40
#define LUI_STYLE_LINECHART_HEIGHT 20

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_LIST_NAV_BG_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_NAV_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_NAV_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_LIST_NAV_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_LIST_ITEM_BG_COLOR LUI_RGB(57, 62, 70)
#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR LUI_RGB(84, 91, 102)
#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR LUI_RGB(109, 118, 133)
#define LUI_STYLE_LIST_ITEM_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_ITEM_BORDER_COLOR LUI_RGB(75, 81, 92)
#define LUI_STYLE_LIST_BORDER_THICKNESS 0
#else
#define LUI_STYLE_LIST_NAV_BG_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_NAV_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_NAV_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_LIST_NAV_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_LIST_ITEM_BG_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR LUI_RGB(109, 118, 133)
#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR LUI_RGB(137, 173, 232)
#define LUI_STYLE_LIST_ITEM_LABEL_COLOR LUI_RGB(0, 0, 0)
#define LUI_STYLE_LIST_ITEM_BORDER_COLOR LUI_RGB(75, 81, 92)
#define LUI_STYLE_LIST_BORDER_THICKNESS 1
#endif
#define LUI_STYLE_LIST_ITEM_BORDER_THICKNESS 0
#define LUI_STYLE_LIST_ITEM_MIN_HEIGHT 30
#define LUI_STYLE_LIST_BORDER_COLOR LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_WIDTH 40
#define LUI_STYLE_LIST_HEIGHT 60

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_BTNGRID_BASE_BG_COLOR LUI_RGB(23, 33, 43)
#define LUI_STYLE_BTNGRID_LABEL_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_BTNGRID_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_BTNGRID_BG_COLOR LUI_RGB(39, 55, 71)
#define LUI_STYLE_BTNGRID_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_BTNGRID_BORDER_COLOR LUI_RGB(75, 81, 92)
#define LUI_STYLE_BTNGRID_BORDER_THICKNESS 0

#else
#define LUI_STYLE_BTNGRID_BASE_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_BTNGRID_LABEL_COLOR LUI_RGB(0, 0, 0)
#define LUI_STYLE_BTNGRID_PRESSED_COLOR LUI_RGB(91, 160, 235)
#define LUI_STYLE_BTNGRID_BG_COLOR LUI_RGB(200, 200, 200)
#define LUI_STYLE_BTNGRID_SELECTION_COLOR LUI_RGB(82, 143, 209)
#define LUI_STYLE_BTNGRID_BORDER_COLOR LUI_RGB(150, 150, 150)
#define LUI_STYLE_BTNGRID_BORDER_THICKNESS 1
#endif
#define LUI_STYLE_BTNGRID_WIDTH 300
#define LUI_STYLE_BTNGRID_HEIGHT 180

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_TEXTBOX_TEXT_COLOR LUI_RGB(238, 238, 238)
#define LUI_STYLE_TEXTBOX_BG_COLOR LUI_RGB(45, 56, 70)
#define LUI_STYLE_TEXTBOX_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_TEXTBOX_TEXT_COLOR LUI_RGB(0, 0, 0)
#define LUI_STYLE_TEXTBOX_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_TEXTBOX_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_TEXTBOX_BORDER_THICKNESS 1
#define LUI_STYLE_TEXTBOX_WIDTH 200
#define LUI_STYLE_TEXTBOX_HEIGHT 20

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_PANEL_BG_COLOR LUI_RGB(23, 33, 43)
#define LUI_STYLE_PANEL_BORDER_COLOR LUI_RGB(74, 129, 188)
#else
#define LUI_STYLE_PANEL_BG_COLOR LUI_RGB(255, 255, 255)
#define LUI_STYLE_PANEL_BORDER_COLOR LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_PANEL_BORDER_THICKNESS 1
#define LUI_STYLE_PANEL_WIDTH 100
#define LUI_STYLE_PANEL_HEIGHT 100

#if LUI_USE_DARK_THEME == 1
#define LUI_STYLE_SCENE_BG_COLOR LUI_RGB(23, 33, 43)
#else
#define LUI_STYLE_SCENE_BG_COLOR LUI_RGB(255, 255, 255)
#endif

/*--------------------------------------------
 *				End Themes (Dark/Light)
 *--------------------------------------------
 */

/*--------------------------------------------
 *				Helper Macros
 *--------------------------------------------
 */

#ifndef NULL
#define NULL ((void *)0)
#endif
// TODO: Inspect the code and use MIN, MAX, and BOUND where needed
#define _LUI_MIN(A,B)    ({ (A) < (B) ? (A) : (B); })
#define _LUI_MAX(A,B)    ({ (A) < (B) ? (B) : (A); })

#define _LUI_BOUNDS(x, low, high) ({\
	(x) > (high) ? (high) : ((x) < (low) ? (low) : (x));\
})

#define _LUI_SWAP(type, a, b)	({ type tmp = (a); (a) = (b); (b) = tmp; })

#define _LUI_CREATE_AND_ADD(type, parent) \
	lui_obj_t* obj = lui_##type##_create(); \
	lui_object_add_to_parent(obj, parent); \
	return obj;

#endif /* INC_LAME_UI_H_ */
