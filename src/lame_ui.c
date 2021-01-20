/*
 * lame_ui.c
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 16-May-2020
 *      Author: rik
 */

#include "lame_ui.h"

//Global variables
//uint8_t lui_scene_total = 0;
//tLuiDispDrv *g_lui_main.disp_drv = NULL;
//tLuiTouchInputDev *g_lui_main.touch_input_dev = NULL;
//tLuiDpadInputDev *g_lui_main.dpad_input_dev = NULL;
//tLuiScene *g_lui_main.scenes[MAX_SCENES];

tLuiMain g_lui_main = {
	.scenes = {NULL},
	.disp_drv = NULL,
	.touch_input_dev = NULL,
	.dpad_input_dev = NULL,
	.total_scenes = 0,
	.active_scene_id = -1 //not using right now
};

#pragma region  Label

/*-------------------------------------------------------------------------------
 * 				LUI_LABEL related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Render a label. This label may or maynot be a part of any scene.
 * The entire scene is not updated.
 */
void lui_label_draw (tLuiLabel *label)
{
	if (label == NULL)
		return;
	// if no display driver is registered, return
	if (lui_disp_drv_check() == 0)
		return;

	uint16_t x_temp = label->common_data.x;
	uint16_t y_temp = label->common_data.y;

	const tImage *img = {0};
	uint16_t width = 0, height = 0;

	tFont *temp_font = label->font;
	// If label has no font set for it, check the global set font in scene
	// If no scene present, i.e., parent_scene_index == -1, return
	// If scene is there but scene has no font, return
	// So, if label has no parent scene, it must have its own font to be rendered
	if (temp_font == NULL)
	{
		// If no parent scene (i.e. parent_scene_index = -1), return
		if (label->common_data.parent_index == -1)
			return;
		// If the parent scene also has no font set, return.
		if (g_lui_main.scenes[label->common_data.parent_index]->font == NULL)
			return;

		// Otherwise use the scene's font (global font)
		temp_font = g_lui_main.scenes[label->common_data.parent_index]->font;
	}

	// Draw the label background color
	lui_draw_rect_fill(label->common_data.x, label->common_data.y, label->common_data.width, label->common_data.height, label->bg_color);

	// Scan chars one by one from the string
	//char
	for (uint16_t char_cnt = 0; *(label->text+char_cnt) != '\0'; char_cnt++)
	{
		if (*(label->text+char_cnt) == '\n')
		{
			x_temp = label->common_data.x;					//go to first col
			y_temp += (temp_font->chars[0].image->height);	//go to next row (row height = height of space)
		}

		else if (*(label->text+char_cnt) == '\t')
		{
			x_temp += 4 * (temp_font->chars[0].image->height);	//Skip 4 spaces (width = width of space)
		}
		else
		{
			// Find the glyph for the char from the font
			for (uint8_t i = 0; i < temp_font->length; i++)
			{
				if (temp_font->chars[i].code == *(label->text+char_cnt))
				{
					img = temp_font->chars[i].image;
					break;
				}
			}
			width = img->width;
			height = img->height;


			// check if not enough space available at the right side
			if (x_temp + width > label->common_data.x + label->common_data.width)
			{
				x_temp = label->common_data.x;					//go to first col
				y_temp += height;	//go to next row

				// check if not enough space available at the bottom
				if(y_temp + height > label->common_data.y + label->common_data.height - 1)
					return;
			}

			lui_draw_char(x_temp, y_temp, label->color, img);

			x_temp += width;		//next char position
		}
	}
}

/*
 * Initialize a label with default values
 */
tLuiLabel* lui_label_create()
{
	tLuiLabel *initial_label = malloc(sizeof(*initial_label));

	initial_label->common_data.x = 0;
	initial_label->common_data.y = 0;
	initial_label->common_data.width = 0;
	initial_label->common_data.height = 0;
	initial_label->text = "";
	initial_label->font = NULL;
	initial_label->color = DEFAULT_TEXT_FORECOLOR;
	initial_label->bg_color = DEFAULT_TEXT_BGCOLOR;

	initial_label->common_data.index = -1;
	initial_label->common_data.parent_index = -1;
	initial_label->common_data.needs_refresh = 1;

	return initial_label;
}

/*
 * Add a label to a scene.
 * One label can be added only to one scene
 * To add it to other scene, remove it first
 */
void lui_label_add_to_scene(tLuiLabel *lbl, tLuiScene *scene)
{
	if (lbl == NULL || scene == NULL)
		return;
	lui_object_add_to_scene(lbl, &(lbl->common_data), scene);
}

/*
 * Remove an existing label from a scene
 */
void lui_label_remove_from_scene(tLuiLabel *lbl)
{
    if (lbl == NULL)
		return;
	lui_object_remove_from_scene(&(lbl->common_data));
}

void* lui_label_destroy(tLuiLabel *lbl)
{
	// Note: after freeing an object's memory, we need to set it to NULL so we can compare later
	// But setting the pointer to NULL inside the function will only change it locally.
	// So, after freeing, return NULL from function and assign the value to the object pointer from the calling side
    if (lbl != NULL)
		lui_object_destroy(lbl, &(lbl->common_data));
	return NULL;
}

void lui_label_set_position(uint16_t x, uint16_t y, tLuiLabel *lbl)
{
	if (lbl == NULL)
		return;
	lui_object_set_position(x, y, &lbl->common_data);
}

void lui_label_set_area(uint16_t width, uint16_t height, tLuiLabel *lbl)
{
	if (lbl == NULL)
		return;
	lui_object_set_area(width, height, &lbl->common_data);
}

void lui_label_set_font(const tFont *font, tLuiLabel *lbl)
{
	if (lbl == NULL)
		return;
	lbl->font = (tFont *)font;
	if (lbl->common_data.parent_index != -1)
		g_lui_main.scenes[lbl->common_data.parent_index]->needs_refresh = 1;
}

void lui_label_set_text(const char *text, tLuiLabel *lbl)
{
	if (lbl == NULL)
		return;
	lbl->text = (char *)text;
	lbl->common_data.needs_refresh = 1;
}

void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, tLuiLabel *lbl)
{
	if (lbl == NULL)
		return;
	if (lbl->color == text_color && lbl->bg_color == bg_color)
		return;
	lbl->common_data.needs_refresh = 1;
	lbl->color = text_color;
	lbl->bg_color = bg_color;
}

#pragma endregion Label

#pragma region LineChart

/*-------------------------------------------------------------------------------
 * 				LUI_LINE_CHART related functions
 *-------------------------------------------------------------------------------
 */

/*
* Draw a line chart
*/
void lui_linechart_draw(tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	// if no display driver is registered, return
	if (lui_disp_drv_check() == 0)
		return;

	uint16_t temp_x = chart->common_data.x;
	uint16_t temp_y = chart->common_data.y;
	uint16_t width = chart->common_data.width;
	uint16_t height = chart->common_data.height;
	uint16_t bg_color = chart->bg_color;
	uint16_t line_color = chart->color;
	uint16_t data_points = chart->data.points;

	double mapped_data[g_lui_main.disp_drv->display_hor_res * 2];

	double x_data_min_new = temp_x;
	double x_data_max_new = temp_x + width;
	//[0][0] element of 2D array is x_min
	// address of [0][0] = base address
	double x_data_min_old = *(chart->data.source);
	//[n][0] element of 2D array is x_max
	//address of [n][0] = base address + (n*2) - 2
	double x_data_max_old = *(chart->data.source + (data_points*2) - 2);


	double y_data_min_new = temp_y + height;
	double y_data_max_new = temp_y;
	double y_data_min_old;
	double y_data_max_old;

	// If data auto-scale is enabled, find out the Y max and min value
	if (chart->data.auto_scale == 1)
	{
		// Initially, Max and Min both are set to the first Y value of the source array
		double y_max = *(chart->data.source + 1);
		double y_min = *(chart->data.source + 1);

		// Now compare max and min with y values from source array to find the maximum and minimum
		for (uint16_t i = 1; i < data_points; i++)
		{
			double y_val = *(chart->data.source + (i*2) + 1);
			if (y_max <= y_val)
				y_max = y_val;
			if (y_min >= y_val)
				y_min = y_val;
		}
		y_data_min_old = y_min;
		y_data_max_old = y_max;
	}
	// If not enabled, use user-supplied max and min value
	else
	{
		y_data_min_old = chart->data.y_min_value;
		y_data_max_old = chart->data.y_max_value;
	}

	// Set font for using in chart
	tFont *temp_font = chart->font;
	// If object has no font set for it, check the global set font in scene
	if (temp_font == NULL)
	{
		// Find object's parent scene's index
		uint8_t parent_scene_index = chart->common_data.parent_index;

		// If the parent scene also has no font set, return.
		if (g_lui_main.scenes[parent_scene_index]->font == NULL)
			return;

		// Otherwise use the scene's font (global font)
		temp_font = g_lui_main.scenes[parent_scene_index]->font;
	}


	// Draw the chart background
	lui_draw_rect_fill(temp_x, temp_y, width, height, bg_color);

	// Draw the scale numbers

	// Draw the chart grid if needed
	if (chart->grid.is_grid)
	{
		uint16_t hor_grid_spacing = height / (chart->grid.hor_count + 1);
		uint16_t vert_grid_spacing = width / (chart->grid.vert_count + 1);

		// Draw the vertical grids from left to right
		for (int i = 1; i <= chart->grid.vert_count; i++)
		{
			lui_draw_line(temp_x + (i * vert_grid_spacing), temp_y, temp_x + (i * vert_grid_spacing), temp_y + height, 1, chart->grid.color);
		}

		// Draw the horizontal grids from bottom to top
		uint16_t y_bottom = temp_y + height;
		for (int i = 1; i <= chart->grid.hor_count; i++)
		{
			lui_draw_line(temp_x, y_bottom - (i * hor_grid_spacing), temp_x + width, y_bottom - (i * hor_grid_spacing), 1, chart->grid.color);
		}
	}


	// Map all the point values to pixel co-ordinate values
	for (int i = 0; i < data_points; i++)
	{
		double x_data_old = *(chart->data.source + (i*2));
		double y_data_old = *(chart->data.source + (i*2) + 1);
		// Mapping range of x values
		mapped_data[i*2] =  lui_map_range(x_data_old, x_data_max_old, x_data_min_old, x_data_max_new, x_data_min_new);

		// Mapping range of y values
		mapped_data[i*2 + 1] =  lui_map_range(y_data_old, y_data_max_old, y_data_min_old, y_data_max_new, y_data_min_new);
	}

	// Now draw the lines using the mapped points to make the graph
	for (int i = 0; i < data_points; i++)
	{
		uint16_t current_x = mapped_data[i*2];
		uint16_t current_y = mapped_data[i*2 + 1];

		// Reached the last point, we don't have any next point
		if (i == data_points - 1)
		{
			// Don't draw line here, just draw the point
			g_lui_main.disp_drv->draw_pixel_cb(current_x, current_y, line_color);
		}

		// We have next points after thispoint
		else
		{
			uint16_t next_x = mapped_data [i*2 + 2];
			uint16_t next_y = mapped_data [i*2 + 3];

			lui_draw_line(current_x, current_y, next_x, next_y, 1, line_color);
		}
	}

	// Draw the chart border if needed
	if (chart->border.is_border)
	{
		uint16_t border_color = chart->border.color;
		lui_draw_rect(temp_x, temp_y, width, height, 1, border_color);
	}
}

/*
 * Initialize a line chart with default values
 */
tLuiLineChart* lui_linechart_create()
{
	tLuiLineChart *initial_line_chart = malloc(sizeof(*initial_line_chart)); //(tLuiLineChart *)malloc(sizeof(tLuiLineChart))

	double tmp_data[] = {0};
	initial_line_chart->data.source = tmp_data;
	initial_line_chart->data.y_max_value = 0;
	initial_line_chart->data.y_min_value = g_lui_main.disp_drv->display_vert_res;
	initial_line_chart->data.points = 0;
	initial_line_chart->data.auto_scale = 1;

	initial_line_chart->common_data.x = 0;
	initial_line_chart->common_data.y = 0;
	initial_line_chart->common_data.width = 100;
	initial_line_chart->common_data.height = 100;

	initial_line_chart->color = DEFAULT_LINE_CHART_FORECOLOR;
	
	initial_line_chart->border.is_border = 1;
	initial_line_chart->border.color = DEFAULT_LINE_CHART_FORECOLOR;

	initial_line_chart->grid.color = DEFAULT_LINE_CHART_FORECOLOR;
	initial_line_chart->grid.hor_count = 5;
	initial_line_chart->grid.vert_count = 5;
	initial_line_chart->grid.is_grid = 1;

	initial_line_chart->bg_color = DEFAULT_LINE_CHART_BGCOLOR;

	initial_line_chart->font = NULL;

	initial_line_chart->common_data.index = -1;
	initial_line_chart->common_data.parent_index = -1;
	initial_line_chart->common_data.needs_refresh = 1;

	return initial_line_chart;
}

/*
 * Add a line chart to a scene.
 * A line chart can be added only to one scene
 * To add it to other scene, remove it first
 */
void lui_linechart_add_to_scene(tLuiLineChart *chart, tLuiScene *scene)
{
	if (chart == NULL || scene == NULL)
		return;
	lui_object_add_to_scene(chart, &(chart->common_data), scene);
}

/*
 * Remove an existing line chart from a scene
 */
void lui_linechart_remove_from_scene(tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	lui_object_remove_from_scene(&(chart->common_data));
}

void* lui_linechart_destroy(tLuiLineChart *chart)
{
	if (chart != NULL)
		lui_object_destroy(chart, &(chart->common_data));
	return NULL;
}

void lui_linechart_set_position(uint16_t x, uint16_t y, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	lui_object_set_position(x, y, &chart->common_data);
}

void lui_linechart_set_area(uint16_t width, uint16_t height, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	lui_object_set_area(width, height, &chart->common_data);
}

void lui_linechart_set_border(uint16_t color, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->border.color == color)
		return;	
	chart->common_data.needs_refresh = 1;
	chart->border.color = color;
}

void lui_linechart_set_border_visible(uint8_t state, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->border.is_border == state)
		return;	
	chart->common_data.needs_refresh = 1;
	chart->border.is_border = state;
}

void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->grid.color == color && chart->grid.hor_count == hor_lines && chart->grid.vert_count == vert_lines)
		return;	
	chart->common_data.needs_refresh = 1;
	chart->grid.color = color;
	chart->grid.hor_count = hor_lines;
	chart->grid.vert_count = vert_lines;
}

void lui_linechart_set_grid_visible(uint8_t state, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->grid.is_grid == state)
		return;	
	chart->common_data.needs_refresh = 1;
	chart->grid.is_grid = state;
}

void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->color == line_color && chart->bg_color == bg_color)
		return;	
	chart->common_data.needs_refresh = 1;
	chart->color = line_color;
	chart->bg_color = bg_color;
}

void lui_linechart_set_data_range(uint16_t y_min, uint16_t y_max, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->data.y_max_value == y_max && chart->data.y_min_value == y_min)
		return;
	chart->common_data.needs_refresh = 1;
	chart->data.y_max_value = y_max;
	chart->data.y_min_value = y_min;
}

void lui_linechart_set_data_source(double *source, uint16_t points, tLuiLineChart *chart)
{
	if (chart == NULL)
		return;
	if (chart->data.points == points)
		return;
	chart->common_data.needs_refresh = 1;
	chart->data.source = source;
	chart->data.points = points;
}

#pragma endregion LineChart

#pragma region Button

/*-------------------------------------------------------------------------------
 * 				LUI_BUTTON related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Draw a button
 */
void lui_button_draw(tLuiButton *btn)
{
	if (btn == NULL)
		return;
	// if no display driver is registered, return
	if (lui_disp_drv_check() == 0)
		return;

	uint16_t temp_x = btn->common_data.x;
	uint16_t temp_y = btn->common_data.y;
	uint16_t btn_height = btn->common_data.height;
	uint16_t btn_width = btn->common_data.width;


	uint8_t str_width_height[2];

	// Draw the button's body color depending on its current state
	if (btn->state == LUI_STATE_IDLE)
		lui_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn->color); // normal situation
	else if (btn->state == LUI_STATE_SELECTED)
		lui_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn->selection_color);
	else if (btn->state == LUI_STATE_PRESSED)
		lui_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn->pressed_color);
	

	// Draw the button label (text)
	// Text will be in the miidle of the button.
	// So, at first we need to calculate its position

	get_string_dimension(btn->label.text, btn->label.font, str_width_height);

	str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
	str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

	temp_x = temp_x + (btn_width - str_width_height[0]) / 2;
	temp_y = temp_y + (btn_height - str_width_height[1]) / 2;

	tLuiLabel btn_label;
	btn_label.text = btn->label.text;
	btn_label.color = btn->label.color;
	// bg_color depends on button's current state color
	if (btn->state == LUI_STATE_IDLE)
		btn_label.bg_color = btn->color; // normal situation
	else if (btn->state == LUI_STATE_SELECTED)
		btn_label.bg_color = btn->selection_color;
	else if (btn->state == LUI_STATE_PRESSED)
		btn_label.bg_color = btn->pressed_color;
	btn_label.common_data.x = temp_x;
	btn_label.common_data.y = temp_y;
	btn_label.common_data.width = str_width_height[0];
	btn_label.common_data.height = str_width_height[1];
	btn_label.font = btn->label.font;
	lui_label_draw(&btn_label);
}

/*
 * Create a button with default variables
 */
tLuiButton* lui_button_create()
{
	tLuiButton *initial_button = malloc(sizeof(*initial_button));

	initial_button->common_data.x = 0;
	initial_button->common_data.y = 0;
	initial_button->common_data.width = 0;
	initial_button->common_data.height = 0;
	initial_button->color = 0xFFFF;
	initial_button->pressed_color = 0xCE59; //grey
	initial_button->selection_color = 0xFF40; //Yellow
	initial_button->state = LUI_STATE_IDLE;

	initial_button->label.text = "";
	initial_button->label.color = 0x0000; //black
	initial_button->label.font = NULL;

	initial_button->dpad_row_pos = -1;
	initial_button->dpad_col_pos = -1;

	initial_button->btn_event_cb = NULL;

	initial_button->common_data.index = -1;
	initial_button->common_data.parent_index = -1;
	initial_button->common_data.needs_refresh = 1;

	return  initial_button;
}

/*
 * Add a button to a scene
 * A button can only be added to one scene
 * To add it to other scene, remove it first
 */
void lui_button_add_to_scene(tLuiButton *btn, tLuiScene *scene)
{
	if (btn == NULL || scene == NULL)
		return;
    lui_object_add_to_scene(btn, &(btn->common_data), scene);
}

/*
 * Remove an existing button from a scene
 */
void lui_button_remove_from_scene(tLuiButton *btn)
{
    if (btn == NULL)
		return;
    lui_object_remove_from_scene(&(btn->common_data));
}

void* lui_button_destroy(tLuiButton *btn)
{
    if (btn == NULL)
		lui_object_destroy(btn, &(btn->common_data));
		return NULL;
}

void lui_button_set_position(uint16_t x, uint16_t y, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	lui_object_set_position(x, y, &btn->common_data);
}

void lui_button_set_area(uint16_t width, uint16_t height, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	lui_object_set_area(width, height, &btn->common_data);
}

void lui_button_set_label_text(const char *text, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	btn->label.text = (char *)text;
	btn->common_data.needs_refresh = 1;
}

void lui_button_set_label_color(uint16_t color, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	if (btn->label.color == color)
		return;
	btn->label.color = color;
	btn->common_data.needs_refresh = 1;
}

void lui_button_set_label_font(const tFont *font, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	btn->label.font = (tFont *)font;
	if (btn->common_data.parent_index != -1)
		g_lui_main.scenes[btn->common_data.parent_index]->needs_refresh = 1;
}

void lui_button_set_colors(uint16_t bg_color, uint16_t pressed_color, uint16_t selection_color, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	if (btn->color == bg_color && btn->pressed_color == pressed_color && btn->selection_color == selection_color)
		return;
	btn->color = bg_color;
	btn->pressed_color = pressed_color;
	btn->selection_color = selection_color;
	btn->common_data.needs_refresh = 1;
}

void lui_button_set_dpad_position(uint8_t row, uint8_t col, tLuiButton *btn)
{
	if (btn == NULL)
		return;
	if ((btn->dpad_row_pos == row && btn->dpad_col_pos == col) ||	// if row/col pos are unchanged or either of them are negative, return
		(btn->dpad_row_pos < 0 ||btn->dpad_col_pos < 0))
		return;
	btn->dpad_row_pos = row;
	btn->dpad_col_pos = col;

	if (btn->common_data.parent_index != -1)
	{
		// Setting maximum values of dpad row and cols
		if (g_lui_main.scenes[btn->common_data.parent_index]->dpad.max_row_pos < row)
			g_lui_main.scenes[btn->common_data.parent_index]->dpad.max_row_pos = row;
		if (g_lui_main.scenes[btn->common_data.parent_index]->dpad.max_col_pos < col)
			g_lui_main.scenes[btn->common_data.parent_index]->dpad.max_col_pos = col;
	}
}

void lui_button_set_event_cb(void (*btn_event_state_change_cb)(uint8_t), tLuiButton *btn)
{
	if (btn == NULL)
		return;
	btn->btn_event_cb = btn_event_state_change_cb;
}

#pragma endregion Button

#pragma region Switch

/*-------------------------------------------------------------------------------
 * 				LUI_SWITCH related functions
 *-------------------------------------------------------------------------------
 */

void lui_switch_draw(tLuiSwitch *swtch)
{
	if (swtch == NULL)
		return;
	// if no display driver is registered, return
	if (lui_disp_drv_check() == 0)
		return;

	uint16_t temp_x = swtch->common_data.x;
	uint16_t temp_y = swtch->common_data.y;
	uint16_t temp_height = swtch->common_data.height;
	uint16_t temp_width = swtch->common_data.width;
	
	lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, swtch->bg_color);
	temp_width = (float)temp_width * 0.3;
	temp_height = (float)temp_height * 0.8;
	temp_x = temp_x + ((swtch->common_data.width / 2) - temp_width) / 2;
	if (swtch->value == 1)
		temp_x += (swtch->common_data.width / 2);
	temp_y = temp_y + (swtch->common_data.height - temp_height) / 2;

	lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, swtch->fore_color);
}

tLuiSwitch* lui_switch_create()
{
	tLuiSwitch *initial_switch = malloc(sizeof(*initial_switch));

	initial_switch->common_data.obj_type = LUI_OBJ_SWITCH;
	initial_switch->common_data.x = 20;
	initial_switch->common_data.y = 20;
	initial_switch->common_data.width = 40;
	initial_switch->common_data.height = 20;

	initial_switch->bg_color = 0xFFFF;
	initial_switch->fore_color = 0x0000; 
	initial_switch->selection_color = 0xFF40; //Yellow
	initial_switch->state = LUI_STATE_IDLE;
	initial_switch->value = 0;

	initial_switch->dpad_row_pos = -1;
	initial_switch->dpad_col_pos = -1;

	initial_switch->sw_event_cb = NULL;

	initial_switch->common_data.index = -1;
	initial_switch->common_data.parent_index = -1;
	initial_switch->common_data.needs_refresh = 1;

	return  initial_switch;
}

void lui_switch_add_to_scene(tLuiSwitch *swtch, tLuiScene *scene)
{
	if (swtch == NULL || scene == NULL)
		return;
	lui_object_add_to_scene(swtch, &swtch->common_data, scene);
}

void lui_switch_remove_from_scene(tLuiSwitch *swtch)
{
	if (swtch == NULL)
		return;
	lui_object_remove_from_scene(&(swtch->common_data));
}

void* lui_switch_destroy(tLuiSwitch *swtch)
{
	if (swtch != NULL)
		lui_object_destroy(swtch, &(swtch->common_data));
	return NULL;
}

void lui_switch_set_position(uint16_t x, uint16_t y, tLuiSwitch *swtch)
{
	if (swtch == NULL)
		return;
	lui_object_set_position(x, y, &swtch->common_data);
}

void lui_switch_set_area(uint16_t width, uint16_t height, tLuiSwitch *swtch)
{
	if (swtch == NULL)
		return;
	lui_object_set_area(width, height, &swtch->common_data);
}

#pragma endregion Switch

/*-------------------------------------------------------------------------------
 * 				LUI_SCENE related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Create a empty scene with default values and return the scene variable
 */
tLuiScene* lui_scene_create()
{
	tLuiScene *initial_scene = malloc(sizeof(*initial_scene));

	initial_scene->label_total = 0;
	initial_scene->line_chart_total = 0;
	initial_scene->button_total = 0;
	initial_scene->switch_total = 0;

	initial_scene->lui_label = NULL;
	initial_scene->lui_button = NULL;
	initial_scene->lui_line_chart = NULL;
	initial_scene->lui_switch = NULL;

	initial_scene->font = NULL;
	initial_scene->background.color = 0x00;
	initial_scene->background.image = NULL;

	// Add the scene to global scenes array and set need_refresh to 1
	initial_scene->index = g_lui_main.total_scenes;
	g_lui_main.scenes[initial_scene->index] = initial_scene;
	g_lui_main.total_scenes++;
	initial_scene->needs_refresh = 1;

	return initial_scene;
}

void* lui_scene_destroy(tLuiScene *scene)
{
	if (scene == NULL)
		return NULL;
	for (int i = scene->index; i < MAX_SCENES - 1; i++)
	{
		g_lui_main.scenes[i] = g_lui_main.scenes[i+1];
		g_lui_main.scenes[i]->index = i;
	}
	g_lui_main.total_scenes--;

	free(scene);
	//scene = NULL;

	return NULL;
}


void lui_scene_set_bg_color(uint16_t color, tLuiScene *scene)
{
	if (scene == NULL)
		return;
	if (scene->background.color == color)
		return;
	scene->background.color = color;
	scene->needs_refresh = 1;
}

void lui_scene_set_bg_image(const tImage *image, tLuiScene *scene)
{
	// NOTE: image rendering is not implemented yet
	if (scene == NULL)
		return;
	scene->background.image = (tImage *)image;
	scene->needs_refresh = 1;
}

void lui_scene_set_font(const tFont *font, tLuiScene *scene)
{
	if (scene == NULL)
		return;
	scene->font = (tFont *)font;
	scene->needs_refresh = 1;
}

/*
 * Update the entire scene that contains some UI elements
 */
void lui_scene_render(tLuiScene *scene)
{
	if (scene == NULL)
		return;

	// if no display driver is registered, return
	if (lui_disp_drv_check() == 0)
		return;

	// Reading input only if user provided input reading callback function
	if (g_lui_main.touch_input_dev != NULL)
		lui_process_touch_input(scene);
	else if (g_lui_main.dpad_input_dev != NULL)
		lui_process_dpad_input(scene);

	uint8_t temp_label_total = scene->label_total;
	uint8_t temp_button_total = scene->button_total;
	uint8_t temp_switch_total = scene->switch_total;
	uint8_t temp_line_chart_total = scene->line_chart_total;

	uint16_t background_width = g_lui_main.disp_drv->display_hor_res;
	uint16_t background_height = g_lui_main.disp_drv->display_vert_res;
	uint16_t background_color = scene->background.color;

	// Render background first (only if scene needs refresh)
	if (scene->needs_refresh == 1)
		g_lui_main.disp_drv->draw_pixels_area_cb(0, 0, background_width, background_height, background_color);

	// Render all the buttons
	for (uint8_t i = 0; i < temp_button_total; i++)
	{
		// if either only this button needs refresh or the entire scene nedds, do the rendering
		if (scene->lui_button[i]->common_data.needs_refresh == 1 || scene->needs_refresh == 1)
		{
			lui_button_draw(scene->lui_button[i]);
			// If event call back is not null and state change happened, call that function
			if (scene->lui_button[i]->btn_event_cb != NULL && scene->lui_button[i]->event != LUI_EVENT_NONE)
			{
				scene->lui_button[i]->btn_event_cb(scene->lui_button[i]->event);
				// State change handled, so reset it
				scene->lui_button[i]->event = LUI_EVENT_NONE;
			}
			// Button rendering done, set the need_refresh bit to 0
			scene->lui_button[i]->common_data.needs_refresh = 0;
		}
		
	}

	// Render all the switches
	for (uint8_t i = 0; i < temp_switch_total; i++)
	{
		if (scene->lui_switch[i]->common_data.parent_index != -1)
		{
			// if either only this switch needs refresh or the entire scene nedds, do the rendering
			if (scene->lui_switch[i]->common_data.needs_refresh == 1 || scene->needs_refresh == 1)
			{
				lui_switch_draw(scene->lui_switch[i]);
				// If event call back is not null and state change happened, call that function
				// if (scene->lui_switch[i]->btn_event_cb != NULL && scene->lui_switch[i]->event != LUI_EVENT_NONE)
				// {
				// 	scene->lui_switch[i]->btn_event_cb(scene->lui_switch[i]->event);
				// 	// State change handled, so reset it
				// 	scene->lui_switch[i]->event = LUI_EVENT_NONE;
				// }
				// Button rendering done, set the need_refresh bit to 0
				scene->lui_switch[i]->common_data.needs_refresh = 0;
			}
		}
	}

	// Render all the labels
	for (uint8_t i = 0; i < temp_label_total; i++)
	{
		if (scene->lui_label[i]->common_data.needs_refresh == 1 || scene->needs_refresh == 1)
		{
			lui_label_draw(scene->lui_label[i]);
			scene->lui_label[i]->common_data.needs_refresh = 0;
		}
	}

	//Render all the line charts
	for (uint8_t i = 0; i < temp_line_chart_total; i++)
	{
		if (scene->lui_line_chart[i]->common_data.needs_refresh == 1 || scene->needs_refresh == 1)
		{
			lui_linechart_draw(scene->lui_line_chart[i]);
			scene->lui_line_chart[i]->common_data.needs_refresh = 0;
		}
	}

	// Rendering done, set the need_refresh bit to 0
	scene->needs_refresh = 0;
}


/*-------------------------------------------------------------------------------
 * 				Element COMMON functions
 *-------------------------------------------------------------------------------
 */

// Assumes no parameter is NULL
void lui_object_add_to_scene(void *object, tLuiCommon *obj_common_data, tLuiScene *scene)
{
    //add the ui element with a new index to scene only if the parent index is -1 (i.e., no parent)
    if (obj_common_data->parent_index != -1)
		return;
	
	switch (obj_common_data->obj_type)
	{
	case LUI_OBJ_LABEL:
		if (scene->label_total >= MAX_LABELS_PER_SCENE - 1)	
			return;
		obj_common_data->index = scene->label_total++;
		if (scene->lui_label == NULL)
			scene->lui_label = malloc(sizeof(scene->lui_label)); //malloc(sizeof(tLuiLabel **))
		else
			scene->lui_label = realloc(scene->lui_switch, sizeof(scene->lui_label) * (scene->label_total));
		scene->lui_label[obj_common_data->index] = (tLuiLabel *)object;
		break;

	case LUI_OBJ_BUTTON:
		if (scene->button_total >= MAX_BUTTONS_PER_SCENE - 1)	
			return;
		obj_common_data->index = scene->button_total++;
		if (scene->lui_button == NULL)
			scene->lui_button = malloc(sizeof(scene->lui_button));
		else
			scene->lui_button = realloc(scene->lui_switch, sizeof(scene->lui_button) * (scene->button_total));
		scene->lui_button[obj_common_data->index] = (tLuiButton *)object;
		break;

	case LUI_OBJ_SWITCH:
		if (scene->switch_total >= MAX_BUTTONS_PER_SCENE)	
			return;
		obj_common_data->index = scene->switch_total++;
		if (scene->lui_switch == NULL)
			scene->lui_switch = malloc(sizeof(scene->lui_switch));
		else
			scene->lui_switch = realloc(scene->lui_switch, sizeof(scene->lui_switch) * (scene->switch_total));
		scene->lui_switch[obj_common_data->index] = (tLuiSwitch *)object;
		break;

	case LUI_OBJ_LINECHART:
		if (scene->line_chart_total >= MAX_LINE_CHARTS_PER_SCENE - 1)	
			return;
		obj_common_data->index = scene->line_chart_total++;
		if (scene->lui_line_chart == NULL)
			scene->lui_line_chart = malloc(sizeof(scene->lui_line_chart));
		else
			scene->lui_line_chart = realloc(scene->lui_line_chart, sizeof(scene->lui_line_chart) * (scene->line_chart_total));
		scene->lui_line_chart[obj_common_data->index] = (tLuiLineChart *)object;
		break;

	default:
		return;
		//break;
	}

	// Common things to do
	obj_common_data->parent_index = scene->index;
	obj_common_data->needs_refresh = 1;
}

void lui_object_remove_from_scene(tLuiCommon *obj_common_data)
{
	// If item's or scene's index is -1, return
	if (obj_common_data->index == -1 || obj_common_data->parent_index == -1)
    	return;
	tLuiScene *parent_scene = g_lui_main.scenes[obj_common_data->parent_index];
	switch (obj_common_data->obj_type)
	{
	case LUI_OBJ_LABEL:
		for (int i = obj_common_data->index; i < parent_scene->label_total - 1; i++)
		{
			parent_scene->lui_label[i] = parent_scene->lui_label[i+1];
			parent_scene->lui_label[i]->common_data.index = i;
		}
		// obj_common_data->index = -1;
		// obj_common_data->parent_index = -1;
		parent_scene->label_total--;

		// resize the label array
		if (parent_scene->label_total == 0)
		{
			free(parent_scene->lui_label);
			parent_scene->lui_label = NULL;
		}
		else
		{
			parent_scene->lui_label = realloc(parent_scene->lui_label, sizeof(tLuiLabel *) * (parent_scene->label_total));
		}
		break;

	case LUI_OBJ_BUTTON:
		for (int i = obj_common_data->index; i < parent_scene->button_total - 1; i++)
		{
			parent_scene->lui_button[i] = parent_scene->lui_button[i+1];
			parent_scene->lui_button[i]->common_data.index = i;
		}
		// obj_common_data->index = -1;
		// obj_common_data->parent_index = -1;
		parent_scene->button_total--;

		// resize the button array
		if (parent_scene->button_total == 0)
		{
			free(parent_scene->lui_button);
			parent_scene->lui_button = NULL;
		}
		else
		{
			parent_scene->lui_line_chart = realloc(parent_scene->lui_line_chart, sizeof(tLuiLineChart *) * (parent_scene->line_chart_total));
		}
		break;

	case LUI_OBJ_SWITCH:
		for (int i = obj_common_data->index; i < parent_scene->switch_total - 1; i++)
		{
			parent_scene->lui_switch[i] = parent_scene->lui_switch[i+1];
			parent_scene->lui_switch[i]->common_data.index = i;
		}
		// obj_common_data->index = -1;
		// obj_common_data->parent_index = -1;
		parent_scene->switch_total--;

		// resize the lui_switch array
		if (parent_scene->switch_total == 0)
		{
			free(parent_scene->lui_switch);
			parent_scene->lui_switch = NULL; //handle dangling pointer
		}
		else
		{
			parent_scene->lui_switch = realloc(parent_scene->lui_switch, sizeof(tLuiSwitch *) * (parent_scene->switch_total));
		}
		break;

	case LUI_OBJ_LINECHART:
		for (int i = obj_common_data->index; i < parent_scene->line_chart_total - 1; i++)
		{
			parent_scene->lui_line_chart[i] = parent_scene->lui_line_chart[i+1];
			parent_scene->lui_line_chart[i]->common_data.index = i;
		}
		// obj_common_data->index = -1;
		// obj_common_data->parent_index = -1;
		parent_scene->line_chart_total--;

		// resize the linechart array
		if (parent_scene->line_chart_total == 0)
		{
			free(parent_scene->lui_line_chart);
			parent_scene->lui_line_chart = NULL;
		}
		else
		{
			parent_scene->lui_line_chart = realloc(parent_scene->lui_line_chart, sizeof(tLuiLineChart *) * (parent_scene->line_chart_total));
		}
		break;

	default:
		return;
		//break;
	}

	// common things to do
	obj_common_data->index = -1;
	obj_common_data->parent_index = -1;
	parent_scene->needs_refresh = 1;
}

void lui_object_destroy(void *object, tLuiCommon *obj_common_data)
{
	lui_object_remove_from_scene(obj_common_data);
	free(object);
}

void lui_object_set_position(uint16_t x, uint16_t y, tLuiCommon *common_data)
{
	if (common_data->x == x && common_data->y == y)
		return;
	if (common_data->parent_index != -1)
		g_lui_main.scenes[common_data->parent_index]->needs_refresh = 1;
	common_data->x = x;
	common_data->y = y;
}

void lui_object_set_area(uint16_t width, uint16_t height, tLuiCommon *common_data)
{
	if (common_data->width == width && common_data->height == height)
		return;

	if (common_data->width < width && common_data->height < height)
		common_data->needs_refresh = 1;
	else
	{	
		if (common_data->parent_index != -1)
			g_lui_main.scenes[common_data->parent_index]->needs_refresh = 1;
	}

	common_data->width = width;
	common_data->height = height;
}

/*------------------------------------------------------------------------------
 * 			INPUT processing functions
 *------------------------------------------------------------------------------
 */

// void lui_process_touch_input(tLuiScene *lui_scene)
/*
// {	
// 	tLuiTouchInputData input;
// 	g_lui_main.touch_input_dev->read_touch_input_cb(&input);
	
// 	tLuiEvent event;
// 	uint8_t input_on_element = 0; //if the input coordinates on the current elemt or not. if yes, stop scanning other elements & return
// 	// Process input for buttons
// 	for (uint8_t i = 0; i < lui_scene->button_total; i++)
// 	{
// 		// default is IDLE
// 		tLuiState new_state = LUI_STATE_IDLE;
// 		// Check if input coordinates ae within the boundary of the button
// 		if (input.x >= lui_scene->lui_button[i]->x &&
// 			input.x < lui_scene->lui_button[i]->x + lui_scene->lui_button[i]->width &&
// 			input.y >= lui_scene->lui_button[i]->y &&
// 			input.y < lui_scene->lui_button[i]->y + lui_scene->lui_button[i]->height)
// 		{
// 			input_on_element = 1;

// 			// if pressed, then....well, then state = PRESSED
// 			if (input.is_pressed == 1)
// 			{
// 				new_state = LUI_STATE_PRESSED;
// 				if (lui_scene->lui_button[i]->state == new_state)
// 					event = LUI_EVENT_NONE;
// 				else
// 					event = LUI_EVENT_PRESSED;
// 			}
// 			// else not pressed, state = SELECTED
// 			else
// 			{
// 				new_state = LUI_STATE_SELECTED;
// 				if (lui_scene->lui_button[i]->state == new_state)
// 					event = LUI_EVENT_NONE;
// 				else
// 				{
// 					if (lui_scene->lui_button[i]->state == LUI_STATE_PRESSED)
// 						event = LUI_EVENT_RELEASED;
// 					else
// 						event = LUI_EVENT_SELECTED;
// 				}
// 			}
// 		}
// 		// else if input is not on control
// 		else
// 		{
// 			new_state = LUI_STATE_IDLE;
// 			if (lui_scene->lui_button[i]->state == new_state)
// 				event = LUI_EVENT_NONE;
// 			else
// 				event = LUI_EVENT_SELECTION_LOST;
// 		}
		
// 		// If new state is not same as the existing state, only then refresh the button
// 		if (event != LUI_EVENT_NONE)
// 		{
// 			lui_scene->lui_button[i]->state = new_state;
// 			lui_scene->lui_button[i]->needs_refresh = 1;
// 			lui_scene->lui_button[i]->is_state_change = 1;
// 		}
// 		else
// 		{
// 			lui_scene->lui_button[i]->needs_refresh = 0;
// 			lui_scene->lui_button[i]->is_state_change = 0;
// 		}
		

// 		// if input is on this ui element, then no more need to scan other elements. Return now
// 		if (input_on_element == 1)
// 			return;
			
// 	}

// 	//Similarly, process inputs for other interactable elements
// }
*/

void lui_process_touch_input(tLuiScene *lui_scene)
{	
	tLuiTouchInputData input;
	g_lui_main.touch_input_dev->read_touch_input_cb(&input);
	
	uint8_t event;
	uint8_t input_on_element = 0; //if the input coordinates on the current elemt or not. if yes, stop scanning other elements & return
	// Process input for buttons
	for (uint8_t i = 0; i < lui_scene->button_total; i++)
	{
		// default is IDLE
		uint8_t new_state = LUI_STATE_IDLE;
		// Check if input coordinates ae within the boundary of the button
		if (input.x >= lui_scene->lui_button[i]->common_data.x &&
			input.x < lui_scene->lui_button[i]->common_data.x + lui_scene->lui_button[i]->common_data.width &&
			input.y >= lui_scene->lui_button[i]->common_data.y &&
			input.y < lui_scene->lui_button[i]->common_data.y + lui_scene->lui_button[i]->common_data.height)
		{
			input_on_element = 1;

			// if pressed, then....well, then state = PRESSED
			if (input.is_pressed == 1)
				new_state = LUI_STATE_PRESSED;
			
			// else not pressed, state = SELECTED
			else
				new_state = LUI_STATE_SELECTED;

		}
		// else if input is not on control
		else
		{
			new_state = LUI_STATE_IDLE;
		}
		
		event = lui_get_event_against_state(new_state, lui_scene->lui_button[i]->state);

		// If new state is not same as the existing state, only then refresh the button
		if (event != LUI_EVENT_NONE)
		{
			lui_scene->lui_button[i]->state = new_state;
			lui_scene->lui_button[i]->common_data.needs_refresh = 1;
			lui_scene->lui_button[i]->event = event;
		}
		else
		{
			lui_scene->lui_button[i]->common_data.needs_refresh = 0;
			lui_scene->lui_button[i]->event = LUI_EVENT_NONE;
		}
		

		// if input is on this ui element, then no more need to scan other elements. Return now
		if (input_on_element == 1)
			return;
			
	}

	//Similarly, process inputs for other interactable elements
}

void lui_process_dpad_input(tLuiScene *lui_scene)
{
	if (lui_scene->dpad.max_col_pos == -1) // no element is configured for dpad, so return
		return;

	tLuiDpadInputData input;
	g_lui_main.dpad_input_dev->read_dpad_input_cb(&input);

	if (input.is_right_pressed)	lui_scene->dpad.current_col_pos++;
	if (input.is_left_pressed)	lui_scene->dpad.current_col_pos--;
	if (input.is_down_pressed)	lui_scene->dpad.current_row_pos++;
	if (input.is_up_pressed)	lui_scene->dpad.current_row_pos--;

	// check dpad boundary
	if (lui_scene->dpad.current_col_pos > lui_scene->dpad.max_col_pos || lui_scene->dpad.current_col_pos < 0)
		lui_scene->dpad.current_col_pos = 0;
	if (lui_scene->dpad.current_row_pos > lui_scene->dpad.max_row_pos)
		lui_scene->dpad.current_row_pos = 0;

	uint8_t event;
	uint8_t input_on_element = 0; //if the input coordinates on the current elemt or not. if yes, stop scanning other elements & return
	for (uint8_t i = 0; i < lui_scene->button_total; i++)
	{
		uint8_t new_state = LUI_STATE_IDLE;
		if (lui_scene->dpad.current_col_pos == lui_scene->lui_button[i]->dpad_col_pos &&
			lui_scene->dpad.current_row_pos == lui_scene->lui_button[i]->dpad_row_pos)
		{
			input_on_element = 1; 

			if (input.is_enter_pressed == 1)
				new_state = LUI_STATE_PRESSED;
			else
				new_state = LUI_STATE_SELECTED;
		}
		// else if input position is not on button position
		else
		{
			new_state = LUI_STATE_IDLE;
		}

		event = lui_get_event_against_state(new_state, lui_scene->lui_button[i]->state);
		
		// If new state is not same as the existing state, only then refresh the button
		if (event != LUI_EVENT_NONE)
		{
			lui_scene->lui_button[i]->state = new_state;
			lui_scene->lui_button[i]->common_data.needs_refresh = 1;
			lui_scene->lui_button[i]->event = event;
		}
		else
		{
			lui_scene->lui_button[i]->common_data.needs_refresh = 0;
			lui_scene->lui_button[i]->event = LUI_EVENT_NONE;
		}
		

		// if input is on this ui element, then no more need to scan other elements. Return now
		if (input_on_element == 1)
			return;

	}

}


/*------------------------------------------------------------------------------
 * 			DISPLAY DRIVER Callback Functions and Display Properties
 *------------------------------------------------------------------------------
 */

tLuiDispDrv* lui_dispdrv_create()
{
	tLuiDispDrv *initial_disp_drv = malloc(sizeof(*initial_disp_drv));

	initial_disp_drv->draw_pixel_cb = NULL;
	initial_disp_drv->draw_pixels_area_cb = NULL;
	initial_disp_drv->display_hor_res = 320;		//horizontal 320px default
	initial_disp_drv->display_vert_res = 240;	//vertical 240px default

	return initial_disp_drv;
}

void lui_dispdrv_register(tLuiDispDrv *dispdrv)
{
	if (dispdrv == NULL)
		return;
	g_lui_main.disp_drv = dispdrv;
}

void* lui_dispdrv_destroy(tLuiDispDrv *dispdrv)
{
	if (dispdrv != NULL)
		free(dispdrv);
	return NULL;
}

void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res, tLuiDispDrv *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->display_hor_res = hor_res;
	dispdrv->display_vert_res = vert_res;
}

void lui_dispdrv_set_draw_pixel_cb(void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color), tLuiDispDrv *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->draw_pixel_cb = draw_pixel_cb;
}

void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), tLuiDispDrv *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->draw_pixels_area_cb = draw_pixels_area_cb;
}


/*------------------------------------------------------------------------------
 * 			INPUT DEVICE Callback Functions and Input properties
 *------------------------------------------------------------------------------
 */

tLuiTouchInputDev* lui_touch_inputdev_create()
{
	tLuiTouchInputDev *initial_touch_inputdev = malloc(sizeof(*initial_touch_inputdev));

	initial_touch_inputdev->read_touch_input_cb = NULL;
	// initial_touch_inputdev.touch_data.is_pressed = 0;
	// initial_touch_inputdev.touch_data.x = 0;
	// initial_touch_inputdev.touch_data.y = 0;

	return initial_touch_inputdev;
}

void lui_touch_inputdev_register (tLuiTouchInputDev *inputdev)
{
	if (inputdev == NULL)
		return;
	g_lui_main.touch_input_dev = inputdev;
}

void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *inputdata), tLuiTouchInputDev *inputdev)
{
	if (inputdev == NULL)
		return;
	inputdev->read_touch_input_cb = read_touch_input_cb;
}

tLuiDpadInputDev* lui_dpad_inputdev_create()
{
	tLuiDpadInputDev *initial_dpad_inputdev = malloc(sizeof(*initial_dpad_inputdev));

	initial_dpad_inputdev->read_dpad_input_cb = NULL;
	// initial_dpad_inputdev.dpad_data.is_up_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_down_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_left_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_right_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_cancel_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_enter_pressed = 0;

	return initial_dpad_inputdev;
}

void lui_dpad_inputdev_register (tLuiDpadInputDev *inputdev)
{
	if (inputdev == NULL)
		return;
	g_lui_main.dpad_input_dev = inputdev;
}

void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(tLuiDpadInputData *inputdata), tLuiDpadInputDev *inputdev)
{
	if (inputdev == NULL)
		return;
	inputdev->read_dpad_input_cb = read_dpad_input_cb;
}



/*------------------------------------------------------------------------------
 * 			PRIVATE FUNCTIONS. TO BE ONLY USED BY THIS LIBRARY
 * 			These functions are helper functions
 *------------------------------------------------------------------------------
 */

uint8_t lui_get_event_against_state(uint8_t new_state, uint8_t old_state)
{
	uint8_t event = LUI_EVENT_NONE;

	if (new_state == old_state)
	{
		event = LUI_EVENT_NONE;
	}
	else
	{
		switch (old_state)
		{
		case LUI_STATE_IDLE:			//old
			switch (new_state)
			{
			case LUI_STATE_SELECTED:	//new
				event = LUI_EVENT_SELECTED;
				break;
			case LUI_STATE_PRESSED:		//new
				event = LUI_EVENT_PRESSED;
				break;
			case LUI_STATE_ENTERED:		//new
				event = LUI_EVENT_ENTERED;
				break;
			default:
				break;
			}
			break;
			
		case LUI_STATE_SELECTED:		//old
			switch (new_state)
			{
			case LUI_STATE_IDLE:		//new
				event = LUI_EVENT_SELECTION_LOST;
				break;
			case LUI_STATE_PRESSED:		//new
				event = LUI_EVENT_PRESSED;
				break;
			case LUI_STATE_ENTERED:		//new
				event = LUI_EVENT_ENTERED;
				break;
			default:
				break;
			}
			break;

		// PRESSED is only applicable for button
		case LUI_STATE_PRESSED:			//old
			switch (new_state)
			{
			case LUI_STATE_IDLE:		//new
				event = LUI_EVENT_SELECTION_LOST;
				break;
			case LUI_STATE_SELECTED:	//new
				event = LUI_EVENT_RELEASED;
				break;
			// for button, ENTERED will never happen
			case LUI_STATE_ENTERED:		//new
				event = LUI_EVENT_NONE;
				break;
			default:
				break;
			}
			break;

		// ENTERED is only applicable for slider
		case LUI_STATE_ENTERED:			//old
			switch (new_state)
			{
			case LUI_STATE_IDLE:		//new
				event = LUI_EVENT_EXITED;
				break;
			case LUI_STATE_SELECTED:	//new
				event = LUI_EVENT_EXITED;
				break;
			// for slider, PRESSED will never happen
			case LUI_STATE_PRESSED:		//new
				event = LUI_EVENT_NONE;
				break;
			default:
				break;
			}
			break;
		
		default:
			break;
		}
	}

	return event;
}

/*
 * Draws a character glyph in left-to-right order
 * Monochrome fonts generated with lcd-image-converter software are supported only
 * Font must be generated by scanning from left to right
 *
 * Returns the last written pixel's X position
 */
void lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph)
{
	uint16_t width = 0, height = 0;

	width = glyph->width;
	height = glyph->height;

	uint16_t temp_x = x;
	uint16_t temp_y = y;

	uint8_t mask = 0x80;
	uint8_t bit_counter = 0;

	const uint8_t *glyph_data_ptr = (const uint8_t *)(glyph->data);
	uint8_t glyph_data = 0;

	// font bitmaps are stored in column major order (scanned from left-to-right, not the conventional top-to-bottom)
	// as font glyphs have heigher height than width, this scanning saves some storage.
	// So, we also render in left-to-right manner.

	// Along x axis (width)
	for (int i = 0; i < width; i++)
	{
		// Along y axis (height)
		for (int j = 0; j < height; j++)
		{

			// load new data only when previous byte (or word, depends on glyph->dataSize) is completely traversed by the mask
			// bit_counter = 0 means glyph_data is completely traversed by the mask
			if (bit_counter == 0)
			{
				glyph_data = *glyph_data_ptr++;
				bit_counter = glyph->dataSize;
			}
			// Decrement bit counter
			bit_counter--;

			//If pixel is blank
			if (glyph_data & mask)
			{
				/* Glyph's bg color is drawn by label drawing function.
				 * So, here we're doing nothing when pixel is blank
				 */
				// Draw the background color
				//lui_disp_drv->draw_pixel_cb(temp_x, temp_y, back_color);
			}

			//if pixel is not blank
			else
			{
				// Draw the foreground color
				g_lui_main.disp_drv->draw_pixel_cb(temp_x, temp_y, fore_color);
			}

			glyph_data <<= 1;
			temp_y++;
		}

		//New col starts. So, row is set to initial value and col is increased by one
		temp_y = y;
		temp_x++;

		//Reset the bit counter cause we're moving to next column, so we start with a new byte
		bit_counter = 0;
	}
}

/*
 * Get the width and height of a string (in pixels).
 * Width: by adding up the width of each glyph (representing a character)
 * Height: Height of any glyph (representing a character)
 */
void get_string_dimension(const char *str, const tFont *font, uint8_t *str_dim)
{
	 str_dim[0] = 0;	// -> width
	 str_dim[1] = 0;	// -> height

	// Scan chars one by one from the string
	for (uint16_t char_cnt = 0; *(str+char_cnt) != '\0'; char_cnt++)
	{
		// Find the glyph for the char from the font
		for (uint8_t i = 0; i < font->length; i++)
		{
			if (font->chars[i].code == *(str+char_cnt))
			{
				// Add width of glyphs
				str_dim[0] += font->chars[i].image->width;
				break;
			}
		}
	}
	// Set height as the height of the glyph of "space"
	str_dim[1] = font->chars[0].image->height;
}

/*
 * Brehensen's algorithm is used.
 * Draw line between ANY two points.
 * Not necessarily start points has to be less than end points.
 */
void lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
{
	/*
	* Brehensen's algorithm is used.
	* Not necessarily start points has to be less than end points.
	*/

	if (x0 == x1)	//vertical line
	{
		g_lui_main.disp_drv->draw_pixels_area_cb(x0, (y0 < y1 ? y0 : y1), (uint16_t)line_width, (uint16_t)abs(y1 - y0 + 1), color);
	}
	else if (y0 == y1)		//horizontal line
	{
		g_lui_main.disp_drv->draw_pixels_area_cb((x0 < x1 ? x0 : x1), y0, (uint16_t)abs(x1 - x0 + 1), (uint16_t)line_width, color);
		//printf("x0:%d y0:%d x1:%d y1:%d w:%d h:%d\n", x0, y0, x1, y1, (uint16_t)abs(x1 - x0 + 1), (uint16_t)line_width);
	}
	else
	{
		if (abs(y1 - y0) < abs(x1 - x0))
		{
			if (x0 > x1)
				lui_plot_line_low(x1, y1, x0, y0, line_width, color);
			else
				lui_plot_line_low(x0, y0, x1, y1, line_width, color);
		}

		else
		{
			if (y0 > y1)
				lui_plot_line_high(x1, y1, x0, y0, line_width, color);
			else
				lui_plot_line_high(x0, y0, x1, y1, line_width, color) ;
		}
	}

}

/*
 * When dy < 0
 * It's called only by line_draw function. Not for user
 */
void lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
{
	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int8_t yi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}

	int16_t D = 2*dy - dx;
	uint16_t y = y0;
	uint16_t x = x0;

	while (x <= x1)
	{
		if (line_width > 1)
			g_lui_main.disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);
		else
			g_lui_main.disp_drv->draw_pixel_cb(x, y, color);
		if (D > 0)
		{
			y = y + yi;
			D = D - 2*dx;
		}
		D = D + 2*dy;
		x++;
	}
}

/*
 * When dx < 0
 * It's called only by line_draw function. Not for user
 */
void lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
{
	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int8_t xi = 1;
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}

	int16_t D = 2*dx - dy;
	uint16_t y = y0;
	uint16_t x = x0;

	while (y <= y1)
	{
		if (line_width > 1)
			g_lui_main.disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);
		else
			g_lui_main.disp_drv->draw_pixel_cb(x, y, color);

		if (D > 0)
		{
			x = x + xi;
			D = D - 2*dy;
		}
		D = D + 2*dx;
		y++;
	}
}

/*
 * Draw a rectangle with a given color and line width
 */
void lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color)
{
	lui_draw_line(x, y, x + w, y, line_width, color);
	lui_draw_line(x + w, y, x + w, y + h, line_width, color);
	lui_draw_line(x + w, y + h, x, y + h, line_width, color);
	lui_draw_line(x, y + h, x, y, line_width, color);
}

/*
 * Fill a rectangular area with a color
 */
void lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	g_lui_main.disp_drv->draw_pixels_area_cb(x, y, w, h, color);
}

/*
 * Map a range of data to a new range of data
 */
double lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min)
{
    double new_val = ((((old_val - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min);
    return new_val;
}

uint8_t lui_disp_drv_check()
{
	uint8_t status = 0;
	// if no display driver is registered, return
	if (g_lui_main.disp_drv == NULL)
		status = 0;
	// If no callback function (for drawing) is provided by user, return
	else if (g_lui_main.disp_drv->draw_pixel_cb == NULL || g_lui_main.disp_drv->draw_pixels_area_cb == NULL)
		status = 0;
	else
		status = 1;

	return status;
}
