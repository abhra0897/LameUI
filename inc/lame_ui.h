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


#define USE_LABEL
#define USE_BUTTON
#define USE_LINE_CHART


#define MAX_SCENES						1
#define MAX_LABELS_PER_SCENE			10
#define MAX_LINE_CHARTS_PER_SCENE		3
#define MAX_BUTTONS_PER_SCENE			3

#define DEFAULT_TEXT_FORECOLOR			0xFFFF
#define DEFAULT_TEXT_BGCOLOR			0x0000
#define DEFAULT_SCENE_BACKGROUND		0x0000
#define DEFAULT_LINE_CHART_FORECOLOR	0xFFFF
#define DEFAULT_LINE_CHART_BGCOLOR		0x0000

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
	LUI_OBJ_LINECHART
};

typedef struct lui_common_s
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

	//private use only
	uint8_t needs_refresh;
	int8_t index;
	int8_t parent_index;
	uint8_t obj_type;
}tLuiCommon;

typedef struct lui_linechart_s
{
	struct
	{
		uint8_t is_border;
		uint16_t color;
	}border;

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
	uint16_t bg_color;
	tFont *font;

	tLuiCommon common_data;
}tLuiLineChart;


typedef struct lui_label_s
{
	char *text;
	tFont *font;
	uint16_t color;
	uint16_t bg_color;

	tLuiCommon common_data;
}tLuiLabel;


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

	void (*btn_event_cb)(uint8_t event);
	uint8_t state;
	uint8_t event;

	tLuiCommon common_data;
}tLuiButton;


typedef struct lui_switch_s
{
	uint16_t bg_color;
	uint16_t selection_color;
	uint16_t fore_color;

	int8_t dpad_row_pos;
	int8_t dpad_col_pos;

	uint8_t state;
	uint8_t event;
	uint8_t value;
	void (*sw_event_cb)(uint8_t event);

	tLuiCommon common_data;
}tLuiSwitch;


typedef struct lui_scene_s
{
	tLuiLineChart **lui_line_chart;
	tLuiLabel **lui_label;
	tLuiButton **lui_button;
	tLuiSwitch **lui_switch;

	struct
	{
		uint16_t color;
		tImage *image;
	}background;

	uint8_t label_total;
	uint8_t line_chart_total;
	uint8_t button_total;
	uint8_t switch_total;
	tFont *font;

	//void (*state_change_cb)(tLuiState state); 

	struct
	{
		int8_t max_row_pos;
		int8_t max_col_pos;
		int8_t current_row_pos;
		int8_t current_col_pos;
	}dpad; // dpad controls are scene specific, so storing it in the scene variable

	uint8_t needs_refresh;
	uint8_t index;	//scene's index will always be non-negative
}tLuiScene;


typedef struct lui_touch_input_data_s
{
	uint8_t is_pressed;
	uint16_t x;
	uint16_t y;
}tLuiTouchInputData;


typedef struct lui_dpad_input_data_s
{
	uint8_t is_left_pressed;
	uint8_t is_right_pressed;
	uint8_t is_down_pressed;
	uint8_t is_up_pressed;
	uint8_t is_enter_pressed;
	uint8_t is_cancel_pressed;
}tLuiDpadInputData;


typedef struct lui_disp_drv_s
{
	void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color);
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	uint16_t display_hor_res;
	uint16_t display_vert_res;
}tLuiDispDrv;

typedef struct lui_touch_input_dev_s
{
	void (*read_touch_input_cb)(tLuiTouchInputData *input);
	//tLuiTouchInputData touch_data;
}tLuiTouchInputDev;

typedef struct lui_dpad_input_dev_s
{
	void (*read_dpad_input_cb)(tLuiDpadInputData *input);
	//tLuiDpadInputData dpad_data;
	
}tLuiDpadInputDev;


typedef struct lui_main_s
{
	tLuiScene *scenes[MAX_SCENES];
	tLuiDispDrv *disp_drv;
	tLuiTouchInputDev *touch_input_dev;
	tLuiDpadInputDev *dpad_input_dev;
	uint8_t total_scenes;
	int8_t active_scene_id;	//not using right now. For future use (possibily)
}tLuiMain;


tLuiLabel* lui_label_create();
void lui_label_draw (tLuiLabel *lbl);
void lui_label_add_to_scene(tLuiLabel *lbl, tLuiScene *scene);
void lui_label_remove_from_scene(tLuiLabel *lbl);
void* lui_label_destroy(tLuiLabel *lbl);
void lui_label_set_position(uint16_t x, uint16_t y, tLuiLabel *lbl);
void lui_label_set_area(uint16_t width, uint16_t height, tLuiLabel *lbl);
void lui_label_set_font(const tFont *font, tLuiLabel *lbl);
void lui_label_set_text(const char *text, tLuiLabel *lbl);
void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, tLuiLabel *lbl);


tLuiLineChart* lui_linechart_create();
void lui_linechart_draw (tLuiLineChart *chart);
void lui_linechart_add_to_scene(tLuiLineChart *chart, tLuiScene *scene);
void lui_linechart_remove_from_scene(tLuiLineChart *chart);
void* lui_linechart_destroy(tLuiLineChart *chart);
void lui_linechart_set_position(uint16_t x, uint16_t y, tLuiLineChart *chart);
void lui_linechart_set_area(uint16_t width, uint16_t height, tLuiLineChart *chart);
void lui_linechart_set_border(uint16_t color, tLuiLineChart *chart);
void lui_linechart_set_border_visible(uint8_t state, tLuiLineChart *chart);
void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, tLuiLineChart *chart);
void lui_linechart_set_grid_visible(uint8_t state, tLuiLineChart *chart);
void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, tLuiLineChart *chart);
void lui_linechart_set_data_range(uint16_t y_min, uint16_t y_max, tLuiLineChart *chart);
void lui_linechart_set_data_source(double *source, uint16_t points, tLuiLineChart *chart);


tLuiButton* lui_button_create();
void lui_button_draw(tLuiButton *btn);
void lui_button_add_to_scene(tLuiButton *btn, tLuiScene *scene);
void lui_button_remove_from_scene(tLuiButton *btn);
void* lui_button_destroy(tLuiButton *btn);
void lui_button_set_position(uint16_t x, uint16_t y, tLuiButton *btn);
void lui_button_set_area(uint16_t width, uint16_t height, tLuiButton *btn);
void lui_button_set_label_text(const char *text, tLuiButton *btn);
void lui_button_set_label_color(uint16_t color, tLuiButton *btn);
void lui_button_set_label_font(const tFont *font, tLuiButton *btn);
void lui_button_set_colors(uint16_t bg_color, uint16_t pressed_color, uint16_t selection_color, tLuiButton *btn);
void lui_button_set_event_cb(void (*btn_event_state_change_cb)(uint8_t), tLuiButton *btn);


tLuiSwitch* lui_switch_create();
void lui_switch_draw(tLuiSwitch *swtch);
void lui_switch_add_to_scene(tLuiSwitch *lui_switch, tLuiScene *scene);
void lui_switch_remove_from_scene(tLuiSwitch *swtch);
void* lui_switch_destroy(tLuiSwitch *swtch);
void lui_switch_set_position(uint16_t x, uint16_t y, tLuiSwitch *swtch);
void lui_switch_set_area(uint16_t width, uint16_t height, tLuiSwitch *swtch);


tLuiScene* lui_scene_create();
void* lui_scene_destroy(tLuiScene *scene);
void lui_scene_set_active(tLuiScene *scene);
void lui_scene_render(tLuiScene *scene);
void lui_scene_set_bg_color(uint16_t color, tLuiScene *scene);
void lui_scene_set_bg_image(const tImage *image, tLuiScene *scene);
void lui_scene_set_font(const tFont *font, tLuiScene *scene);


void lui_object_add_to_scene(void *object, tLuiCommon *common_data, tLuiScene *scene);
void lui_object_remove_from_scene(tLuiCommon *common_data);
void lui_object_destroy(void *object, tLuiCommon *obj_common_data);
void lui_object_set_area(uint16_t width, uint16_t height, tLuiCommon *common_data);
void lui_object_set_position(uint16_t x, uint16_t y, tLuiCommon *common_data);

void lui_process_touch_input(tLuiScene *scene);
void lui_process_dpad_input(tLuiScene *scene);


tLuiDispDrv* lui_dispdrv_create();
void lui_dispdrv_register(tLuiDispDrv *dispdrv);
void* lui_dispdrv_destroy(tLuiDispDrv *dispdrv);
void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res, tLuiDispDrv *dispdrv);
void lui_dispdrv_set_draw_pixel_cb(void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color), tLuiDispDrv *dispdrv);
void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), tLuiDispDrv *dispdrv);
uint8_t lui_disp_drv_check();


tLuiTouchInputDev* lui_touch_inputdev_create();
void lui_touch_inputdev_register (tLuiTouchInputDev *touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *touch_inputdata), tLuiTouchInputDev *touch_inputdev);
tLuiDpadInputDev* lui_dpad_inputdev_create();
void lui_dpad_inputdev_register (tLuiDpadInputDev *dpad_inputdev);
void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(tLuiDpadInputData *dpad_inputdata), tLuiDpadInputDev *dpad_inputdev);


//-------------------------------------------------------------------------------
//------------ PRIVATE HELPER FUNCTIONS TO BE USED BY THIS LIBRARY ONLY----------
//-------------------------------------------------------------------------------
uint8_t lui_get_event_against_state(uint8_t new_state, uint8_t old_state);
void lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph);
void get_string_dimension(const char *str, const tFont *font, uint8_t *str_dim);
void lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
double lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
void lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif /* INC_LAME_UI_H_ */
