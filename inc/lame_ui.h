/*
 * lame_ui.h
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 16-May-2020
 *      Author: rik
 */

#ifndef INC_LAME_UI_H_
#define INC_LAME_UI_H_

#include <stdlib.h>
#include <stdio.h>
#include "bitmap_typedefs.h"


#define LUI_MAX_SCENES						1
#define LUI_MAX_LABELS_PER_SCENE			10
#define LUI_MAX_LINE_CHARTS_PER_SCENE		3
#define LUI_MAX_BUTTONS_PER_SCENE			3

#define LUI_DEFAULT_TEXT_FORECOLOR			0xFFFF
#define LUI_DEFAULT_TEXT_BGCOLOR			0x0000
#define LUI_DEFAULT_SCENE_BACKGROUND		0x0000
#define LUI_DEFAULT_LINE_CHART_FORECOLOR	0xFFFF
#define LUI_DEFAULT_LINE_CHART_BGCOLOR		0x0000


#define	_LUI_R_POS_RGB   11	// Red last bit position for RGB display
#define	_LUI_G_POS_RGB   5 	// Green last bit position for RGB display
#define	_LUI_B_POS_RGB   0	// Blue last bit position for RGB display

#define	LUI_RGB(R,G,B) \
	(((uint16_t)(R >> 3) << _LUI_R_POS_RGB) | \
	((uint16_t)(G >> 2) << _LUI_G_POS_RGB) | \
	((uint16_t)(B >> 3) << _LUI_B_POS_RGB))

enum lui_state_e
{
	LUI_STATE_IDLE = 0,
	LUI_STATE_SELECTED,
	LUI_STATE_PRESSED,
	LUI_STATE_ENTERED
};

// change all enums with int or #define later to save memory
enum lui_event_e
{
	LUI_EVENT_NONE = 0,
	LUI_EVENT_SELECTED,
	LUI_EVENT_SELECTION_LOST,
	LUI_EVENT_PRESSED,
	LUI_EVENT_RELEASED,
	LUI_EVENT_ENTERED,
	LUI_EVENT_EXITED,
	LUI_EVENT_VALUE_CHANGED
};

enum lui_obj_type_e
{
	LUI_OBJ_LABEL = 0,
	LUI_OBJ_BUTTON,
	LUI_OBJ_SWITCH,
	LUI_OBJ_LINECHART,
	LUI_OBJ_GENERIC
};

// typedef struct lui_common_s
// {
// 	uint16_t x;
// 	uint16_t y;
// 	uint16_t width;
// 	uint16_t height;

// 	//private use only
// 	uint8_t needs_refresh;
// 	int8_t index;
// 	int8_t parent_index;
// 	uint8_t obj_type;
// }lui_common_t;

typedef struct lui_linechart_s
{
	struct
	{
		uint8_t hor_count;
		uint8_t vert_count;
		uint16_t color;
		uint8_t is_grid;
	}grid;

	struct
	{
		double *source;
		uint16_t points;
		uint8_t auto_scale;
		double y_max_value;
		double y_min_value;
	}data;

	uint16_t color;
	tFont *font;
}lui_chart_t;


typedef struct lui_label_s
{
	char *text;
	tFont *font;
	uint16_t color;
}lui_label_t;


typedef struct lui_button_s
{
	struct
	{
		char *text;
		uint16_t color;
		tFont *font;
	}label;

	uint16_t pressed_color;
	uint16_t selection_color;
	uint16_t color;

	int8_t dpad_row_pos;
	int8_t dpad_col_pos;

}lui_button_t;


typedef struct lui_switch_s
{
	uint16_t selection_color;
	uint16_t fore_color;

	int8_t dpad_row_pos;
	int8_t dpad_col_pos;
}lui_switch_t;

typedef struct lui_obj_s
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

	uint16_t bg_color;
	uint16_t border_color;
	uint8_t state;
	uint8_t event;
	int32_t value;
	void (*obj_event_cb)(struct lui_obj_s *obj);
	//private use only
	uint8_t needs_refresh;
	int8_t index;
	int8_t parent_index;
	uint8_t obj_type;

	void *obj_main_data;
}lui_obj_t;


typedef struct lui_scene_s
{
	lui_obj_t **obj_collection;

	struct
	{
		uint16_t color;
		tImage *image;
	}background;

	uint8_t obj_total;

	tFont *font;

	//void (*state_change_cb)(tLuiState state); 

	struct
	{
		int8_t max_row_pos;
		int8_t max_col_pos;
		int8_t current_row_pos;
		int8_t current_col_pos;
	}dpad; // dpad controls are scene specific, so storing it in the scene variable

	lui_obj_t *active_obj;

	uint8_t needs_refresh;
	uint8_t index;	//scene's index will always be non-negative
}lui_scene_t;


typedef struct lui_touch_input_data_s
{
	uint8_t is_pressed;
	uint16_t x;
	uint16_t y;
}lui_touch_input_data_t;


typedef struct lui_dpad_input_data_s
{
	uint8_t is_left_pressed;
	uint8_t is_right_pressed;
	uint8_t is_down_pressed;
	uint8_t is_up_pressed;
	uint8_t is_enter_pressed;
	uint8_t is_cancel_pressed;
}lui_dpad_input_data_t;


typedef struct lui_disp_drv_s
{
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	void (*render_complete_cb)();
	uint16_t display_hor_res;
	uint16_t display_vert_res;
}lui_dispdrv_t;

typedef struct lui_touch_input_dev_s
{
	void (*read_touch_input_cb)(lui_touch_input_data_t *input);
	//lui_touch_input_data_t touch_data;
}lui_touch_input_dev_t;

typedef struct lui_dpad_input_dev_s
{
	void (*read_dpad_input_cb)(lui_dpad_input_data_t *input);
	//lui_dpad_input_data_t dpad_data;
	
}lui_dpad_input_dev_t;


typedef struct lui_main_s
{
	lui_scene_t *scenes[LUI_MAX_SCENES];
	lui_dispdrv_t *disp_drv;
	lui_touch_input_dev_t *touch_input_dev;
	lui_dpad_input_dev_t *dpad_input_dev;
	uint8_t total_scenes;
	int8_t active_scene_id;	//not using right now. For future use (possibily)
}lui_main_t;


lui_obj_t* lui_label_create();
void lui_label_draw (lui_obj_t *lbl);
void lui_label_set_font(const tFont *font, lui_obj_t *obj);
void lui_label_set_text(const char *text, lui_obj_t *obj);
void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, lui_obj_t *obj);

// Experiments
void lui_scene_render_new(lui_scene_t *scene);
void lui_obj_render(lui_obj_t *obj);
//=====================


lui_obj_t* lui_linechart_create();
void lui_linechart_draw (lui_obj_t *obj);
void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, lui_obj_t *obj);
void lui_linechart_set_grid_visible(uint8_t state, lui_obj_t *obj);
void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, lui_obj_t *obj);
void lui_linechart_set_data_auto_scale(uint8_t auto_scale, lui_obj_t *obj);
void lui_linechart_set_data_range(double y_min, double y_max, lui_obj_t *obj);
void lui_linechart_set_data_source(double *source, uint16_t points, lui_obj_t *obj);


lui_obj_t* lui_button_create();
void lui_button_draw(lui_obj_t *btn);
void lui_button_set_label_text(const char *text, lui_obj_t *btn);
void lui_button_set_label_color(uint16_t color, lui_obj_t *btn);
void lui_button_set_label_font(const tFont *font, lui_obj_t *btn);
void lui_button_set_colors(uint16_t bg_color, uint16_t pressed_color, uint16_t selection_color, lui_obj_t *btn);

lui_obj_t* lui_switch_create();
void lui_switch_draw(lui_obj_t *swtch);
void lui_switch_set_colors(uint16_t fore_color, uint16_t bg_color, uint16_t selection_color, lui_obj_t *obj);
int8_t lui_switch_get_value(lui_obj_t *obj);
void lui_switch_set_value(uint8_t value, lui_obj_t *obj);

lui_scene_t* lui_scene_create();
void* lui_scene_destroy(lui_scene_t *scene);
void lui_scene_set_active(lui_scene_t *scene);
void lui_scene_render(lui_scene_t *scene);
void lui_scene_set_bg_color(uint16_t color, lui_scene_t *scene);
void lui_scene_set_bg_image(const tImage *image, lui_scene_t *scene);
void lui_scene_set_font(const tFont *font, lui_scene_t *scene);


void lui_object_add_to_scene(lui_obj_t *obj, lui_scene_t *scene);
void lui_object_remove_from_scene(lui_obj_t *obj);
void lui_object_destroy(lui_obj_t *obj);
void lui_object_set_area(uint16_t width, uint16_t height, lui_obj_t *obj);
void lui_object_set_position(uint16_t x, uint16_t y, lui_obj_t *obj);
void lui_object_set_callback(void (*obj_event_cb)(lui_obj_t *), lui_obj_t *obj);
int8_t lui_object_get_state(lui_obj_t *obj);
int8_t lui_object_get_event(lui_obj_t *obj);

lui_obj_t* _lui_process_touch_input(lui_scene_t *scene);
void _lui_process_dpad_input(lui_scene_t *scene);
void _lui_set_obj_props_on_input(lui_touch_input_data_t input, lui_obj_t *obj);
uint8_t _lui_check_if_active_obj(lui_touch_input_data_t input, lui_obj_t *obj);


lui_dispdrv_t* lui_dispdrv_create();
void lui_dispdrv_register(lui_dispdrv_t *dispdrv);
void* lui_dispdrv_destroy(lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res, lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_render_complete_cb(void (*render_complete_cb)(), lui_dispdrv_t *dispdrv);
uint8_t _lui_disp_drv_check();


lui_touch_input_dev_t* lui_touch_inputdev_create();
void lui_touch_inputdev_register (lui_touch_input_dev_t *touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(lui_touch_input_data_t *touch_inputdata), lui_touch_input_dev_t *touch_inputdev);
lui_dpad_input_dev_t* lui_dpad_inputdev_create();
void lui_dpad_inputdev_register (lui_dpad_input_dev_t *dpad_inputdev);
void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(lui_dpad_input_data_t *dpad_inputdata), lui_dpad_input_dev_t *dpad_inputdev);


//-------------------------------------------------------------------------------
//------------ PRIVATE HELPER FUNCTIONS TO BE USED BY THIS LIBRARY ONLY----------
//-------------------------------------------------------------------------------
uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state);
void _lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph);
void _get_string_dimension(const char *str, const tFont *font_obj, uint8_t *str_dim);
void _lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
void _lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void _lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif /* INC_LAME_UI_H_ */
