/*
 * lame_ui.c
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 21-March-2020
 *      Author: rik
 */

#include "lame_ui.h"

//Global variables
//uint8_t lui_scene_total = 0;
//lui_dispdrv_t *g_lui_main.disp_drv = NULL;
//lui_touch_input_dev_t *g_lui_main.touch_input_dev = NULL;
//lui_dpad_input_dev_t *g_lui_main.dpad_input_dev = NULL;
//lui_scene_t *g_lui_main.scenes[MAX_SCENES];

lui_main_t g_lui_main = {
	.scenes = {NULL},
	.disp_drv = NULL,
	.touch_input_dev = NULL,
	.dpad_input_dev = NULL,
	.total_scenes = 0,
	.active_scene = NULL, //not using right now
	//.active_obj = malloc(sizeof(*(initial_scene->active_obj))), // experimental new
	.active_obj = NULL,
	.total_created_objects = 0
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
void lui_label_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t x_temp = obj->x;
	uint16_t y_temp = obj->y;

	const tImage *img = {0};
	uint16_t width = 0, height = 0;

	tFont *temp_font = lbl->font;
	// If label has no font set for it, check the global set font in scene
	// If no scene present, i.e., parent_scene_index == -1, return
	// If scene is there but scene has no font, return
	// So, if label has no parent scene, it must have its own font to be rendered
	if (temp_font == NULL && g_lui_main.active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}

	// Draw the label background color
	_lui_draw_rect_fill(obj->x, obj->y, obj->width, obj->height, obj->bg_color);

	// Scan chars one by one from the string
	//char
	for (uint16_t char_cnt = 0; *(lbl->text+char_cnt) != '\0'; char_cnt++)
	{
		if (*(lbl->text+char_cnt) == '\n')
		{
			x_temp = obj->x;					//go to first col
			y_temp += (temp_font->chars[0].image->height);	//go to next row (row height = height of space)
		}

		else if (*(lbl->text+char_cnt) == '\t')
		{
			x_temp += 4 * (temp_font->chars[0].image->height);	//Skip 4 spaces (width = width of space)
		}
		else
		{
			// Find the glyph for the char from the font
			for (uint8_t i = 0; i < temp_font->length; i++)
			{
				if (temp_font->chars[i].code == *(lbl->text+char_cnt))
				{
					img = temp_font->chars[i].image;
					break;
				}
			}
			width = img->width;
			height = img->height;


			// check if not enough space available at the right side
			if (x_temp + width > obj->x + obj->width)
			{
				x_temp = obj->x;					//go to first col
				y_temp += height;	//go to next row

				// check if not enough space available at the bottom
				if(y_temp + height > obj->y + obj->height - 1)
					return;
			}

			_lui_draw_char(x_temp, y_temp, lbl->color, img);

			x_temp += width;		//next char position
		}
	}

}

/*
 * Initialize a label with default values
 */
lui_obj_t* lui_label_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if (g_lui_main.total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	g_lui_main.total_created_objects++;

	lui_label_t *initial_label = malloc(sizeof(*initial_label));
	lui_obj_t *obj = malloc(sizeof(*obj));
	
	initial_label->text = "";
	initial_label->font = NULL;
	initial_label->color = LUI_DEFAULT_TEXT_FORECOLOR;

	obj->obj_type = LUI_OBJ_LABEL;
	obj->x = 0;
	obj->y = 0;
	obj->width = 0;
	obj->height = 0;
	obj->bg_color = LUI_DEFAULT_TEXT_BGCOLOR;
	obj->index = -1;
	obj->parent = NULL;
	obj->children = NULL;
	obj->children_count = 0;
	obj->needs_refresh = 1;

	obj->obj_main_data = (void *)initial_label;

	return obj;
}



// void* lui_label_destroy(lui_label_t *lbl)
// {
// 	// Note: after freeing an object's memory, we need to set it to NULL so we can compare later
// 	// But setting the pointer to NULL inside the function will only change it locally.
// 	// So, after freeing, return NULL from function and assign the value to the object pointer from the calling side
//     if (lbl != NULL)
// 		_lui_object_destroy(lbl, &(lbl->common_data));
// 	return NULL;
// }

// void lui_label_set_position(uint16_t x, uint16_t y, lui_label_t *lbl)
// {
// 	if (lbl == NULL)
// 		return;
// 	_lui_object_set_position(x, y, &lbl->common_data);
// }

// void lui_label_set_area(uint16_t width, uint16_t height, lui_label_t *lbl)
// {
// 	if (lbl == NULL)
// 		return;
	
// 	_lui_object_set_area(width, height, &lbl->common_data);
// }

void lui_label_set_font(const tFont *font, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	lbl->font = (tFont *)font;
	_lui_object_set_need_refresh_recurse(obj->parent);
	obj->needs_refresh = 1;	//not quite neccesary, the above function already sets the flag for children too.
		//g_lui_main.scenes[obj->parent_index]->needs_refresh = 1;
}

void lui_label_set_text(const char *text, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	lbl->text = (char *)text;
	obj->needs_refresh = 1;
}

void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	if (lbl->color == text_color && obj->bg_color == bg_color)
		return;
	obj->needs_refresh = 1;
	lbl->color = text_color;
	obj->bg_color = bg_color;
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
void lui_linechart_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t width = obj->width;
	uint16_t height = obj->height;
	uint16_t line_color = chart->color;
	uint16_t data_points = chart->data.points;

	double mapped_data[g_lui_main.disp_drv->display_hor_res * 2];

	double x_data_min_new = temp_x;
	double x_data_max_new = temp_x + width - 1;
	//[0][0] element of 2D array is x_min
	// address of [0][0] = base address
	double x_data_min_old = *(chart->data.source);
	//[n][0] element of 2D array is x_max
	//address of [n][0] = base address + (n*2) - 2
	double x_data_max_old = *(chart->data.source + (data_points*2) - 2);


	double y_data_min_new = temp_y + height - 1;
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
	if (temp_font == NULL && g_lui_main.active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}


	// Draw the chart background
	_lui_draw_rect_fill(temp_x, temp_y, width, height, obj->bg_color);

	// Draw the scale numbers

	// Draw the chart grid if needed
	if (chart->grid.is_grid)
	{
		uint16_t hor_grid_spacing = height / (chart->grid.hor_count + 1);
		uint16_t vert_grid_spacing = width / (chart->grid.vert_count + 1);

		// Draw the vertical grids from left to right
		for (int i = 1; i <= chart->grid.vert_count; i++)
		{
			uint16_t temp_x_new = temp_x + (i * vert_grid_spacing);
			_lui_draw_line(temp_x_new, temp_y, temp_x_new, temp_y + height - 1, 1, chart->grid.color);
		}

		// Draw the horizontal grids from bottom to top
		uint16_t y_bottom = temp_y + height;
		for (int i = 1; i <= chart->grid.hor_count; i++)
		{
			uint16_t temp_y_new = y_bottom - (i * hor_grid_spacing);
			_lui_draw_line(temp_x, temp_y_new, temp_x + width - 1, temp_y_new, 1, chart->grid.color);
		}
	}


	// Map all the point values to pixel co-ordinate values
	for (int i = 0; i < data_points; i++)
	{
		double x_data_old = *(chart->data.source + (i*2));
		double y_data_old = *(chart->data.source + (i*2) + 1);
		// Mapping range of x values
		mapped_data[i*2] =  _lui_map_range(x_data_old, x_data_max_old, x_data_min_old, x_data_max_new, x_data_min_new);

		// Mapping range of y values
		mapped_data[i*2 + 1] =  _lui_map_range(y_data_old, y_data_max_old, y_data_min_old, y_data_max_new, y_data_min_new);
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
			g_lui_main.disp_drv->draw_pixels_area_cb(current_x, current_y, 1, 1, line_color);
		}

		// We have next points after thispoint
		else
		{
			uint16_t next_x = mapped_data [i*2 + 2];
			uint16_t next_y = mapped_data [i*2 + 3];

			_lui_draw_line(current_x, current_y, next_x, next_y, 1, line_color);
		}
	}

	// Draw the chart border if needed
	if (obj->border_color != obj->bg_color)
	{
		_lui_draw_rect(temp_x, temp_y, width, height, 1, obj->border_color);
	}
}

/*
 * Initialize a line chart with default values
 */
lui_obj_t* lui_linechart_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if (g_lui_main.total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	g_lui_main.total_created_objects++;

	lui_chart_t *initial_line_chart = malloc(sizeof(*initial_line_chart)); //(lui_chart_t *)malloc(sizeof(lui_chart_t))
	lui_obj_t *obj = malloc(sizeof(*obj));

	double tmp_data[] = {0};
	initial_line_chart->data.source = tmp_data;
	initial_line_chart->data.y_max_value = 0;
	initial_line_chart->data.y_min_value = g_lui_main.disp_drv->display_vert_res;
	initial_line_chart->data.points = 0;
	initial_line_chart->data.auto_scale = 1;

	initial_line_chart->color = LUI_DEFAULT_LINE_CHART_FORECOLOR;
	initial_line_chart->grid.color = LUI_DEFAULT_LINE_CHART_FORECOLOR;
	initial_line_chart->grid.hor_count = 5;
	initial_line_chart->grid.vert_count = 5;
	initial_line_chart->grid.is_grid = 1;
	initial_line_chart->font = NULL;


	obj->obj_type = LUI_OBJ_LINECHART;
	obj->x = 0;
	obj->y = 0;
	obj->width = 0;
	obj->height = 0;
	obj->border_color = LUI_DEFAULT_LINE_CHART_FORECOLOR;
	obj->bg_color = LUI_DEFAULT_LINE_CHART_BGCOLOR;
	obj->index = -1;
	obj->parent = NULL;
	obj->children = NULL;
	obj->children_count = 0;
	obj->needs_refresh = 1;

	obj->obj_main_data = (void *)initial_line_chart;

	return obj;
}


void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->grid.color == color && chart->grid.hor_count == hor_lines && chart->grid.vert_count == vert_lines)
		return;	
	obj->needs_refresh = 1;
	chart->grid.color = color;
	chart->grid.hor_count = hor_lines;
	chart->grid.vert_count = vert_lines;
}

void lui_linechart_set_grid_visible(uint8_t state, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->grid.is_grid == state)
		return;	
	obj->needs_refresh = 1;
	chart->grid.is_grid = state;
}

void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->color == line_color && obj->bg_color == bg_color)
		return;	
	obj->needs_refresh = 1;
	chart->color = line_color;
	obj->bg_color = bg_color;
}

void lui_linechart_set_data_auto_scale(uint8_t auto_scale, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (auto_scale == chart->data.auto_scale)
		return;
	chart->data.auto_scale = auto_scale ? 1 : 0;
	obj->needs_refresh = 1;
}

void lui_linechart_set_data_range(double y_min, double y_max, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->data.y_max_value == y_max && chart->data.y_min_value == y_min)
		return;
	obj->needs_refresh = 1;
	chart->data.y_max_value = y_max;
	chart->data.y_min_value = y_min;
}

void lui_linechart_set_data_source(double *source, uint16_t points, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->data.points == points)
		return;
	obj->needs_refresh = 1;
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
void lui_button_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t btn_height = obj->height;
	uint16_t btn_width = obj->width;


	uint8_t str_width_height[2];

	// Draw the button's body color depending on its current state
	uint16_t btn_color = btn->color;
	if (obj->state == LUI_STATE_SELECTED)
		btn_color = btn->selection_color;
	else if (obj->state == LUI_STATE_PRESSED)
		btn_color = btn->pressed_color;
	// else if (btn->state == LUI_STATE_IDLE)
	// 	btn_color = btn->color;

	_lui_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn_color);
	

	// Draw the button label (text)
	// Text will be in the miidle of the button.
	// So, at first we need to calculate its position

	tFont *temp_font = btn->label.font;
	// If it has no font set for it, check the global set font in scene
	// If no scene present, i.e., parent_scene_index == -1, return
	// If scene is there but scene has no font, return
	// So, if it has no parent scene, it must have its own font to be rendered
	if (temp_font == NULL && g_lui_main.active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}

	_get_string_dimension(btn->label.text, temp_font, str_width_height);

	str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
	str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

	temp_x = temp_x + (btn_width - str_width_height[0]) / 2;
	temp_y = temp_y + (btn_height - str_width_height[1]) / 2;

	lui_label_t btn_label;
	lui_obj_t lbl_obj;
	btn_label.text = btn->label.text;
	btn_label.color = btn->label.color;
	// bg_color depends on button's current state color
	if (obj->state == LUI_STATE_IDLE)
		lbl_obj.bg_color = btn->color; // normal situation
	else if (obj->state == LUI_STATE_SELECTED)
		lbl_obj.bg_color = btn->selection_color;
	else if (obj->state == LUI_STATE_PRESSED)
		lbl_obj.bg_color = btn->pressed_color;
	lbl_obj.x = temp_x;
	lbl_obj.y = temp_y;
	lbl_obj.width = str_width_height[0];
	lbl_obj.height = str_width_height[1];
	btn_label.font = temp_font;
	lbl_obj.obj_main_data = &btn_label;
	lui_label_draw(&lbl_obj);
}

/*
 * Create a button with default variables
 */
lui_obj_t* lui_button_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if (g_lui_main.total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	g_lui_main.total_created_objects++;

	lui_button_t *initial_button = malloc(sizeof(*initial_button));
	lui_obj_t *obj = malloc(sizeof(*obj));

	initial_button->color = 0xFFFF;
	initial_button->pressed_color = 0xCE59; //grey
	initial_button->selection_color = 0xFF40; //Yellow
	
	initial_button->label.text = "";
	initial_button->label.color = 0x0000; //black
	initial_button->label.font = NULL;

	initial_button->dpad_row_pos = -1;
	initial_button->dpad_col_pos = -1;

	


	obj->obj_type = LUI_OBJ_BUTTON;
	obj->x = 0;
	obj->y = 0;
	obj->width = 0;
	obj->height = 0;
	obj->border_color = LUI_DEFAULT_LINE_CHART_FORECOLOR;
	obj->bg_color = 0xFFFF;
	obj->state = LUI_STATE_IDLE;
	obj->obj_event_cb = NULL;
	obj->index = -1;
	obj->parent = NULL;
	obj->children = NULL;
	obj->children_count = 0;
	obj->needs_refresh = 1;

	obj->obj_main_data = (void *)initial_button;

	return  obj;
}


void lui_button_set_label_text(const char *text, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	btn->label.text = (char *)text;
	obj->needs_refresh = 1;
}

void lui_button_set_label_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	if (btn->label.color == color)
		return;
	btn->label.color = color;
	obj->needs_refresh = 1;
}

void lui_button_set_label_font(const tFont *font, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	btn->label.font = (tFont *)font;
	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh_recurse(obj->parent);
	obj->needs_refresh = 1;	//not quite neccesary, the above function already sets the flag for children too.
}

void lui_button_set_colors(uint16_t bg_color, uint16_t pressed_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	if (btn->color == bg_color && btn->pressed_color == pressed_color && btn->selection_color == selection_color)
		return;
	btn->color = bg_color;
	btn->pressed_color = pressed_color;
	btn->selection_color = selection_color;
	obj->needs_refresh = 1;
}

void lui_button_set_dpad_position(uint8_t row, uint8_t col, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	if ((btn->dpad_row_pos == row && btn->dpad_col_pos == col) ||	// if row/col pos are unchanged or either of them are negative, return
		(btn->dpad_row_pos < 0 ||btn->dpad_col_pos < 0))
		return;
	btn->dpad_row_pos = row;
	btn->dpad_col_pos = col;
}

#pragma endregion Button

#pragma region Switch

/*-------------------------------------------------------------------------------
 * 				LUI_SWITCH related functions
 *-------------------------------------------------------------------------------
 */

void lui_switch_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_switch_t *swtch = (lui_switch_t *)(obj->obj_main_data);
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t temp_height = obj->height;
	uint16_t temp_width = obj->width;

	uint16_t swtch_color = swtch->fore_color;
	if (obj->state == LUI_STATE_SELECTED || obj->state == LUI_STATE_PRESSED)
		swtch_color = swtch->selection_color;
	
	_lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, obj->bg_color);	// switch bg (color is constant regardless the state)
	_lui_draw_rect(temp_x, temp_y, temp_width, temp_height, 1, swtch_color);	// switch border

	temp_width = (float)temp_width * 0.3;
	temp_height = (float)temp_height * 0.6;
	temp_x = temp_x + ((obj->width / 2) - temp_width) / 2;
	if (obj->value == 1)
		temp_x += (obj->width / 2);
	temp_y = temp_y + (obj->height - temp_height) / 2;

	_lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, swtch_color);// switch slider
}

lui_obj_t* lui_switch_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if (g_lui_main.total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	g_lui_main.total_created_objects++;

	lui_obj_t *obj = malloc(sizeof(*obj));
	lui_switch_t *initial_switch = malloc(sizeof(*initial_switch));

	
	initial_switch->fore_color = 0x0000; 
	initial_switch->selection_color = 0xFF40; //Yellow	
	obj->value = 0;
	initial_switch->dpad_row_pos = -1;
	initial_switch->dpad_col_pos = -1;
	

	obj->obj_type = LUI_OBJ_SWITCH;
	obj->x = 20;
	obj->y = 20;
	obj->width = 40;
	obj->height = 20;
	obj->bg_color = 0xFFFF;
	obj->state = LUI_STATE_IDLE;
	obj->value = 0;
	obj->obj_event_cb = NULL;
	obj->index = -1;
	obj->parent = NULL;
	obj->children = NULL;
	obj->children_count = 0;
	obj->needs_refresh = 1;

	obj->obj_main_data = (void *)initial_switch;

	return  obj;
}

void lui_switch_set_colors(uint16_t fore_color, uint16_t bg_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_switch_t *swtch = (lui_switch_t *)(obj->obj_main_data);

	if (swtch->fore_color == fore_color && obj->bg_color == bg_color && swtch->selection_color == selection_color)
		return;
	swtch->fore_color = fore_color;
	obj->bg_color = bg_color;
	swtch->selection_color = selection_color;
	obj->needs_refresh = 1;
}

int8_t lui_switch_get_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	return obj->value;
}

void lui_switch_set_value(uint8_t value, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (value > 1)
		value = 1;
	obj->value = value;
	obj->needs_refresh = 1;
}




#pragma endregion Switch

/*-------------------------------------------------------------------------------
 * 				LUI_SCENE related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Create a empty scene with default values and return the scene variable
 */
lui_obj_t* lui_scene_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if (g_lui_main.total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	g_lui_main.total_created_objects++;

	lui_scene_t *initial_scene = malloc(sizeof(*initial_scene));
	lui_obj_t *obj = malloc(sizeof(*obj));

	//initial_scene->obj_total = 0;

	//initial_scene->obj_collection = NULL;

	initial_scene->font = NULL;
	initial_scene->bg_image = NULL;

	// Add the scene to global scenes array and set need_refresh to 1
	//initial_scene->index = g_lui_main.total_scenes;
	//g_lui_main.scenes[initial_scene->index] = initial_scene;
	//g_lui_main.total_scenes++;
	//initial_scene->needs_refresh = 1;

	obj->obj_type = LUI_OBJ_SCENE;
	obj->x = 0;
	obj->y = 0;
	obj->width = g_lui_main.disp_drv->display_hor_res;
	obj->height = g_lui_main.disp_drv->display_vert_res;
	obj->bg_color = 0xFFFF;
	obj->state = LUI_STATE_IDLE;
	obj->value = 0;
	obj->obj_event_cb = NULL;
	obj->index = g_lui_main.total_scenes;
	obj->parent = NULL;
	obj->children = NULL;
	obj->children_count = 0;
	_lui_object_set_need_refresh_recurse(obj);
	obj->needs_refresh = 1;

	obj->obj_main_data = (void *)initial_scene;

	g_lui_main.scenes[obj->index] = obj;
	g_lui_main.total_scenes++;

	return obj;
}



void lui_scene_set_bg_image(const tImage *image, lui_obj_t *obj_scene)
{
	// NOTE: image rendering is not implemented yet
	if (obj_scene == NULL)
		return;
	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	scene->bg_image = (tImage *)image;

	_lui_object_set_need_refresh_recurse(obj_scene); 
}

void lui_scene_set_font(const tFont *font, lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;
	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	scene->font = (tFont *)font;
	
	_lui_object_set_need_refresh_recurse(obj_scene); 
}

void lui_scene_set_active(lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;
	g_lui_main.active_scene = obj_scene;
}

lui_obj_t* lui_scene_get_active()
{
	return g_lui_main.active_scene;
}

/*
 * Update the entire scene that contains some UI elements
 */
void lui_scene_render(lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;
	
	g_lui_main.active_scene = obj_scene; // set this scene as active

	lui_obj_t *obj_caused_cb = NULL;
	// Reading input only if user provided input reading callback function
	if (g_lui_main.touch_input_dev != NULL)
		obj_caused_cb = _lui_process_touch_input_of_act_scene();
	// else if (g_lui_main.dpad_input_dev != NULL)
	// 	_lui_process_dpad_input(scene);


	_lui_object_render_recurse(obj_scene);

	// If user is buffering the draw_pixels_area calls instead of instantly flushing to display, 
	// this callback signals that render is finished and buffer should be flushed to display now
	if (g_lui_main.disp_drv->render_complete_cb != NULL)
		g_lui_main.disp_drv->render_complete_cb();

	//All rendering done, now we'll handle the callback
	// if the object that caused callback is not NULL and if the object has a callback func,
	if (obj_caused_cb != NULL && obj_caused_cb->obj_event_cb != NULL)
	{
		// Call the user-supplied callback
		obj_caused_cb->obj_event_cb(obj_caused_cb);
		//event callback handled, so reset it
		obj_caused_cb->event = LUI_EVENT_NONE;
	}	
}


/*-------------------------------------------------------------------------------
 * 				Element COMMON functions
 *-------------------------------------------------------------------------------
 */

// Assumes no parameter is NULL
void lui_object_add_to_parent(lui_obj_t *obj, lui_obj_t *parent_obj)
{
	if (obj == NULL || parent_obj == NULL)
		return;
	// scene cannot be added to any parent, so return
	if (obj->obj_type == LUI_OBJ_SCENE)
		return;
	// only panel and scene can be parent, otherwise return
	if (parent_obj->obj_type != LUI_OBJ_PANEL && parent_obj->obj_type != LUI_OBJ_SCENE)
		return;

    //add the ui element with a new index to scene only if no parent already exists
    if (obj->parent != NULL)
		return;
	

	obj->index = parent_obj->children_count++;
	if (parent_obj->children == NULL)
		parent_obj->children = malloc(sizeof(parent_obj->children));
	else
		parent_obj->children = realloc(parent_obj->children, sizeof(parent_obj->children) * (parent_obj->children_count));
	parent_obj->children[obj->index] = (lui_obj_t *)obj;


	// Common things to do
	obj->parent = parent_obj;
	_lui_object_set_need_refresh_recurse(obj->parent);
	obj->needs_refresh = 1;
}

void lui_object_remove_from_parent(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// If item's or scene's index is -1, return
	if (obj->index == -1 || obj->parent == NULL)
    	return;

	for (int i = obj->index; i < obj->parent->children_count - 1; i++)
	{
		obj->parent->children[i] = obj->parent->children[i+1];
		obj->parent->children[i]->index = i;
	}

	obj->parent->children_count--;

	// resize the array
	if (obj->parent->children_count == 0)
	{
		free(obj->parent->children);
		obj->parent->children = NULL;
	}
	else
	{
		obj->parent->children = realloc(obj->parent->children, sizeof(lui_obj_t *) * (obj->parent->children_count));
	}
	
	// common things to do
	obj->index = -1;
	_lui_object_set_need_refresh_recurse(obj->parent);
	obj->parent = NULL;
}


void lui_object_set_position(uint16_t x, uint16_t y, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->x == x && obj->y == y)
		return;	
	obj->x = x;
	obj->y = y;
	// setting position of children as well.
	for(uint8_t i = 0; i < obj->children_count; i++)
	{
		uint16_t child_new_x = obj->children[i]->x + x;
		uint16_t child_new_y = obj->children[i]->y + y;
		lui_object_set_position(child_new_x, child_new_y, obj->children[i]);
	}

	_lui_object_set_need_refresh_recurse(obj->parent);
	obj->needs_refresh = 1;
}

void lui_object_set_area(uint16_t width, uint16_t height, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	if (obj->width == width && obj->height == height)
		return;

	if (obj->width < width && obj->height < height)
		obj->needs_refresh = 1;
	else
	{	_lui_object_set_need_refresh_recurse(obj->parent);
		obj->needs_refresh = 1;	//not quite neccesary, the above function already sets the flag for children too.
	}
		
	obj->width = width;
	obj->height = height;	
}

void lui_object_set_border_color(uint16_t border_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->border_color == border_color)
		return;
	obj->border_color = border_color;
	obj->needs_refresh = 1;
}

void lui_object_set_bg_color(uint16_t bg_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->bg_color == bg_color)
		return;
	obj->bg_color = bg_color;
	_lui_object_set_need_refresh_recurse(obj);
	obj->needs_refresh = 1;
}

void lui_object_set_callback(void (*obj_event_cb)(lui_obj_t *), lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	obj->obj_event_cb = obj_event_cb;
}

int8_t lui_object_get_state(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	return obj->state;
}

int8_t lui_object_get_event(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	return obj->event;
}



/*------------------------------------------------------------------------------
 * 			INPUT processing functions
 *------------------------------------------------------------------------------
 */

// void _lui_process_touch_input(lui_scene_t *lui_scene)
/*
// {	
// 	lui_touch_input_data_t input;
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

uint8_t _lui_check_if_active_obj(lui_touch_input_data_t input, lui_obj_t *obj)
{
	if (input.x >= obj->x && 
		input.x < obj->x + obj->width &&
		input.y >= obj->y &&
		input.y < obj->y + obj->height)
	{
		g_lui_main.active_obj = obj;
		return 1;
	}
	else
	{
		// in case input is not on "obj" and previous "active_obj" is same as "obj",
		// set "input_on_obj" to NULL.
		if (g_lui_main.active_obj == obj)
			g_lui_main.active_obj = NULL;
		return 0;
	}	
}

void _lui_set_obj_props_on_input(lui_touch_input_data_t input, lui_obj_t *obj)
{
	uint8_t is_obj_active = _lui_check_if_active_obj(input, obj);
	uint8_t new_state = LUI_STATE_IDLE;

	if (is_obj_active == 1)
	{
		// if pressed, then....well, then state = PRESSED
		if (input.is_pressed == 1)
			new_state = LUI_STATE_PRESSED;
		
		// else not pressed, state = SELECTED
		else
			new_state = LUI_STATE_SELECTED;
	}
	obj->event = _lui_get_event_against_state(new_state, obj->state);		

	if (obj->event != LUI_EVENT_NONE)
	{
		obj->state = new_state;
		obj->needs_refresh = 1;
	}
	

	
	// Special case for switch: if event is LUI_EVENT_PRESSED, then set event to LUI_EVENT_VALUE_CHANGED
	// then set the value to `value` property
	if (obj->obj_type == LUI_OBJ_SWITCH)
	{	
		if (obj->event == LUI_EVENT_PRESSED)
		{
			obj->event = LUI_EVENT_VALUE_CHANGED;	// for switch, being pressed means being toggled, thus value changed
			obj->value = (obj->value == 1) ? 0 : 1;	// toggle the value (1->0 or 0-1)
		}	
	}
}

lui_obj_t* _lui_process_touch_input_of_act_scene()
{	
	uint8_t scan_all_objs_flag = 0;
	lui_obj_t *obj_caused_cb = NULL;
	lui_obj_t *last_active_obj = g_lui_main.active_obj;
	lui_touch_input_data_t input_data;
	g_lui_main.touch_input_dev->read_touch_input_cb(&input_data);

	if (last_active_obj == NULL)
	{
		scan_all_objs_flag = 1;
	}

	else
	{
		// sets object parameters based on input. also may modify g_lui_main.active_obj
		_lui_set_obj_props_on_input(input_data, last_active_obj);
		if (last_active_obj->event != LUI_EVENT_NONE)
		{
			if (g_lui_main.active_obj != last_active_obj /* *state == LUI_STATE_IDLE*/)
			{
				scan_all_objs_flag = 1;
			}
			else
			{
				obj_caused_cb = last_active_obj;
			}
			
		}
		else
		{
			obj_caused_cb = NULL;
		}
		
	}

	if (scan_all_objs_flag)
	{
		//_lui_scan_all_objects_recurse(input_data, obj, obj_caused_cb);
		obj_caused_cb = _lui_scan_all_obj_except_last_act_obj(input_data, g_lui_main.active_scene, last_active_obj);
		if (obj_caused_cb == NULL)
			obj_caused_cb = last_active_obj;
	}
	
	return obj_caused_cb;

}

void _lui_scan_all_objects_recurse(lui_touch_input_data_t input_data, lui_obj_t *obj, lui_obj_t *obj_caused_cb)
{
	lui_obj_t *last_active_obj = g_lui_main.active_obj;
	// we already considered the case of last_active_object, so we won't do it here
	if (obj == last_active_obj)
		return;

	// If object is not button or switch, go to the next object
	if (obj->obj_type == LUI_OBJ_BUTTON ||
		obj->obj_type == LUI_OBJ_SWITCH)
	{
		// sets object parameters based on input. also, may modify lui_scene->active_obj
		_lui_set_obj_props_on_input(input_data, obj);
		if (obj->event != LUI_EVENT_NONE)
		{
			obj_caused_cb = obj;
			return;
		}
		else
		{
			obj_caused_cb = last_active_obj;	//last_active_object can be NULL too
		}
	}
	else if (obj->obj_type == LUI_OBJ_PANEL ||
		obj->obj_type == LUI_OBJ_SCENE)
	{
		for (uint8_t i = 0; i < obj->children_count; i++)
		{
			_lui_scan_all_objects_recurse(input_data, obj->children[i], obj_caused_cb);
			// if obj_caused_cb becomes not NULL, break
			if (obj_caused_cb != NULL)
				break;
		}
	}
	
}

lui_obj_t* _lui_scan_all_obj_except_last_act_obj(lui_touch_input_data_t input_data, lui_obj_t *obj_root, lui_obj_t *last_act_obj)
{
	// Note: This function is made by converting a tail-recursive function to iterative function
	// The simple way is to use a stack.
	// see the answer: https://codereview.stackexchange.com/a/163621
	lui_obj_t *obj_caused_cb = NULL;

	for (uint8_t i = 0; i < obj_root->children_count; i++)
	{
		lui_obj_t *obj_stack[30]; //30 is maximum no ob objects in ascene. replace it with macro later
		uint8_t stack_cnt = 0;
		obj_stack[stack_cnt++] = obj_root->children[i];	//push to stack
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_cnt > 0)
		{
			// get current object from stack
			lui_obj_t *obj_current = obj_stack[--stack_cnt];	//pop

			// if current object is == last_active_object, continue. because, last_active_object is already processed
			if (obj_current == last_act_obj)
				continue;

			// we are only interested in objects that take input, like button, switch
			if (obj_current->obj_type == LUI_OBJ_BUTTON ||
				obj_current->obj_type == LUI_OBJ_SWITCH)
			{
				// sets object parameters based on input. also, may modify g_lui_main.active_obj
				_lui_set_obj_props_on_input(input_data, obj_current);
				if (obj_current->event != LUI_EVENT_NONE)
				{
					obj_caused_cb = obj_current;
					return obj_caused_cb;
				}
				else
				{
					obj_caused_cb = NULL;
				}
			}

			// push current object's children into stack
			for (uint8_t j = 0; j < obj_current->children_count; j++)
			{
				// for safety, if stack is about to overflow, return result
				if (stack_cnt > LUI_MAX_OBJECTS)
					return obj_caused_cb;
				obj_stack[stack_cnt++] = obj_current->children[j]; // push to stack
			}

		}
	}

	return obj_caused_cb;
}


// void _lui_process_dpad_input(lui_scene_t *lui_scene)
// {
// 	if (lui_scene->dpad.max_col_pos == -1) // no element is configured for dpad, so return
// 		return;

// 	lui_dpad_input_data_t input;
// 	g_lui_main.dpad_input_dev->read_dpad_input_cb(&input);

// 	if (input.is_right_pressed)	lui_scene->dpad.current_col_pos++;
// 	if (input.is_left_pressed)	lui_scene->dpad.current_col_pos--;
// 	if (input.is_down_pressed)	lui_scene->dpad.current_row_pos++;
// 	if (input.is_up_pressed)	lui_scene->dpad.current_row_pos--;

// 	// check dpad boundary
// 	if (lui_scene->dpad.current_col_pos > lui_scene->dpad.max_col_pos || lui_scene->dpad.current_col_pos < 0)
// 		lui_scene->dpad.current_col_pos = 0;
// 	if (lui_scene->dpad.current_row_pos > lui_scene->dpad.max_row_pos)
// 		lui_scene->dpad.current_row_pos = 0;

// 	uint8_t event;
// 	uint8_t input_on_element = 0; //if the input coordinates on the current elemt or not. if yes, stop scanning other elements & return
// 	for (uint8_t i = 0; i < lui_scene->obj_total; i++)
// 	{
// 		uint8_t new_state = LUI_STATE_IDLE;
// 		if (lui_scene->dpad.current_col_pos == lui_scene->lui_button[i]->dpad_col_pos &&
// 			lui_scene->dpad.current_row_pos == lui_scene->lui_button[i]->dpad_row_pos)
// 		{
// 			input_on_element = 1; 

// 			if (input.is_enter_pressed == 1)
// 				new_state = LUI_STATE_PRESSED;
// 			else
// 				new_state = LUI_STATE_SELECTED;
// 		}
// 		// else if input position is not on button position
// 		else
// 		{
// 			new_state = LUI_STATE_IDLE;
// 		}

// 		event = _lui_get_event_against_state(new_state, lui_scene->lui_button[i]->state);
		
// 		// If new state is not same as the existing state, only then refresh the button
// 		if (event != LUI_EVENT_NONE)
// 		{
// 			lui_scene->lui_button[i]->state = new_state;
// 			lui_scene->lui_button[i]->needs_refresh = 1;
// 			lui_scene->lui_button[i]->event = event;
// 		}
// 		else
// 		{
// 			lui_scene->lui_button[i]->needs_refresh = 0;
// 			lui_scene->lui_button[i]->event = LUI_EVENT_NONE;
// 		}
		

// 		// if input is on this ui element, then no more need to scan other elements. Return now
// 		if (input_on_element == 1)
// 			return;

// 	}

// }


/*------------------------------------------------------------------------------
 * 			DISPLAY DRIVER Callback Functions and Display Properties
 *------------------------------------------------------------------------------
 */

lui_dispdrv_t* lui_dispdrv_create()
{
	lui_dispdrv_t *initial_disp_drv = malloc(sizeof(*initial_disp_drv));

	initial_disp_drv->draw_pixels_area_cb = NULL;
	initial_disp_drv->render_complete_cb = NULL;
	initial_disp_drv->display_hor_res = 320;		//horizontal 320px default
	initial_disp_drv->display_vert_res = 240;	//vertical 240px default

	return initial_disp_drv;
}

void lui_dispdrv_register(lui_dispdrv_t *dispdrv)
{
	if (dispdrv == NULL)
		return;
	g_lui_main.disp_drv = dispdrv;
}

void* lui_dispdrv_destroy(lui_dispdrv_t *dispdrv)
{
	if (dispdrv != NULL)
		free(dispdrv);
	return NULL;
}

void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res, lui_dispdrv_t *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->display_hor_res = hor_res;
	dispdrv->display_vert_res = vert_res;
}


void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), lui_dispdrv_t *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->draw_pixels_area_cb = draw_pixels_area_cb;
}

void lui_dispdrv_set_render_complete_cb(void (*render_complete_cb)(), lui_dispdrv_t *dispdrv)
{
	if (dispdrv == NULL)
		return;
	dispdrv->render_complete_cb = render_complete_cb;
}


/*------------------------------------------------------------------------------
 * 			INPUT DEVICE Callback Functions and Input properties
 *------------------------------------------------------------------------------
 */

lui_touch_input_dev_t* lui_touch_inputdev_create()
{
	lui_touch_input_dev_t *initial_touch_inputdev = malloc(sizeof(*initial_touch_inputdev));

	initial_touch_inputdev->read_touch_input_cb = NULL;
	// initial_touch_inputdev.touch_data.is_pressed = 0;
	// initial_touch_inputdev.touch_data.x = 0;
	// initial_touch_inputdev.touch_data.y = 0;

	return initial_touch_inputdev;
}

void lui_touch_inputdev_register (lui_touch_input_dev_t *inputdev)
{
	if (inputdev == NULL)
		return;
	g_lui_main.touch_input_dev = inputdev;
}

void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(lui_touch_input_data_t *inputdata), lui_touch_input_dev_t *inputdev)
{
	if (inputdev == NULL)
		return;
	inputdev->read_touch_input_cb = read_touch_input_cb;
}

lui_dpad_input_dev_t* lui_dpad_inputdev_create()
{
	lui_dpad_input_dev_t *initial_dpad_inputdev = malloc(sizeof(*initial_dpad_inputdev));

	initial_dpad_inputdev->read_dpad_input_cb = NULL;
	// initial_dpad_inputdev.dpad_data.is_up_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_down_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_left_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_right_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_cancel_pressed = 0;
	// initial_dpad_inputdev.dpad_data.is_enter_pressed = 0;

	return initial_dpad_inputdev;
}

void lui_dpad_inputdev_register (lui_dpad_input_dev_t *inputdev)
{
	if (inputdev == NULL)
		return;
	g_lui_main.dpad_input_dev = inputdev;
}

void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(lui_dpad_input_data_t *inputdata), lui_dpad_input_dev_t *inputdev)
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
void _lui_object_render_recurse(lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// draw the object id it needs refresh (if its parent need refresh, then the object is also set to need refresh)
	if (obj->needs_refresh == 1)
	{
		switch (obj->obj_type)
		{
			case LUI_OBJ_SCENE:
				// for scene, just draw the full screen background 
				g_lui_main.disp_drv->draw_pixels_area_cb(0, 0, g_lui_main.disp_drv->display_hor_res, g_lui_main.disp_drv->display_vert_res, obj->bg_color);
				break;
			case LUI_OBJ_PANEL:
				// for panel, just draw background and optional border
				// TBD
				break;
			case LUI_OBJ_BUTTON:
				lui_button_draw(obj);
				break;
			case LUI_OBJ_SWITCH:
				lui_switch_draw(obj);
				break;
			case LUI_OBJ_LABEL:
				lui_label_draw(obj);
				break;
			case LUI_OBJ_LINECHART:
				lui_linechart_draw(obj);
				break;
			default:
				break;
		}
	}

	//render all children recursively (if any)
	for (uint8_t i = 0; i < obj->children_count; i++)
	{
		_lui_object_render_recurse(obj->children[i]);
	}

	obj->needs_refresh = 0;
}

void _lui_object_set_need_refresh_recurse(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// already flasg is 1, no need to recursively waste time. Return.
	if (obj->needs_refresh == 1)
		return;
	obj->needs_refresh = 1;
	for (uint8_t i = 0; i < obj->children_count; i++)
	{
		_lui_object_set_need_refresh_recurse(obj->children[i]);
	}
}


tFont* _lui_get_font_from_active_scene()
{
	if (g_lui_main.active_scene == NULL)
		return NULL;
	lui_scene_t *act_scene = (lui_scene_t *)(g_lui_main.active_scene->obj_main_data);
	return (act_scene->font);
}


uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state)
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
void _lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph)
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
				//g_lui_main.disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, back_color);
			}

			//if pixel is not blank
			else
			{
				// Draw the foreground color
				g_lui_main.disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, fore_color);
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
void _get_string_dimension(const char *str, const tFont *font_obj, uint8_t *str_dim)
{
	 str_dim[0] = 0;	// -> width
	 str_dim[1] = 0;	// -> height

	// Scan chars one by one from the string
	for (uint16_t char_cnt = 0; *(str+char_cnt) != '\0'; char_cnt++)
	{
		// Find the glyph for the char from the font
		for (uint8_t i = 0; i < font_obj->length; i++)
		{
			if (font_obj->chars[i].code == *(str+char_cnt))
			{
				// Add width of glyphs
				str_dim[0] += font_obj->chars[i].image->width;
				break;
			}
		}
	}
	// Set height as the height of the glyph of "space"
	str_dim[1] = font_obj->chars[0].image->height;
}

/*
 * Brehensen's algorithm is used.
 * Draw line between ANY two points.
 * Not necessarily start points has to be less than end points.
 */
void _lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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
				_lui_plot_line_low(x1, y1, x0, y0, line_width, color);
			else
				_lui_plot_line_low(x0, y0, x1, y1, line_width, color);
		}

		else
		{
			if (y0 > y1)
				_lui_plot_line_high(x1, y1, x0, y0, line_width, color);
			else
				_lui_plot_line_high(x0, y0, x1, y1, line_width, color) ;
		}
	}

}

/*
 * When dy < 0
 * It's called only by line_draw function. Not for user
 */
void _lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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
		g_lui_main.disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);

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
void _lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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
		g_lui_main.disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);

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
void _lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color)
{
	_lui_draw_line(x, y, x+w-1, y, line_width, color);
	_lui_draw_line(x+w-1, y, x+w-1, y+h-1, line_width, color);
	_lui_draw_line(x, y+h-1, x+w-1, y+h-1, line_width, color);
	_lui_draw_line(x, y, x, y+h-1, line_width, color);
}

/*
 * Fill a rectangular area with a color
 */
void _lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	g_lui_main.disp_drv->draw_pixels_area_cb(x, y, w, h, color);
}

/*
 * Map a range of data to a new range of data
 */
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min)
{
    double new_val = ((((old_val - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min);
    return new_val;
}

uint8_t _lui_disp_drv_check()
{
	uint8_t status = 0;
	// if no display driver is registered, return
	if (g_lui_main.disp_drv == NULL)
		status = 0;
	// If no callback function (for drawing) is provided by user, return
	else if (g_lui_main.disp_drv->draw_pixels_area_cb == NULL)
		status = 0;
	else
		status = 1;

	return status;
}






// ==================================================================================================
// Experimental
//==================================================================================================

// void lui_obj_render(lui_obj_t *obj)
// {
// 	if (obj == NULL)
// 		return;
	
// 	if (obj->obj_type == LUI_OBJ_LABEL)
// 	{
// 		lui_label_draw_new(obj);
// 	}

// 	g_lui_main.disp_drv->render_complete_cb();
// }


// void lui_label_draw_new(lui_obj_t *obj)
// {
// 	lui_label_t *lbl_temp = (lui_label_t *)(obj->obj_main_data);
// 	if (obj == NULL)
// 		return;
// 	// if no display driver is registered, return
// 	if (_lui_disp_drv_check() == 0)
// 		return;

// 	uint16_t x_temp = obj->x;
// 	uint16_t y_temp = obj->y;

// 	const tImage *img = {0};
// 	uint16_t width = 0, height = 0;

// 	tFont *temp_font = lbl_temp->font;
// 	// If label has no font set for it, check the global set font in scene
// 	// If no scene present, i.e., parent_scene_index == -1, return
// 	// If scene is there but scene has no font, return
// 	// So, if label has no parent scene, it must have its own font to be rendered
// 	if (temp_font == NULL)
// 	{
// 		// If no parent scene (i.e. parent_scene_index = -1), return
// 		if (obj->parent_index == -1)
// 			return;
// 		// If the parent scene also has no font set, return.
// 		if (g_lui_main.scenes[obj->parent_index]->font == NULL)
// 			return;

// 		// Otherwise use the scene's font (global font)
// 		temp_font = g_lui_main.scenes[obj->parent_index]->font;
// 	}

// 	// Draw the label background color
// 	_lui_draw_rect_fill(obj->x, obj->y, obj->width, obj->height, lbl_temp->bg_color);

// 	// Scan chars one by one from the string
// 	//char
// 	for (uint16_t char_cnt = 0; *(lbl_temp->text+char_cnt) != '\0'; char_cnt++)
// 	{
// 		if (*(lbl_temp->text+char_cnt) == '\n')
// 		{
// 			x_temp = obj->x;					//go to first col
// 			y_temp += (temp_font->chars[0].image->height);	//go to next row (row height = height of space)
// 		}

// 		else if (*(lbl_temp->text+char_cnt) == '\t')
// 		{
// 			x_temp += 4 * (temp_font->chars[0].image->height);	//Skip 4 spaces (width = width of space)
// 		}
// 		else
// 		{
// 			// Find the glyph for the char from the font
// 			for (uint8_t i = 0; i < temp_font->length; i++)
// 			{
// 				if (temp_font->chars[i].code == *(lbl_temp->text+char_cnt))
// 				{
// 					img = temp_font->chars[i].image;
// 					break;
// 				}
// 			}
// 			width = img->width;
// 			height = img->height;


// 			// check if not enough space available at the right side
// 			if (x_temp + width > obj->x + obj->width)
// 			{
// 				x_temp = obj->x;					//go to first col
// 				y_temp += height;	//go to next row

// 				// check if not enough space available at the bottom
// 				if(y_temp + height > obj->y + obj->height - 1)
// 					return;
// 			}

// 			_lui_draw_char(x_temp, y_temp, lbl_temp->color, img);

// 			x_temp += width;		//next char position
// 		}
// 	}

// }