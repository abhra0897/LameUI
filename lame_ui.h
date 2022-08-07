/*
 *  Created on: 02-Apr-2020
 */
/**
 * @file lame_ui.h
 * @author Avra Mitra
 * @brief One and only header file for LameUI library.
 * @version 0.1
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
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

/* Set number of maximum objects that can be created */
#define	LUI_MAX_OBJECTS			130

/* Comment out OR set value to 0 for using light theme. */
#define LUI_USE_DARK_THEME		1

/* Now select which widgets to use. This helps to save Flash 	*/
/* Below, comment out the object that you don't want to use.	*/
/* Unused objects won't compile and save some flash				*/
#define LUI_USE_LINECHART
#define LUI_USE_SWITCH
#define LUI_USE_CHECKBOX
#define LUI_USE_SLIDER
#define LUI_USE_LIST
#define LUI_USE_PANEL
#define LUI_USE_TEXTBOX
#define LUI_USE_BUTTONGRID
#if defined(LUI_USE_BUTTONGRID)
	#define LUI_USE_KEYBOARD	/* To use keyboard, buttongrid must be used */
#endif


/*--------------------------------------------
 *					End
 *--------------------------------------------
 */


#define	_LUI_R_POS_RGB   					11	// Red last bit position for RGB display
#define	_LUI_G_POS_RGB   					5 	// Green last bit position for RGB display
#define	_LUI_B_POS_RGB   					0	// Blue last bit position for RGB display

#define	LUI_RGB(R,G,B) 					(((uint16_t)(R >> 3) << _LUI_R_POS_RGB) | \
											((uint16_t)(G >> 2) << _LUI_G_POS_RGB) | \
											((uint16_t)(B >> 3) << _LUI_B_POS_RGB))


/*------------------------------------------------------------------------------------
 *				Default Dark Theme. User may modify here
 *------------------------------------------------------------------------------------
 */
/* Comment out OR set value to 0 for enabling light mode. */
#define LUI_USE_DARK_THEME		1


#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_BUTTON_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_BUTTON_PRESSED_COLOR		LUI_RGB(91, 160, 235)
	#define	LUI_STYLE_BUTTON_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_BUTTON_BG_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_BUTTON_BORDER_COLOR		LUI_RGB(75, 81, 92)
#else
	#define LUI_STYLE_BUTTON_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_BUTTON_PRESSED_COLOR		LUI_RGB(91, 160, 235)
	#define	LUI_STYLE_BUTTON_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_BUTTON_BG_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_BUTTON_BORDER_COLOR		LUI_RGB(75, 81, 92)
#endif
#define LUI_STYLE_BUTTON_BORDER_VISIBLE		0
#define LUI_STYLE_BUTTON_WIDTH				40
#define LUI_STYLE_BUTTON_HEIGHT				30

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_LABEL_TEXT_COLOR			LUI_RGB(238, 238, 238)
	#define LUI_STYLE_LABEL_BG_COLOR			LUI_RGB(23, 33, 43)
	#define LUI_STYLE_LABEL_BORDER_COLOR		LUI_RGB(74, 129, 188)
	#define LUI_STYLE_LABEL_TEXT_COLOR			LUI_RGB(238, 238, 238)
#else
	#define LUI_STYLE_LABEL_TEXT_COLOR			LUI_RGB(0, 0, 0)
	#define LUI_STYLE_LABEL_BG_COLOR			LUI_RGB(255, 255, 255)
	#define LUI_STYLE_LABEL_BORDER_COLOR		LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_LABEL_BORDER_VISIBLE		0
#define LUI_STYLE_LABEL_WIDTH				0 /*40*/
#define LUI_STYLE_LABEL_HEIGHT				0 /*30*/

#if LUI_USE_DARK_THEME == 1
	#define	LUI_STYLE_SWITCH_SELECTION_COLOR	LUI_RGB(0, 170, 179)
	#define	LUI_STYLE_SWITCH_KNOB_OFF_COLOR		LUI_RGB(57, 62, 70)
	#define	LUI_STYLE_SWITCH_KNOB_ON_COLOR		LUI_RGB(74, 129, 188)
	#define LUI_STYLE_SWITCH_BG_COLOR			LUI_RGB(23, 33, 43)
	#define LUI_STYLE_SWITCH_BORDER_COLOR		LUI_RGB(74, 129, 188)
#else
	#define	LUI_STYLE_SWITCH_SELECTION_COLOR	LUI_RGB(0, 170, 179)
	#define	LUI_STYLE_SWITCH_KNOB_OFF_COLOR		LUI_RGB(150, 150, 150)
	#define	LUI_STYLE_SWITCH_KNOB_ON_COLOR		LUI_RGB(74, 129, 188)
	#define LUI_STYLE_SWITCH_BG_COLOR			LUI_RGB(255, 255, 255)
	#define LUI_STYLE_SWITCH_BORDER_COLOR		LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_SWITCH_BORDER_VISIBLE		1
#define LUI_STYLE_SWITCH_WIDTH				40
#define LUI_STYLE_SWITCH_HEIGHT				20

#if LUI_USE_DARK_THEME == 1
	#define	LUI_STYLE_CHECKBOX_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define	LUI_STYLE_CHECKBOX_TICK_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_CHECKBOX_BG_COLOR			LUI_RGB(23, 33, 43)
	#define LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR	LUI_RGB(74, 129, 188)
	#define LUI_STYLE_CHECKBOX_BORDER_COLOR		LUI_RGB(74, 129, 188)
#else
	#define	LUI_STYLE_CHECKBOX_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define	LUI_STYLE_CHECKBOX_TICK_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_CHECKBOX_BG_COLOR			LUI_RGB(255, 255, 255)
	#define LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR	LUI_RGB(74, 129, 188)
	#define LUI_STYLE_CHECKBOX_BORDER_COLOR		LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_CHECKBOX_BORDER_VISIBLE	1
#define LUI_STYLE_CHECKBOX_WIDTH			20
#define LUI_STYLE_CHECKBOX_HEIGHT			LUI_STYLE_CHECKBOX_WIDTH

#if LUI_USE_DARK_THEME == 1
	#define	LUI_STYLE_SLIDER_SELECTION_COLOR	LUI_RGB(0, 170, 179)
	#define	LUI_STYLE_SLIDER_KNOB_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_SLIDER_BG_COLOR			LUI_RGB(57, 62, 70)
	#define LUI_STYLE_SLIDER_BG_FILLED_COLOR	LUI_RGB(45, 77, 112) /*54, 94, 138*/
	#define LUI_STYLE_SLIDER_BORDER_COLOR		LUI_RGB(74, 129, 188)
#else
	#define	LUI_STYLE_SLIDER_SELECTION_COLOR	LUI_RGB(0, 170, 179)
	#define	LUI_STYLE_SLIDER_KNOB_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_SLIDER_BG_COLOR			LUI_RGB(238, 238, 238)
	#define LUI_STYLE_SLIDER_BG_FILLED_COLOR	LUI_RGB(45, 77, 112) /*54, 94, 138*/
	#define LUI_STYLE_SLIDER_BORDER_COLOR		LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_SLIDER_BORDER_VISIBLE		0
#define LUI_STYLE_SLIDER_KNOB_WIDTH			20
#define LUI_STYLE_SLIDER_WIDTH				80
#define LUI_STYLE_SLIDER_HEIGHT				20

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_LINECHART_LINE_COLOR		LUI_RGB(74, 129, 188)
	#define LUI_STYLE_LINECHART_GRID_COLOR		LUI_RGB(75, 81, 92)
	#define LUI_STYLE_LINECHART_BG_COLOR		LUI_RGB(35, 46, 60)
	#define LUI_STYLE_LINECHART_BORDER_COLOR	LUI_RGB(74, 129, 188)
#else
	#define LUI_STYLE_LINECHART_LINE_COLOR		LUI_RGB(74, 129, 188)
	#define LUI_STYLE_LINECHART_GRID_COLOR		LUI_RGB(150, 150, 150)
	#define LUI_STYLE_LINECHART_BG_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_LINECHART_BORDER_COLOR	LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_LINECHART_GRID_VISIBLE	1
#define LUI_STYLE_LINECHART_BORDER_VISIBLE	1
#define LUI_STYLE_LINECHART_WIDTH			40
#define LUI_STYLE_LINECHART_HEIGHT			20

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_LIST_NAV_BG_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_LIST_NAV_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_LIST_NAV_PRESSED_COLOR	LUI_RGB(91, 160, 235)
	#define LUI_STYLE_LIST_NAV_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_LIST_ITEM_BG_COLOR		LUI_RGB(57, 62, 70)
	#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR	LUI_RGB(84, 91, 102)
	#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR	LUI_RGB(109, 118, 133)
	#define LUI_STYLE_LIST_ITEM_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define	LUI_STYLE_LIST_ITEM_BORDER_COLOR	LUI_RGB(75, 81, 92)
	#define LUI_STYLE_LIST_BORDER_VISIBLE		0
#else
	#define LUI_STYLE_LIST_NAV_BG_COLOR			LUI_RGB(74, 129, 188)
	#define LUI_STYLE_LIST_NAV_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_LIST_NAV_PRESSED_COLOR	LUI_RGB(91, 160, 235)
	#define LUI_STYLE_LIST_NAV_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_LIST_ITEM_BG_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR	LUI_RGB(84, 91, 102)
	#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR	LUI_RGB(109, 118, 133)
	#define LUI_STYLE_LIST_ITEM_LABEL_COLOR		LUI_RGB(0, 0, 0)
	#define	LUI_STYLE_LIST_ITEM_BORDER_COLOR	LUI_RGB(75, 81, 92)
	#define LUI_STYLE_LIST_BORDER_VISIBLE		1
#endif
#define LUI_STYLE_LIST_ITEM_BORDER_VISIBLE	1
#define LUI_STYLE_LIST_ITEM_MIN_HEIGHT		30
#define LUI_STYLE_LIST_BORDER_COLOR			LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_WIDTH				40
#define LUI_STYLE_LIST_HEIGHT				60

#if LUI_USE_DARK_THEME == 1
	#define	LUI_STYLE_BTNGRID_BASE_BG_COLOR		LUI_RGB(23, 33, 43)
	#define LUI_STYLE_BTNGRID_LABEL_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_BTNGRID_PRESSED_COLOR		LUI_RGB(91, 160, 235)
	#define	LUI_STYLE_BTNGRID_BG_COLOR			LUI_RGB(39, 55, 71)
	#define	LUI_STYLE_BTNGRID_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_BTNGRID_BORDER_COLOR		LUI_RGB(75, 81, 92)
	#define LUI_STYLE_BTNGRID_BORDER_VISIBLE	0

#else
	#define	LUI_STYLE_BTNGRID_BASE_BG_COLOR		LUI_RGB(255, 255, 255)
	#define LUI_STYLE_BTNGRID_LABEL_COLOR		LUI_RGB(0, 0, 0)
	#define LUI_STYLE_BTNGRID_PRESSED_COLOR		LUI_RGB(91, 160, 235)
	#define	LUI_STYLE_BTNGRID_BG_COLOR			LUI_RGB(200, 200, 200)
	#define	LUI_STYLE_BTNGRID_SELECTION_COLOR	LUI_RGB(82, 143, 209)
	#define LUI_STYLE_BTNGRID_BORDER_COLOR		LUI_RGB(150, 150, 150)
	#define LUI_STYLE_BTNGRID_BORDER_VISIBLE	1
#endif
#define	LUI_STYLE_BTNGRID_WIDTH				300
#define LUI_STYLE_BTNGRID_HEIGHT			180

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_TEXTBOX_TEXT_COLOR		LUI_RGB(238, 238, 238)
	#define LUI_STYLE_TEXTBOX_BG_COLOR			LUI_RGB(45, 56, 70)
	#define LUI_STYLE_TEXTBOX_BORDER_COLOR		LUI_RGB(74, 129, 188)
#else
	#define LUI_STYLE_TEXTBOX_TEXT_COLOR		LUI_RGB(0, 0, 0)
	#define LUI_STYLE_TEXTBOX_BG_COLOR			LUI_RGB(255, 255, 255)
	#define LUI_STYLE_TEXTBOX_BORDER_COLOR		LUI_RGB(74, 129, 188)
#endif
#define LUI_STYLE_TEXTBOX_BORDER_VISIBLE	1
#define LUI_STYLE_TEXTBOX_WIDTH				200
#define LUI_STYLE_TEXTBOX_HEIGHT			20

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_PANEL_BG_COLOR			LUI_RGB(23, 33, 43)
	#define LUI_STYLE_PANEL_BORDER_COLOR		LUI_RGB(74, 129, 188)
#else
	#define LUI_STYLE_PANEL_BG_COLOR			LUI_RGB(255, 255, 255)
	#define LUI_STYLE_PANEL_BORDER_COLOR		LUI_RGB(74, 129, 188)
	#endif
#define LUI_STYLE_PANEL_BORDER_VISIBLE		1
#define LUI_STYLE_PANEL_WIDTH				100
#define LUI_STYLE_PANEL_HEIGHT				100

#if LUI_USE_DARK_THEME == 1
	#define LUI_STYLE_SCENE_BG_COLOR			LUI_RGB(23, 33, 43)
#else
	#define LUI_STYLE_SCENE_BG_COLOR			LUI_RGB(255, 255, 255)
#endif

/*------------------------------------------------------------------------------------
 *									End Theme
 *------------------------------------------------------------------------------------
 */

#ifndef NULL
	#define NULL							((void* )0)
#endif


#define LUI_INPUT_TYPE_TOUCH				1

/**
 * @defgroup LUI_STATE LameUI input states
 * @brief All input states of LameUI
 * 
 * Also see: @ref LUI_EVENT
 * @{
 */
#define	LUI_STATE_IDLE						0	///< Idle state. Object is not under the pointing device
#define	LUI_STATE_SELECTED					1	///< Object is under the pointing device
#define	LUI_STATE_PRESSED					2	///< Object is under the pointig device and the pointing device button is pressed
#define	LUI_STATE_ENTERED					3	///< Object is in entered state. Example: Text box is clicked
/**@} */

/**
 * @defgroup LUI_EVENT LaemUI input events
 * @brief All input events of LameUI. Events depend on previous and current state
 * 
 * Also see: @ref LUI_STATE
 * @{
 */
#define	LUI_EVENT_NONE						0	///< No event occured
#define	LUI_EVENT_SELECTED					1	///< Object is selected (under the pointing device)
#define	LUI_EVENT_SELECTION_LOST			2	///< Selection of object is lost (no more under the pointing device)
#define	LUI_EVENT_PRESSED					3	///< Object is pressed (object under pointing device and pointing device button is low)
#define	LUI_EVENT_RELEASED					4	///< Object is released (object under pointing device but pointing device button is released)
#define	LUI_EVENT_ENTERED					5	///< Object is entered to input mode. Example: Clicking on a text box
#define	LUI_EVENT_EXITED					6	///< Opposite of above. Example: Clicking outside of a text box
#define	LUI_EVENT_VALUE_CHANGED				7	///< Value of a object is changed. Example: a slider is dragged
#define	LUI_EVENT_CHECK_CHANGED				8	///< Check status of an object is changed. Example: checkbox is checked/unchecked
/**@} */

/**
 * @defgroup LUI_OBJ LameUI object types
 * @brief All types of objects (widgets) in LameUI
 * @{
 */
#define	LUI_OBJ_NONE						0	///< No type
#define	LUI_OBJ_LABEL						1	///< Label widget. See: \ref lui_label
#define	LUI_OBJ_BUTTON						2	///< Button widget. See: \ref lui_button
#define	LUI_OBJ_SWITCH						3	///< Switch widget. See: \ref lui_switch
#define	LUI_OBJ_LINECHART					4	///< Line Chart widget. See: \ref lui_linechart
#define	LUI_OBJ_PANEL						5	///< Panel widget. See: \ref lui_panel
#define	LUI_OBJ_SCENE						6	///< Scene widget. See: \ref lui_scene
#define	LUI_OBJ_GENERIC						7	///< Generic widget (user defined widget)
#define LUI_OBJ_LIST						8	///< List widget. See: \ref lui_list
#define LUI_OBJ_CHECKBOX					9	///< Checknox widget. See: \ref lui_checkbox
#define LUI_OBJ_SLIDER						10	///< Slider widget. See: \ref lui_slider
#define LUI_OBJ_BTNGRID						11	///< Buttongrid widget. See: \ref lui_btngrid
#define LUI_OBJ_TEXTBOX						12	///< Textbox widget. See: \ref lui_textbox
/**@} */

#define LUI_LAYER_POPUP						255

/**
 * @defgroup LUI_KEYBOARD_MODE Keyboard modes
 * @brief Keyboard modes of LameUI
 * @{
 */
#define LUI_KEYBOARD_MODE_TXT_LOWER			1	///< lowercase text mode
#define LUI_KEYBOARD_MODE_TXT_UPPER			2	///< UPPERCASE TEXT MODE 
#define LUI_KEYBOARD_MODE_TXT_SPCL			3	///< $pec!a1 charac+er m0d3;
/**@} */

#define LUI_ICON_HOME						"\x01"
#define LUI_ICON_RELAOD						"\x02"
#define LUI_ICON_BATTERY_HALF				"\x03"
#define LUI_ICON_LOCATION					"\x04"
#define LUI_ICON_BATTERY_FULL				"\x05"
#define LUI_ICON_CHECKMARK					"\x06"
#define LUI_ICON_RETURN_DOWN_BACK			"\x07"
#define LUI_ICON_ARROW_DOWN					"\x08"
#define LUI_ICON_BACKSPACE					"\x09"
#define LUI_ICON_PAUSE						"\x0A"
#define LUI_ICON_REMOVE						"\x0B"
#define LUI_ICON_POWER						"\x0C"
#define LUI_ICON_HOURGLASS					"\x0D"
#define LUI_ICON_VOLUME_MEDIUM				"\x0E"
#define LUI_ICON_ARROW_BACK					"\x0F"
#define LUI_ICON_CLOSE						"\x10"
#define LUI_ICON_BLUETOOTH					"\x11"
#define LUI_ICON_PLAY						"\x12"
#define LUI_ICON_VOLUME_OFF					"\x13"
#define LUI_ICON_ARROW_FORWARD				"\x14"
#define LUI_ICON_BATTERY_CHARGING			"\x15"
#define LUI_ICON_ARROW_UP					"\x16"
#define LUI_ICON_WIFI						"\x17"
#define LUI_ICON_SEARCH						"\x18"
#define LUI_ICON_WARNING					"\x19"
#define LUI_ICON_SETTINGS					"\x1A"
#define LUI_ICON_ADD						"\x1B"
#define LUI_ICON_BATTERy_DEAED				"\x1C"
#define LUI_ICON_STOP						"\x1D"



#define LUI_BTNGRID_MASK_BTN_DISABLED	0x40
#define LUI_BTNGRID_MASK_BTN_HIDDEN		0x20
#define LUI_BTNGRID_MASK_BTN_CHECKABLE	0x10
#define LUI_BTNGRID_MASK_BTN_CHECKED		0x80
#define LUI_BTNGRID_MASK_BTN_WIDTH_UNIT		0x0F


#pragma pack(push, 1)

typedef struct
{
	uint8_t* mem_block;
	uint16_t block_max_sz;
    uint16_t mem_allocated;
}_lui_mem_block_t;

/* This is a generic bitmap */
typedef struct 
{
    const uint16_t size_x; //size is in pixels
    const uint16_t size_y;
    const uint8_t * const payload;
} lui_bitmap_t;
 
/* This is a font glyph description - for now it does not support kerning */
typedef struct 
{
    const char character; //ASCII code
    const uint8_t width;
    const uint16_t payload_index;
} _lui_glyph_t;
 
typedef struct 
{
    const lui_bitmap_t * const bitmap;
    const uint8_t glyph_count;
    const _lui_glyph_t *glyphs; //pointer to array of glypt_t elements
} lui_font_t; 

extern const lui_font_t FONT_lui_default;


typedef struct _lui_area_s
{
    uint16_t x1;
    uint16_t y1;
	uint16_t x2;
    uint16_t y2;
}lui_area_t;


struct _lui_common_style_s
{
	uint16_t bg_color;
	uint16_t border_color;
	uint16_t width;
	uint16_t height;
	uint8_t border_visible;
};

struct _lui_label_style_s
{
	uint16_t text_color;
};
struct _lui_button_style_s
{
	uint16_t label_color;
	uint16_t pressed_color;
	uint16_t selection_color;
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
	uint8_t grid_visible;
};
#endif

#if defined(LUI_USE_LIST)
struct _lui_list_style_s
{
	struct _lui_button_style_s item_btn;
	struct _lui_common_style_s item_btn_common;
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
};
#endif

/**
 * @brief Generic object datatype.
 * 
 * lui_obj_t contains all the common data of objects along with address of
 * the extended data as per the type of the object 
 * 
 */
typedef struct _lui_obj_s
{
	uint16_t x;		///< Horizontal position (px)
	uint16_t y;		///< Vertical position (px)
	uint8_t layer;	///< Rendering layer (0 - 128)
	struct _lui_common_style_s common_style;	///< Common style properties of objects
	uint8_t state;	///< Input state
	uint8_t event;	///< Input event
	int32_t value;	///< User defined value
	void (*obj_event_cb)(struct _lui_obj_s* obj);	///< Input event callback function
	
	uint8_t needs_refresh;	///< Object refresh flag
	uint8_t visible;		///< Object visibility flag
	uint8_t enabled;		///< Object input enable flag
	uint8_t obj_type;		///< Object type
	struct _lui_obj_s* parent;	///< Parent of object
	uint8_t children_count;		///< Object's children count
	struct _lui_obj_s* first_child;		///< First child of object
	struct _lui_obj_s* next_sibling;	///< Next sibling of object
	
	void* obj_main_data;	///< Main (extended) data of the object
}lui_obj_t;

#if defined(LUI_USE_LINECHART)
typedef struct _lui_linechart_s
{
	struct
	{
		uint8_t hor_count;
		uint8_t vert_count;
		//uint16_t color;
		//uint8_t is_grid;
	}grid;

	struct _lui_linechart_style_s style;

	struct
	{
		double* source;
		uint16_t points;
		uint8_t auto_scale;
		double y_max_value;
		double y_min_value;
	}data;

	//uint16_t color;
	const lui_font_t* font;
}lui_chart_t;
#endif

typedef struct _lui_label_s
{
	char* text;
	const lui_font_t* font;
	//uint16_t color;
	struct _lui_label_style_s style;
}lui_label_t;


typedef struct _lui_button_s
{
	struct
	{
		char* text;
		//uint16_t color;
		const lui_font_t* font;
	}label;
	//uint16_t pressed_color;
	//uint16_t selection_color;
	struct _lui_button_style_s style;
}lui_button_t;

#if defined(LUI_USE_SWITCH)
typedef struct _lui_switch_s
{

	struct _lui_switch_style_s style;
}lui_switch_t;
#endif

#if defined(LUI_USE_CHECKBOX)
typedef struct _lui_checkbox_s
{
	struct _lui_checkbox_style_s style;
}lui_checkbox_t;
#endif

#if defined(LUI_USE_SLIDER)
typedef struct _lui_slider_s
{
	int16_t range_min;
	int16_t range_max;
	// knob's center's x position relative to slider's start x position
	uint16_t knob_center_rel_x; 

	struct _lui_slider_style_s style;
}lui_slider_t;
#endif

#if defined(LUI_USE_LIST)
typedef struct _lui_list_s
{
	uint8_t page_count;
	uint8_t current_page_index;
	int8_t selected_button_index; // not using currently
	uint8_t buttons_per_page;
	uint8_t button_item_min_height;
	const lui_font_t* font;

	//struct _lui_list_style_s style;
}lui_list_t;
#endif

#if defined(LUI_USE_KEYBOARD)
typedef struct _lui_keyboard_s
{
	lui_obj_t* target_txtbox;
	uint8_t keyboard_mode;

}lui_keyboard_t;
#endif

#if defined(LUI_USE_BUTTONGRID)
typedef struct _lui_btngrid_s
{
	const char** texts;
    uint8_t* btn_properties;
    lui_area_t* btn_area;
    uint8_t btn_cnt;
	uint8_t row_cnt;
    const lui_font_t* font;
	int16_t active_btn_index;
	uint8_t needs_full_render;
	struct _lui_btngrid_style_s style;
	#if defined(LUI_USE_KEYBOARD)
		struct _lui_keyboard_s* kb_data;
	#endif
}lui_btngrid_t;
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
}lui_textbox_t;
#endif


#if defined(LUI_USE_PANEL)
typedef struct _lui_panel_s
{
	lui_bitmap_t* bg_image;
}lui_panel_t;
#endif

typedef struct _lui_scene_s
{
	lui_bitmap_t* bg_image;
	const lui_font_t* font;

}lui_scene_t;


typedef struct _lui_touch_input_data_s
{
	uint8_t is_pressed;
	int16_t x;
	int16_t y;
}lui_touch_input_data_t;


typedef struct _lui_disp_drv_s
{
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	void (*render_complete_cb)();
	uint16_t display_hor_res;
	uint16_t display_vert_res;
}lui_dispdrv_t;

typedef struct _lui_touch_input_dev_s
{
	void (*read_touch_input_cb)(lui_touch_input_data_t* input);
	//lui_touch_input_data_t touch_data;
}lui_touch_input_dev_t;


typedef struct _lui_main_s
{
	//lui_obj_t* scenes[LUI_MAX_SCENES];
	const lui_font_t* default_font;
	lui_obj_t* active_scene;
	lui_obj_t* active_obj;
	lui_dispdrv_t* disp_drv;
	lui_touch_input_dev_t* touch_input_dev;
	// lui_touch_input_data_t last_touch_data;
	uint8_t input_state_pressed;
	uint8_t input_event_clicked;
	uint8_t total_scenes;
	uint8_t total_created_objects;	//increases as new objs are created. It never decreases
}_lui_main_t;

#pragma pack(pop)

void lui_init(uint8_t mem_block[], uint16_t size);
void lui_update();


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
 * @brief Set position of an object
 * 
 * @param obj target object
 * @param x x position
 * @param y y position
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
 * @brief Set border's visibility of an object
 * 
 * @param obj target object
 * @param is_visible 1: visible; 0: invisible
 */
void lui_object_set_border_visibility(lui_obj_t* obj, uint8_t is_visible);

/**
 * @brief Set background color of an object
 * 
 * @param obj target object
 * @param bg_color background color
 */
void lui_object_set_bg_color(lui_obj_t* obj, uint16_t color);

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
void lui_object_set_visibility(lui_obj_t* obj, uint8_t visible);

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
 * @brief Enable or disble input handling of an object. If disabled,
 * object won't cause input event callback
 * 
 * @param obj target object
 * @param is_enabled 1: input enabled; 0: input disabled
 * @return uint8_t 1: success; 0: failed
 */
uint8_t lui_object_set_enable_input(lui_obj_t* obj,  uint8_t is_enabled);

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
 * @param obj_parent target object
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
static int _lui_obj_layer_cmprtr(const void* p1, const void* p2);
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
 * API for <tt><b>label</b></tt> widgets
 * 
 * @subsection label_example Example
 * @code
 * const char* text = "Hi Universe";
 * lui_obj_t* my_label = lui_label_create();
 * lui_label_set_text(my_label, "Hello World");
 * // lui_label_set_text(my_label, text);
 * lui_label_set_text_color(my_label, lui_rgb(255, 20, 80));	// Setting text color
 * lui_object_set_bg_color(my_label, lui_rgb(10, 10, 10));		// Setting background color
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
 * @brief Draw a <tt>label</tt> widget
 * 
 * @param obj label object
 */
void lui_label_draw (lui_obj_t* obj_lbl);

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
 * API for <b><tt>linechart</tt></b> widget
 * 
 * @subsection linechart_example Example
 * @code
 * double fib_data[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
 * lui_obj_t* my_chart = lui_linechart_create();
 * lui_linechart_set_data_source(my_chart, fib_data, 10);
 * lui_linechart_set_grid_count(my_chart, 4, 2);
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
lui_obj_t* lui_linechart_create();

/**
 * @brief Draw a linechart
 * 
 * @param obj_linechart linechart object
 */
void lui_linechart_draw (lui_obj_t* obj_linechart);

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
 * @param obj_linechart linechart object
 * @param source array of data points
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
 * API for <b><tt>button</tt></b> widget
 * 
 * @subsection button_example Example
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
 * lui_object_set_callback(my_button, button_callback);		// set button call back function
 * @endcode
 * @{
 */

/**
 * @brief Create a button with initial values
 * 
 * @return lui_obj_t* created button object
 */
lui_obj_t* lui_button_create();

/**
 * @brief Draw a button object
 * 
 * @param obj_btn button object
 */
void lui_button_draw(lui_obj_t* obj_btn);

/**
 * @brief Set the text of the button's label
 * 
 * @param obj_btn button object
 * @param text char array of text
 */
void lui_button_set_label_text(lui_obj_t* obj_btn, const char* text);

/**
 * @brief Set text color of button's label
 * 
 * @param obj_btn button object
 * @param color 16-bit text color
 */
void lui_button_set_label_color(lui_obj_t* obj_btn, uint16_t color);

/**
 * @brief Set font of button's label
 * 
 * @param obj_btn button object
 * @param font font object. If NULL is passed, default font will be used
 */
void lui_button_set_label_font(lui_obj_t* obj_btn, const lui_font_t* font);

/**
 * @brief Set other colors of button object
 * 
 * @param obj_btn button object
 * @param pressed_color 16-bit color of button when it's pressed
 * @param selection_color 16-bit color of button when it's selected (hovering)
 */
void lui_button_set_extra_colors(lui_obj_t* obj_btn, uint16_t pressed_color, uint16_t selection_color);
/**@}*/

#if defined(LUI_USE_SWITCH)
/**
 * @defgroup lui_switch Switch widget API
 * 
 * API for <b><tt>switch</tt></b> widget
 * 
 * @subsection switch_example Example
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
 * // Set a callback function to do stuffs when switch is toggled
 * lui_object_set_callback(my_switch, switch_callback)
 * @endcode
 * @{
 */

/**
 * @brief Create a switch with initial values
 * 
 * @return lui_obj_t* created switch object
 */
lui_obj_t* lui_switch_create();

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
 * API for <b><tt>checkbox</tt></b> widget
 * 
 * @subsection checkbox_example Example
 * @code
 * chkbox_cricket = lui_checkbox_create();
 * lui_object_set_position(chkbox_cricket, 0, 0);
 * lui_checkbox_set_value(chkbox_cricket, 1); // This checkbox is selected
 * 
 * lui_obj_t* lbl_cricket = lui_label_create();
 * lui_label_set_text(lbl_cricket, "Cricket");
 * lui_object_set_position(lbl_cricket, 25, 0);
 * 
 * chkbox_hockey = lui_checkbox_create();
 * lui_object_set_position(chkbox_hockey, 0, 25);
 * 
 * lui_obj_t* lbl_hockey = lui_label_create();
 * lui_label_set_text(lbl_hockey, "Hockey");
 * lui_object_set_position(lbl_hockey, 25, 25);
 * @endcode
 * @{
 */

/**
 * @brief Create a checkbox object with initial values
 * 
 * @return lui_obj_t* 
 */
lui_obj_t* lui_checkbox_create();

/**
 * @brief Draw a checbox object
 * 
 * @param obj_checkbox checkbox object
 */
void lui_checkbox_draw(lui_obj_t* obj_checkbox);

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
 * API for <b><tt>slider</tt></b> widget
 * 
 * @subsection slider_example Example
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
 * // Create a slider object
 * slider_led_brightness = lui_slider_create();
 * // Setting slider's area is important
 * lui_object_set_area(slider_led_brightness, 160, 20);
 * // Set range of slider 0-255
 * lui_slider_set_range(slider_led_brightness, 0, 255);
 * // Set default value of slider
 * lui_slider_set_value(slider_led_brightness, 50);
 * // Set callback function 
 * lui_object_set_callback(slider_led_brightness, slider_event_handler_cb);
 * @endcode
 * @{
 */

/**
 * @brief Create a slider object with initial values
 * 
 * @return lui_obj_t* created slider object
 */
lui_obj_t* lui_slider_create();

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
/**@}*/
#endif

#if defined(LUI_USE_LIST)
/**
 * @defgroup lui_list List widget API
 * 
 * API for <b><tt>list</tt></b> widget
 * 
 * <tt>list</tt> is a collection of <tt>button</tt> objects. Each item added to
 * a list is returned as <tt>button</tt> object.
 * 
 * @subsection list_example Example
 * @code
 * lui_obj_t* my_list = lui_list_create();
 * // Setting list area is important. Else items won't be properly rendered
 * lui_object_set_area(my_list, 110, 160);
 * 
 * // Now, let's add some items. Each item is a button object.
 * lui_obj_t* item1 = lui_list_add_item(my_list, LUI_ICON_POWER "Shut Down");
 * lui_obj_t* item2 = lui_list_add_item(my_list, LUI_ICON_RELAOD " Restart ");
 * lui_obj_t* item3 = lui_list_add_item(my_list, "Suspend");
 * lui_obj_t* item4 = lui_list_add_item(my_list, "Log Out");
 * lui_obj_t* item5 = lui_list_add_item(my_list, "Switch User");
 * lui_obj_t* item6 = lui_list_add_item(my_list, "Hibernate");
 * lui_obj_t* item7 = lui_list_add_item(my_list, "Lock screen");
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
lui_obj_t* lui_list_create();

/**
 * @brief Draw list object
 * 
 * @param obj_list list object
 */
void lui_list_draw(lui_obj_t* obj_list);

/**
 * @brief Add an item to a list. This creates a button object and returns it
 * 
 * @param obj_list list object
 * @param text text of the item
 * @return lui_obj_t* item of the list as a button object
 */
lui_obj_t* lui_list_add_item(lui_obj_t* obj_list, const char* text);

/**
 * @brief Deletes an item from list.
 * 
 * @param obj_item_addr the button item to be deleted
 */
void lui_list_delete_item(lui_obj_t* obj_item_addr);

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
 * API for <b><tt>buttongrid</tt></b> widget
 * 
 * <tt>buttongrid</tt> is a grid of buttons. It's way more effcient than adding
 * individual button objects to forma grid/matrix. For example, if we need 20
 * buttons in a 5x4 grid, creating 20 individual button objects will take lots of
 * RAM. Rather using a 5x4 buttongrid will only create a single object and saves
 * RAM significantly.
 * 
 * Practical examples of buttongrid are qwerty keyboards, numpads etc..
 * 
 * @subsection buttongrid_example1 Example 1
 * Let's create a simple numpad using buttongrid.
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
 * @subsection buttongrid_example2 Example 2
 * Let's create a simple control panel and use most of the features of buttongrid.
 * Also we'll see how to use callback for buttongrid.
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
 * lui_object_set_area(controlpanel, 300, 200);
 * 
 * // Text map of the controlpanel
 * const char* controlpanel_txt_map[] =
 * {
 *     "Motor 1", "Fan 1", "Heater L1", "Heater L2" "\n",
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
lui_obj_t* lui_btngrid_create();

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
 * @{
 */
lui_obj_t* lui_keyboard_create();
const char* lui_keyboard_get_key_text(lui_obj_t* obj, uint8_t btn_index);
void lui_keyboard_set_mode(lui_obj_t* obj, uint8_t mode);
void lui_keyboard_set_font(lui_obj_t* obj, const lui_font_t* font);
void lui_keyboard_set_target_txtbox(lui_obj_t* obj_kb, lui_obj_t* obj_txtbox);
void lui_keyboard_sys_cb(lui_obj_t* obj_sender);
/**@}*/
#endif

#if defined(LUI_USE_TEXTBOX)
/**
 * @defgroup lui_textbox Textbox widget API
 * @{
 */
lui_obj_t* lui_textbox_create();
void lui_textbox_draw();
void lui_textbox_set_caret_index(lui_obj_t* obj, uint16_t caret_index);
uint16_t lui_textbox_get_caret_index(lui_obj_t* obj);
void lui_textbox_insert_char(lui_obj_t* obj, char c);
void lui_textbox_insert_string(lui_obj_t* obj, char* str, uint16_t len);
void lui_textbox_delete_char(lui_obj_t* obj);
void lui_textbox_set_text_buffer(lui_obj_t* obj, char* text_buffer, uint16_t buff_size);
void lui_textbox_set_font(lui_obj_t* obj, const lui_font_t* font);
/**@}*/
#endif

#if defined(LUI_USE_PANEL)
/**
 * @defgroup lui_panel Panel widget API
 * @{
 */
lui_obj_t* lui_panel_create();
void lui_panel_draw(lui_obj_t* obj_panel);
void lui_panel_set_bg_image(lui_obj_t* obj_panel, const lui_bitmap_t* image);
/**@}*/
#endif

/**
 * @defgroup lui_scene Scene widget API
 * @{
 */
lui_obj_t* lui_scene_create();
void lui_scene_draw(lui_obj_t* obj_scene);
void lui_scene_set_active(lui_obj_t* obj_scene);
lui_obj_t* lui_scene_get_active();
void lui_scene_set_bg_image(lui_obj_t* obj_scene, const lui_bitmap_t* image);
void lui_scene_set_font(lui_obj_t* obj_scene, const lui_font_t* font);
/**@}*/

/**
 * @defgroup lui_dispdrv Display Driver API
 * @{
 */
lui_dispdrv_t* lui_dispdrv_create();
void lui_dispdrv_register(lui_dispdrv_t* dispdrv);
void lui_dispdrv_set_resolution(lui_dispdrv_t* dispdrv, uint16_t hor_res, uint16_t vert_res);
void lui_dispdrv_set_draw_pixels_area_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color));
void lui_dispdrv_set_render_complete_cb(lui_dispdrv_t* dispdrv, void (*render_complete_cb)());
/**@}*/
uint8_t _lui_disp_drv_check();

/**
 * @defgroup lui_input Input Device API
 * @{
 */
lui_touch_input_dev_t* lui_touch_inputdev_create();
void lui_touch_inputdev_register (lui_touch_input_dev_t* touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* touch_inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* touch_inputdata));
/**@}*/

//-------------------------------------------------------------------------------
//-------------------------------- HELPER FUNCTIONS------------------------------
//-------------------------------------------------------------------------------
void _lui_mem_init(uint8_t mem_block[], uint16_t size);
void* _lui_mem_alloc(uint16_t element_size);
lui_obj_t* _lui_process_input_of_act_scene();
lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj_root, lui_obj_t* obj_excluded);
lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj);
void _lui_set_obj_props_on_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
uint8_t _lui_check_if_active_obj_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
const lui_font_t* _lui_get_font_from_active_scene();
uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state);

/**
 * @defgroup lui_gfx Graphics related API (for drawing shapes and text)
 * @{
 */
void lui_gfx_draw_string_advanced(const char* str, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font);
void lui_gfx_draw_string_simple(const char* str, uint16_t x, uint16_t y, uint16_t fore_color, const lui_font_t* font);
void lui_gfx_draw_char(char c, uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font);
void lui_gfx_get_string_dimension(const char* str, const lui_font_t* font_obj, uint16_t max_w, uint16_t* str_dim);
void lui_gfx_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_gfx_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void lui_gfx_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
uint16_t lui_rgb(uint16_t red, uint16_t green, uint16_t blue);
/**@}*/
const _lui_glyph_t* _lui_gfx_get_glyph_from_char(char c, const lui_font_t* font);
void _lui_gfx_render_char_glyph(uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const _lui_glyph_t* glyph, const lui_font_t* font);
void _lui_gfx_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_gfx_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
#endif /* INC_LAME_UI_H_ */