/*
 *  Created on: 02-Apr-2020
 */
/**
 * @file lame_ui.h
 * @author Avra Mitra
 * @brief One and only header file for LameUI library.
 * @version 0.1
 * @date 2022-04-24
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

#define LUI_MEM_MAX_SIZE					20000	// Max allocable size (in Bytes) for UI elements
/* NOTE: LUI_MEM_DEFRAG_EN increases memory overhead. Enable it only if free() and destroy() are used often */
#define LUI_MEM_DEFRAG_EN					1		// Enable memory defragment (1: enable, 0: disable) 
#define	LUI_MAX_OBJECTS						130
//#define LUI_MAX_SCENES						3


#define	_LUI_R_POS_RGB   					11	// Red last bit position for RGB display
#define	_LUI_G_POS_RGB   					5 	// Green last bit position for RGB display
#define	_LUI_B_POS_RGB   					0	// Blue last bit position for RGB display

#define	_LUI_RGB(R,G,B) 					(((uint16_t)(R >> 3) << _LUI_R_POS_RGB) | \
											((uint16_t)(G >> 2) << _LUI_G_POS_RGB) | \
											((uint16_t)(B >> 3) << _LUI_B_POS_RGB))


/*------------------------------------------------------------------------------------
 *				Default Dark Theme. User may modify here
 *------------------------------------------------------------------------------------
 */
#define LUI_STYLE_BUTTON_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_BUTTON_PRESSED_COLOR		_LUI_RGB(91, 160, 235)
#define	LUI_STYLE_BUTTON_SELECTION_COLOR	_LUI_RGB(82, 143, 209)
#define LUI_STYLE_BUTTON_BG_COLOR			_LUI_RGB(74, 129, 188)
#define LUI_STYLE_BUTTON_BORDER_COLOR		_LUI_RGB(75, 81, 92)
#define LUI_STYLE_BUTTON_BORDER_VISIBLE		0
#define LUI_STYLE_BUTTON_WIDTH				40
#define LUI_STYLE_BUTTON_HEIGHT				30

#define LUI_STYLE_LABEL_TEXT_COLOR			_LUI_RGB(238, 238, 238)
#define LUI_STYLE_LABEL_BG_COLOR			_LUI_RGB(23, 33, 43)
#define LUI_STYLE_LABEL_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_LABEL_BORDER_VISIBLE		0
#define LUI_STYLE_LABEL_WIDTH				0 /*40*/
#define LUI_STYLE_LABEL_HEIGHT				0 /*30*/

#define	LUI_STYLE_SWITCH_SELECTION_COLOR	_LUI_RGB(0, 170, 179)
#define	LUI_STYLE_SWITCH_KNOB_OFF_COLOR		_LUI_RGB(57, 62, 70)
#define	LUI_STYLE_SWITCH_KNOB_ON_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_SWITCH_BG_COLOR			_LUI_RGB(23, 33, 43)
#define LUI_STYLE_SWITCH_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_SWITCH_BORDER_VISIBLE		1
#define LUI_STYLE_SWITCH_WIDTH				40
#define LUI_STYLE_SWITCH_HEIGHT				20

#define	LUI_STYLE_CHECKBOX_SELECTION_COLOR	_LUI_RGB(82, 143, 209)
#define	LUI_STYLE_CHECKBOX_TICK_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_CHECKBOX_BG_COLOR			_LUI_RGB(23, 33, 43)
#define LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR	_LUI_RGB(74, 129, 188)
#define LUI_STYLE_CHECKBOX_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_CHECKBOX_BORDER_VISIBLE	1
#define LUI_STYLE_CHECKBOX_WIDTH			20
#define LUI_STYLE_CHECKBOX_HEIGHT			LUI_STYLE_CHECKBOX_WIDTH

#define	LUI_STYLE_SLIDER_SELECTION_COLOR	_LUI_RGB(0, 170, 179)
#define	LUI_STYLE_SLIDER_KNOB_COLOR			_LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BG_COLOR			_LUI_RGB(57, 62, 70)
#define LUI_STYLE_SLIDER_BG_FILLED_COLOR	_LUI_RGB(45, 77, 112) /*54, 94, 138*/
#define LUI_STYLE_SLIDER_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_SLIDER_BORDER_VISIBLE		0
#define LUI_STYLE_SLIDER_KNOB_WIDTH			20
#define LUI_STYLE_SLIDER_WIDTH				80
#define LUI_STYLE_SLIDER_HEIGHT				20

#define LUI_STYLE_LINECHART_LINE_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_LINECHART_GRID_COLOR		_LUI_RGB(75, 81, 92)
#define LUI_STYLE_LINECHART_GRID_VISIBLE	1
#define LUI_STYLE_LINECHART_BG_COLOR		_LUI_RGB(35, 46, 60)
#define LUI_STYLE_LINECHART_BORDER_COLOR	_LUI_RGB(74, 129, 188)
#define LUI_STYLE_LINECHART_BORDER_VISIBLE	1
#define LUI_STYLE_LINECHART_WIDTH			40
#define LUI_STYLE_LINECHART_HEIGHT			20

#define LUI_STYLE_LIST_NAV_BG_COLOR			_LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_NAV_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_NAV_PRESSED_COLOR	_LUI_RGB(91, 160, 235)
#define LUI_STYLE_LIST_NAV_SELECTION_COLOR	_LUI_RGB(82, 143, 209)
#define LUI_STYLE_LIST_ITEM_BG_COLOR		_LUI_RGB(57, 62, 70)
#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR	_LUI_RGB(84, 91, 102)
#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR	_LUI_RGB(109, 118, 133)
#define LUI_STYLE_LIST_ITEM_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define	LUI_STYLE_LIST_ITEM_BORDER_COLOR	_LUI_RGB(75, 81, 92)
#define LUI_STYLE_LIST_ITEM_BORDER_VISIBLE	1
#define LUI_STYLE_LIST_ITEM_MIN_HEIGHT		30
#define LUI_STYLE_LIST_BORDER_COLOR			_LUI_RGB(74, 129, 188)
#define LUI_STYLE_LIST_BORDER_VISIBLE		0
#define LUI_STYLE_LIST_WIDTH				40
#define LUI_STYLE_LIST_HEIGHT				60

#define	LUI_STYLE_BTNGRID_BASE_BG_COLOR		_LUI_RGB(23, 33, 43)
#define LUI_STYLE_BTNGRID_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_BTNGRID_PRESSED_COLOR		_LUI_RGB(91, 160, 235)
#define	LUI_STYLE_BTNGRID_BG_COLOR			_LUI_RGB(39, 55, 71)
#define	LUI_STYLE_BTNGRID_SELECTION_COLOR	_LUI_RGB(82, 143, 209)
#define LUI_STYLE_BTNGRID_BORDER_COLOR		_LUI_RGB(75, 81, 92)
#define LUI_STYLE_BTNGRID_BORDER_VISIBLE	0
#define	LUI_STYLE_BTNGRID_WIDTH				300
#define LUI_STYLE_BTNGRID_HEIGHT			180

#define LUI_STYLE_TEXTBOX_TEXT_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_TEXTBOX_BG_COLOR			_LUI_RGB(45, 56, 70)
#define LUI_STYLE_TEXTBOX_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_TEXTBOX_BORDER_VISIBLE	1
#define LUI_STYLE_TEXTBOX_WIDTH				200
#define LUI_STYLE_TEXTBOX_HEIGHT			20

#define LUI_STYLE_PANEL_BG_COLOR			_LUI_RGB(23, 33, 43)
#define LUI_STYLE_PANEL_BORDER_COLOR		_LUI_RGB(74, 129, 188)
#define LUI_STYLE_PANEL_BORDER_VISIBLE		1
#define LUI_STYLE_PANEL_WIDTH				100
#define LUI_STYLE_PANEL_HEIGHT				100

#define LUI_STYLE_SCENE_BG_COLOR			_LUI_RGB(23, 33, 43)
/*------------------------------------------------------------------------------------
 *									End Theme
 *------------------------------------------------------------------------------------
 */

#ifndef NULL
	#define NULL							((void* )0)
#endif


#define LUI_INPUT_TYPE_TOUCH				1
#define LUI_INPUT_TYPE_ENCODER				2

#define	LUI_STATE_IDLE						0
#define	LUI_STATE_SELECTED					1
#define	LUI_STATE_PRESSED					2
#define	LUI_STATE_ENTERED					3


#define	LUI_EVENT_NONE						0
#define	LUI_EVENT_SELECTED					1
#define	LUI_EVENT_SELECTION_LOST			2
#define	LUI_EVENT_PRESSED					3
#define	LUI_EVENT_RELEASED					4
#define	LUI_EVENT_ENTERED					5
#define	LUI_EVENT_EXITED					6
#define	LUI_EVENT_VALUE_CHANGED				7
#define	LUI_EVENT_CHECK_CHANGED				8


#define	LUI_OBJ_NONE						0
#define	LUI_OBJ_LABEL						1
#define	LUI_OBJ_BUTTON						2
#define	LUI_OBJ_SWITCH						3
#define	LUI_OBJ_LINECHART					4
#define	LUI_OBJ_PANEL						5
#define	LUI_OBJ_SCENE						6
#define	LUI_OBJ_GENERIC						7
#define LUI_OBJ_LIST						8
#define LUI_OBJ_CHECKBOX					9
#define LUI_OBJ_SLIDER						10
#define LUI_OBJ_BTNGRID						11
#define LUI_OBJ_TEXTBOX						12

#define LUI_KEYBOARD_MODE_TXT_LOWER			1
#define LUI_KEYBOARD_MODE_TXT_UPPER			2
#define LUI_KEYBOARD_MODE_TXT_SPCL			3
#define LUI_KEYBOARD_MODE_NUMPAD			4

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



#define _LUI_BTNGRID_MASK_BTN_IS_DISABLED	0x40
#define _LUI_BTNGRID_MASK_BTN_IS_HIDDEN		0x20
#define _LUI_BTNGRID_MASK_BTN_IS_CHECKABLE	0x10
#define _LUI_BTNGRID_MASK_BTN_IS_CHECKED	0x80
#define _LUI_BTNGRID_MASK_BTN_WIDTH_UNIT	0x0F


/* Defined at the top for user's convenience */
// #define LUI_MEM_MAX_SIZE					10000

#define LUI_MEM_CHUNK_STATUS_FREE			0
#define LUI_MEM_CHUNK_STATUS_USED			1
#define LUI_MEM_CHUNK_TYPE_NEW				2
#define LUI_MEM_CHUNK_TYPE_REUSE			3
#define LUI_MEM_CHUNK_TYPE_NONE				4

#pragma pack(push, 1)

typedef struct
{
    uint8_t mem_block[LUI_MEM_MAX_SIZE];
	uint8_t* mem_start;
	uint8_t* mem_end;
    uint16_t mem_chunk_count;
    uint16_t mem_allocated;
}_lui_mem_block_t;

typedef struct _lui_mem_chunk
{
    uint16_t alloc_size;
    uint8_t alloc_status;
	#if (LUI_MEM_DEFRAG_EN == 1)
		struct _lui_mem_chunk* prev_chunk;
		struct _lui_mem_chunk* next_chunk;
	#endif
}_lui_mem_chunk_t;

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
	uint8_t border_visible;
	uint16_t width;
	uint16_t height;
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
struct _lui_switch_style_s
{
	uint16_t knob_on_color;
	uint16_t knob_off_color;
	uint16_t selection_color;
};
struct _lui_checkbox_style_s
{
	uint16_t tick_color;
	uint16_t bg_checked_color;
	uint16_t selection_color;
};
struct _lui_slider_style_s
{
	uint16_t knob_color;
	uint16_t bg_filled_color;
	uint16_t selection_color;
	uint8_t knob_width;
};
struct _lui_linechart_style_s
{
	uint16_t line_color;
	uint16_t grid_color;
	uint8_t grid_visible;
};
struct _lui_list_style_s
{
	struct _lui_button_style_s item_btn;
	struct _lui_common_style_s item_btn_common;
};
struct _lui_btngrid_style_s
{
	uint8_t btn_margin_hor;
    uint8_t btn_margin_vert;
	uint16_t btn_label_color;
	uint16_t btn_pressed_color;
	uint16_t btn_bg_color;
};
struct _lui_textbox_style_s
{
	uint16_t text_color;
};

typedef struct _lui_obj_s
{
	uint16_t x;
	uint16_t y;
	struct _lui_common_style_s common_style;
	uint8_t state;
	uint8_t event;
	int32_t value;
	void (*obj_event_cb)(struct _lui_obj_s* obj);
	//private use only
	uint8_t needs_refresh;
	uint8_t visible;
	uint8_t enabled;
	//int8_t index;
	
	uint8_t obj_type;

	struct _lui_obj_s* parent;
	uint8_t children_count;
	struct _lui_obj_s* first_child;
	struct _lui_obj_s* next_sibling;
	

	void* obj_main_data;
}lui_obj_t;


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

	int8_t encoder_index;
	struct _lui_button_style_s style;
}lui_button_t;


typedef struct _lui_switch_s
{
	int8_t encoder_index;

	struct _lui_switch_style_s style;
}lui_switch_t;


typedef struct _lui_checkbox_s
{
	int8_t encoder_index;

	struct _lui_checkbox_style_s style;
}lui_checkbox_t;


typedef struct _lui_slider_s
{
	int8_t encoder_index;
	int16_t range_min;
	int16_t range_max;
	// knob's center's x position relative to slider's start x position
	uint16_t knob_center_rel_x; 

	struct _lui_slider_style_s style;
}lui_slider_t;


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

typedef struct _lui_keyboard_s
{
	lui_obj_t* target_txtbox;
	uint8_t keyboard_mode;

}lui_keyboard_t;

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
	struct _lui_keyboard_s* kb_data;
}lui_btngrid_t;




typedef struct _lui_textbox_s
{
	char* text_buffer;
	const lui_font_t* font;
	uint16_t max_len;
	uint16_t caret_index;
	int8_t edit_operation;
	uint16_t edit_len;
	uint16_t used_chars;
	uint8_t needs_full_render;
	struct _lui_textbox_style_s style;
}lui_textbox_t;


typedef struct _lui_encoder_processed_s
{
	int8_t max_encoder_index;
	int8_t current_encoder_index;
	uint8_t object_state;
}_lui_encoder_processed_t;

typedef struct _lui_panel_s
{
	lui_bitmap_t* bg_image;
	_lui_encoder_processed_t encoder; // these encoder controls are required when the panel is a popup
}lui_panel_t;

typedef struct _lui_scene_s
{
	lui_bitmap_t* bg_image;
	lui_obj_t* obj_popup;
	uint8_t popup_type_locked; // Locked: can't be dismissed by clicking outside
	const lui_font_t* font;
	_lui_encoder_processed_t encoder; // encoder controls are scene specific, so storing it in the scene variable

}lui_scene_t;


typedef struct _lui_touch_input_data_s
{
	uint8_t is_pressed;
	int16_t x;
	int16_t y;
}lui_touch_input_data_t;


typedef struct _lui_encoder_input_data_s
{
	int16_t steps;
	uint8_t is_pressed;
}lui_encoder_input_data_t;


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

typedef struct _lui_encoder_input_dev_s
{
	void (*read_encoder_input_cb)(lui_encoder_input_data_t* input);
	//lui_encoder_input_data_t encoder_data;
	
}lui_encoder_input_dev_t;


typedef struct _lui_main_s
{
	//lui_obj_t* scenes[LUI_MAX_SCENES];
	lui_font_t* default_font;
	lui_obj_t* active_scene;
	lui_obj_t* active_obj;
	lui_dispdrv_t* disp_drv;
	lui_touch_input_dev_t* touch_input_dev;
	//lui_encoder_input_dev_t* encoder_input_dev;
	// lui_touch_input_data_t last_touch_data;
	uint8_t input_state_pressed;
	uint8_t input_event_clicked;
	uint8_t total_scenes;
	uint8_t total_created_objects;	//increases as new objs are created. It never decreases
}lui_main_t;

#pragma pack(pop)

void lui_init();
void lui_update();


lui_obj_t* _lui_object_create();
void lui_object_add_to_parent(lui_obj_t* obj, lui_obj_t* parent_obj);
void lui_object_remove_from_parent(lui_obj_t* obj);
void* lui_object_destroy(lui_obj_t* obj);
void lui_object_set_area(lui_obj_t* obj, uint16_t width, uint16_t height);
void lui_object_set_width(lui_obj_t* obj, uint16_t width);
void lui_object_set_height(lui_obj_t* obj, uint16_t height);
void lui_object_set_position(lui_obj_t* obj, uint16_t x, uint16_t y);
void lui_object_set_x_pos(lui_obj_t* obj, uint16_t x);
void lui_object_set_y_pos(lui_obj_t* obj, uint16_t y);
void lui_object_set_border_color(lui_obj_t* obj, uint16_t border_color);
void lui_object_set_border_visibility(lui_obj_t* obj, uint8_t is_visible);
void lui_object_set_bg_color(lui_obj_t* obj, uint16_t color);
void lui_object_set_callback(lui_obj_t* obj, void (*obj_event_cb)(lui_obj_t* ));
int8_t lui_object_get_state(lui_obj_t* obj);
int8_t lui_object_get_event(lui_obj_t* obj);
void lui_object_set_visibility(lui_obj_t* obj, uint8_t visible);
uint8_t lui_object_set_enable_input(lui_obj_t* obj,  uint8_t is_enabled);


lui_obj_t* lui_label_create();
void lui_label_draw (lui_obj_t* obj_lbl);
void lui_label_set_font(lui_obj_t* obj_lbl, const lui_font_t* font);
void lui_label_set_text(lui_obj_t* obj_lbl, const char* text);
void lui_label_set_text_color(lui_obj_t* obj_lbl, uint16_t text_color);


lui_obj_t* lui_linechart_create();
void lui_linechart_draw (lui_obj_t* obj_linechart);
void lui_linechart_set_grid_count(lui_obj_t* obj_linechart, uint16_t hor_lines, uint16_t vert_lines);
void lui_linechart_set_grid_color(lui_obj_t* obj_linechart, uint16_t color);
void lui_linechart_set_grid_visible(lui_obj_t* obj_linechart, uint8_t state);
void lui_linechart_set_line_color(lui_obj_t* obj_linechart, uint16_t line_color);
void lui_linechart_set_data_auto_scale(lui_obj_t* obj_linechart, uint8_t auto_scale);
void lui_linechart_set_data_range(lui_obj_t* obj_linechart, double y_min, double y_max);
void lui_linechart_set_data_source(lui_obj_t* obj_linechart, double* source, uint16_t points);


lui_obj_t* lui_button_create();
void lui_button_draw(lui_obj_t* obj_btn);
void lui_button_set_label_text(lui_obj_t* obj_btn, const char* text);
void lui_button_set_label_color(lui_obj_t* obj_btn, uint16_t color);
void lui_button_set_label_font(lui_obj_t* obj_btn, const lui_font_t* font);
void lui_button_set_extra_colors(lui_obj_t* obj_btn, uint16_t pressed_color, uint16_t selection_color);
void lui_button_set_encoder_index(lui_obj_t* obj_btn, uint8_t index);


lui_obj_t* lui_switch_create();
void lui_switch_draw(lui_obj_t* obj_swtch);
void lui_switch_set_extra_colors(lui_obj_t* obj_swtch, uint16_t knob_off_color, uint16_t knob_on_color, uint16_t selection_color);
int8_t lui_switch_get_value(lui_obj_t* obj_swtch);
void lui_switch_set_value(lui_obj_t* obj_swtch, uint8_t value);
void lui_switch_set_on(lui_obj_t* obj_swtch);
void lui_switch_set_off(lui_obj_t* obj_swtch);
void lui_switch_set_encoder_index(lui_obj_t* obj_swtch, uint8_t index);


lui_obj_t* lui_checkbox_create();
void lui_checkbox_draw(lui_obj_t* obj_checkbox);
void lui_checkbox_set_extra_colors(lui_obj_t* obj_checkbox, uint16_t bg_checked_color, uint16_t tick_color, uint16_t selection_color);
int8_t lui_checkbox_get_value(lui_obj_t* obj_checkbox);
void lui_checkbox_set_value(lui_obj_t* obj_checkbox, uint8_t value);
void lui_checkbox_set_checked(lui_obj_t* obj_swtch);
void lui_checkbox_set_unchecked(lui_obj_t* obj_swtch);
void lui_checkbox_set_encoder_index(lui_obj_t* obj_checkbox, uint8_t index);


lui_obj_t* lui_slider_create();
void lui_slider_draw(lui_obj_t* obj_slider);
void lui_slider_set_extra_colors(lui_obj_t* obj_slider, uint16_t knob_color, uint16_t bg_filled_color, uint16_t selection_color);
void lui_slider_set_value(lui_obj_t* obj_slider, int16_t value);
void lui_slider_set_range(lui_obj_t* obj_slider, int16_t range_min, int16_t range_max);
int16_t lui_slider_get_value(lui_obj_t* obj_slider);
int16_t lui_slider_get_min_value(lui_obj_t* obj_slider);
int16_t lui_slider_get_max_value(lui_obj_t* obj_slider);
//void lui_slider_set_knob_length(uint8_t length, lui_obj_t* obj_slider);
void lui_slider_set_encoder_index(lui_obj_t* obj_slider, uint8_t index);


lui_obj_t* lui_list_create();
void lui_list_draw(lui_obj_t* obj_list);
lui_obj_t* lui_list_add_item(lui_obj_t* obj_list, const char* text);
void lui_list_delete_item(lui_obj_t** obj_item_addr);
void lui_list_prepare(lui_obj_t* obj_list);
void lui_list_set_item_min_height(lui_obj_t* obj_list, uint8_t height);
void lui_list_set_font(lui_obj_t* obj_list, const lui_font_t* font);
void lui_list_set_nav_btn_label_color(lui_obj_t* obj_list, uint16_t color);
void lui_list_set_nav_btn_bg_color(lui_obj_t* obj_list, uint16_t color);
void lui_list_set_nav_btn_extra_colors(lui_obj_t* obj_list, uint16_t pressed_color, uint16_t selection_color);
void _lui_list_add_nav_buttons(lui_obj_t* obj_list);
void _lui_list_nav_btn_cb(lui_obj_t* obj_list);
void _lui_list_add_button_obj(lui_obj_t* obj_list, lui_obj_t* obj_btn);


lui_obj_t* lui_btngrid_create();
void lui_btngrid_draw(lui_obj_t* obj);
void lui_btngrid_set_textmap(lui_obj_t* obj, const char* texts[]);
void lui_btngrid_set_propertymap(lui_obj_t* obj, uint8_t properties[]);
void lui_btngrid_set_btn_property_bits(lui_obj_t* obj, uint16_t btn_index, uint8_t property_byte);
void lui_btngrid_set_btn_text(lui_obj_t* obj, uint8_t btn_index, char* text);
void lui_btngrid_set_btn_width_unit(lui_obj_t* obj, uint16_t btn_index, uint8_t width_unit);
void lui_btngrid_set_btn_hidden(lui_obj_t* obj, uint16_t btn_index, uint8_t hidden);
void lui_btngrid_set_btn_checkable(lui_obj_t* obj, uint16_t btn_index, uint8_t checkable);
void lui_btngrid_set_btn_checked(lui_obj_t* obj, uint16_t btn_index, uint8_t checked);
int16_t lui_btngrid_get_acive_btn_index(lui_obj_t* obj);
int8_t lui_btngrid_get_btn_check_status(lui_obj_t* obj, uint8_t btn_index);
void lui_btngrid_set_font(lui_obj_t* obj, const lui_font_t* font);
void lui_btngrid_set_extra_colors(lui_obj_t* obj, uint16_t btn_color, uint16_t label_color, uint16_t btn_pressed_color);
void lui_btngrid_set_btn_margin(lui_obj_t* obj, uint8_t margin_x, uint16_t margin_y);
void _lui_btngrid_calc_btn_area(lui_obj_t* obj);


lui_obj_t* lui_keyboard_create();
const char* lui_keyboard_get_key_text(lui_obj_t* obj, uint8_t btn_index);
void lui_keyboard_set_font(lui_obj_t* obj, const lui_font_t* font);
void lui_keyboard_set_target_txtbox(lui_obj_t* obj_kb, lui_obj_t* obj_txtbox);

lui_obj_t* lui_textbox_create();
void lui_textbox_draw();
void lui_textbox_set_caret_index(lui_obj_t* obj, uint16_t caret_index);
uint16_t lui_textbox_get_caret_index(lui_obj_t* obj);
void lui_textbox_insert_char(lui_obj_t* obj, char c);
void lui_textbox_insert_string(lui_obj_t* obj, char* str, uint16_t len);
void lui_textbox_delete_char(lui_obj_t* obj);
void lui_textbox_set_text_buffer(lui_obj_t* obj, char* text_buffer, uint16_t buff_size);
void lui_textbox_set_font(lui_obj_t* obj, const lui_font_t* font);


lui_obj_t* lui_panel_create();
void lui_panel_draw(lui_obj_t* obj_panel);
void lui_panel_set_bg_image(lui_obj_t* obj_panel, const lui_bitmap_t* image);


lui_obj_t* lui_scene_create();
void lui_scene_draw(lui_obj_t* obj_scene);
void lui_scene_set_active(lui_obj_t* obj_scene);
lui_obj_t* lui_scene_get_active();
void lui_scene_set_bg_image(lui_obj_t* obj_scene, const lui_bitmap_t* image);
void lui_scene_set_font(lui_obj_t* obj_scene, const lui_font_t* font);
void lui_scene_set_popup(lui_obj_t* obj_scene, lui_obj_t* obj);
void lui_scene_unset_popup(lui_obj_t* obj_scene);
void lui_scene_set_popup_locked(lui_obj_t* obj_scene, uint8_t is_locked);


lui_dispdrv_t* lui_dispdrv_create();
void lui_dispdrv_register(lui_dispdrv_t* dispdrv);
void* lui_dispdrv_destroy(lui_dispdrv_t* dispdrv);
void lui_dispdrv_set_resolution(lui_dispdrv_t* dispdrv, uint16_t hor_res, uint16_t vert_res);
void lui_dispdrv_set_draw_pixels_area_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color));
void lui_dispdrv_set_render_complete_cb(lui_dispdrv_t* dispdrv, void (*render_complete_cb)());
uint8_t _lui_disp_drv_check();


lui_touch_input_dev_t* lui_touch_inputdev_create();
void lui_touch_inputdev_register (lui_touch_input_dev_t* touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* touch_inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* touch_inputdata));
lui_encoder_input_dev_t* lui_encoder_inputdev_create();
void lui_encoder_inputdev_register(lui_encoder_input_dev_t* encoder_inputdev);
void lui_encoder_inputdev_set_read_input_cb(lui_encoder_input_dev_t* encoder_inputdev, void (*read_encoder_input_cb)(lui_encoder_input_data_t* encoder_inputdata));


//-------------------------------------------------------------------------------
//-------------------------------- HELPER FUNCTIONS------------------------------
//-------------------------------------------------------------------------------
void _lui_mem_init();
void* _lui_mem_alloc(uint16_t element_size);
void _lui_mem_free(void* ptr);
void _lui_object_set_need_refresh(lui_obj_t* obj);
void _lui_object_render_parent_with_children(lui_obj_t* obj);
void _lui_object_render(lui_obj_t* obj);
lui_obj_t* _lui_process_input_of_act_scene();
lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t* touch_input_data, lui_encoder_input_data_t* encoder_input_data, _lui_encoder_processed_t* encoder_processed, lui_obj_t* obj_root, lui_obj_t* obj_excluded);
lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t* touch_input_data, lui_encoder_input_data_t* encoder_input_data, _lui_encoder_processed_t* encoder_processed, lui_obj_t* obj);
void _lui_set_obj_props_on_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
uint8_t _lui_check_if_active_obj_touch_input(lui_touch_input_data_t* input_data, lui_obj_t* obj);
void _lui_set_obj_props_on_encoder_input(lui_encoder_input_data_t* input, _lui_encoder_processed_t* encoder_processed, lui_obj_t* obj);
uint8_t _lui_check_if_active_obj_encoder_input(_lui_encoder_processed_t* encoder_processed, lui_obj_t* obj);
lui_font_t* _lui_get_font_from_active_scene();
uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state);

void lui_gfx_draw_string_advanced(const char* str, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font);
void lui_gfx_draw_string_simple(const char* str, uint16_t x, uint16_t y, uint16_t fore_color, const lui_font_t* font);
void lui_gfx_draw_char(char c, uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font);
_lui_glyph_t* _lui_gfx_get_glyph_from_char(char c, const lui_font_t* font);
void _lui_gfx_render_char_glyph(uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const _lui_glyph_t* glyph, const lui_font_t* font);
void lui_gfx_get_string_dimension(const char* str, const lui_font_t* font_obj, uint16_t max_w, uint16_t* str_dim);
void _lui_gfx_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_gfx_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_gfx_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_gfx_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void lui_gfx_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
uint16_t lui_rgb(uint16_t red, uint16_t green, uint16_t blue);
#endif /* INC_LAME_UI_H_ */
