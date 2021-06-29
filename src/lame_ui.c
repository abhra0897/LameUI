/*
 * lame_ui.c
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 27-Apr-2021
 *      Author: rik
 */

#include "lame_ui.h"


_lui_mem_block_t g_mem_block;
lui_main_t *g_lui_main;


/*-------------------------------------------------------------------------------
 * 				Main functions
 *-------------------------------------------------------------------------------
 */

void lui_init()
{
	_lui_mem_init();
	g_lui_main = _lui_mem_alloc(sizeof(lui_main_t));

	// g_lui_main->scenes = {NULL};
	 g_lui_main->disp_drv = NULL;
	 g_lui_main->touch_input_dev = NULL;
	 g_lui_main->dpad_input_dev = NULL;
	//  g_lui_main->last_touch_data.x = -1;
	//  g_lui_main->last_touch_data.y = -1;
	//  g_lui_main->last_touch_data.is_pressed = -1;
	 g_lui_main->total_scenes = 0;
	 g_lui_main->active_scene = NULL;
	 g_lui_main->active_obj = NULL;
	 g_lui_main->total_created_objects = 0;
}

/*
 * Update the entire scene that contains some UI elements
 */
void lui_update()
{
	if ( g_lui_main->active_scene == NULL)
		return;

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;


	lui_obj_t *obj_caused_cb = NULL;
	// Reading input only if user provided input reading callback function
	if ( g_lui_main->touch_input_dev != NULL)
		obj_caused_cb = _lui_process_touch_input_of_act_scene();
	// else if ( g_lui_main->dpad_input_dev != NULL)
	// 	_lui_process_dpad_input(scene);


	_lui_object_render_parent_with_children( g_lui_main->active_scene);

	// If user is buffering the draw_pixels_area calls instead of instantly flushing to display, 
	// this callback signals that render is finished and buffer should be flushed to display now
	if ( g_lui_main->disp_drv->render_complete_cb != NULL)
		 g_lui_main->disp_drv->render_complete_cb();

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
 * 							END
 *-------------------------------------------------------------------------------
 */

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

	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;

	if (!(obj->visible))
		return;
	
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t x_temp = obj->x;
	uint16_t y_temp = obj->y;

	const tImage *img = {0};
	uint16_t glyph_width = 0, glyph_height = 0;

	tFont *temp_font = lbl->font;
	// If label has no font set for it, check the global set font in scene
	// If no scene present, i.e., parent_scene_index == -1, return
	// If scene is there but scene has no font, return
	// So, if label has no parent scene, it must have its own font to be rendered
	if (temp_font == NULL &&  g_lui_main->active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}

	// Draw the label background color
	_lui_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);

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
			glyph_width = img->width;
			glyph_height = img->height;


			// check if not enough space available at the right side
			if (x_temp + glyph_width > obj->x + obj->common_style.width)
			{
				x_temp = obj->x;					//go to first col
				y_temp += glyph_height;	//go to next row

				// check if not enough space available at the bottom
				if(y_temp + glyph_height > obj->y + obj->common_style.height - 1)
					return;
			}

			_lui_draw_char(x_temp, y_temp, lbl->style.text_color, img);

			x_temp += glyph_width;		//next char position
		}
	}

	// Draw the chart border if needed
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(obj->x, obj->y, obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
	}

}

/*
 * Initialize a label with default values
 */
lui_obj_t* lui_label_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_label_t *initial_label =  _lui_mem_alloc(sizeof(*initial_label));
	
	initial_label->text = "";
	initial_label->font = NULL;
	initial_label->style.text_color = LUI_STYLE_LABEL_TEXT_COLOR;

	lui_obj_t *obj = _lui_object_create();
	// objeect type
	obj->obj_type = LUI_OBJ_LABEL;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_LABEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_LABEL_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_LABEL_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_LABEL_WIDTH;
	obj->common_style.height = LUI_STYLE_LABEL_HEIGHT;

	obj->obj_main_data = (void *)initial_label;

	return obj;
}

void lui_label_set_font(const tFont *font, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;
	
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	lbl->font = (tFont *)font;
	_lui_object_set_need_refresh(obj->parent);
}

void lui_label_set_text(const char *text, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;
	
	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	lbl->text = (char *)text;

	_lui_object_set_need_refresh(obj);
}

void lui_label_set_text_color(uint16_t text_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;

	lui_label_t *lbl = (lui_label_t *)(obj->obj_main_data);
	if (lbl->style.text_color == text_color)
		return;
	_lui_object_set_need_refresh(obj);
	lbl->style.text_color = text_color;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

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

	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t width = obj->common_style.width;
	uint16_t height = obj->common_style.height;
	uint16_t line_color = chart->style.line_color;
	uint16_t data_points = chart->data.points;

	double mapped_data[ g_lui_main->disp_drv->display_hor_res * 2];

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
	if (temp_font == NULL &&  g_lui_main->active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}


	// Draw the chart background
	_lui_draw_rect_fill(temp_x, temp_y, width, height, obj->common_style.bg_color);

	// Draw the scale numbers

	// Draw the chart grid if needed
	if (chart->style.grid_visible)
	{
		uint16_t hor_grid_spacing = height / (chart->grid.hor_count + 1);
		uint16_t vert_grid_spacing = width / (chart->grid.vert_count + 1);

		// Draw the vertical grids from left to right
		for (int i = 1; i <= chart->grid.vert_count; i++)
		{
			uint16_t temp_x_new = temp_x + (i * vert_grid_spacing);
			_lui_draw_line(temp_x_new, temp_y, temp_x_new, temp_y + height - 1, 1, chart->style.grid_color);
		}

		// Draw the horizontal grids from bottom to top
		uint16_t y_bottom = temp_y + height;
		for (int i = 1; i <= chart->grid.hor_count; i++)
		{
			uint16_t temp_y_new = y_bottom - (i * hor_grid_spacing);
			_lui_draw_line(temp_x, temp_y_new, temp_x + width - 1, temp_y_new, 1, chart->style.grid_color);
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
			 g_lui_main->disp_drv->draw_pixels_area_cb(current_x, current_y, 1, 1, line_color);
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
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(temp_x, temp_y, width, height, 1, obj->common_style.border_color);
	}
}

/*
 * Initialize a line chart with default values
 */
lui_obj_t* lui_linechart_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_chart_t *initial_line_chart =  _lui_mem_alloc(sizeof(*initial_line_chart));

	double tmp_data[] = {0};
	initial_line_chart->data.source = tmp_data;
	initial_line_chart->data.y_max_value = 0;
	initial_line_chart->data.y_min_value =  g_lui_main->disp_drv->display_vert_res;
	initial_line_chart->data.points = 0;
	initial_line_chart->data.auto_scale = 1;

	initial_line_chart->style.line_color = LUI_STYLE_LINECHART_LINE_COLOR;
	initial_line_chart->style.grid_color = LUI_STYLE_LINECHART_GRID_COLOR;
	initial_line_chart->style.grid_visible = LUI_STYLE_LINECHART_GRID_VISIBLE;
	initial_line_chart->grid.hor_count = 5;
	initial_line_chart->grid.vert_count = 5;
	initial_line_chart->font = NULL;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_LINECHART;
	// object common style
	obj->common_style.border_color = LUI_STYLE_LINECHART_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_LINECHART_BORDER_VISIBLE;
	obj->common_style.bg_color = LUI_STYLE_LINECHART_BG_COLOR;
	obj->common_style.height = LUI_STYLE_LINECHART_HEIGHT;
	obj->common_style.width = LUI_STYLE_LINECHART_WIDTH;
	
	obj->obj_main_data = (void *)initial_line_chart;

	return obj;
}

void lui_linechart_set_grid_count(uint16_t hor_lines, uint16_t vert_lines, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;

	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->grid.hor_count == hor_lines && chart->grid.vert_count == vert_lines)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->grid.hor_count = hor_lines;
	chart->grid.vert_count = vert_lines;
}

void lui_linechart_set_grid_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;

	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->style.grid_color == color)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_color = color;
}

void lui_linechart_set_grid_visible(uint8_t state, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->style.grid_visible == state)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_visible = state;
}

void lui_linechart_set_line_color(uint16_t line_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->style.line_color == line_color)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.line_color = line_color;
}

void lui_linechart_set_data_auto_scale(uint8_t auto_scale, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (auto_scale == chart->data.auto_scale)
		return;
	chart->data.auto_scale = auto_scale ? 1 : 0;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_data_range(double y_min, double y_max, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	if (chart->data.y_max_value == y_max && chart->data.y_min_value == y_min && chart->data.auto_scale == 0)
		return;
	_lui_object_set_need_refresh(obj);
	chart->data.y_max_value = y_max;
	chart->data.y_min_value = y_min;
	chart->data.auto_scale = 0;
}

void lui_linechart_set_data_source(double *source, uint16_t points, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t *chart = (lui_chart_t *)(obj->obj_main_data);
	// if (chart->data.points == points)
	// 	return;
	_lui_object_set_need_refresh(obj);
	chart->data.source = source;
	chart->data.points = points;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

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
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t btn_height = obj->common_style.height;
	uint16_t btn_width = obj->common_style.width;


	uint8_t str_width_height[2];

	// Draw the button's body color depending on its current state
	uint16_t btn_color = obj->common_style.bg_color;
	if (obj->state == LUI_STATE_SELECTED)
		btn_color = btn->style.selection_color;
	else if (obj->state == LUI_STATE_PRESSED)
		btn_color = btn->style.pressed_color;
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
	if (temp_font == NULL &&  g_lui_main->active_scene != NULL)
	{
		temp_font = _lui_get_font_from_active_scene();
	}

	_get_string_dimension(btn->label.text, temp_font, str_width_height);

	str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
	str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

	temp_x = temp_x + (btn_width - str_width_height[0]) / 2;
	temp_y = temp_y + (btn_height - str_width_height[1]) / 2;

	// lui_obj_t *lbl_obj =   _lui_mem_alloc(sizeof(*lbl_obj));
	// lui_label_t *btn_label =   _lui_mem_alloc(sizeof(*btn_label));

	lui_obj_t *lbl_obj =  lui_label_create();
	lui_label_t *btn_label =  lbl_obj->obj_main_data;

	lbl_obj->obj_type = LUI_OBJ_LABEL;
	lbl_obj->visible = 1;
	btn_label->text = btn->label.text;
	btn_label->style.text_color = btn->style.label_color;
	// bg_color depends on button's current state color
	if (obj->state == LUI_STATE_IDLE)
		lbl_obj->common_style.bg_color = obj->common_style.bg_color; // normal situation
	else if (obj->state == LUI_STATE_SELECTED)
		lbl_obj->common_style.bg_color = btn->style.selection_color;
	else if (obj->state == LUI_STATE_PRESSED)
		lbl_obj->common_style.bg_color = btn->style.pressed_color;
	lbl_obj->common_style.border_color = lbl_obj->common_style.bg_color;
	lbl_obj->x = temp_x;
	lbl_obj->y = temp_y;
	lbl_obj->common_style.width = str_width_height[0];
	lbl_obj->common_style.height = str_width_height[1];
	btn_label->font = temp_font;
	lbl_obj->obj_main_data = btn_label;
	lui_label_draw(lbl_obj);

	//lbl_obj = NULL;
	 _lui_mem_free(btn_label);
	 _lui_mem_free(lbl_obj);
	 g_lui_main->total_created_objects--;
	

	// Finally Draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(obj->x, obj->y, btn_width, btn_height, 1, obj->common_style.border_color);
	}
}

/*
 * Create a button with default variables
 */
lui_obj_t* lui_button_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_button_t *initial_button =  _lui_mem_alloc(sizeof(*initial_button));

	initial_button->style.pressed_color = LUI_STYLE_BUTTON_PRESSED_COLOR;
	initial_button->style.selection_color = LUI_STYLE_BUTTON_SELECTION_COLOR;
	
	initial_button->label.text = "";
	initial_button->style.label_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->label.font = NULL;

	initial_button->dpad_row_pos = -1;
	initial_button->dpad_col_pos = -1;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_BUTTON;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_BUTTON_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_BUTTON_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_BUTTON_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_BUTTON_WIDTH;
	obj->common_style.height = LUI_STYLE_BUTTON_HEIGHT;
	
	obj->obj_main_data = (void *)initial_button;

	return  obj;
}

void lui_button_set_label_text(const char *text, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	btn->label.text = (char *)text;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	if (btn->style.label_color == color)
		return;
	btn->style.label_color = color;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_font(const tFont *font, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	btn->label.font = (tFont *)font;
	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);
}

void lui_button_set_extra_colors(uint16_t pressed_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t *btn = (lui_button_t *)(obj->obj_main_data);
	if (btn->style.pressed_color == pressed_color && btn->style.selection_color == selection_color)
		return;
	btn->style.pressed_color = pressed_color;
	btn->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
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

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_LIST related functions
 *-------------------------------------------------------------------------------
 */

void lui_list_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	if (!(obj->visible))
		return;

	// draw the background first (hard coding the color for now)
	 g_lui_main->disp_drv->draw_pixels_area_cb(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);

	// draw the border if needed
	// Finally Draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(obj->x, obj->y,  obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
	}
}


lui_obj_t* lui_list_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_list_t *initial_list =  _lui_mem_alloc(sizeof(*initial_list));

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_LIST;
	obj->obj_main_data = (void *)initial_list;

	initial_list->page_count = 0;
	initial_list->current_page_index = 0;
	initial_list->selected_button_index = -1; // unused
	initial_list->buttons_per_page = 0;
	initial_list->button_item_min_height = 0;

	// set common styles for list object
	obj->common_style.bg_color = LUI_STYLE_LIST_ITEM_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_LIST_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_LIST_BORDER_VISIBLE;
	obj->common_style.height = LUI_STYLE_LIST_HEIGHT;
	obj->common_style.width = LUI_STYLE_LIST_WIDTH;

	// create navigation buttons
	_lui_list_add_nav_buttons(obj);

	initial_list->font = NULL;

	return obj;
}


void lui_list_prepare(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	// list has no item (button), so return
	if (obj->children_count == 0)
		return;

	lui_list_t *list = (lui_list_t *)(obj->obj_main_data);
	
	// set font to active scene's font if no font is given
	// is active scene's font is null too, return
	if (list->font == NULL)
		list->font = _lui_get_font_from_active_scene();
	if (list->font == NULL)
		return;

	// usable height depends on navigation buttons' existense
	// if no nav button (for single page list), total height is usable
	uint16_t list_usable_height = obj->common_style.height - 1;
	if (list->button_item_min_height < list->font->chars[0].image->height)
		list->button_item_min_height = list->font->chars[0].image->height;
	uint8_t button_height = list->button_item_min_height;

	// buttons_per_page is calculated excluding the nav butons. Only items are considered
	list->buttons_per_page = list_usable_height / button_height;
	
	// when item per page is heigher than total item count, we need more than 1 page
	// whenever more than 1 page is needed, nav buttons are added.
	// so, we calculate the usable_height and recalculate items per page accordingly.
	// we have 2 extra children for nav buttons, so subtracting 2 from children count
	if (list->buttons_per_page < obj->children_count - 2)
	{
		list_usable_height = (obj->common_style.height - 1) - button_height; // leaving some space for navigation buttons
		list->buttons_per_page = list_usable_height / button_height;
		
	}

	// if items per page is less than or equal to total items, we need only 1 page
	// so, set item heights in such way that they fill the entire height
	else
	{
		list->buttons_per_page = obj->children_count - 2;
		button_height = (obj->common_style.height - 1) / list->buttons_per_page;
	}
	
	// fast way to round up a/b: (a+b-1)/b
	list->page_count = ((obj->children_count - 2) +  list->buttons_per_page - 1) / list->buttons_per_page; // we have 2 extra children for nav buttons, so subtracting 2 from children count
	list->current_page_index = 0;

	
	// set x, y coordinates and height, width of all the list items.
	// list items are treated as children
	// since first two children are nav buttons, we start from 3rd child
	lui_obj_t *list_item = obj->first_child->next_sibling->next_sibling;
	uint8_t item_pos = 0; 
	uint16_t first_item_pos = (list->current_page_index * list->buttons_per_page);
	uint16_t last_item_pos = first_item_pos + list->buttons_per_page - 1;
	while (list_item != NULL)
	{
		lui_button_t *button_item = list_item->obj_main_data;
		list_item->x = obj->x + 1;
		// starting offset is set by item_pos and btn_per_page. Taking remainder of item_pos/btn_per_page so that 
		// everytime item_pos becomes big enough to come to next page, the offset becomes 0
		list_item->y = obj->y + ((item_pos % list->buttons_per_page) * button_height) + 1;
		list_item->common_style.width = obj->common_style.width - 2;
		list_item->common_style.height = button_height;
		button_item->label.font = list->font;

		// here, we're checking if the item is in current page. Only the it will be visible
		// this is done by checking if the index of item is within the range of current page
		if (item_pos >= first_item_pos && item_pos <= last_item_pos)
		{
			list_item->visible = 1;
		}
		else
		{
			list_item->visible = 0;
		}

		// go to next list item
		list_item = list_item->next_sibling;
		item_pos++;
	}

	// navigation button (prev and next) x,y, w, h set.
	lui_button_t *button_nav_prev = obj->first_child->obj_main_data;
	lui_button_t *button_nav_nxt = obj->first_child->next_sibling->obj_main_data;

	obj->first_child->next_sibling->x = obj->x + (obj->common_style.width / 2) + 10; // index 1 = nav button nxt
	obj->first_child->x = obj->x + 10;	// index 0 = nav button prev
	obj->first_child->next_sibling->y = obj->first_child->y = obj->y + list_usable_height;
	obj->first_child->next_sibling->common_style.width = obj->first_child->common_style.width = (obj->common_style.width / 2) - 20; // 4 is just margin
	obj->first_child->next_sibling->common_style.height = obj->first_child->common_style.height = button_height - 2;
	button_nav_nxt->label.font = button_nav_prev->label.font = list->font;
	
	// both nav buttons won't be rendered unless the list is multi page
	// if list page count is more than 0, draw the nav button
	if (list->page_count > 0)
	{
		// draw "next" or "prev" button only if there's a next ore previous page
			if (list->current_page_index + 1 < list->page_count)
				obj->first_child->next_sibling->visible = 1;
			else
				obj->first_child->next_sibling->visible = 0;

			if (list->current_page_index - 1 >= 0)
				obj->first_child->visible = 1;
			else
				obj->first_child->visible = 0;
	}

	// whenever prepare function is called, list (and all children of it) will be redrawn
	_lui_object_set_need_refresh(obj);
}


lui_obj_t* lui_list_add_item(const char *text, lui_obj_t *obj)
{
	if (obj == NULL)
		return NULL;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return NULL;

	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_list_t *list = obj->obj_main_data;
	list->button_item_min_height = LUI_STYLE_LIST_ITEM_MIN_HEIGHT;

	lui_button_t *initial_button =  _lui_mem_alloc(sizeof(*initial_button));
	initial_button->style.pressed_color = LUI_STYLE_LIST_ITEM_PRESSED_COLOR;
	initial_button->style.selection_color = LUI_STYLE_LIST_ITEM_SELECTION_COLOR;
	initial_button->label.text = (char*)text;
	initial_button->style.label_color = LUI_STYLE_LIST_ITEM_LABEL_COLOR;
	initial_button->label.font = list->font; // here it can be null too

	initial_button->dpad_row_pos = -1;
	initial_button->dpad_col_pos = -1;

	lui_obj_t *obj_btn_item = _lui_object_create();
	// object type
	obj_btn_item->obj_type = LUI_OBJ_BUTTON;
	// object common style
	obj_btn_item->common_style.bg_color = LUI_STYLE_LIST_ITEM_BG_COLOR;
	obj_btn_item->common_style.border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;
	obj_btn_item->common_style.border_visible = LUI_STYLE_LIST_ITEM_BORDER_VISIBLE;
	obj_btn_item->common_style.width = LUI_STYLE_LIST_WIDTH;
	obj_btn_item->common_style.height = LUI_STYLE_LIST_ITEM_MIN_HEIGHT;
	
	obj_btn_item->obj_main_data = (void *)initial_button;

	_lui_list_add_button_obj(obj_btn_item, obj);

	return  obj_btn_item;
}


void lui_list_delete_item(lui_obj_t **obj_item_addr)
{
	if (obj_item_addr == NULL)
		return;
	if (*obj_item_addr == NULL)
		return;
	
	if ((*obj_item_addr)->obj_type != LUI_OBJ_BUTTON)
		return;
	if ((*obj_item_addr)->parent == NULL)
		return;
	if ((*obj_item_addr)->parent->obj_type != LUI_OBJ_LIST)
		return;

	lui_object_remove_from_parent(*obj_item_addr);
	*obj_item_addr = NULL;
	 _lui_mem_free(*obj_item_addr);
	 g_lui_main->total_created_objects--;
}


void lui_list_set_item_min_height(uint8_t height, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_list_t *list = obj->obj_main_data;

	// if list has a font and the item height is >= the glyph's height, only then set that height
	if (list->font != NULL)
	{
		if (height >= list->font->chars[0].image->height)
		{
			list->button_item_min_height = height;
			_lui_object_set_need_refresh(obj);
		}
	}
	// if no font, then add whatever height is passed
	else
	{
		list->button_item_min_height = height;
		_lui_object_set_need_refresh(obj);
	}

	// this min_height is applied to all items in `prepare` funt
}


void lui_list_set_font(const tFont *font, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;
	
	lui_list_t *list = obj->obj_main_data;
	list->font = (tFont *)font;

	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);

	// this font is set to all items in `prepare` func
}


void lui_list_set_nav_btn_label_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_label_color(color, obj->first_child);
	lui_button_set_label_color(color, obj->first_child->next_sibling);
}


void lui_list_set_nav_btn_bg_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_object_set_bg_color(color, obj->first_child);
	lui_object_set_bg_color(color, obj->first_child->next_sibling);
}


void lui_list_set_nav_btn_extra_colors(uint16_t pressed_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_extra_colors(pressed_color, selection_color, obj->first_child);
	lui_button_set_extra_colors(pressed_color, selection_color, obj->first_child->next_sibling);
}


void lui_list_set_nav_btn_label_text(const char *btn_prev_text, const char *btn_nxt_text, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_label_text(btn_prev_text, obj->first_child);
	lui_button_set_label_text(btn_nxt_text, obj->first_child->next_sibling);
}


void lui_list_set_nav_btn_border_color(uint16_t color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_object_set_border_color(color, obj->first_child);
	lui_object_set_border_color(color, obj->first_child->next_sibling);
}


void _lui_list_add_button_obj(lui_obj_t *obj_btn, lui_obj_t *obj_list)
{
	if (obj_list->first_child == NULL)
    {
        obj_list->first_child = obj_btn;
    }
    else
    {
        lui_obj_t *next_child = obj_list->first_child;
        
        while (next_child->next_sibling != NULL)
        {
            next_child = next_child->next_sibling;
        }
        
        next_child->next_sibling = obj_btn;
    }

	// Common things to do
	obj_btn->parent = obj_list;
	obj_list->children_count++;
	_lui_object_set_need_refresh(obj_btn->parent);
}


void _lui_list_add_nav_buttons(lui_obj_t *obj)
{
	// create navigation buttons
	lui_obj_t *obj_nav_btn_prev = lui_button_create();
	lui_obj_t *obj_nav_btn_nxt = lui_button_create();
	lui_button_set_label_text("Prev", obj_nav_btn_prev);
	lui_button_set_label_text("Next", obj_nav_btn_nxt);
	// set nav button styles (nav button height and width are calculated y `prepare` function)
	lui_button_set_label_color(LUI_STYLE_LIST_NAV_LABEL_COLOR, obj_nav_btn_prev);
	lui_button_set_label_color(LUI_STYLE_LIST_NAV_LABEL_COLOR, obj_nav_btn_nxt);
	lui_button_set_extra_colors(LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR, obj_nav_btn_prev);
	lui_button_set_extra_colors(LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR, obj_nav_btn_nxt);
	obj_nav_btn_nxt->common_style.border_visible = obj_nav_btn_prev->common_style.border_visible = 0;
	obj_nav_btn_nxt->common_style.border_color = obj_nav_btn_prev->common_style.border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;
	obj_nav_btn_nxt->common_style.bg_color = obj_nav_btn_prev->common_style.bg_color = LUI_STYLE_LIST_NAV_BG_COLOR;
	obj_nav_btn_prev->obj_event_cb = obj_nav_btn_nxt->obj_event_cb = _lui_list_nav_btn_cb;

	_lui_list_add_button_obj(obj_nav_btn_prev, obj);
	_lui_list_add_button_obj(obj_nav_btn_nxt, obj);
}


void _lui_list_nav_btn_cb(lui_obj_t *obj)
{
	uint8_t event = lui_object_get_event(obj);
	lui_list_t *list = obj->parent->obj_main_data;
	if (event == LUI_EVENT_RELEASED)
	{
		if (obj == obj->parent->first_child)	// first child is nav_prev btn
		{
			if (list->current_page_index > 0)
			{
				list->current_page_index--;
				_lui_object_set_need_refresh(obj->parent);	
			}
		}
		else if (obj == obj->parent->first_child->next_sibling)	// 2nd child is nav_nxt button
		{
			if (list->current_page_index < list->page_count - 1)
			{
				list->current_page_index++;
				_lui_object_set_need_refresh(obj->parent);
			}
		}

		// list items start after first two buttons. First two buttons are nav buttons
		lui_obj_t *list_item = obj->parent->first_child->next_sibling->next_sibling;
		uint8_t item_pos = 0; 
		uint16_t first_item_pos = (list->current_page_index * list->buttons_per_page);
		uint16_t last_item_pos = first_item_pos + list->buttons_per_page - 1;
		while (list_item != NULL)
		{
			// here, we're checking if the item is in current page. Only the it will be visible
			// this is done by checking if the index of item is within the range of current page
			if (item_pos >= first_item_pos && item_pos <= last_item_pos)
			{
				list_item->visible = 1;
			}
			else
			{
				list_item->visible = 0;
			}

			// go to next list item
			list_item = list_item->next_sibling;
			item_pos++;
			
		}

		// if list page count is more than 0, draw the nav button
		if (list->page_count > 0)
		{
			// draw "next" or "prev" button only if there's a next ore previous page
			if (list->current_page_index + 1 < list->page_count)
				lui_object_set_visibility(1, obj->parent->first_child->next_sibling); // 2nd child: NEXT button
			else
				lui_object_set_visibility(0, obj->parent->first_child->next_sibling);

			if (list->current_page_index - 1 >= 0)
				lui_object_set_visibility(1, obj->parent->first_child);	// 1st child: PREV button
			else
				lui_object_set_visibility(0, obj->parent->first_child);
		}
	}
}
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_SWITCH related functions
 *-------------------------------------------------------------------------------
 */

void lui_switch_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return;

	if (!(obj->visible))
		return;
	
	lui_switch_t *swtch = (lui_switch_t *)(obj->obj_main_data);
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t temp_height = obj->common_style.height;
	uint16_t temp_width = obj->common_style.width;

	uint16_t swtch_color;	
	if (obj->value == 1)
	{
		swtch_color = swtch->style.knob_on_color;
	}
	else
	{
		swtch_color = swtch->style.knob_off_color;
	}
	if (obj->state == LUI_STATE_SELECTED || obj->state == LUI_STATE_PRESSED)
	{
		swtch_color = swtch->style.selection_color;
	}
	
	
	_lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, obj->common_style.bg_color);	// switch bg (color is constant regardless the state)
	if (obj->common_style.border_visible == 1)
		_lui_draw_rect(temp_x, temp_y, temp_width, temp_height, 1, obj->common_style.border_color);	// switch border

	temp_width = (float)temp_width * 0.3;
	temp_height = (float)temp_height * 0.6;
	temp_x = temp_x + ((obj->common_style.width / 2) - temp_width) / 2;
	if (obj->value == 1)
		temp_x += (obj->common_style.width / 2);
	temp_y = temp_y + (obj->common_style.height - temp_height) / 2;

	_lui_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, swtch_color);// switch slider
}

lui_obj_t* lui_switch_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_switch_t *initial_switch =  _lui_mem_alloc(sizeof(*initial_switch));
	
	initial_switch->style.knob_off_color = LUI_STYLE_SWITCH_KNOB_OFF_COLOR;
	initial_switch->style.knob_on_color = LUI_STYLE_SWITCH_KNOB_ON_COLOR;
	initial_switch->style.selection_color = LUI_STYLE_SWITCH_SELECTION_COLOR;
	initial_switch->dpad_row_pos = -1;
	initial_switch->dpad_col_pos = -1;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_SWITCH;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_SWITCH_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_SWITCH_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_SWITCH_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_SWITCH_WIDTH;
	obj->common_style.height = LUI_STYLE_SWITCH_HEIGHT;
	
	obj->obj_main_data = (void *)initial_switch;

	return  obj;
}

void lui_switch_set_extra_colors(uint16_t knob_off_color, uint16_t knob_on_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return;
	
	lui_switch_t *swtch = (lui_switch_t *)(obj->obj_main_data);

	if (swtch->style.knob_off_color == knob_off_color && swtch->style.knob_on_color == knob_on_color && swtch->style.selection_color == selection_color)
		return;
	swtch->style.knob_off_color = knob_off_color;
	swtch->style.knob_on_color = knob_on_color;
	swtch->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_switch_get_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return -1;
	
	return obj->value;
}

void lui_switch_set_value(uint8_t value, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return;
	
	if (value > 1)
		value = 1;
	obj->value = value;
	_lui_object_set_need_refresh(obj);
}

void lui_switch_set_on(lui_obj_t *obj)
{
	lui_switch_set_value(1, obj);
}

void lui_switch_set_off(lui_obj_t *obj)
{
	lui_switch_set_value(0, obj);
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_CHECKBOX related functions
 *-------------------------------------------------------------------------------
 */

void lui_checkbox_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return;

	if (!(obj->visible))
		return;
	
	lui_checkbox_t *chkbox = obj->obj_main_data;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t bg_color;
	if (obj->value == 1)
	{
		bg_color = chkbox->style.bg_checked_color;
	}
	else
	{
		bg_color = obj->common_style.bg_color;
	}
	if (obj->state == LUI_STATE_SELECTED || obj->state == LUI_STATE_PRESSED)
	{
		bg_color = chkbox->style.selection_color;
	}

	_lui_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.width, bg_color);
	// draw the tick mark if needed
	if (obj->value == 1)
	{
		uint16_t point_1_x = obj->x + (obj->common_style.width * .2), point_1_y = obj->y + (obj->common_style.width * .55);
		uint16_t point_2_x = obj->x + (obj->common_style.width * .4), point_2_y = obj->y + (obj->common_style.width * .75);
		uint16_t point_3_x = obj->x + (obj->common_style.width * .75), point_3_y = obj->y + (obj->common_style.width * .3);
		
		_lui_draw_line(point_1_x, point_1_y, point_2_x, point_2_y, 2, chkbox->style.tick_color);
		_lui_draw_line(point_2_x, point_2_y, point_3_x, point_3_y, 2, chkbox->style.tick_color);
	}

	// draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(obj->x, obj->y,  obj->common_style.width,  obj->common_style.width, 1, obj->common_style.border_color);
	}
	
}

lui_obj_t* lui_checkbox_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_checkbox_t *initial_chkbox =  _lui_mem_alloc(sizeof(*initial_chkbox));
	
	initial_chkbox->style.bg_checked_color = LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR;
	initial_chkbox->style.selection_color = LUI_STYLE_CHECKBOX_SELECTION_COLOR;
	initial_chkbox->style.tick_color = LUI_STYLE_CHECKBOX_TICK_COLOR;
	initial_chkbox->dpad_row_pos = -1;
	initial_chkbox->dpad_col_pos = -1;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_CHECKBOX;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_CHECKBOX_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_CHECKBOX_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_CHECKBOX_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_CHECKBOX_WIDTH;
	obj->common_style.height = LUI_STYLE_CHECKBOX_HEIGHT;
	
	obj->obj_main_data = (void *)initial_chkbox;

	return  obj;
}

void lui_checkbox_set_extra_colors(uint16_t bg_checked_color, uint16_t tick_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return;
	
	lui_checkbox_t *chkbox = obj->obj_main_data;

	if (chkbox->style.bg_checked_color == bg_checked_color && chkbox->style.tick_color == tick_color && chkbox->style.selection_color == selection_color)
		return;
	chkbox->style.bg_checked_color = bg_checked_color;
	chkbox->style.tick_color = tick_color;
	chkbox->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_checkbox_get_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return -1;
	
	return obj->value;
}

void lui_checkbox_set_value(uint8_t value, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return;
	
	if (value > 1)
		value = 1;
	obj->value = value;
	_lui_object_set_need_refresh(obj);
}

void lui_switch_set_checked(lui_obj_t *obj)
{
	lui_checkbox_set_value(1, obj);
}

void lui_switch_set_unchecked(lui_obj_t *obj)
{
	lui_checkbox_set_value(0, obj);
}


/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_SLIDER related functions
 *-------------------------------------------------------------------------------
 */

void lui_slider_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;

	if (!(obj->visible))
		return;
	
	lui_slider_t *slider = obj->obj_main_data;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t knob_color = slider->style.knob_color;
	if (/*obj->state == LUI_STATE_SELECTED ||*/ obj->state == LUI_STATE_PRESSED)
	{
		knob_color = slider->style.selection_color;
	}

	// draw the filled region (left) first
	_lui_draw_rect_fill(obj->x, obj->y, slider->knob_center_rel_x, obj->common_style.height, slider->style.bg_filled_color);
	// draw the remaining region (right) 
	_lui_draw_rect_fill(obj->x + slider->knob_center_rel_x, obj->y, obj->common_style.width - slider->knob_center_rel_x, obj->common_style.height, obj->common_style.bg_color);
	// draw the knob
	_lui_draw_rect_fill(obj->x + slider->knob_center_rel_x - (slider->style.knob_width / 2), obj->y, slider->style.knob_width, obj->common_style.height, knob_color);

	// draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		_lui_draw_rect(obj->x, obj->y,  obj->common_style.width,  obj->common_style.height, 1, obj->common_style.border_color);
	}
}

lui_obj_t* lui_slider_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_slider_t *initial_slider =  _lui_mem_alloc(sizeof(*initial_slider));
	
	initial_slider->style.bg_filled_color = LUI_STYLE_SLIDER_BG_FILLED_COLOR;
	initial_slider->style.knob_color = LUI_STYLE_SLIDER_KNOB_COLOR;
	initial_slider->style.selection_color = LUI_STYLE_SLIDER_SELECTION_COLOR;
	initial_slider->style.knob_width = LUI_STYLE_SLIDER_KNOB_WIDTH;
	initial_slider->range_min = 0;
	initial_slider->range_max = 100;
	initial_slider->knob_center_rel_x = (LUI_STYLE_SLIDER_KNOB_WIDTH / 2);
	initial_slider->dpad_row_pos = -1;
	initial_slider->dpad_col_pos = -1;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_SLIDER;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_SLIDER_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_SLIDER_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_SLIDER_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_SLIDER_WIDTH;
	obj->common_style.height = LUI_STYLE_SLIDER_HEIGHT;

	obj->obj_main_data = (void *)initial_slider;

	return  obj;
}


void lui_slider_set_extra_colors(uint16_t knob_color, uint16_t bg_filled_color, uint16_t selection_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t *slider = obj->obj_main_data;

	if (slider->style.knob_color == knob_color && slider->style.bg_filled_color == bg_filled_color && slider->style.selection_color == selection_color)
		return;
	slider->style.knob_color = knob_color;
	slider->style.bg_filled_color = bg_filled_color;
	slider->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_value(int16_t value, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t *slider = obj->obj_main_data;
	
	
	if (value == obj->value)
		return;

	if (value > slider->range_max)
	{
		obj->value = slider->range_max;
	}
	else if (value < slider->range_min)
	{
		obj->value = slider->range_min;
	}
	else
	{
		obj->value = value;
	}
	

	// calculate knob's center x position relative to the slider, when value of slider is manually set by user (y is always same)
	slider->knob_center_rel_x = _lui_map_range(obj->value, slider->range_max, slider->range_min, obj->common_style.width - (slider->style.knob_width / 2), (slider->style.knob_width / 2));
	
	_lui_object_set_need_refresh(obj);
}
void lui_slider_set_range(int16_t range_min, int16_t range_max, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t *slider = obj->obj_main_data;

	if (range_min == slider->range_min && range_max == slider->range_max)
		return;
	
	slider->range_max = range_max;
	slider->range_min = range_min;

	// limiting within max and min
	obj->value = obj->value > range_max ? range_max : (obj->value < range_min ? range_min : obj->value);

	// calculate knob's center x position relative to the slider, when value of slider is manually set by user (y is always same)
	slider->knob_center_rel_x = _lui_map_range(obj->value, slider->range_max, slider->range_min, obj->common_style.width - (slider->style.knob_width / 2), (slider->style.knob_width / 2));

	_lui_object_set_need_refresh(obj);
}


int16_t lui_slider_get_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	
	return obj->value;
}

int16_t lui_slider_get_min_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;

	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	lui_slider_t *slider = obj->obj_main_data;

	return slider->range_min;
}

int16_t lui_slider_get_max_value(lui_obj_t *obj)
{
	if (obj == NULL)
		return -1;

	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	lui_slider_t *slider = obj->obj_main_data;
	
	return slider->range_max;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_PANEL related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Initialize a label with default values
 */
lui_obj_t* lui_panel_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_PANEL;
	// object comon style
	obj->common_style.bg_color = LUI_STYLE_PANEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_PANEL_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_PANEL_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_PANEL_WIDTH;
	obj->common_style.height = LUI_STYLE_PANEL_HEIGHT;
	obj->obj_main_data = (void *)NULL;	// will add panel specific main data later

	return obj;
}

void lui_panel_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	if (!(obj->visible))
		return;
	
	// for panel, just draw background and optional border
	 g_lui_main->disp_drv->draw_pixels_area_cb(obj->x, obj->y, obj->common_style.width,  obj->common_style.height, obj->common_style.bg_color);
	if (obj->common_style.border_visible == 1)
		_lui_draw_rect(obj->x, obj->y, obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
}
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

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
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_scene_t *initial_scene =  _lui_mem_alloc(sizeof(*initial_scene));

	initial_scene->font = NULL;
	initial_scene->bg_image = NULL;
	initial_scene->obj_popup = NULL;

	lui_obj_t *obj = _lui_object_create();
	// object type
	obj->obj_type = LUI_OBJ_SCENE;
	// object common style
	obj->common_style.width =  g_lui_main->disp_drv->display_hor_res;
	obj->common_style.height =  g_lui_main->disp_drv->display_vert_res;
	obj->common_style.bg_color = LUI_STYLE_SCENE_BG_COLOR;	
	
	//obj->index =  g_lui_main->total_scenes;
	obj->obj_main_data = (void *)initial_scene;

	_lui_object_set_need_refresh(obj);
	 //g_lui_main->scenes[obj->index] = obj;
	 g_lui_main->total_scenes++;

	return obj;
}

void lui_scene_draw(lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_SCENE)
		return;

	if (!(obj->visible))
		return;

	_lui_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);
	// TBD: draw background image
}

void lui_scene_set_bg_image(const tImage *image, lui_obj_t *obj_scene)
{
	// NOTE: image rendering is not implemented yet
	if (obj_scene == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	scene->bg_image = (tImage *)image;

	_lui_object_set_need_refresh(obj_scene); 
}

void lui_scene_set_font(const tFont *font, lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	scene->font = (tFont *)font;
	
	_lui_object_set_need_refresh(obj_scene); 
}

void lui_scene_set_popup(lui_obj_t *obj, lui_obj_t *obj_scene)
{
	if (obj_scene == NULL || obj == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	// object can only be used as popup if it has no parent. Because, here it'll be added to this scene
	if (obj->parent != NULL)
		return;

	lui_object_add_to_parent(obj, obj_scene);

	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	scene->obj_popup = obj;

	// when setting a popup, reset the current active object. because now only popup will get input
	if ( g_lui_main->active_obj != NULL)
	{
		 g_lui_main->active_obj->state = LUI_STATE_IDLE;
		 g_lui_main->active_obj = NULL;
	}
	

	_lui_object_set_need_refresh(obj); 
}

void lui_scene_unset_popup(lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;

	lui_scene_t *scene = (lui_scene_t *)(obj_scene->obj_main_data);
	if (scene->obj_popup != NULL)
	{
		lui_object_remove_from_parent(scene->obj_popup);
		scene->obj_popup = NULL;
	}
	
}

void lui_scene_set_active(lui_obj_t *obj_scene)
{
	if (obj_scene == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	 g_lui_main->active_scene = obj_scene;
	_lui_object_set_need_refresh(obj_scene);
}

lui_obj_t* lui_scene_get_active()
{
	return  g_lui_main->active_scene;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_OBJECT (generic) functions
 *-------------------------------------------------------------------------------
 */

lui_obj_t* _lui_object_create()
{
	lui_obj_t *obj =  _lui_mem_alloc(sizeof(*obj));

	obj->x = 0;
	obj->y = 0;
	obj->state = LUI_STATE_IDLE;
	obj->event = LUI_EVENT_NONE;
	obj->value = 0;
	obj->obj_event_cb = NULL;
	obj->needs_refresh = 1;
	obj->visible = 1;
	//obj->index_in_pool = -1;
	obj->parent = NULL;
	obj->first_child = NULL;
	obj->next_sibling = NULL;
	obj->children_count = 0;

	return obj;
}

void lui_object_add_to_parent(lui_obj_t *obj_child, lui_obj_t *obj_parent)
{
	if (obj_child == NULL || obj_parent == NULL)
		return;
	// scene cannot be added to any parent, so return
	if (obj_child->obj_type == LUI_OBJ_SCENE)
		return;
	// only panel and scene can be parent, otherwise return
	if (obj_parent->obj_type != LUI_OBJ_PANEL && obj_parent->obj_type != LUI_OBJ_SCENE)
		return;

    //add the ui element with a new index to scene only if no parent already exists
    if (obj_child->parent != NULL)
		return;
	

	if (obj_parent->first_child == NULL)
    {
        obj_parent->first_child = obj_child;
    }
    else
    {
        lui_obj_t *next_child = obj_parent->first_child;
        
        while (next_child->next_sibling != NULL)
        {
            next_child = next_child->next_sibling;
        }
        
        next_child->next_sibling = obj_child;
    }
    obj_child->parent = obj_parent;
	obj_parent->children_count++;
	_lui_object_set_need_refresh(obj_child);
}

void lui_object_remove_from_parent(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	// If item's or parent's index is -1, return
	if (obj->parent == NULL)
    	return;

	// if object is the head (first child)
    if (obj == obj->parent->first_child)
    {
        obj->parent->first_child = obj->next_sibling;
    }
    else
    {
        lui_obj_t *child = obj->parent->first_child;
        while (child->next_sibling != obj)
        {
            child = child->next_sibling;
        }
        child->next_sibling = obj->next_sibling;
    }

	// common things to do
	obj->parent->children_count--; 
    obj->next_sibling = NULL;
	_lui_object_set_need_refresh(obj->parent);
	obj->parent = NULL;
}


void lui_object_set_position(uint16_t x, uint16_t y, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->obj_type == LUI_OBJ_SCENE)
		return;
	if (obj->x == x && obj->y == y)
		return;	

	uint16_t obj_old_x = obj->x;
	uint16_t obj_old_y = obj->y;

	if (obj->parent != NULL)
	{
		obj->x = obj->parent->x + x;
		obj->y = obj->parent->y + y;
	}
	else
	{
		obj->x = x;
		obj->y = y;
	}
	
	lui_obj_t *child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
        uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 

			child->x = child->x + (obj->x - obj_old_x); // offset the child (currewnt obj) based on parent
			child->y = child->y + (obj->y - obj_old_y);

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}

		}
	}

	// object's position is changed, so parent must be redrawn
	_lui_object_set_need_refresh(obj->parent);
}

void lui_object_set_x_pos(uint16_t x, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_object_set_position(x, obj->y, obj);
}

void lui_object_set_y_pos(uint16_t y, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	lui_object_set_position(obj->x, y, obj);
}

void lui_object_set_area(uint16_t width, uint16_t height, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	if (obj->common_style.width == width && obj->common_style.height == height)
		return;

	if (obj->common_style.width < width && obj->common_style.height < height)
		_lui_object_set_need_refresh(obj);		
	else	
		_lui_object_set_need_refresh(obj->parent);

		
	obj->common_style.width = width;
	obj->common_style.height = height;	

}

void lui_object_set_width(uint16_t width, lui_obj_t *obj)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(width, obj->common_style.height, obj);
}

void lui_object_set_height(uint16_t height, lui_obj_t *obj)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(obj->common_style.width, height, obj);
}

void lui_object_set_border_color(uint16_t border_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_color == border_color)
		return;
	obj->common_style.border_color = border_color;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_border_visibility(uint8_t is_visible, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_visible == is_visible)
		return;
	obj->common_style.border_visible = (is_visible == 0) ? 0 : 1;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_bg_color(uint16_t bg_color, lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	if (obj->common_style.bg_color == bg_color)
		return;
	obj->common_style.bg_color = bg_color;
	_lui_object_set_need_refresh(obj);
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

void lui_object_set_visibility(uint8_t visible, lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// already flag is same as `visible`, no need to waste time in loop. Return.
	if (obj->visible == visible)
		return;

	obj->visible = visible;

	lui_obj_t *child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
        uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 
			child->visible = visible;

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}
		}
	}
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * 			INPUT processing functions
 *------------------------------------------------------------------------------
 */

uint8_t _lui_check_if_active_obj(lui_touch_input_data_t input, lui_obj_t *obj)
{
	if (input.x >= obj->x && 
		input.x < obj->x + obj->common_style.width &&
		input.y >= obj->y &&
		input.y < obj->y + obj->common_style.height)
	{
		 g_lui_main->active_obj = obj;
		return 1;
	}
	else
	{
		// in case input is not on "obj" and previous "active_obj" is same as "obj",
		// set "input_on_obj" to NULL.
		if ( g_lui_main->active_obj == obj)
			 g_lui_main->active_obj = NULL;
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
		{
			new_state = LUI_STATE_PRESSED;
		}	
		// else not pressed, state = SELECTED
		else
		{
			new_state = LUI_STATE_SELECTED;
		}
	}
	obj->event = _lui_get_event_against_state(new_state, obj->state);		

	if (obj->event != LUI_EVENT_NONE)
	{
		obj->state = new_state;
		_lui_object_set_need_refresh(obj);
	}
	

	
	// Special case for switch and checkbox: if event is LUI_EVENT_PRESSED, then set event to LUI_EVENT_VALUE_CHANGED
	// then set the value to `value` property
	if (obj->obj_type == LUI_OBJ_SWITCH ||
		obj->obj_type == LUI_OBJ_CHECKBOX)
	{	
		if (obj->event == LUI_EVENT_RELEASED)
		{
			obj->event = LUI_EVENT_VALUE_CHANGED;	// for switch and checkbox, being pressed means being toggled, thus value changed
			obj->value = (obj->value == 1) ? 0 : 1;	// toggle the value (1->0 or 0-1)
			obj->needs_refresh = 1;
		}	
	}

	// Special case for slider: If knob is kep pressed and if input pos is not same as knob's current position,
	// set new position to knob  and value to slider, also set VALUE_CHANGED event
	else if (obj->obj_type == LUI_OBJ_SLIDER)
	{
		lui_slider_t *slider = obj->obj_main_data;
		if (obj->state == LUI_STATE_PRESSED &&
			input.x != (obj->x + slider->knob_center_rel_x))
		{
			uint16_t max_knob_center_actual_x = obj->x + obj->common_style.width - (slider->style.knob_width / 2);
			uint16_t min_knob_center_actual_x = obj->x + (slider->style.knob_width / 2);

			// cap to minimum/maximum allowed position to prevent the knob from going out of boundary
			if (input.x > max_knob_center_actual_x)
			{
				slider->knob_center_rel_x = max_knob_center_actual_x - obj->x;
			}
			else if (input.x < min_knob_center_actual_x)
			{
				slider->knob_center_rel_x = min_knob_center_actual_x - obj->x;
			}
			else
			{
				slider->knob_center_rel_x = input.x - obj->x;
			}
			
			obj->value = _lui_map_range(slider->knob_center_rel_x, obj->common_style.width - (slider->style.knob_width / 2), (slider->style.knob_width / 2), slider->range_max, slider->range_min);
			obj->event = LUI_EVENT_VALUE_CHANGED;
			obj->needs_refresh = 1;
		}
	}
}

lui_obj_t* _lui_process_touch_input_of_act_scene()
{	
	uint8_t scan_all_objs_flag = 0;
	lui_obj_t *obj_caused_cb = NULL;
	lui_obj_t *last_active_obj =  g_lui_main->active_obj;
	lui_scene_t *scene_main_data = (lui_scene_t *)( g_lui_main->active_scene->obj_main_data);
	lui_touch_input_data_t input_data;
	 g_lui_main->touch_input_dev->read_touch_input_cb(&input_data);

	if (last_active_obj == NULL)
	{
		scan_all_objs_flag = 1;
	}

	else
	{
		// sets object parameters based on input. also may modify  g_lui_main->active_obj
		_lui_set_obj_props_on_input(input_data, last_active_obj);
		if (last_active_obj->event != LUI_EVENT_NONE)
		{
			if ( g_lui_main->active_obj != last_active_obj /* *state == LUI_STATE_IDLE*/)
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
		// if popup exists, only scan input for popup (and its children)
		if (scene_main_data->obj_popup != NULL)
			obj_caused_cb = _lui_scan_all_obj_for_input(input_data, scene_main_data->obj_popup, last_active_obj);
		// else scan is for active scene and its children
		else
			obj_caused_cb = _lui_scan_all_obj_for_input(input_data,  g_lui_main->active_scene, last_active_obj);
		
		if (obj_caused_cb == NULL)
			obj_caused_cb = last_active_obj;
	}
	
	return obj_caused_cb;

}

lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t input_data, lui_obj_t *obj_root, lui_obj_t *obj_excluded)
{
	// Note: This function is made by converting a tail-recursive function to iterative function
	// The simple way is to use a stack.
	// see the answer: https://codereview.stackexchange.com/a/163621
	lui_obj_t *obj_caused_cb = NULL;

	obj_caused_cb = _lui_scan_individual_object_for_input(input_data, obj_root, obj_excluded);
	if (obj_caused_cb != NULL)
		return obj_caused_cb;

	lui_obj_t *child_of_root = obj_root->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
        uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 

			obj_caused_cb = _lui_scan_individual_object_for_input(input_data, child, obj_excluded);
			if (obj_caused_cb != NULL)
				return obj_caused_cb;

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}

		}
	}

	return obj_caused_cb;
}

lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t input_data, lui_obj_t *obj, lui_obj_t *obj_excluded)
{
	lui_obj_t *obj_caused_cb = NULL;
	// if current object is == last_active_object, continue. because, last_active_object is already processed
	if (obj == obj_excluded)
		return NULL;

	// TODO: if invisible, still proceed. But, return NULL if disabled.
	// So, add a `disabled` property in object. 
	if (!(obj->visible))
		return NULL;

	// we are only interested in objects that take input, like button, switch
	if (obj->obj_type == LUI_OBJ_BUTTON ||
		obj->obj_type == LUI_OBJ_SWITCH ||
		obj->obj_type == LUI_OBJ_CHECKBOX ||
		obj->obj_type == LUI_OBJ_SLIDER)
	{
		// sets object parameters based on input. also, may modify  g_lui_main->active_obj
		_lui_set_obj_props_on_input(input_data, obj);
		if (obj->event != LUI_EVENT_NONE)
		{
			obj_caused_cb = obj;
		}
		else
		{
			obj_caused_cb = NULL;
		}
	}

	return obj_caused_cb;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
 * 			DISPLAY DRIVER Callback Functions and Display Properties
 *------------------------------------------------------------------------------
 */

lui_dispdrv_t* lui_dispdrv_create()
{
	lui_dispdrv_t *initial_disp_drv =  _lui_mem_alloc(sizeof(*initial_disp_drv));

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
	 g_lui_main->disp_drv = dispdrv;
}

void* lui_dispdrv_destroy(lui_dispdrv_t *dispdrv)
{
	if (dispdrv != NULL)
		 _lui_mem_free(dispdrv);
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

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * 			INPUT DEVICE Callback Functions and Input properties
 *------------------------------------------------------------------------------
 */

lui_touch_input_dev_t* lui_touch_inputdev_create()
{
	lui_touch_input_dev_t *initial_touch_inputdev =  _lui_mem_alloc(sizeof(*initial_touch_inputdev));

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
	 g_lui_main->touch_input_dev = inputdev;
}

void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(lui_touch_input_data_t *inputdata), lui_touch_input_dev_t *inputdev)
{
	if (inputdev == NULL)
		return;
	inputdev->read_touch_input_cb = read_touch_input_cb;
}

lui_dpad_input_dev_t* lui_dpad_inputdev_create()
{
	lui_dpad_input_dev_t *initial_dpad_inputdev =  _lui_mem_alloc(sizeof(*initial_dpad_inputdev));

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
	 g_lui_main->dpad_input_dev = inputdev;
}

void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(lui_dpad_input_data_t *inputdata), lui_dpad_input_dev_t *inputdev)
{
	if (inputdev == NULL)
		return;
	inputdev->read_dpad_input_cb = read_dpad_input_cb;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * 			These functions are HELPER FUNCTIONS
 *------------------------------------------------------------------------------
 */

void _lui_object_render_parent_with_children(lui_obj_t *obj_parent)
{
	if (obj_parent == NULL)
		return;

	// first render the parent, then render all its children in a loop
	_lui_object_render(obj_parent);

	lui_obj_t *child_of_root = obj_parent->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 

			_lui_object_render(child);

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}
		}
	}
}

void _lui_object_render(lui_obj_t *obj)
{
	if (obj == NULL)
		return;
	
	// draw it only if it needs refresh
	if (obj->needs_refresh == 1)
	{
		switch (obj->obj_type)
		{
			case LUI_OBJ_SCENE:
				lui_scene_draw(obj);
				break;
			case LUI_OBJ_PANEL:
				lui_panel_draw(obj);
				break;
			case LUI_OBJ_BUTTON:
				lui_button_draw(obj);
				break;
			case LUI_OBJ_SWITCH:
				lui_switch_draw(obj);
				break;
			case LUI_OBJ_CHECKBOX:
				lui_checkbox_draw(obj);
				break;
			case LUI_OBJ_SLIDER:
				lui_slider_draw(obj);
				break;
			case LUI_OBJ_LABEL:
				lui_label_draw(obj);
				break;
			case LUI_OBJ_LINECHART:
				lui_linechart_draw(obj);
				break;
			case LUI_OBJ_LIST:
				lui_list_draw(obj);
				break;
			default:
				break;
		}

		obj->needs_refresh = 0;	// drawing is done, so set need_refresh back to 0
	}
}

void _lui_object_set_need_refresh(lui_obj_t *obj)
{
	if (obj == NULL)
		return;

	// already flag is 1, no need to waste time in loop. Return.
	if (obj->needs_refresh == 1)
		return;

	obj->needs_refresh = 1;

	lui_obj_t *child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t *obj_stack[LUI_MAX_OBJECTS] = {NULL};
        uint8_t stack_counter = 0;
        obj_stack[stack_counter++] = child_of_root;
        child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t *child = obj_stack[--stack_counter]; 
			child->needs_refresh = 1;

			// get the child of current object
            child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack
				obj_stack[stack_counter++] = child; 
				// get sibling of the child
                child = child->next_sibling;
			}
		}
	}
}


tFont* _lui_get_font_from_active_scene()
{
	if ( g_lui_main->active_scene == NULL)
		return NULL;
	lui_scene_t *act_scene = (lui_scene_t *)( g_lui_main->active_scene->obj_main_data);
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
				// g_lui_main->disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, back_color);
			}

			//if pixel is not blank
			else
			{
				// Draw the foreground color
				 g_lui_main->disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, fore_color);
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
		 g_lui_main->disp_drv->draw_pixels_area_cb(x0, (y0 < y1 ? y0 : y1), (uint16_t)line_width, (uint16_t)abs(y1 - y0 + 1), color);
	}
	else if (y0 == y1)		//horizontal line
	{
		 g_lui_main->disp_drv->draw_pixels_area_cb((x0 < x1 ? x0 : x1), y0, (uint16_t)abs(x1 - x0 + 1), (uint16_t)line_width, color);
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
		 g_lui_main->disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);

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
		 g_lui_main->disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);

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
	uint16_t x_new = x+w-1;
	uint16_t y_new = y+h-1;
	_lui_draw_line(x, y, x_new, y, line_width, color);
	_lui_draw_line(x_new, y, x_new, y_new, line_width, color);
	_lui_draw_line(x, y_new, x_new, y_new, line_width, color);
	_lui_draw_line(x, y, x, y_new, line_width, color);
}

/*
 * Fill a rectangular area with a color
 */
void _lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	 g_lui_main->disp_drv->draw_pixels_area_cb(x, y, w, h, color);
}

/*
 * Map a range of data to a new range of data
 */
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min)
{
    double new_val = ((((old_val - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min);
    return new_val;
}

/*
 * Check if display driver is 
 * registered by the stupid user  
 */
uint8_t _lui_disp_drv_check()
{
	uint8_t status = 0;
	// if no display driver is registered, return
	if ( g_lui_main->disp_drv == NULL)
		status = 0;
	// If no callback function (for drawing) is provided by user, return
	else if ( g_lui_main->disp_drv->draw_pixels_area_cb == NULL)
		status = 0;
	else
		status = 1;

	return status;
}

uint16_t lui_rgb(uint16_t red, uint16_t green, uint16_t blue)
{
	return _LUI_RGB(red, green, blue);
}

void _lui_mem_init()
{
    g_mem_block.mem_start = &(g_mem_block.mem_block[0]);
    g_mem_block.mem_end = g_mem_block.mem_start + LUI_MEM_MAX_SIZE;
    g_mem_block.mem_chunk_count = 0;
    g_mem_block.mem_allocated = 0;
    
    _lui_mem_chunk_t *first_chunk = (_lui_mem_chunk_t *)(g_mem_block.mem_start);
    first_chunk->next_chunk = NULL;
    first_chunk->prev_chunk = NULL;
    first_chunk->alloc_size = 0;
    first_chunk->alloc_status = LUI_MEM_CHUNK_STATUS_FREE;
}

void* _lui_mem_alloc(uint16_t element_size)
{
    _lui_mem_chunk_t *chunk_metadata = (_lui_mem_chunk_t *)(g_mem_block.mem_start);
    
    
    uint16_t chunk_metadata_size = sizeof(_lui_mem_chunk_t);
    uint8_t chunk_type =  LUI_MEM_CHUNK_TYPE_NONE;
    
    if ((element_size + chunk_metadata_size) > ( LUI_MEM_MAX_SIZE - (g_mem_block.mem_allocated + g_mem_block.mem_chunk_count * chunk_metadata_size)))
    {
        return NULL;
    }
    while (g_mem_block.mem_end > (uint8_t *)chunk_metadata + element_size + chunk_metadata_size)
    {
        if (chunk_metadata->alloc_status ==  LUI_MEM_CHUNK_STATUS_FREE)
        {
            if (chunk_metadata->alloc_size == 0 ||
                chunk_metadata->alloc_size > (element_size + chunk_metadata_size))
            {
                chunk_type =  LUI_MEM_CHUNK_TYPE_NEW;
                break;
            }
            else if (chunk_metadata->alloc_size == (element_size + chunk_metadata_size))
            {
                chunk_type =  LUI_MEM_CHUNK_TYPE_REUSE;
                break;
            }
        }
        chunk_metadata = (_lui_mem_chunk_t *)((uint8_t *)chunk_metadata + chunk_metadata->alloc_size);
    }
    
    if (chunk_type !=  LUI_MEM_CHUNK_TYPE_NONE)
    {
        chunk_metadata->alloc_status =  LUI_MEM_CHUNK_STATUS_USED;
        if (chunk_type ==  LUI_MEM_CHUNK_TYPE_NEW)
        {
            /* Resize the chunk */
            chunk_metadata->alloc_size = element_size + chunk_metadata_size;
            /* Set next chunk address */
            chunk_metadata->next_chunk = (_lui_mem_chunk_t *)((uint8_t *)chunk_metadata + chunk_metadata->alloc_size);
            /* Set next chunk's previous chunk address which is the current chunk */
            chunk_metadata->next_chunk->prev_chunk = chunk_metadata;
            /* Increase chunk count by 1 */
            ++(g_mem_block.mem_chunk_count);
        }

        g_mem_block.mem_allocated += element_size;
        return ((uint8_t *)chunk_metadata + chunk_metadata_size);
    }
    else
    {
        return NULL;
    }
}


void _lui_mem_free(void *ptr)
{
    _lui_mem_chunk_t *chunk_metadata = (_lui_mem_chunk_t *)ptr;
    // Get the actual chunk_metadata
    --chunk_metadata;
    
    if (chunk_metadata == NULL)
    {
        return;
    }
    if (chunk_metadata->alloc_status == LUI_MEM_CHUNK_STATUS_FREE)
    {
        return;
    }
    
    chunk_metadata->alloc_status =  LUI_MEM_CHUNK_STATUS_FREE;
    g_mem_block.mem_allocated -= chunk_metadata->alloc_size - sizeof(_lui_mem_chunk_t);
    
    /* If only one chunk is there, set the chunk count to 0 */
    if (g_mem_block.mem_chunk_count == 1)
    {
        g_mem_block.mem_chunk_count = 0;
        chunk_metadata->alloc_size = 0;
        chunk_metadata->next_chunk = NULL;
        chunk_metadata->prev_chunk = NULL;
        
        return;
    }
    
    // merge with next chunk if that is free too
    /* Note: Theoretically, for the right-most chunk, `next_chunk` should be NULL and this condition
     * should not be called. BUT, the right-most chunk actually has a not-null `next_chunk` which is
     * set during allocation to point out which one will be it's next chunk.
     * So, even for right-most chunk, this condition is satisfied and `chunk_count` is decremented
     * even though nothing is "actually" merged
     */
    if (chunk_metadata->next_chunk != NULL)
    {
        if (chunk_metadata->next_chunk->alloc_status == LUI_MEM_CHUNK_STATUS_FREE)
        {
            /* merge next chunk with current chunk and increase current chunk's size */
            chunk_metadata->alloc_size += chunk_metadata->next_chunk->alloc_size;
            chunk_metadata->next_chunk = chunk_metadata->next_chunk->next_chunk;
            if (chunk_metadata->next_chunk != NULL)
            {
                chunk_metadata->next_chunk->prev_chunk = chunk_metadata;
            }
            /* As two chunks are merged, chunk count reduced*/
            --(g_mem_block.mem_chunk_count);
        }
    }
    
    // merge with previous chunk if that is free too
    if (chunk_metadata->prev_chunk != NULL)
    {
        if (chunk_metadata->prev_chunk->alloc_status == LUI_MEM_CHUNK_STATUS_FREE)
        {
            /* merge current chunk with previous chunk and increase previous chunk's size */
            chunk_metadata->prev_chunk->alloc_size += chunk_metadata->alloc_size;
            chunk_metadata->prev_chunk->next_chunk = chunk_metadata->next_chunk;
            if (chunk_metadata->next_chunk != NULL)
            {
                chunk_metadata->next_chunk->prev_chunk = chunk_metadata->prev_chunk;
            }
            /* As two chunks are merged, chunk count reduced*/
            --(g_mem_block.mem_chunk_count);
        }
    }
    
    /* If chunk count becomes 0 after merging, set the remaining chunk's size to 0
     * Also set its prev_chunk and next_chunk to NULL
     */
    if (g_mem_block.mem_chunk_count == 0)
    {
        _lui_mem_chunk_t *first_chunk = (_lui_mem_chunk_t *)(g_mem_block.mem_start);
        first_chunk->alloc_size = 0;
        first_chunk->next_chunk = NULL;
        first_chunk->prev_chunk = NULL;
    }  
}
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */