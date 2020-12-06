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

typedef enum button_state
{
	BUTTON_IDLE = 0,
	BUTTON_SELECTED,
	BUTTON_PRESSED
}tLuiButtonState;


typedef enum slider_state
{
	SLIDER_IDLE = 0,
	SLIDER_SELECTED,
	SLIDER_ENTERED
}tLuiSliderState;


typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

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

	//private use only
	uint8_t needs_refresh;
	int8_t index;
	int8_t parent_index;
}tLuiLineChart;


typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	char *text;
	tFont *font;
	uint16_t color;
	uint16_t bg_color;

	//private use only	
	uint8_t needs_refresh;
	int8_t index;
	int8_t parent_index;
}tLuiLabel;


typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

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

	void (*btn_event_state_change_cb)(tLuiButtonState state);

	//private use only
	uint8_t is_state_change;
	tLuiButtonState state;
	uint8_t needs_refresh;
	int8_t index;
	int8_t parent_index;
}tLuiButton;


typedef struct
{
	tLuiLineChart *lui_line_chart[MAX_LINE_CHARTS_PER_SCENE];
	tLuiLabel *lui_label[MAX_LABELS_PER_SCENE];
	tLuiButton *lui_button[MAX_BUTTONS_PER_SCENE];

	struct
	{
		uint16_t color;
		tImage *image;
	}background;

	uint8_t label_total;
	uint8_t line_chart_total;
	uint8_t button_total;
	tFont *font;

	struct
	{
		int8_t max_row_pos;
		int8_t max_col_pos;
		int8_t current_row_pos;
		int8_t current_col_pos;
	}dpad; // dpad controls are scene specific, so storing it in the scene variable

	uint8_t needs_refresh;
	int8_t index;
}tLuiScene;


typedef struct
{
	uint8_t is_pressed;
	uint16_t x;
	uint16_t y;
}tLuiTouchInputData;


typedef struct
{
	uint8_t is_left_pressed;
	uint8_t is_right_pressed;
	uint8_t is_down_pressed;
	uint8_t is_up_pressed;
	uint8_t is_enter_pressed;
	uint8_t is_cancel_pressed;
}tLuiDpadInputData;


typedef struct
{
	void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color);
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	uint16_t display_hor_res;
	uint16_t display_vert_res;
}tLuiDispDrv;

typedef struct
{
	void (*read_touch_input_cb)(tLuiTouchInputData *input);
	//tLuiTouchInputData touch_data;
}tLuiTouchInputDev;

typedef struct
{
	void (*read_dpad_input_cb)(tLuiDpadInputData *input);
	//tLuiDpadInputData dpad_data;
	
}tLuiDpadInputDev;



tLuiLabel lui_label_create();
void lui_label_draw (tLuiLabel *lui_label);
void lui_label_add_to_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene);
void lui_label_remove_from_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene);
void lui_label_set_position(uint16_t x, uint16_t y, tLuiLabel *lui_lbl);
void lui_label_set_area(uint16_t width, uint16_t height, tLuiLabel *lui_lbl);
void lui_label_set_font(const tFont *font, tLuiLabel *lui_lbl);
void lui_label_set_text(const char *text, tLuiLabel *lui_lbl);
void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, tLuiLabel *lui_lbl);


tLuiLineChart lui_linechart_create();
void lui_linechart_draw (tLuiLineChart *lui_line_chart);
void lui_linechart_add_to_scene(tLuiLineChart *lui_line_chart, tLuiScene *lui_scene);
void lui_linechart_remove_from_scene(tLuiLineChart *lui_line_chart, tLuiScene *lui_scene);
void lui_linechart_set_position(uint16_t x, uint16_t y, tLuiLineChart *lui_chart);
void lui_linechart_set_area(uint16_t width, uint16_t height, tLuiLineChart *lui_chart);
void lui_linechart_set_border(uint16_t color, tLuiLineChart *lui_chart);
void lui_linechart_set_border_visible(uint8_t state, tLuiLineChart *lui_chart);
void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, tLuiLineChart *lui_chart);
void lui_linechart_set_grid_visible(uint8_t state, tLuiLineChart *lui_chart);
void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, tLuiLineChart *lui_chart);
void lui_linechart_set_data_range(uint16_t y_min, uint16_t y_max, tLuiLineChart *lui_chart);
void lui_linechart_set_data_source(double *source, uint16_t points, tLuiLineChart *lui_chart);


tLuiButton lui_button_create();
void lui_button_draw(tLuiButton *lui_btn);
void lui_button_add_to_scene(tLuiButton *lui_btn, tLuiScene *lui_scene);
void lui_button_remove_from_scene(tLuiButton *lui_btn, tLuiScene *lui_scene);
void lui_button_set_position(uint16_t x, uint16_t y, tLuiButton *lui_btn);
void lui_button_set_area(uint16_t width, uint16_t height, tLuiButton *lui_btn);
void lui_button_set_label_text(const char *text, tLuiButton *lui_btn);
void lui_button_set_label_color(uint16_t color, tLuiButton *lui_btn);
void lui_button_set_label_font(const tFont *font, tLuiButton *lui_btn);
void lui_button_set_colors(uint16_t bg_color, uint16_t pressed_color, uint16_t selection_color, tLuiButton *lui_btn);
void lui_button_set_state_change_cb(void (*btn_event_state_change_cb)(tLuiButtonState), tLuiButton *lui_btn);


tLuiScene lui_scene_create();
void lui_scene_add(tLuiScene *lui_scene);
void lui_scene_render(tLuiScene *lui_scene);
void lui_scene_set_bg_color(uint16_t color, tLuiScene *lui_scene);
void lui_scene_set_bg_image(const tImage *image, tLuiScene *lui_scene);
void lui_scene_set_font(const tFont *font, tLuiScene *lui_scene);


void lui_process_touch_input(tLuiScene *lui_scene);
void lui_process_dpad_input(tLuiScene *lui_scene);


tLuiDispDrv lui_dispdrv_create();
void lui_dispdrv_register(tLuiDispDrv *disp_drv);
void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res);
void lui_dispdrv_set_draw_pixel_cb(void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color));
void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color));



tLuiTouchInputDev lui_touch_inputdev_create();
void lui_touch_inputdev_register (tLuiTouchInputDev *touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *input));
tLuiDpadInputDev lui_dpad_inputdev_create();
void lui_dpad_inputdev_register (tLuiDpadInputDev *dpad_inputdev);
void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(tLuiDpadInputData *input));


//-------------------------------------------------------------------------------
//------------ PRIVATE HELPER FUNCTIONS TO BE USED BY THIS LIBRARY ONLY----------
//-------------------------------------------------------------------------------
void lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph);
void get_string_dimension(const char *str, const tFont *font, uint8_t *str_dim);
void lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
double lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
void lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif /* INC_LAME_UI_H_ */
