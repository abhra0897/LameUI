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

	//private use only
	uint8_t is_pressed;
	uint8_t is_selected;
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
		tImage image;
	}background;

	uint8_t label_total;
	uint8_t line_chart_total;
	uint8_t button_total;
	tFont *font;

	uint8_t needs_refresh;
	int8_t index;
}tLuiScene;


typedef struct
{
	uint8_t is_pressed;
	struct
	{
		uint16_t x;
		uint16_t y;
	}position;
}tLuiInputDev;


typedef struct
{
	void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color);
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	void (*read_input_cb)(tLuiInputDev *input);

	uint16_t display_hor_res;
	uint16_t display_vert_res;
}tLuiDispDrv;




tLuiLabel lui_label_create();
void lui_label_draw (tLuiLabel *lui_label);
void lui_label_add_to_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene);
void lui_label_remove_from_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene);

tLuiLineChart lui_linechart_create();
void lui_linechart_draw (tLuiLineChart *lui_line_chart);
void lui_linechart_add_to_scene(tLuiLineChart *lui_line_chart, tLuiScene *lui_scene);
void lui_linechart_remove_from_scene(tLuiLineChart *lui_line_chart, tLuiScene *lui_scene);

tLuiButton lui_button_create();
void lui_button_draw(tLuiButton *lui_btn);
void lui_button_add_to_scene(tLuiButton *lui_btn, tLuiScene *lui_scene);
void lui_button_remove_from_scene(tLuiButton *lui_btn, tLuiScene *lui_scene);

tLuiScene lui_scene_create();
void lui_scene_add(tLuiScene *lui_scene);
void lui_scene_render(tLuiScene *lui_scene);


tLuiDispDrv lui_dispdrv_create();
void lui_dispdrv_register(tLuiDispDrv *disp_drv);

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
