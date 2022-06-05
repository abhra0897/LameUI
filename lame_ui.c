/*
 *  Created on: 02-Apr-2020
 */
/**
 * @file lame_ui.c
 * @author Avra Mitra
 * @brief Source FIle of LameUI GUI library. Must include lame_ui.h. No other file is mandatory.
 * @version 0.1
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "lame_ui.h"


uint8_t g_needs_render = 0;
_lui_mem_block_t g_mem_block;
_lui_main_t* g_lui_main;


/*-------------------------------------------------------------------------------
 * 				Main functions
 *-------------------------------------------------------------------------------
 */

void lui_init(uint8_t mem_block[], uint16_t size)
{
	_lui_mem_init(mem_block, size);
	g_lui_main = _lui_mem_alloc(sizeof(_lui_main_t));
	if (g_lui_main == NULL)
		return;

	// 	g_lui_main->scenes = {NULL};
	g_lui_main->default_font = &FONT_lui_default;
	g_lui_main->disp_drv = NULL;
	g_lui_main->touch_input_dev = NULL;
	//  g_lui_main->last_touch_data.x = -1;
	//  g_lui_main->last_touch_data.y = -1;
	//  g_lui_main->last_touch_data.is_pressed = -1;
	g_lui_main->input_event_clicked = 0;
	g_lui_main->input_state_pressed = 0;
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

	
	lui_obj_t* obj_caused_cb = NULL;
	// Reading input
	obj_caused_cb = _lui_process_input_of_act_scene();
	if (g_needs_render)
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
void lui_label_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;

	if (!(obj->visible))
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;


	lui_gfx_draw_string_advanced(lbl->text, obj->x, obj->y, obj->common_style.width, obj->common_style.height, lbl->style.text_color, obj->common_style.bg_color, 1, lbl->font);

	// Draw the label border if needed
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(obj->x, obj->y, obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
	}

}

/*
 * Initialize a label with default values
 */
lui_obj_t* lui_label_create(void)
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_label_t* initial_label =  _lui_mem_alloc(sizeof(*initial_label));
	if (initial_label == NULL)
		return NULL;
	
	initial_label->text = "";
	initial_label->font = g_lui_main->default_font;
	initial_label->style.text_color = LUI_STYLE_LABEL_TEXT_COLOR;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// objeect type
	obj->obj_type = LUI_OBJ_LABEL;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_LABEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_LABEL_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_LABEL_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_LABEL_WIDTH;
	obj->common_style.height = LUI_STYLE_LABEL_HEIGHT;

	obj->obj_main_data = (void* )initial_label;

	return obj;
}

void lui_label_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (obj == NULL)
		return;
	if (font == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	lbl->font = font;
	_lui_object_set_need_refresh(obj->parent);
}

void lui_label_set_text(lui_obj_t* obj, const char* text)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	lbl->text = (char* )text;

	_lui_object_set_need_refresh(obj);
}

void lui_label_set_text_color(lui_obj_t* obj, uint16_t text_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LABEL)
		return;

	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
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
void lui_linechart_draw(lui_obj_t* obj)
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

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);

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


	// Draw the chart background
	lui_gfx_draw_rect_fill(temp_x, temp_y, width, height, obj->common_style.bg_color);

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
			lui_gfx_draw_line(temp_x_new, temp_y, temp_x_new, temp_y + height - 1, 1, chart->style.grid_color);
		}

		// Draw the horizontal grids from bottom to top
		uint16_t y_bottom = temp_y + height;
		for (int i = 1; i <= chart->grid.hor_count; i++)
		{
			uint16_t temp_y_new = y_bottom - (i * hor_grid_spacing);
			lui_gfx_draw_line(temp_x, temp_y_new, temp_x + width - 1, temp_y_new, 1, chart->style.grid_color);
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

			lui_gfx_draw_line(current_x, current_y, next_x, next_y, 1, line_color);
		}
	}

	// Draw the chart border if needed
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(temp_x, temp_y, width, height, 1, obj->common_style.border_color);
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

	lui_chart_t* initial_line_chart =  _lui_mem_alloc(sizeof(*initial_line_chart));
	if (initial_line_chart == NULL)
		return NULL;

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
	initial_line_chart->font = g_lui_main->default_font;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_LINECHART;
	// object common style
	obj->common_style.border_color = LUI_STYLE_LINECHART_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_LINECHART_BORDER_VISIBLE;
	obj->common_style.bg_color = LUI_STYLE_LINECHART_BG_COLOR;
	obj->common_style.height = LUI_STYLE_LINECHART_HEIGHT;
	obj->common_style.width = LUI_STYLE_LINECHART_WIDTH;
	
	obj->obj_main_data = (void* )initial_line_chart;

	return obj;
}

void lui_linechart_set_grid_count(lui_obj_t* obj, uint16_t hor_lines, uint16_t vert_lines)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->grid.hor_count == hor_lines && chart->grid.vert_count == vert_lines)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->grid.hor_count = hor_lines;
	chart->grid.vert_count = vert_lines;
}

void lui_linechart_set_grid_color(lui_obj_t* obj, uint16_t color)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.grid_color == color)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_color = color;
}

void lui_linechart_set_grid_visible(lui_obj_t* obj, uint8_t state)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.grid_visible == state)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_visible = state;
}

void lui_linechart_set_line_color(lui_obj_t* obj, uint16_t line_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.line_color == line_color)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.line_color = line_color;
}

void lui_linechart_set_data_auto_scale(lui_obj_t* obj, uint8_t auto_scale)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (auto_scale == chart->data.auto_scale)
		return;
	chart->data.auto_scale = auto_scale ? 1 : 0;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_data_range(lui_obj_t* obj, double y_min, double y_max)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->data.y_max_value == y_max && chart->data.y_min_value == y_min && chart->data.auto_scale == 0)
		return;
	_lui_object_set_need_refresh(obj);
	chart->data.y_max_value = y_max;
	chart->data.y_min_value = y_min;
	chart->data.auto_scale = 0;
}

void lui_linechart_set_data_source(lui_obj_t* obj, double* source, uint16_t points)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LINECHART)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
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
void lui_button_draw(lui_obj_t* obj)
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

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);

	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t btn_height = obj->common_style.height;
	uint16_t btn_width = obj->common_style.width;


	uint16_t str_width_height[2];

	// Draw the button's body color depending on its current state
	uint16_t btn_color = obj->common_style.bg_color;
	if (obj->state == LUI_STATE_SELECTED)
		btn_color = btn->style.selection_color;
	else if (obj->state == LUI_STATE_PRESSED)
		btn_color = btn->style.pressed_color;
	// else if (btn->state == LUI_STATE_IDLE)
	// 	btn_color = btn->color;

	lui_gfx_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn_color);
	
	// Draw the button label (text)
	// Text will be in the miidle of the button.
	// So, at first we need to calculate its position


	lui_gfx_get_string_dimension(btn->label.text, btn->label.font, btn_width, str_width_height);

	str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
	str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

	temp_x = temp_x + (btn_width - str_width_height[0]) / 2;
	temp_y = temp_y + (btn_height - str_width_height[1]) / 2;

	lui_gfx_draw_string_advanced(btn->label.text, temp_x, temp_y, str_width_height[0], str_width_height[1], btn->style.label_color, 0, 0, btn->label.font);
	

	// Finally Draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(obj->x, obj->y, btn_width, btn_height, 1, obj->common_style.border_color);
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

	lui_button_t* initial_button =  _lui_mem_alloc(sizeof(*initial_button));
	if (initial_button == NULL)
		return NULL;

	initial_button->style.pressed_color = LUI_STYLE_BUTTON_PRESSED_COLOR;
	initial_button->style.selection_color = LUI_STYLE_BUTTON_SELECTION_COLOR;
	
	initial_button->label.text = "";
	initial_button->style.label_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->label.font = g_lui_main->default_font;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_BUTTON;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_BUTTON_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_BUTTON_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_BUTTON_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_BUTTON_WIDTH;
	obj->common_style.height = LUI_STYLE_BUTTON_HEIGHT;
	
	obj->obj_main_data = (void* )initial_button;

	return  obj;
}

void lui_button_set_label_text(lui_obj_t* obj, const char* text)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->label.text = (char* )text;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_color(lui_obj_t* obj, uint16_t color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.label_color == color)
		return;
	btn->style.label_color = color;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->label.font = (lui_font_t* )font;
	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);
}

void lui_button_set_extra_colors(lui_obj_t* obj, uint16_t pressed_color, uint16_t selection_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BUTTON)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.pressed_color == pressed_color && btn->style.selection_color == selection_color)
		return;
	btn->style.pressed_color = pressed_color;
	btn->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_LIST related functions
 *-------------------------------------------------------------------------------
 */

void lui_list_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	if (!(obj->visible))
		return;

	/* draw the background first */
	g_lui_main->disp_drv->draw_pixels_area_cb(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);

	/* Finally Draw the border if needed */
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(obj->x, obj->y,  obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
	}
}

lui_obj_t* lui_list_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_list_t* initial_list =  _lui_mem_alloc(sizeof(*initial_list));
	if (initial_list == NULL)
		return NULL;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_LIST;
	obj->obj_main_data = (void* )initial_list;
	obj->enabled = 1;
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

	initial_list->font = g_lui_main->default_font;

	return obj;
}

void lui_list_prepare(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	// list has no item (button), so return
	if (obj->children_count == 0)
		return;

	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
	

	// usable height depends on navigation buttons' existense
	// if no nav button (for single page list), total height is usable
	uint16_t list_usable_height = obj->common_style.height - 1;
	if (list->button_item_min_height < list->font->bitmap->size_y)
		list->button_item_min_height = list->font->bitmap->size_y;
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
	lui_obj_t* list_item = obj->first_child->next_sibling->next_sibling;
	uint8_t item_pos = 0; 
	uint16_t first_item_pos = (list->current_page_index * list->buttons_per_page);
	uint16_t last_item_pos = first_item_pos + list->buttons_per_page - 1;
	while (list_item != NULL)
	{
		lui_button_t* button_item = list_item->obj_main_data;
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
	lui_button_t* button_nav_prev = obj->first_child->obj_main_data;
	lui_button_t* button_nav_nxt = obj->first_child->next_sibling->obj_main_data;

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

lui_obj_t* lui_list_add_item(lui_obj_t* obj, const char* text)
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


	lui_list_t* list = obj->obj_main_data;
	list->button_item_min_height = LUI_STYLE_LIST_ITEM_MIN_HEIGHT;

	lui_button_t* initial_button =  _lui_mem_alloc(sizeof(*initial_button));
	if (initial_button == NULL)
		return NULL;
	initial_button->style.pressed_color = LUI_STYLE_LIST_ITEM_PRESSED_COLOR;
	initial_button->style.selection_color = LUI_STYLE_LIST_ITEM_SELECTION_COLOR;
	initial_button->label.text = (char*)text;
	initial_button->style.label_color = LUI_STYLE_LIST_ITEM_LABEL_COLOR;
	initial_button->label.font = list->font; // here it can be null too

	lui_obj_t* obj_btn_item = _lui_object_create();
	// object type
	obj_btn_item->obj_type = LUI_OBJ_BUTTON;
	obj_btn_item->enabled = 1;
	// object common style
	obj_btn_item->common_style.bg_color = LUI_STYLE_LIST_ITEM_BG_COLOR;
	obj_btn_item->common_style.border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;
	obj_btn_item->common_style.border_visible = LUI_STYLE_LIST_ITEM_BORDER_VISIBLE;
	obj_btn_item->common_style.width = LUI_STYLE_LIST_WIDTH;
	obj_btn_item->common_style.height = LUI_STYLE_LIST_ITEM_MIN_HEIGHT;
	
	obj_btn_item->obj_main_data = (void* )initial_button;

	_lui_list_add_button_obj(obj, obj_btn_item);

	return  obj_btn_item;
}

void lui_list_delete_item(lui_obj_t** obj_item_addr)
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
}

void lui_list_set_item_min_height(lui_obj_t* obj, uint8_t height)
{
	if (obj == NULL)
		return;
	lui_list_t* list = obj->obj_main_data;

	// if list has a font and the item height is >= the glyph's height, only then set that height
	if (list->font != NULL)
	{
		if (height >= list->font->bitmap->size_y)
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

void lui_list_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (obj == NULL)
		return;
	if (font == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;
	
	lui_list_t* list = obj->obj_main_data;
	list->font = font;

	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);

	// this font is set to all items in `prepare` func
}

void lui_list_set_nav_btn_label_color(lui_obj_t* obj, uint16_t color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_label_color(obj->first_child, color);
	lui_button_set_label_color(obj->first_child->next_sibling, color);
}

void lui_list_set_nav_btn_bg_color(lui_obj_t* obj, uint16_t color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_object_set_bg_color(obj->first_child, color);
	lui_object_set_bg_color(obj->first_child->next_sibling, color);
}

void lui_list_set_nav_btn_extra_colors(lui_obj_t* obj, uint16_t pressed_color, uint16_t selection_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_extra_colors(obj->first_child, pressed_color, selection_color);
	lui_button_set_extra_colors(obj->first_child->next_sibling, pressed_color, selection_color);
}

void lui_list_set_nav_btn_label_text(lui_obj_t* obj, const char* btn_prev_text, const char* btn_nxt_text)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_button_set_label_text(obj->first_child, btn_prev_text);
	lui_button_set_label_text(obj->first_child->next_sibling, btn_nxt_text);
}

void lui_list_set_nav_btn_border_color(lui_obj_t* obj, uint16_t color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_LIST)
		return;

	lui_object_set_border_color(obj->first_child, color);
	lui_object_set_border_color(obj->first_child->next_sibling, color);
}

void _lui_list_add_button_obj(lui_obj_t* obj_list, lui_obj_t* obj_btn)
{
	if (obj_list->first_child == NULL)
	{
		obj_list->first_child = obj_btn;
	}
	else
	{
		lui_obj_t* next_child = obj_list->first_child;
		
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

void _lui_list_add_nav_buttons(lui_obj_t* obj)
{
	// create navigation buttons
	lui_obj_t* obj_nav_btn_prev = lui_button_create();
	lui_obj_t* obj_nav_btn_nxt = lui_button_create();
	lui_button_set_label_text(obj_nav_btn_prev, LUI_ICON_ARROW_BACK);
	lui_button_set_label_text( obj_nav_btn_nxt, LUI_ICON_ARROW_FORWARD);
	// set nav button styles (nav button height and width are calculated y `prepare` function)
	lui_button_set_label_color(obj_nav_btn_prev, LUI_STYLE_LIST_NAV_LABEL_COLOR);
	lui_button_set_label_color(obj_nav_btn_nxt, LUI_STYLE_LIST_NAV_LABEL_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_prev, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_nxt, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	obj_nav_btn_nxt->common_style.border_visible = obj_nav_btn_prev->common_style.border_visible = 0;
	obj_nav_btn_nxt->common_style.border_color = obj_nav_btn_prev->common_style.border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;
	obj_nav_btn_nxt->common_style.bg_color = obj_nav_btn_prev->common_style.bg_color = LUI_STYLE_LIST_NAV_BG_COLOR;
	obj_nav_btn_prev->obj_event_cb = obj_nav_btn_nxt->obj_event_cb = _lui_list_nav_btn_cb;

	_lui_list_add_button_obj(obj, obj_nav_btn_prev);
	_lui_list_add_button_obj(obj, obj_nav_btn_nxt);
}

void lui_list_set_page_index(lui_obj_t* obj, uint8_t index)
{
	lui_list_t* list = obj->obj_main_data;

	if (index >= list->page_count || index == list->current_page_index)
		return;
	list->current_page_index = index;
	_lui_object_set_need_refresh(obj);


	// list items start after first two buttons. First two buttons are nav buttons
	lui_obj_t* list_item = obj->first_child->next_sibling->next_sibling;
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
		// draw "next" or "prev" button only if there's a next or a previous page
		if (list->current_page_index + 1 < list->page_count)
			lui_object_set_visibility(obj->first_child->next_sibling, 1); // 2nd child: NEXT button
		else
			lui_object_set_visibility(obj->first_child->next_sibling, 0);

		if (list->current_page_index - 1 >= 0)
			lui_object_set_visibility(obj->first_child, 1);	// 1st child: PREV button
		else
			lui_object_set_visibility(obj->first_child, 0);
	}
	
}

void _lui_list_nav_btn_cb(lui_obj_t* obj)
{
	uint8_t event = lui_object_get_event(obj);
	lui_list_t* list = obj->parent->obj_main_data;
	uint8_t index = list->current_page_index;

	if (event == LUI_EVENT_PRESSED)
	{
		/* first child is nav_prev btn */
		if (obj == obj->parent->first_child && list->current_page_index > 0)
		{
			index--;
		}
		/* 2nd child is nav_nxt button */
		else if (obj == obj->parent->first_child->next_sibling && list->current_page_index < list->page_count - 1)
		{
			index++;
		}

		lui_list_set_page_index(obj->parent, index);
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

void lui_switch_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return;

	if (!(obj->visible))
		return;
	
	lui_switch_t* swtch = (lui_switch_t* )(obj->obj_main_data);
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
	
	
	lui_gfx_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, obj->common_style.bg_color);	// switch bg (color is constant regardless the state)
	if (obj->common_style.border_visible == 1)
		lui_gfx_draw_rect(temp_x, temp_y, temp_width, temp_height, 1, obj->common_style.border_color);	// switch border

	temp_width = (float)temp_width * 0.3;
	temp_height = (float)temp_height * 0.6;
	temp_x = temp_x + ((obj->common_style.width / 2) - temp_width) / 2;
	if (obj->value == 1)
		temp_x += (obj->common_style.width / 2);
	temp_y = temp_y + (obj->common_style.height - temp_height) / 2;

	lui_gfx_draw_rect_fill(temp_x, temp_y, temp_width, temp_height, swtch_color);// switch slider
}

lui_obj_t* lui_switch_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_switch_t* initial_switch =  _lui_mem_alloc(sizeof(*initial_switch));
	if (initial_switch == NULL)
		return NULL;
	
	initial_switch->style.knob_off_color = LUI_STYLE_SWITCH_KNOB_OFF_COLOR;
	initial_switch->style.knob_on_color = LUI_STYLE_SWITCH_KNOB_ON_COLOR;
	initial_switch->style.selection_color = LUI_STYLE_SWITCH_SELECTION_COLOR;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_SWITCH;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_SWITCH_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_SWITCH_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_SWITCH_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_SWITCH_WIDTH;
	obj->common_style.height = LUI_STYLE_SWITCH_HEIGHT;
	
	obj->obj_main_data = (void* )initial_switch;

	return  obj;
}

void lui_switch_set_extra_colors(lui_obj_t* obj, uint16_t knob_off_color, uint16_t knob_on_color, uint16_t selection_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return;
	
	lui_switch_t* swtch = (lui_switch_t* )(obj->obj_main_data);

	if (swtch->style.knob_off_color == knob_off_color && swtch->style.knob_on_color == knob_on_color && swtch->style.selection_color == selection_color)
		return;
	swtch->style.knob_off_color = knob_off_color;
	swtch->style.knob_on_color = knob_on_color;
	swtch->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_switch_get_value(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SWITCH)
		return -1;
	
	return obj->value;
}

void lui_switch_set_value(lui_obj_t* obj, uint8_t value)
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

void lui_switch_set_on(lui_obj_t* obj)
{
	lui_switch_set_value(obj, 1);
}

void lui_switch_set_off(lui_obj_t* obj)
{
	lui_switch_set_value(obj, 0);
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_CHECKBOX related functions
 *-------------------------------------------------------------------------------
 */

void lui_checkbox_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return;

	if (!(obj->visible))
		return;
	
	lui_checkbox_t* chkbox = obj->obj_main_data;
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

	lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.width, bg_color);
	// draw the tick mark if needed
	if (obj->value == 1)
	{
		uint16_t point_1_x = obj->x + (obj->common_style.width * .2), point_1_y = obj->y + (obj->common_style.width * .55);
		uint16_t point_2_x = obj->x + (obj->common_style.width * .4), point_2_y = obj->y + (obj->common_style.width * .75);
		uint16_t point_3_x = obj->x + (obj->common_style.width * .75), point_3_y = obj->y + (obj->common_style.width * .3);
		
		lui_gfx_draw_line(point_1_x, point_1_y, point_2_x, point_2_y, 2, chkbox->style.tick_color);
		lui_gfx_draw_line(point_2_x, point_2_y, point_3_x, point_3_y, 2, chkbox->style.tick_color);
	}

	// draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(obj->x, obj->y,  obj->common_style.width,  obj->common_style.width, 1, obj->common_style.border_color);
	}
	
}

lui_obj_t* lui_checkbox_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_checkbox_t* initial_chkbox =  _lui_mem_alloc(sizeof(*initial_chkbox));
	if (initial_chkbox == NULL)
		return NULL;
	
	initial_chkbox->style.bg_checked_color = LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR;
	initial_chkbox->style.selection_color = LUI_STYLE_CHECKBOX_SELECTION_COLOR;
	initial_chkbox->style.tick_color = LUI_STYLE_CHECKBOX_TICK_COLOR;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_CHECKBOX;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_CHECKBOX_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_CHECKBOX_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_CHECKBOX_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_CHECKBOX_WIDTH;
	obj->common_style.height = LUI_STYLE_CHECKBOX_HEIGHT;
	
	obj->obj_main_data = (void* )initial_chkbox;

	return  obj;
}

void lui_checkbox_set_extra_colors(lui_obj_t* obj, uint16_t bg_checked_color, uint16_t tick_color, uint16_t selection_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return;
	
	lui_checkbox_t* chkbox = obj->obj_main_data;

	if (chkbox->style.bg_checked_color == bg_checked_color && chkbox->style.tick_color == tick_color && chkbox->style.selection_color == selection_color)
		return;
	chkbox->style.bg_checked_color = bg_checked_color;
	chkbox->style.tick_color = tick_color;
	chkbox->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_checkbox_get_value(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_CHECKBOX)
		return -1;
	
	return obj->value;
}

void lui_checkbox_set_value(lui_obj_t* obj, uint8_t value)
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

void lui_checkbox_set_checked(lui_obj_t* obj)
{
	lui_checkbox_set_value(obj, 1);
}

void lui_checkbox_set_unchecked(lui_obj_t* obj)
{
	lui_checkbox_set_value(obj, 0);
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_SLIDER related functions
 *-------------------------------------------------------------------------------
 */

void lui_slider_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;

	if (!(obj->visible))
		return;
	
	lui_slider_t* slider = obj->obj_main_data;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t knob_color = slider->style.knob_color;
	if (obj->state == LUI_STATE_SELECTED || obj->state == LUI_STATE_PRESSED)
	{
		knob_color = slider->style.selection_color;
	}

	// draw the filled region (left) first
	lui_gfx_draw_rect_fill(obj->x, obj->y, slider->knob_center_rel_x, obj->common_style.height, slider->style.bg_filled_color);
	// draw the remaining region (right) 
	lui_gfx_draw_rect_fill(obj->x + slider->knob_center_rel_x, obj->y, obj->common_style.width - slider->knob_center_rel_x, obj->common_style.height, obj->common_style.bg_color);
	// draw the knob
	lui_gfx_draw_rect_fill(obj->x + slider->knob_center_rel_x - (slider->style.knob_width / 2), obj->y, slider->style.knob_width, obj->common_style.height, knob_color);

	// draw the border if needed
	if (obj->common_style.border_visible == 1)
	{
		lui_gfx_draw_rect(obj->x, obj->y,  obj->common_style.width,  obj->common_style.height, 1, obj->common_style.border_color);
	}
}

lui_obj_t* lui_slider_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_slider_t* initial_slider =  _lui_mem_alloc(sizeof(*initial_slider));
	if (initial_slider == NULL)
		return NULL;
	
	initial_slider->style.bg_filled_color = LUI_STYLE_SLIDER_BG_FILLED_COLOR;
	initial_slider->style.knob_color = LUI_STYLE_SLIDER_KNOB_COLOR;
	initial_slider->style.selection_color = LUI_STYLE_SLIDER_SELECTION_COLOR;
	initial_slider->style.knob_width = LUI_STYLE_SLIDER_KNOB_WIDTH;
	initial_slider->range_min = 0;
	initial_slider->range_max = 100;
	initial_slider->knob_center_rel_x = (LUI_STYLE_SLIDER_KNOB_WIDTH / 2);

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_SLIDER;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_SLIDER_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_SLIDER_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_SLIDER_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_SLIDER_WIDTH;
	obj->common_style.height = LUI_STYLE_SLIDER_HEIGHT;

	obj->obj_main_data = (void* )initial_slider;

	return  obj;
}

void lui_slider_set_extra_colors(lui_obj_t* obj, uint16_t knob_color, uint16_t bg_filled_color, uint16_t selection_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t* slider = obj->obj_main_data;

	if (slider->style.knob_color == knob_color && slider->style.bg_filled_color == bg_filled_color && slider->style.selection_color == selection_color)
		return;
	slider->style.knob_color = knob_color;
	slider->style.bg_filled_color = bg_filled_color;
	slider->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_value(lui_obj_t* obj, int16_t value)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t* slider = obj->obj_main_data;
	
	
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

void lui_slider_set_range(lui_obj_t* obj, int16_t range_min, int16_t range_max)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return;
	
	lui_slider_t* slider = obj->obj_main_data;

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

int16_t lui_slider_get_value(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	
	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	
	return obj->value;
}

int16_t lui_slider_get_min_value(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;

	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	lui_slider_t* slider = obj->obj_main_data;

	return slider->range_min;
}

int16_t lui_slider_get_max_value(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;

	// type check
	if (obj->obj_type != LUI_OBJ_SLIDER)
		return -1;
	lui_slider_t* slider = obj->obj_main_data;
	
	return slider->range_max;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_BTNGRID related functions
 *-------------------------------------------------------------------------------
 */

void lui_btngrid_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_btngrid_t* btngrid = obj->obj_main_data;
		
	uint16_t btn_color = btngrid->style.btn_bg_color;
	uint16_t btn_width = 0;
	uint16_t btn_height = 0;
	static int16_t last_act_btn_index = -1;

	
	/* If no event occured yet drawing function is called, that means first time rendering of a btngrid.
	 * So, draw the base first
	*/
	if (/* obj->event == LUI_EVENT_NONE && obj->needs_refresh */ btngrid->needs_full_render)
	{
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width,  obj->common_style.height, obj->common_style.bg_color);
	}

	uint16_t j = 0;
	for (uint16_t i = 0; i < btngrid->btn_cnt; i++)
	{	
		while (btngrid->texts[j][0] == '\n' || btngrid->texts[j][0] == '\0')
		{
			++j;
		}

		/**
		 * Draw a button only if the btngrid needs full render,
		 * or when a buttons index matches an active button's index 
		 */
		if (btngrid->needs_full_render || (i == btngrid->active_btn_index || i == last_act_btn_index))
		{
			if (!(btngrid->btn_properties[i] & _LUI_BTNGRID_MASK_BTN_IS_HIDDEN))
			{
				btn_color = btngrid->style.btn_bg_color;
				if (i == btngrid->active_btn_index)
				{
					if (obj->state == LUI_STATE_SELECTED)
					{
						btn_color = btngrid->style.btn_pressed_color;
					}
					else if (obj->state == LUI_STATE_PRESSED)
					{
						btn_color = btngrid->style.btn_pressed_color;
					}
				}

				/**
				 * This is to handle when a checkable button lost its focus but check state
				 * is changed to "Checked". Or, when manually the check state is set
				 */
				else if (i == last_act_btn_index || btngrid->needs_full_render)
				{
					if (btngrid->btn_properties[i] & _LUI_BTNGRID_MASK_BTN_IS_CHECKABLE)
					{ 
						if (btngrid->btn_properties[i] & _LUI_BTNGRID_MASK_BTN_IS_CHECKED)
						{
							btn_color = btngrid->style.btn_pressed_color;
						}
					}
				}

				btn_width = btngrid->btn_area[i].x2 - btngrid->btn_area[i].x1 + 1;
				btn_height = btngrid->btn_area[i].y2 - btngrid->btn_area[i].y1 + 1;
				lui_gfx_draw_rect_fill(btngrid->btn_area[i].x1, btngrid->btn_area[i].y1, btn_width, btn_height, btn_color);

				if (obj->common_style.border_visible)
				{
					lui_gfx_draw_rect(btngrid->btn_area[i].x1, btngrid->btn_area[i].y1, btn_width, btn_height, 1, obj->common_style.border_color);
				}

				uint16_t str_width_height[2];
				lui_gfx_get_string_dimension(btngrid->texts[j], btngrid->font, btn_width, str_width_height);

				str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
				str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

				uint16_t temp_x = btngrid->btn_area[i].x1 + (btn_width - str_width_height[0]) / 2;
				uint16_t temp_y = btngrid->btn_area[i].y1 + (btn_height - str_width_height[1]) / 2;

				lui_gfx_draw_string_advanced(btngrid->texts[j], temp_x, temp_y, str_width_height[0], str_width_height[1], btngrid->style.btn_label_color, 0, 0, btngrid->font);
			}
		}

		++j;
	}
	last_act_btn_index = btngrid->active_btn_index;
	btngrid->needs_full_render = 0;
}

lui_obj_t* lui_btngrid_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_btngrid_t* initial_btngrid =  _lui_mem_alloc(sizeof(*initial_btngrid));
	if (initial_btngrid == NULL)
		return NULL;
	
	initial_btngrid->style.btn_label_color = LUI_STYLE_BTNGRID_LABEL_COLOR;
	initial_btngrid->style.btn_pressed_color = LUI_STYLE_BTNGRID_PRESSED_COLOR;
	initial_btngrid->style.btn_bg_color = LUI_STYLE_BTNGRID_BG_COLOR;
	initial_btngrid->font = g_lui_main->default_font;
	initial_btngrid->texts = NULL;
	initial_btngrid->btn_properties = NULL;
	initial_btngrid->btn_area = NULL;
	initial_btngrid->btn_cnt = 0;
	initial_btngrid->active_btn_index = -1;
	initial_btngrid->style.btn_margin_hor = 2;
	initial_btngrid->style.btn_margin_vert = 2;
	initial_btngrid->needs_full_render = 1;
	initial_btngrid->kb_data = NULL;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_BTNGRID;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_BTNGRID_BASE_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_BTNGRID_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_BTNGRID_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_BTNGRID_WIDTH;
	obj->common_style.height = LUI_STYLE_BTNGRID_HEIGHT;

	obj->obj_main_data = (void* )initial_btngrid;

	return  obj;
}

void lui_btngrid_set_textmap(lui_obj_t* obj, const char* texts[])
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;

	if (texts == NULL)
		return;

	lui_btngrid_t* btngrid = obj->obj_main_data;
	uint16_t buttons = 0;
	uint8_t rows = 1;


	for (uint16_t i = 0; texts[i][0] != '\0'; i++)
	{
		if (strcmp(texts[i], "\n") != 0)
		{
			++buttons;
		}
		else
		{
			++rows;
		}
	}

	/* Already btngrid exists and new button count is greater than prev button count */
	if (btngrid->btn_cnt > 0 && buttons > btngrid->btn_cnt)
	{
		return;
	}
	/* Buttongrid may or maynot already exists */
	else
	{	
		/* Btngrid doesn't already exists, so, allocate memory for area map and property map */
		if (btngrid->btn_cnt == 0)
		{
			btngrid->btn_area = _lui_mem_alloc(buttons * sizeof(lui_area_t));
			if (btngrid->btn_area == NULL)
				return;
			btngrid->btn_properties = _lui_mem_alloc(buttons * sizeof(uint8_t));
			if (btngrid->btn_properties == NULL)
				return;
		}

		for (int i = 0; i < buttons; i++)
		{
			btngrid->btn_area[i].x1 = 0;
			btngrid->btn_area[i].y1 = 0;
			btngrid->btn_area[i].x2 = 0;
			btngrid->btn_area[i].y2 = 0;
			btngrid->btn_properties[i] = 1;
		}

		btngrid->btn_cnt = buttons;
		btngrid->row_cnt = rows;
		btngrid->texts = texts;
		btngrid->needs_full_render = 1;
		_lui_object_set_need_refresh(obj);

		_lui_btngrid_calc_btn_area(obj);
	}
}

void lui_btngrid_set_propertymap(lui_obj_t* obj, const uint8_t properties[])
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	if (properties == NULL)
		return;

	lui_btngrid_t* btngrid = obj->obj_main_data;
	if (btngrid->btn_properties == NULL)
		return;
	
	memcpy(btngrid->btn_properties, properties, btngrid->btn_cnt);
	btngrid->needs_full_render = 1;
	_lui_object_set_need_refresh(obj);
	_lui_btngrid_calc_btn_area(obj);
}

void lui_btngrid_set_btn_property_bits(lui_obj_t* obj, uint16_t btn_index, uint8_t property_byte)
{
	if (obj == NULL)
		return;
		// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	
	lui_btngrid_t* btngrid = obj->obj_main_data;

	if (btngrid->btn_properties == NULL || btn_index >= btngrid->btn_cnt)
		return;
	if (btngrid->btn_properties[btn_index] == property_byte)
		return;
	if ((property_byte & _LUI_BTNGRID_MASK_BTN_WIDTH_UNIT) == 0)
		return;

	btngrid->btn_properties[btn_index] = property_byte;
	_lui_btngrid_calc_btn_area(obj);

	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void lui_btngrid_set_btn_text(lui_obj_t* obj, uint8_t btn_index, char* text)
{
	if (obj == NULL)
		return;
		// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	
	lui_btngrid_t* btngrid = obj->obj_main_data;

	if (btngrid->texts == NULL)
		return;

	uint8_t txt_index = 0;
	for (txt_index = 0; txt_index < btn_index; txt_index++)
	{	
		while (btngrid->texts[txt_index][0] == '\n' || btngrid->texts[txt_index][0] == '\0')
		{
			++txt_index;
		}
	}
	btngrid->texts[txt_index] = text;
	_lui_object_set_need_refresh(obj);
}

void lui_btngrid_set_btn_width_unit(lui_obj_t* obj, uint16_t btn_index, uint8_t width_unit)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~_LUI_BTNGRID_MASK_BTN_WIDTH_UNIT;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property | (width_unit & _LUI_BTNGRID_MASK_BTN_WIDTH_UNIT));
}

void lui_btngrid_set_btn_hidden(lui_obj_t* obj, uint16_t btn_index, uint8_t hidden)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~_LUI_BTNGRID_MASK_BTN_IS_HIDDEN;
	property = hidden ? (property | _LUI_BTNGRID_MASK_BTN_IS_HIDDEN) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
}

void lui_btngrid_set_btn_checkable(lui_obj_t* obj, uint16_t btn_index, uint8_t checkable)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~_LUI_BTNGRID_MASK_BTN_IS_CHECKABLE;
	property = checkable ? (property | _LUI_BTNGRID_MASK_BTN_IS_CHECKABLE) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
	if (checkable)
	{
		((lui_btngrid_t* )(obj->obj_main_data))->needs_full_render = 0; /* if something is set to checkable, no need to redarw entire btngrid */
	}
}

void lui_btngrid_set_btn_checked(lui_obj_t* obj, uint16_t btn_index, uint8_t checked)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~_LUI_BTNGRID_MASK_BTN_IS_CHECKED;
	property = checked ? (property | _LUI_BTNGRID_MASK_BTN_IS_CHECKED) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
}

int16_t lui_btngrid_get_acive_btn_index(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return -1;
	
	return ((lui_btngrid_t* )(obj->obj_main_data))->active_btn_index;
}

int8_t lui_btngrid_get_btn_check_status(lui_obj_t* obj, uint8_t btn_index)
{
	if (obj == NULL)
		return -1;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return -1;
	
	uint8_t props = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index];
	return  (props & _LUI_BTNGRID_MASK_BTN_IS_CHECKED) ? 1 : 0;
}

void lui_btngrid_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (obj == NULL)
		return;
	if (font == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;

	lui_btngrid_t* btngrid = obj->obj_main_data;
	btngrid->font = font;
	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void lui_btngrid_set_extra_colors(lui_obj_t* obj, uint16_t btn_color, uint16_t label_color, uint16_t btn_pressed_color)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	
	lui_btngrid_t* btngrid = obj->obj_main_data;

	if (btngrid->style.btn_bg_color == btn_color && btngrid->style.btn_label_color == label_color && btngrid->style.btn_pressed_color == btn_pressed_color)
		return;
	btngrid->style.btn_bg_color = btn_color;
	btngrid->style.btn_label_color = label_color;
	btngrid->style.btn_pressed_color = btn_pressed_color;
	
	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void lui_btngrid_set_btn_margin(lui_obj_t* obj, uint8_t margin_x, uint16_t margin_y)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
	
	lui_btngrid_t* btngrid = obj->obj_main_data;

	if (btngrid->style.btn_margin_hor == margin_x && btngrid->style.btn_margin_vert == margin_y)
		return;
	btngrid->style.btn_margin_hor = margin_x;
	btngrid->style.btn_margin_vert = margin_y;
	
	_lui_btngrid_calc_btn_area(obj);

	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void _lui_btngrid_calc_btn_area(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;
		
	lui_btngrid_t* btngrid = obj->obj_main_data;
	uint8_t units_in_row = 0;
	uint8_t btns_in_row = 0;
	uint8_t btn_index = 0;
	uint8_t unit_index = 0;
	uint16_t raw_height = obj->common_style.height / btngrid->row_cnt;


	float w = 0;
	uint16_t h = 0;

	for (uint16_t i = 0; i < btngrid->btn_cnt + btngrid->row_cnt; i++)
	{
		while (strcmp(btngrid->texts[i], "\n") != 0 && strcmp(btngrid->texts[i], "\0") != 0)
		{
			units_in_row += (btngrid->btn_properties[unit_index++] & _LUI_BTNGRID_MASK_BTN_WIDTH_UNIT);
			++btns_in_row;
			++i;          
		}
		
		float raw_width = (float)(obj->common_style.width) / (float)units_in_row;

		w = 0.0;
		h += raw_height;
		for (int j = 0; j < btns_in_row; j++)
		{
			lui_area_t area;
			float this_btn_w = raw_width * (float)(btngrid->btn_properties[btn_index] & _LUI_BTNGRID_MASK_BTN_WIDTH_UNIT);
			w += this_btn_w;

			area.x1 = obj->x + w - this_btn_w + btngrid->style.btn_margin_hor;
			area.x2 = obj->x + w - btngrid->style.btn_margin_hor;
			area.y1 = obj->y + h - raw_height + btngrid->style.btn_margin_vert;
			area.y2 = obj->y + h - btngrid->style.btn_margin_vert;

			btngrid->btn_area[btn_index++] = area;
			
		}
		
		btns_in_row = 0;
		units_in_row = 0;
	}

	_lui_object_set_need_refresh(obj);
	((lui_btngrid_t*)(obj->obj_main_data))->needs_full_render = 1;
}


/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_KEYBOARD related functions
 *-------------------------------------------------------------------------------
 */

static const char* kb_txt_lower_textmap[] = 
{
	"1#", "q", "w", "e", "r", "t", "y","u","i","o","p", LUI_ICON_BACKSPACE,"\n", //buttons: 0-11
	"ABC", "a", "s", "d", "f", "g", "h","j","k","l", LUI_ICON_RETURN_DOWN_BACK,"\n",   //buttons: 12-22
	"-","_","z", "x", "c","v","b","n","m",",",".",":","\n",           //buttons: 23-34
	LUI_ICON_CLOSE, LUI_ICON_ARROW_BACK, "  ", LUI_ICON_ARROW_FORWARD, LUI_ICON_CHECKMARK, "\0"
};
static const char* kb_txt_upper_textmap[] = 
{
	"1#", "Q", "W", "E", "R", "T", "Y","U","I","O","P", LUI_ICON_BACKSPACE,"\n", //BUTTONS: 0-11
	"abc", "A", "S", "D", "F", "G", "H","J","K","L", LUI_ICON_RETURN_DOWN_BACK,"\n",   //BUTTONS: 12-22
	"-","_","Z", "X", "C","V","B","N","M",",",".",":","\n",           //BUTTONS: 23-34
	LUI_ICON_CLOSE, LUI_ICON_ARROW_BACK, "  ", LUI_ICON_ARROW_FORWARD, LUI_ICON_CHECKMARK, "\0"
};
static const char* kb_spcl_textmap[] = 
{
	"1", "2", "3", "4", "5", "6", "7","8","9","0",".", LUI_ICON_BACKSPACE,"\n", //BUTTONS: 0-11
	"abc", "+", "-", "/", "*", "=", "%","!","?","#", "\\","\n",   //BUTTONS: 12-22
	"<",">","(", ")", "{","}","[","]","\"","'",";","@","\n",           //BUTTONS: 23-34
	LUI_ICON_CLOSE, LUI_ICON_ARROW_BACK, "  ", LUI_ICON_ARROW_FORWARD, LUI_ICON_CHECKMARK, "\0"
};
static const uint8_t kb_txt_propertymap[] = 
{
	5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
	6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 6, 2, 2
};

lui_obj_t* lui_keyboard_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_keyboard_t* initial_kb =  _lui_mem_alloc(sizeof(*initial_kb));
	if (initial_kb == NULL)
		return NULL;
	initial_kb->target_txtbox = NULL;
	initial_kb->keyboard_mode = LUI_KEYBOARD_MODE_TXT_LOWER;
	
	lui_obj_t* obj_btngrid = lui_btngrid_create();
	if (obj_btngrid == NULL)
		return NULL;
	((lui_btngrid_t* )(obj_btngrid->obj_main_data))->kb_data = initial_kb;
	uint16_t h = 0.4 * (float)g_lui_main->disp_drv->display_vert_res;
	lui_object_set_position(obj_btngrid, 0, (g_lui_main->disp_drv->display_vert_res - h) - 2);
	lui_object_set_area(obj_btngrid, g_lui_main->disp_drv->display_hor_res, h);
	lui_btngrid_set_textmap(obj_btngrid, kb_txt_lower_textmap);
	lui_btngrid_set_propertymap(obj_btngrid, kb_txt_propertymap);
	lui_object_set_callback(obj_btngrid, lui_keyboard_sys_cb);
	/* For keyboard, by default it's invisible */
	obj_btngrid->visible = 0;
	/* Keyboard sits in the popup layer */
	obj_btngrid->layer = LUI_LAYER_POPUP;

	return  obj_btngrid;
}

void lui_keyboard_sys_cb(lui_obj_t* obj_sender)
{
	if (obj_sender == NULL)
		return;

	if (obj_sender->obj_type != LUI_OBJ_BTNGRID)
		return;

	lui_btngrid_t* btngrid = obj_sender->obj_main_data;

	int16_t active_btn_id = btngrid->active_btn_index;
	const char* btn_text = lui_keyboard_get_key_text(obj_sender, active_btn_id);
	if (active_btn_id == -1)
		return;


	uint16_t caret_index = lui_textbox_get_caret_index(btngrid->kb_data->target_txtbox);
	if (strcmp(btn_text, LUI_ICON_CHECKMARK) == 0)
	{
		btngrid->kb_data->target_txtbox->state = LUI_STATE_IDLE;
		btngrid->kb_data->target_txtbox->event = LUI_EVENT_EXITED;
		btngrid->kb_data->target_txtbox->obj_event_cb(btngrid->kb_data->target_txtbox);
	}
	else if (strcmp(btn_text, LUI_ICON_CLOSE) == 0)
	{
		lui_textbox_t* txtbox = btngrid->kb_data->target_txtbox->obj_main_data;
		txtbox->used_chars = 1;
		txtbox->text_buffer[0] = '\0';
		txtbox->caret_index = 0;
		btngrid->kb_data->target_txtbox->needs_refresh = 1;
	}
	else if (strcmp(btn_text, LUI_ICON_ARROW_BACK) == 0)
	{
		lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, --caret_index);
	}
	else if (strcmp(btn_text, LUI_ICON_ARROW_FORWARD) == 0)
	{
		lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, ++caret_index);
	}
	else if (strcmp(btn_text, LUI_ICON_BACKSPACE) == 0)
	{
		if (caret_index > 0)
		{
			lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, --caret_index);
			lui_textbox_delete_char(btngrid->kb_data->target_txtbox);
		}
	}
	else if (strcmp(btn_text, LUI_ICON_RETURN_DOWN_BACK) == 0)
	{
		lui_textbox_insert_char(btngrid->kb_data->target_txtbox, '\n');
		lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, ++caret_index);
	}
	else if (strcmp(btn_text, "ABC") == 0 || strcmp(btn_text, "abc") == 0 || strcmp(btn_text, "1#") == 0 )
	{
		if (strcmp(btn_text, "1#") == 0)
		{
			btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_SPCL;
			btngrid->texts = kb_spcl_textmap;
		}
		else if (strcmp(btn_text, "ABC") == 0)
		{
			btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_UPPER;
			btngrid->texts = kb_txt_upper_textmap;
		}
		else
		{
			btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_LOWER;
			btngrid->texts = kb_txt_lower_textmap;
		}
		obj_sender->needs_refresh = 1;
		btngrid->needs_full_render = 1;
		g_needs_render = 1;
	}
	else
	{
		lui_textbox_insert_char(btngrid->kb_data->target_txtbox, btn_text[0]);
		lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, lui_textbox_get_caret_index(btngrid->kb_data->target_txtbox) + 1);
	}

}

void lui_keyboard_set_mode(lui_obj_t* obj, uint8_t mode)
{
	if (obj == NULL)
		return;
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return;

	lui_btngrid_t* btngrid = obj->obj_main_data;

	if (mode == LUI_KEYBOARD_MODE_TXT_LOWER)
	{
		btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_LOWER;
		btngrid->texts = kb_txt_lower_textmap;
	}
	else if (mode == LUI_KEYBOARD_MODE_TXT_UPPER)
	{
		btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_UPPER;
		btngrid->texts = kb_txt_upper_textmap;
	}
	else if (mode == LUI_KEYBOARD_MODE_TXT_SPCL)
	{
		btngrid->kb_data->keyboard_mode = LUI_KEYBOARD_MODE_TXT_SPCL;
		btngrid->texts = kb_spcl_textmap;
	}
	else
	{
		return;
	}

	obj->needs_refresh = 1;
	btngrid->needs_full_render = 1;
	g_needs_render = 1;


}

void lui_keyboard_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	lui_btngrid_set_font(obj, font);
}

const char* lui_keyboard_get_key_text(lui_obj_t* obj, uint8_t btn_index)
{
	if (obj == NULL)
		return NULL;
	// type check
	if (obj->obj_type != LUI_OBJ_BTNGRID)
		return NULL;
	
	lui_btngrid_t* btngrid = obj->obj_main_data;

	uint8_t counter = 0;
	uint8_t txt_index = 0;
	while (counter <= btn_index)
	{
		if ((strcmp(btngrid->texts[txt_index], "\0") == 0))
		{
			return NULL;
		}
		if ((strcmp(btngrid->texts[txt_index], "\n") != 0))
		{
			++counter;
		}
		++txt_index;
	}

	return btngrid->texts[--txt_index];
}

void lui_keyboard_set_target_txtbox(lui_obj_t* obj_kb, lui_obj_t* obj_txtbox)
{
	if (obj_kb == NULL)
		return;
	// type check
	if (obj_kb->obj_type != LUI_OBJ_BTNGRID || (obj_txtbox != NULL && obj_txtbox->obj_type != LUI_OBJ_TEXTBOX))
		return;

	lui_btngrid_t* btngrid_kb = obj_kb->obj_main_data;
	static uint16_t txtbox_orig_w = 0, txtbox_orig_h = 0, txtbox_orig_x = 0, txtbox_orig_y = 0;
	static uint8_t txtbox_orig_layer = 0;

	/* If previous target textbox exists, reset it to its original layer, position, and area. */
	if (btngrid_kb->kb_data->target_txtbox)
	{
		lui_object_set_area(btngrid_kb->kb_data->target_txtbox, txtbox_orig_w, txtbox_orig_h);
		lui_object_set_position(btngrid_kb->kb_data->target_txtbox, txtbox_orig_x, txtbox_orig_y);
		lui_object_set_layer(btngrid_kb->kb_data->target_txtbox, txtbox_orig_layer);
	}

	/* Now, if new target textbox is not null, store its values in static vars and apply new values */
	if (obj_txtbox)
	{
		txtbox_orig_w = obj_txtbox->common_style.width;
		txtbox_orig_h = obj_txtbox->common_style.height;
		txtbox_orig_x = obj_txtbox->x;
		txtbox_orig_y = obj_txtbox->y;
		txtbox_orig_layer = obj_txtbox->layer;

		//lui_object_set_area(obj_txtbox, obj_kb->common_style.width, g_lui_main->disp_drv->display_vert_res - obj_kb->common_style.height - 4);
		lui_object_set_area(obj_txtbox, obj_kb->common_style.width, txtbox_orig_h + 2);
		lui_object_set_position(obj_txtbox, 0, obj_kb->y - (txtbox_orig_h + 2));
		lui_object_set_layer(obj_txtbox, obj_kb->layer);
		lui_object_set_visibility(obj_kb, 1);

	}
	else
	{
		lui_object_set_visibility(obj_kb, 0);
	}

	btngrid_kb->kb_data->target_txtbox = obj_txtbox;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_TEXTBOX related functions
 *-------------------------------------------------------------------------------
 */

void lui_textbox_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	
	lui_textbox_t* txtbox = obj->obj_main_data;
	
	if (txtbox->needs_full_render == 1)
	{
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);
		if (obj->common_style.border_visible)
		{
			
			lui_gfx_draw_rect(obj->x, obj->y, obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
		}
		txtbox->needs_full_render = 0;
	}

	if (txtbox->text_buffer == NULL)
		return;

	uint8_t pad = 10;
	uint16_t caret_x = obj->x + pad;
	uint16_t caret_y = obj->y + pad;
	uint8_t caret_h = txtbox->font->bitmap->size_y;
	uint8_t caret_w = 4;
	uint8_t glyph_w = 0;
	uint8_t glyph_h = 0;

	lui_gfx_draw_string_advanced(txtbox->text_buffer, obj->x + pad, obj->y + pad, obj->common_style.width - 2*pad, 
									obj->common_style.height - 2*pad, txtbox->style.text_color, 
									obj->common_style.bg_color, 1, txtbox->font);
	

	/* No need to draw caret when textbox is in Idle state */
	if (obj->state == LUI_STATE_IDLE)
		return;
	/* Calculate caret coordinates */
	for (uint16_t i = 0; i < txtbox->caret_index; i++)
	{
		if (txtbox->text_buffer[i] == '\n')
		{
			caret_x = obj->x + pad;
			caret_y += (txtbox->font->bitmap->size_y);	//go to next row (row height = height of space)
		}
		else
		{
			// Find the glyph for the char from the font
			_lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(txtbox->text_buffer[i], txtbox->font);
			glyph_h = txtbox->font->bitmap->size_y;
			if (glyph == NULL)
				glyph_w = txtbox->font->bitmap->size_y / 2;
			else
				glyph_w = glyph->width;


			// check if not enough space available at the right side
			if (caret_x + glyph_w > obj->x + obj->common_style.width - pad)
			{
				caret_x = obj->x + pad;			//go to first col
				caret_y += glyph_h;					//go to next row
			}
			// check if not enough space available at the bottom
			if(caret_y + glyph_h > obj->y + obj->common_style.height - pad)
			{
				break;
			}
			caret_x += glyph_w;		//next char position
		}
	}

	
	if (caret_x > obj->x)
		--caret_x;
	/* Draw the caret now, only if the caret does not go out of the boundary */
	if ((caret_x + caret_w < obj->x + obj->common_style.width - pad) &&
		(caret_y + caret_h - 1 < obj->y + obj->common_style.height))
	{
		lui_gfx_draw_line(caret_x + 2, caret_y + 1, caret_x + 2, caret_y + caret_h - 1, caret_w, ~(obj->common_style.bg_color));
		//lui_gfx_draw_char(txtbox->text_buffer[txtbox->caret_index], caret_x, caret_y, obj->common_style.bg_color, txtbox->style.text_color, 1, txtbox->font);
	}
}

lui_obj_t* lui_textbox_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_textbox_t* initial_textbox =  _lui_mem_alloc(sizeof(*initial_textbox));
	if (initial_textbox == NULL)
		return NULL;
	
	initial_textbox->style.text_color = LUI_STYLE_TEXTBOX_TEXT_COLOR;
	initial_textbox->text_buffer = NULL;
	initial_textbox->caret_index = 0;
	initial_textbox->font = g_lui_main->default_font;
	initial_textbox->max_len = 0;
	initial_textbox->used_chars = 1;
	initial_textbox->needs_full_render = 1;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_TEXTBOX;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_TEXTBOX_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_TEXTBOX_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_TEXTBOX_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_TEXTBOX_WIDTH;
	obj->common_style.height = LUI_STYLE_TEXTBOX_HEIGHT;

	obj->obj_main_data = (void* )initial_textbox;

	return  obj;
}

void lui_textbox_set_caret_index(lui_obj_t* obj, uint16_t caret_index)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;
	
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;

	if (txtbox->caret_index == caret_index)
		return;

	txtbox->caret_index = caret_index;
	if (caret_index > txtbox->used_chars - 1)
		txtbox->caret_index = txtbox->used_chars - 1;
	
	_lui_object_set_need_refresh(obj);
}

uint16_t lui_textbox_get_caret_index(lui_obj_t* obj)
{
	if (obj == NULL)
		return 0;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return 0;
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	return txtbox->caret_index;
}

void lui_textbox_insert_char(lui_obj_t* obj, char c)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return;
	
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	if (txtbox->used_chars + 1 > txtbox->max_len)
		return;
	for (int32_t i = txtbox->used_chars - 1; i >= txtbox->caret_index; i--)
	{
		txtbox->text_buffer[i + 1] = txtbox->text_buffer[i];
	}
	txtbox->text_buffer[txtbox->caret_index] = c;
	++(txtbox->used_chars);

	_lui_object_set_need_refresh(obj);
}

void lui_textbox_delete_char(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return;
	
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	if (txtbox->used_chars <= 1)
		return;
	for (int32_t i = txtbox->caret_index; i < txtbox->used_chars; i++)
	{
		txtbox->text_buffer[i] = txtbox->text_buffer[i + 1];
	}
	--(txtbox->used_chars);

	_lui_object_set_need_refresh(obj);
}

void lui_textbox_insert_string(lui_obj_t* obj, char* str, uint16_t len)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return;
	lui_textbox_t* txtbox = obj->obj_main_data;
	uint16_t caret_index = txtbox->caret_index;
	for (uint16_t i = 0; i < len; i++)
	{
		if (str[i] == '\0')
			return;
		lui_textbox_insert_char(obj, str[i]);
		++txtbox->caret_index;
	}
	/* Restore the original caret index */
	txtbox->caret_index = caret_index;
	_lui_object_set_need_refresh(obj);
}

void lui_textbox_set_text_buffer(lui_obj_t* obj, char* text_buffer, uint16_t buff_size)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;

	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	txtbox->text_buffer = text_buffer;
	txtbox->max_len = buff_size;
	txtbox->text_buffer[0] = '\0';
	txtbox->used_chars = 1;
	txtbox->caret_index = 0;

	_lui_object_set_need_refresh(obj);
}

void lui_textbox_set_text_color(lui_obj_t* obj, uint16_t text_color)
{
	if (obj == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;

	lui_textbox_t* txtbox = obj->obj_main_data;
	txtbox->style.text_color = text_color;
	_lui_object_set_need_refresh(obj);
	txtbox->needs_full_render = 1;
}

void lui_textbox_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (obj == NULL)
		return;
	if (font == NULL)
		return;
	// type check
	if (obj->obj_type != LUI_OBJ_TEXTBOX)
		return;

	lui_textbox_t* txtbox = obj->obj_main_data;
	txtbox->font = font;
	_lui_object_set_need_refresh(obj);
	txtbox->needs_full_render = 1;
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

	lui_panel_t* initial_panel = _lui_mem_alloc(sizeof(*initial_panel));
	if (initial_panel == NULL)
		return NULL;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_PANEL;
	obj->enabled = 1;
	// object comon style
	obj->common_style.bg_color = LUI_STYLE_PANEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_PANEL_BORDER_COLOR;
	obj->common_style.border_visible = LUI_STYLE_PANEL_BORDER_VISIBLE;
	obj->common_style.width = LUI_STYLE_PANEL_WIDTH;
	obj->common_style.height = LUI_STYLE_PANEL_HEIGHT;
	obj->obj_main_data = (void* )initial_panel;	// will add panel specific main data later

	return obj;
}

void lui_panel_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;

	if (!(obj->visible))
		return;
	
	// for panel, just draw background and optional border
	g_lui_main->disp_drv->draw_pixels_area_cb(obj->x, obj->y, obj->common_style.width,  obj->common_style.height, obj->common_style.bg_color);
	if (obj->common_style.border_visible == 1)
		lui_gfx_draw_rect(obj->x, obj->y, obj->common_style.width, obj->common_style.height, 1, obj->common_style.border_color);
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

	lui_scene_t* initial_scene =  _lui_mem_alloc(sizeof(*initial_scene));
	if (initial_scene == NULL)
		return NULL;

	initial_scene->font = g_lui_main->default_font;
	initial_scene->bg_image = NULL;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_SCENE;
	// object common style
	obj->common_style.width =  g_lui_main->disp_drv->display_hor_res;
	obj->common_style.height =  g_lui_main->disp_drv->display_vert_res;
	obj->common_style.bg_color = LUI_STYLE_SCENE_BG_COLOR;

	
	//obj->index =  g_lui_main->total_scenes;
	obj->obj_main_data = (void* )initial_scene;

	_lui_object_set_need_refresh(obj);
	 //g_lui_main->scenes[obj->index] = obj;
	 g_lui_main->total_scenes++;

	return obj;
}

void lui_scene_draw(lui_obj_t* obj)
{
	if (obj == NULL)
		return;

	// type check
	if (obj->obj_type != LUI_OBJ_SCENE)
		return;

	if (!(obj->visible))
		return;

	lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);
	// TBD: draw background image
}

void lui_scene_set_bg_image(lui_obj_t* obj_scene, const lui_bitmap_t* image)
{
	// NOTE: image rendering is not implemented yet
	if (obj_scene == NULL)
		return;
	
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	lui_scene_t* scene = (lui_scene_t* )(obj_scene->obj_main_data);
	scene->bg_image = (lui_bitmap_t* )image;

	_lui_object_set_need_refresh(obj_scene); 
}

void lui_scene_set_font(lui_obj_t* obj_scene, const lui_font_t* font)
{
	if (obj_scene == NULL)
		return;
	if (font == NULL)
		return;
	// type check
	if (obj_scene->obj_type != LUI_OBJ_SCENE)
		return;
	
	lui_scene_t* scene = (lui_scene_t* )(obj_scene->obj_main_data);
	scene->font = font;
	
	_lui_object_set_need_refresh(obj_scene); 
}

void lui_scene_set_active(lui_obj_t* obj_scene)
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

lui_obj_t* _lui_object_create(void)
{
	lui_obj_t* obj =  _lui_mem_alloc(sizeof(*obj));
	if (obj == NULL)
		return NULL;

	obj->x = 0;
	obj->y = 0;
	obj->layer = 0;
	obj->state = LUI_STATE_IDLE;
	obj->event = LUI_EVENT_NONE;
	obj->value = 0;
	obj->obj_event_cb = NULL;
	obj->needs_refresh = 1;
	obj->visible = 1;
	obj->enabled = 0;
	//obj->index_in_pool = -1;
	obj->parent = NULL;
	obj->first_child = NULL;
	obj->next_sibling = NULL;
	obj->children_count = 0;
	g_needs_render = 1;
	return obj;
}

/**
 * @brief Add a child object to a parent object, thus grouping them together
 * 
 * @param obj child object
 * @param parent_obj parent object
 */
void lui_object_add_to_parent(lui_obj_t* obj_child, lui_obj_t* obj_parent)
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
		lui_obj_t* next_child = obj_parent->first_child;
		
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

/**
 * @brief Remove an object from its current parent
 * 
 * @param obj Child object
 */
void lui_object_remove_from_parent(lui_obj_t* obj)
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
		lui_obj_t* child = obj->parent->first_child;
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

/**
 * @brief Set position of an object
 * 
 * @param obj target object
 * @param x x position
 * @param y y position
 */
void lui_object_set_position(lui_obj_t* obj, uint16_t x, uint16_t y)
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
	
	lui_obj_t* child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t* obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
		obj_stack[stack_counter++] = child_of_root;
		child_of_root = child_of_root->next_sibling;
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t* child = obj_stack[--stack_counter]; 

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

/**
 * @brief Set only x position of an object
 * 
 * @param obj target object
 * @param x x position
 */
void lui_object_set_x_pos(lui_obj_t* obj, uint16_t x)
{
	if (obj == NULL)
		return;
	lui_object_set_position(obj, x, obj->y);
}

/**
 * @brief Set only y position of an object
 * 
 * @param obj target object
 * @param y y position
 */
void lui_object_set_y_pos(lui_obj_t* obj, uint16_t y)
{
	if (obj == NULL)
		return;
	lui_object_set_position(obj, obj->x, y);
}

/**
 * @brief Set drawing area of an object
 * 
 * @param obj target object
 * @param width width
 * @param height height
 */
void lui_object_set_area(lui_obj_t* obj, uint16_t width, uint16_t height)
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

	/* If object is an button grid, we need to recalculate the layout when area is changed
	 * But, we must check if text map is not null.
	 */
	if (obj->obj_type == LUI_OBJ_BTNGRID && ((lui_btngrid_t* )(obj->obj_main_data))->texts != NULL)
	{
		_lui_btngrid_calc_btn_area(obj);
	}

}

/**
 * @brief Set width of an object
 * 
 * @param obj target object
 * @param width width
 */
void lui_object_set_width(lui_obj_t* obj, uint16_t width)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(obj, width, obj->common_style.height);
}

/**
 * @brief Set height of an object
 * 
 * @param obj target object
 * @param height height
 */
void lui_object_set_height(lui_obj_t* obj, uint16_t height)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(obj, obj->common_style.width, height);
}

/**
 * @brief Set border color of an object
 * 
 * @param obj target object
 * @param border_color border color
 */
void lui_object_set_border_color(lui_obj_t* obj, uint16_t border_color)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_color == border_color)
		return;
	obj->common_style.border_color = border_color;
	_lui_object_set_need_refresh(obj);
}

/**
 * @brief Set border's visibility of an object
 * 
 * @param obj target object
 * @param is_visible 1: visible; 0: invisible
 */
void lui_object_set_border_visibility(lui_obj_t* obj, uint8_t is_visible)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_visible == is_visible)
		return;
	obj->common_style.border_visible = (is_visible == 0) ? 0 : 1;
	_lui_object_set_need_refresh(obj);
}

/**
 * @brief Set background color of an object
 * 
 * @param obj target object
 * @param bg_color background color
 */
void lui_object_set_bg_color(lui_obj_t* obj, uint16_t bg_color)
{
	if (obj == NULL)
		return;
	if (obj->common_style.bg_color == bg_color)
		return;
	obj->common_style.bg_color = bg_color;
	_lui_object_set_need_refresh(obj);
}

/**
 * @brief Set event call back function for input handling
 * 
 * This function is called when an event occurs against this object
 * 
 * @param obj target object
 * @param obj_event_cb function pointer of the callback function
 */
void lui_object_set_callback(lui_obj_t* obj, void (*obj_event_cb)(lui_obj_t* ))
{
	if (obj == NULL)
		return;
	obj->obj_event_cb = obj_event_cb;
}

/**
 * @brief Get the input state of an object
 * 
 * @param obj target object
 * @return int8_t state ID
 */
int8_t lui_object_get_state(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	return obj->state;
}

/**
 * @brief Get input event of an object
 * 
 * @param obj target object
 * @return int8_t event ID
 */
int8_t lui_object_get_event(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	return obj->event;
}

/**
 * @brief Set visibility of an object
 * 
 * @param obj target object
 * @param is_visible 1: visible; 0: hidden
 */
void lui_object_set_visibility(lui_obj_t* obj, uint8_t is_visible)
{
	if (obj == NULL)
		return;

	// already flag is same as `visible`, no need to waste time in loop. Return.
	if (obj->visible == is_visible)
		return;

	obj->visible = is_visible;

	/**
	 * @brief When object becomes visible, set needs_refresh bit for itself recursively. 
	 * When object becomes invisble, set needs_refresh bit for its parent recursively.
	 */
	if (is_visible)
	{
		_lui_object_set_need_refresh(obj);
		if (obj->obj_type == LUI_OBJ_BTNGRID)
		{
			((lui_btngrid_t* )(obj->obj_main_data))->needs_full_render = 1;
		}
		else if (obj->obj_type == LUI_OBJ_TEXTBOX)
		{
			((lui_textbox_t* )(obj->obj_main_data))->needs_full_render = 1;
		}
	}
	else
	{
		_lui_object_set_need_refresh(obj->parent);
	}
}

/**
 * @brief Enable or disble input handling of an object. If disabled,
 * object won't cause input event callback
 * 
 * @param obj target object
 * @param is_enabled 1: input enabled; 0: input disabled
 * @return uint8_t 1: success; 0: failed
 */
uint8_t lui_object_set_enable_input(lui_obj_t* obj,  uint8_t is_enabled)
{
	if (obj == NULL)
		return 0;

	if (obj->obj_type == LUI_OBJ_BUTTON ||
		obj->obj_type == LUI_OBJ_SWITCH ||
		obj->obj_type == LUI_OBJ_CHECKBOX ||
		obj->obj_type == LUI_OBJ_SLIDER)
	{
		obj->enabled = is_enabled;
	}
	else
	{
		obj->enabled = 0;
	}
	return obj->enabled;
}

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
void lui_object_set_layer(lui_obj_t* obj, uint8_t layer_index)
{
	if (obj == NULL)
		return;
	if (obj->obj_type == LUI_OBJ_SCENE)
		return;

	if (obj->parent != NULL)
	{
		obj->layer = obj->parent->layer + layer_index;
		if (obj->layer < obj->parent->layer)
			obj->layer = obj->parent->layer;
	}
	else
	{
		obj->layer = layer_index;
	}
	
	lui_obj_t* child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t* obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
		obj_stack[stack_counter++] = child_of_root;
		child_of_root = child_of_root->next_sibling;
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t* child = obj_stack[--stack_counter]; 

			child->layer = child->layer + obj->layer;

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

	// object's layer is changed, so parent must be redrawn
	_lui_object_set_need_refresh(obj->parent);
}

/**
 * @brief Get rendering layer index of an object
 * 
 * @param obj target object
 * @return int16_t layer index. Returns -1 if object is NULL
 */
int16_t lui_object_get_layer(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	
	return obj->layer;
}

/**
 * @private
 * @brief Compares two objects' layers and returns (layer1 - layer2)
 * 
 * @param p1 pointer to object 1
 * @param p2 pointer to object 2
 * @return int (layer1 - layer2)
 */
static int _lui_obj_layer_cmprtr(const void* p1, const void* p2)
{
	int l1 = (*((lui_obj_t** )p1))->layer;
	int l2 = (*((lui_obj_t** )p2))->layer;
	// int t1 = (*((lui_obj_t** )p1))->obj_type;
	// int t2 = (*((lui_obj_t** )p2))->obj_type;

	return (l1 - l2);
}

/**
 * @private
 * @brief Render an object along with all its children (if any)
 * 
 * @param obj_parent target object
 */
void _lui_object_render_parent_with_children(lui_obj_t* obj_parent)
{
	if (obj_parent == NULL)
		return;
	if (!obj_parent->visible)
		return;


	lui_obj_t* obj_arr[LUI_MAX_OBJECTS];
	int16_t arr_counter = 0;

	/* first render the parent, then render all its children in a loop */
	if (obj_parent->layer > 0)
		obj_arr[arr_counter++] = obj_parent;
	else
		_lui_object_render(obj_parent);
	
	/*
	 * NOTE: objects are added to render stack only if they're visible.
	 * That means, if a parent is not visible, its children also won't be 
	 * added to the render stack, even if those children are visible.
	 */

	lui_obj_t* child_of_root = obj_parent->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t* obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
		/* Push child in stack, but only if it's visible */
		if (child_of_root->visible)
		{
			obj_stack[stack_counter++] = child_of_root;
		}
		child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			/* pop from stack */
			lui_obj_t* child = obj_stack[--stack_counter]; 

			if (child->layer > 0)
				obj_arr[arr_counter++] = child;
			else
				_lui_object_render(child);

			/* get the child of current object */
			child = child->first_child;
			/* push all children of current object into stack too */
			while (child != NULL)
			{
				/* push child to stack, but only if it is visible */
				if (child->visible)
				{
					obj_stack[stack_counter++] = child;
				}
				/* get sibling of the child */
				child = child->next_sibling;
			}
		}
	}
	/* Sort the objects based on their layers. bottom -> top */
	qsort((void* )obj_arr, arr_counter, sizeof(obj_arr[0]), _lui_obj_layer_cmprtr);

	for (uint8_t i = 0; i < arr_counter; i++)
	{
		_lui_object_render(obj_arr[i]);
	}
	
	g_needs_render = 0;
}

/**
 * @private
 * @brief Render a single object (and NOT its children)
 * 
 * @param obj target object
 */
void _lui_object_render(lui_obj_t* obj)
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
			case LUI_OBJ_BTNGRID:
				lui_btngrid_draw(obj);
				break;
			case LUI_OBJ_TEXTBOX:
				lui_textbox_draw(obj);
				break;
			default:
				break;
		}

		obj->needs_refresh = 0;	// drawing is done, so set need_refresh back to 0
	}
}

/**
 * @private
 * @brief Set needs_refresh flag for an object. This flag determines if onject will be redrawn
 * 
 * When setting this flag for an object, flags of children and/or parent might also be set,
 * depending on the requirement.
 * 
 * @param obj target object
 */
void _lui_object_set_need_refresh(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	/* already flag is 1, no need to waste time in loop. Return. */
	if (obj->needs_refresh == 1)
		return;
	/* Object's visibility is 0, return */
	if (!obj->visible)
		return;

	obj->needs_refresh = 1;


	/*
	 * NOTE: needs_refresh_bit is set to 1 only when object is visible.
	 * If a parent is invisible, its children's needs_refresh bit won't be changed too.
	 */

	lui_obj_t* child_of_root = obj->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t* obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
		// push child to stack, but only if it's visible
		if (child_of_root->visible)
		{
			obj_stack[stack_counter++] = child_of_root;
		}
		child_of_root = child_of_root->next_sibling;

		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t* child = obj_stack[--stack_counter]; 
			child->needs_refresh = 1;

			/* When child is either btngrid or textbox, set needs_full_render bit to 1 */
			if (child->obj_type == LUI_OBJ_BTNGRID)
			{
				((lui_btngrid_t* )(child->obj_main_data))->needs_full_render = 1;
			}
			else if (child->obj_type == LUI_OBJ_TEXTBOX)
			{
				((lui_textbox_t* )(child->obj_main_data))->needs_full_render = 1;
			}

			// get the child of current object
			child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				/* push child to stack, but only if it's visible */
				if (child->visible)
				{
					obj_stack[stack_counter++] = child;
				}
				// get sibling of the child
				child = child->next_sibling;
			}
		}
	}

	g_needs_render = 1;
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * 			INPUT processing functions
 *------------------------------------------------------------------------------
 */

lui_obj_t* _lui_process_input_of_act_scene()
{	
	uint8_t input_dev_type = 0;

	if ( g_lui_main->touch_input_dev != NULL)
	{
		input_dev_type = LUI_INPUT_TYPE_TOUCH;
	}
	else
	{
		return NULL;
	}

	uint8_t scan_all_objs_flag = 0;
	lui_obj_t* obj_caused_cb = NULL;
	lui_obj_t* last_active_obj =  g_lui_main->active_obj;
	lui_scene_t* scene_main_data = (lui_scene_t* )( g_lui_main->active_scene->obj_main_data);
	lui_touch_input_data_t input_touch_data;
	uint8_t input_is_pressed = 0;
	
	
	if (input_dev_type == LUI_INPUT_TYPE_TOUCH)
	{
		g_lui_main->touch_input_dev->read_touch_input_cb(&input_touch_data);
		input_is_pressed = input_touch_data.is_pressed;
	}
	

	/* If previous "pressed" value is 1 and now it's 0, that means a "click" happened */
	if (g_lui_main->input_state_pressed && !input_is_pressed)
	{
		g_lui_main->input_event_clicked = 1;
	}
	else
	{
		g_lui_main->input_event_clicked = 0;
	}
	g_lui_main->input_state_pressed = input_is_pressed;
	

	if (last_active_obj == NULL)
	{
		scan_all_objs_flag = 1;
	}

	else
	{
		// sets object parameters based on input. also may modify  g_lui_main->active_obj
		if ( input_dev_type == LUI_INPUT_TYPE_TOUCH)
		{
			_lui_set_obj_props_on_touch_input(&input_touch_data, last_active_obj);
		}

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
		obj_caused_cb = _lui_scan_all_obj_for_input(&input_touch_data, g_lui_main->active_scene, last_active_obj);

		if (obj_caused_cb == NULL)
		{
			obj_caused_cb = last_active_obj;
		}
	}
	
	return obj_caused_cb;

}

lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj_root, lui_obj_t* obj_excluded)
{
	// Note: This function is made by converting a tail-recursive function to iterative function
	// The simple way is to use a stack.
	// see the answer: https://codereview.stackexchange.com/a/163621

	if (obj_root->obj_type != LUI_OBJ_SCENE)
	{
		if (obj_root == obj_excluded || !(obj_root->enabled) || !(obj_root->visible))
			return NULL;
	}

	lui_obj_t* obj_caused_cb = NULL;
	uint8_t popup_layer_exists = 0;
	lui_obj_t* obj_arr[LUI_MAX_OBJECTS];
	int16_t arr_counter = 0;

	/* obj_arr[arr_counter++] = obj_root;
	if (obj_root->layer == 255)
		popup_layer_exists = 1; */

	lui_obj_t* child_of_root = obj_root->first_child;
	while (child_of_root != NULL)
	{
		lui_obj_t* obj_stack[LUI_MAX_OBJECTS] = {NULL};
		uint8_t stack_counter = 0;
		if ((child_of_root != obj_excluded) && child_of_root->enabled && child_of_root->visible)
		{
			obj_stack[stack_counter++] = child_of_root;
		}
		child_of_root = child_of_root->next_sibling;
		
		// loop until stack is empty. in this way all children (and their children too) will be traversed
		while (stack_counter > 0)
		{
			// pop from stack
			lui_obj_t* child = obj_stack[--stack_counter]; 
			obj_arr[arr_counter++] = child;
			if (child->layer == 255)
				popup_layer_exists = 1;

			// get the child of current object
			child = child->first_child;
			// push all children of current object into stack too
			while (child != NULL)
			{
				// push child to stack, only if it's not excluded
				if ((child != obj_excluded) && child->enabled && child->visible)
				{
					obj_stack[stack_counter++] = child;
				}
				// get sibling of the child
				child = child->next_sibling;
			}
		}
	}

	/* Sort the objects based on their layers. bottom -> top */
	qsort((void* )obj_arr, arr_counter, sizeof(obj_arr[0]), _lui_obj_layer_cmprtr);

	/* Scan for inputs from top layer to bottom layer */
	while(arr_counter--)
	{
		if (popup_layer_exists && obj_arr[arr_counter]->layer < 255)
			return NULL;
		obj_caused_cb = _lui_scan_individual_object_for_input(touch_input_data, obj_arr[arr_counter]);
		if (obj_caused_cb != NULL)
			return obj_caused_cb;
	}

	return obj_caused_cb;
}

lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t* touch_input_data, lui_obj_t* obj)
{
	lui_obj_t* obj_caused_cb = NULL;
	/* if (!(obj->enabled) || !(obj->visible))
	{
		return NULL;
	} */

	if (touch_input_data != NULL) // Touch input
	{
		// sets object parameters based on input. also, may modify  g_lui_main->active_obj
		_lui_set_obj_props_on_touch_input(touch_input_data, obj);
	}
	
	if (obj->event != LUI_EVENT_NONE)
	{
		obj_caused_cb = obj;
	}
	else
	{
		obj_caused_cb = NULL;
	}


	return obj_caused_cb;
}

uint8_t _lui_check_if_active_obj_touch_input(lui_touch_input_data_t* input, lui_obj_t* obj)
{
	uint8_t is_active = 0;

	if (input->x >= obj->x && 
		input->x < obj->x + obj->common_style.width &&
		input->y >= obj->y &&
		input->y < obj->y + obj->common_style.height)
	{
		
		if (obj->obj_type == LUI_OBJ_BTNGRID)
		{
			lui_btngrid_t* btngrid = obj->obj_main_data;
			for (uint16_t i = 0; i < btngrid->btn_cnt; i++)
			{
				if (input->x >= btngrid->btn_area[i].x1 && 
					input->x <  btngrid->btn_area[i].x2 &&
					input->y >= btngrid->btn_area[i].y1 &&
					input->y <  btngrid->btn_area[i].y2)
				{
					if (!(btngrid->btn_properties[i] & _LUI_BTNGRID_MASK_BTN_IS_DISABLED) &&
						!(btngrid->btn_properties[i] & _LUI_BTNGRID_MASK_BTN_IS_HIDDEN))
					{
						btngrid->active_btn_index = i;
						g_lui_main->active_obj = obj;
						is_active = 1;
					}
					break;
				}
			}
		}
		else
		{
			g_lui_main->active_obj = obj;
			is_active = 1;
		}
	}

	if (is_active == 0)
	{
		// in case input is not on "obj" and previous "active_obj" is same as "obj",
		// set "input_on_obj" to NULL.
		if (g_lui_main->active_obj == obj)
		{
			g_lui_main->active_obj = NULL;
			if (obj->obj_type == LUI_OBJ_BTNGRID)
			{
				((lui_btngrid_t* )(obj->obj_main_data))->active_btn_index = -1;
			}
		}
	}

	return is_active;
}

void _lui_set_obj_props_on_touch_input(lui_touch_input_data_t* input, lui_obj_t* obj)
{
	uint8_t is_obj_active = _lui_check_if_active_obj_touch_input(input, obj);
	uint8_t new_state = LUI_STATE_IDLE;
	uint8_t old_state = obj->state;

	if (is_obj_active == 1)
	{
		// if pressed, then....well, then state = PRESSED
		if (input->is_pressed == 1)
		{
			new_state = LUI_STATE_PRESSED;
		}	
		// else not pressed, state = SELECTED
		else
		{
			new_state = LUI_STATE_SELECTED;
		}
	}
	else
	{
		new_state = LUI_STATE_IDLE;
	}
	if (obj->obj_type == LUI_OBJ_TEXTBOX && obj->state == LUI_STATE_ENTERED)
	{
		new_state = LUI_STATE_ENTERED;
	}
	obj->event = _lui_get_event_against_state(new_state, old_state);		

	if (obj->event != LUI_EVENT_NONE)
	{
		obj->state = new_state;
		if (obj->obj_type == LUI_OBJ_BTNGRID)
		{
			lui_btngrid_t* btngrid = obj->obj_main_data;
			btngrid->btn_properties[btngrid->active_btn_index] |= (new_state & _LUI_BTNGRID_MASK_BTN_IS_CHECKED);
		}
		_lui_object_set_need_refresh(obj);
	}
	

	// Special case for TextBox. When clicked on a textbox, the state becomes ENTERED
	// To EXIT the state, the close or ok button from keyboard must be pressed
	if (obj->obj_type == LUI_OBJ_TEXTBOX)
	{
		if (obj->event == LUI_EVENT_PRESSED)
		{
			obj->state = LUI_STATE_ENTERED;
			obj->event = LUI_EVENT_ENTERED;
		}
		else
		{
			obj->state = old_state;
			obj->event = LUI_EVENT_NONE;
		}
	}

	// Special case for switch and checkbox: if event is LUI_EVENT_PRESSED, then set event to LUI_EVENT_VALUE_CHANGED
	// then set the value to `value` property
	else if (obj->obj_type == LUI_OBJ_SWITCH ||
		obj->obj_type == LUI_OBJ_CHECKBOX)
	{	
		if (obj->event == LUI_EVENT_RELEASED || obj->event == LUI_EVENT_SELECTION_LOST)
		{
			obj->event = LUI_EVENT_VALUE_CHANGED;	// for switch and checkbox, being pressed means being toggled, thus value changed
			obj->value = (obj->value == 1) ? 0 : 1;	// toggle the value (1->0 or 0-1)
			obj->needs_refresh = 1;
			g_needs_render = 1;
		}	
	}

	/*
	 * Special case for slider: If knob is kep pressed and if input pos is not same as knob's current position,
	 * set new position to knob  and value to slider, also set VALUE_CHANGED event 
	 */
	else if (obj->obj_type == LUI_OBJ_SLIDER)
	{
		lui_slider_t* slider = obj->obj_main_data;
		if (obj->state == LUI_STATE_PRESSED &&
			input->x != (obj->x + slider->knob_center_rel_x))
		{
			uint16_t max_knob_center_actual_x = obj->x + obj->common_style.width - (slider->style.knob_width / 2);
			uint16_t min_knob_center_actual_x = obj->x + (slider->style.knob_width / 2);

			// cap to minimum/maximum allowed position to prevent the knob from going out of boundary
			if (input->x > max_knob_center_actual_x)
			{
				slider->knob_center_rel_x = max_knob_center_actual_x - obj->x;
			}
			else if (input->x < min_knob_center_actual_x)
			{
				slider->knob_center_rel_x = min_knob_center_actual_x - obj->x;
			}
			else
			{
				slider->knob_center_rel_x = input->x - obj->x;
			}
			
			obj->value = _lui_map_range(slider->knob_center_rel_x, obj->common_style.width - (slider->style.knob_width / 2), (slider->style.knob_width / 2), slider->range_max, slider->range_min);
			obj->event = LUI_EVENT_VALUE_CHANGED;
			obj->needs_refresh = 1;
			g_needs_render = 1;
		}
	}

	/*
	 * When a different button in a grid is active than the previous one,
	 * then the state of btngrid doesn't change and so no event occurs. 
	 * To handle this, when we detect btn index change, we compare current or old state with STATE_IDLE
	 * so we get a proper event.
	 * To visualize the difference, comment out the below block and move the mouse really fast in a 
	 * densely populated btngrid(like qwerty kb). You'll see the previous button remains selected
	 * even though mouse is on a different button.
	 */
	else if (obj->obj_type == LUI_OBJ_BTNGRID)
	{
		static int16_t last_active_btn = -1;
		lui_btngrid_t* btngrid = obj->obj_main_data;
		if (btngrid->active_btn_index != last_active_btn)
		{
			if (btngrid->active_btn_index == -1)
			{
				obj->event = _lui_get_event_against_state(LUI_STATE_IDLE, old_state);	
			}
			else
			{
				obj->event = _lui_get_event_against_state(new_state, LUI_STATE_IDLE);
			}		
		}

		if (obj->event == LUI_EVENT_PRESSED)
		{
			uint8_t is_checkable = btngrid->btn_properties[btngrid->active_btn_index] & _LUI_BTNGRID_MASK_BTN_IS_CHECKABLE;
			if (is_checkable)
			{
				obj->event = LUI_EVENT_CHECK_CHANGED;	
				//uint8_t prop = btngrid->btn_properties[btngrid->active_btn_index];
				btngrid->btn_properties[btngrid->active_btn_index] ^= _LUI_BTNGRID_MASK_BTN_IS_CHECKED;	// toggle check state of the active button
				//prop = btngrid->btn_properties[btngrid->active_btn_index];
			}
		}	

		if (obj->event != LUI_EVENT_NONE)
		{
			obj->needs_refresh = 1;
			g_needs_render = 1;
		}
		last_active_btn = btngrid->active_btn_index;
	}
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
	lui_dispdrv_t* initial_disp_drv =  _lui_mem_alloc(sizeof(*initial_disp_drv));
	if (initial_disp_drv == NULL)
		return NULL;

	initial_disp_drv->draw_pixels_area_cb = NULL;
	initial_disp_drv->render_complete_cb = NULL;
	initial_disp_drv->display_hor_res = 320;		//horizontal 320px default
	initial_disp_drv->display_vert_res = 240;	//vertical 240px default

	return initial_disp_drv;
}

void lui_dispdrv_register(lui_dispdrv_t* dispdrv)
{
	if (dispdrv == NULL)
		return;
	 g_lui_main->disp_drv = dispdrv;
}

void lui_dispdrv_set_resolution(lui_dispdrv_t* dispdrv, uint16_t hor_res, uint16_t vert_res)
{
	if (dispdrv == NULL)
		return;
	dispdrv->display_hor_res = hor_res;
	dispdrv->display_vert_res = vert_res;
}

void lui_dispdrv_set_draw_pixels_area_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color))
{
	if (dispdrv == NULL)
		return;
	dispdrv->draw_pixels_area_cb = draw_pixels_area_cb;
}

void lui_dispdrv_set_render_complete_cb(lui_dispdrv_t* dispdrv, void (*render_complete_cb)())
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
	lui_touch_input_dev_t* initial_touch_inputdev =  _lui_mem_alloc(sizeof(*initial_touch_inputdev));
	if (initial_touch_inputdev == NULL)
		return NULL;

	initial_touch_inputdev->read_touch_input_cb = NULL;
	// initial_touch_inputdev.touch_data.is_pressed = 0;
	// initial_touch_inputdev.touch_data.x = 0;
	// initial_touch_inputdev.touch_data.y = 0;

	return initial_touch_inputdev;
}

void lui_touch_inputdev_register (lui_touch_input_dev_t* inputdev)
{
	if (inputdev == NULL)
		return;
	 g_lui_main->touch_input_dev = inputdev;
}

void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* inputdata))
{
	if (inputdev == NULL)
		return;
	inputdev->read_touch_input_cb = read_touch_input_cb;
}
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * 			Graphics Functions
 *------------------------------------------------------------------------------
 */

void lui_gfx_draw_string_advanced(const char* str, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font)
{
	uint16_t x_temp = x;
	uint16_t y_temp = y;
    const _lui_glyph_t *glyph;

	if (w == 0 || h == 0)
	{
		uint16_t max_w = w == 0 ? g_lui_main->disp_drv->display_hor_res - x : w;
		uint16_t max_h = h == 0 ? g_lui_main->disp_drv->display_vert_res - y : h;
		uint16_t area[2] = {0, 0};

		lui_gfx_get_string_dimension(str, font, max_w, area);
		area[1] = area[1] > max_h ? max_h : area[1];

		w = w == 0 ? area[0] : w;
		h = h == 0 ? area[1] : h;
	}
	if (is_bg)
	{
		lui_gfx_draw_rect_fill(x, y, w, h, bg_color);
	}
	
	// Scan chars one by one from the string
	//char
	while (*str != '\0')
	{
		if (*str == '\n')
		{
			x_temp = x;					//go to first col
			y_temp += (font->bitmap->size_y);	//go to next row (row height = height of space)
		}
		else
		{
			uint8_t glyph_width = 0;
            glyph = _lui_gfx_get_glyph_from_char(*str, font);
			if (glyph == NULL)
				glyph_width = font->bitmap->size_y / 2;
			else
				glyph_width = glyph->width;
			// check if not enough space available at the right side
			if (x_temp + glyph_width > x + w)
			{
				x_temp = x;					//go to first col
				y_temp += font->bitmap->size_y;	//go to next row
			}

			// check if not enough space available at the bottom
			if(y_temp + font->bitmap->size_y > y + h)
				return;

			_lui_gfx_render_char_glyph(x_temp, y_temp, fore_color, 0, 0, glyph, font);

			x_temp += glyph_width;		//next char position
		}

        str++;
	}
}

void lui_gfx_draw_string_simple(const char* str, uint16_t x, uint16_t y, uint16_t fore_color, const lui_font_t* font)
{
	lui_gfx_draw_string_advanced(str, x, y, 0, 0, fore_color, 0, 0, font);
}

void lui_gfx_draw_char(char c, uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font)
{
	if (c == '\0')
		return;
	_lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(c, font);
	_lui_gfx_render_char_glyph(x, y, fore_color, bg_color, is_bg, glyph, font);
}

/*
 * Brehensen's algorithm is used.
 * Draw line between ANY two points.
 * Not necessarily start points has to be less than end points.
 */
void lui_gfx_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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
				_lui_gfx_plot_line_low(x1, y1, x0, y0, line_width, color);
			else
				_lui_gfx_plot_line_low(x0, y0, x1, y1, line_width, color);
		}

		else
		{
			if (y0 > y1)
				_lui_gfx_plot_line_high(x1, y1, x0, y0, line_width, color);
			else
				_lui_gfx_plot_line_high(x0, y0, x1, y1, line_width, color) ;
		}
	}

}

/*
 * Draw a rectangle with a given color and line width
 */
void lui_gfx_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color)
{
	uint16_t x_new = x+w-1;
	uint16_t y_new = y+h-1;
	lui_gfx_draw_line(x, y, x_new, y, line_width, color);
	lui_gfx_draw_line(x_new, y, x_new, y_new, line_width, color);
	lui_gfx_draw_line(x, y_new, x_new, y_new, line_width, color);
	lui_gfx_draw_line(x, y, x, y_new, line_width, color);
}

/*
 * Fill a rectangular area with a color
 */
void lui_gfx_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	 g_lui_main->disp_drv->draw_pixels_area_cb(x, y, w, h, color);
}

/*
 * Get the width and height of a string (in pixels).
 * Width: by adding up the width of each glyph (representing a character)
 * Height: Height of any glyph (representing a character)
 */
void lui_gfx_get_string_dimension(const char* str, const lui_font_t* font, uint16_t max_w, uint16_t* str_dim)
{
	str_dim[0] = 0;	// -> width
	str_dim[1] = 0;	// -> height

	uint8_t needs_wrap = 0;
	uint16_t temp_w = 0;
	uint16_t temp_w_highest = 0;
	// height is the height of space
	uint16_t temp_h = font->bitmap->size_y;

	// Scan chars one by one from the string
	while (*str != '\0')
	{
		if (*str == '\n')
		{
			temp_h += font->bitmap->size_y;
			temp_w = 0;
		}
		else
		{
            uint8_t glyph_width = 0;
            const _lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(*str, font);
			if (glyph == NULL)
				glyph_width = font->bitmap->size_y / 2;
			else
				glyph_width = glyph->width;

            // Add width of glyphs
            if (temp_w + glyph_width > max_w)
            {
                temp_h += font->bitmap->size_y;
                temp_w = 0;
                needs_wrap = 1;
            }

            temp_w += glyph_width;
            temp_w_highest = temp_w_highest < temp_w ? temp_w : temp_w_highest;
		}

        str++;
	}
	str_dim[0] = needs_wrap ? max_w : temp_w_highest;
	str_dim[1] = temp_h; 
}


uint16_t lui_rgb(uint16_t red, uint16_t green, uint16_t blue)
{
	return LUI_RGB(red, green, blue);
}

_lui_glyph_t* _lui_gfx_get_glyph_from_char(char c, const lui_font_t* font)
{
	_lui_glyph_t* glyph = NULL;
	uint8_t i = 0;
	while (i < font->glyph_count)
	{
		if (font->glyphs[i].character == c)
		{
			glyph = &(font->glyphs[i]);
			break;
		}
		
		++i;
	}
	return glyph;
}

/*
 * Draws a character glyph in top-to-bottom, left-to-right order
 * Monochrome fonts generated with lcd-image-converter software are supported only
 * Font must be generated by scanning from left to right
 *
 * Returns the last written pixel's X position
 */
void _lui_gfx_render_char_glyph(uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const _lui_glyph_t* glyph, const lui_font_t* font)
{
	if (font == NULL)
		return;
	
	if (glyph == NULL)
	{
		lui_gfx_draw_rect_fill(x, y, font->bitmap->size_y / 2, font->bitmap->size_y, fore_color);
		return;
	}

	uint16_t temp_x = x;
	uint16_t temp_y = y;

	uint16_t width = 0;
	uint16_t index_offset = 0;
	width = glyph->width;
	index_offset = glyph->payload_index;//((height / 8) + (height % 8 ? 1 : 0)) * x_offset;


    uint8_t mask = 0x80;
    uint8_t bit_counter = 0;
    for (uint8_t w = 0; w < width; w++)
    {
        bit_counter = 0;
        for (uint8_t h = 0; h < font->bitmap->size_y; h++)
        {
            if (bit_counter >= 8)   
            {
                ++index_offset;
                bit_counter = 0;
            }
            uint8_t bit = mask & (font->bitmap->payload[index_offset] << bit_counter);
            if (bit)
            {
                g_lui_main->disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, fore_color);
            }
            else
            {
                if (is_bg)
                    g_lui_main->disp_drv->draw_pixels_area_cb(temp_x, temp_y, 1, 1, bg_color);
            }
            ++bit_counter;
            ++temp_y;
        }
        ++index_offset;
        ++temp_x;
        temp_y = y;
    }
}

/*
 * When dy < 0
 * It's called only by line_draw function. Not for user
 */
void _lui_gfx_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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
void _lui_gfx_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
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

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
 * 			Other Helper Functions
 *------------------------------------------------------------------------------
 */

lui_font_t* _lui_get_font_from_active_scene()
{
	if ( g_lui_main->active_scene == NULL)
		return NULL;
	lui_scene_t* act_scene = (lui_scene_t* )( g_lui_main->active_scene->obj_main_data);
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

void _lui_mem_init(uint8_t mem_block[], uint16_t size)
{
	g_mem_block.mem_block = mem_block;
	g_mem_block.block_max_sz = size;
	g_mem_block.mem_allocated = 0;
}

void* _lui_mem_alloc(uint16_t element_size)
{
	if (g_mem_block.mem_allocated + element_size > g_mem_block.block_max_sz)
		return NULL;
	uint8_t* nxt_addr = g_mem_block.mem_block + g_mem_block.mem_allocated;
	g_mem_block.mem_allocated += element_size;
	return nxt_addr;
}
/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*------------------------------------------------------------------------------
 * 			Default Font data
 *------------------------------------------------------------------------------
 */
/* Default Font data. DON'T EDIT!*/
static const uint8_t lui_default_payload[4164] ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE6,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x10,0x00,0x02,0x1E,0x00,0x03,0xF0,0x00,0x1E,0x10,0x00,0x02,0x10,0x00,0x02,0x1E,0x00,0x03,0xF8,
0x00,0x1E,0x10,0x00,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x02,0x00,0x0C,0x82,0x00,0x08,0x42,0x00,0x38,0x43,0x80,0x08,0x42,0x00,0x08,0x26,0x00,
0x08,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x10,0x80,0x00,0x10,0x82,0x00,0x10,0x8C,0x00,0x0F,0x18,0x00,0x00,0x60,0x00,0x01,0x80,0x00,0x06,
0x3C,0x00,0x0C,0x42,0x00,0x10,0x42,0x00,0x00,0x42,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x0F,0x44,0x00,0x11,0x82,0x00,0x10,0xC2,
0x00,0x10,0xC2,0x00,0x11,0x22,0x00,0x0E,0x14,0x00,0x00,0x08,0x00,0x00,0x74,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x03,0xFC,0x00,0x0C,0x03,0x00,0x30,0x00,0xC0,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x30,0x00,0xC0,0x0C,0x03,0x00,0x03,
0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x80,0x00,0x03,0x80,0x00,0x1E,0x00,0x00,0x03,0x80,0x00,0x04,0x80,0x00,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x03,0xFC,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0x00,0x07,0x00,0x00,0x38,0x00,0x01,0xC0,0x00,0x0E,0x00,0x00,0x70,0x00,0x00,0x40,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF8,0x00,0x0C,0x0C,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x0C,0x0C,0x00,0x07,0xF8,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x08,0x00,0x00,0x1F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x10,0x1A,0x00,0x10,0x32,0x00,0x10,
0x42,0x00,0x18,0x82,0x00,0x0F,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x02,0x00,0x10,0x02,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x19,0x82,0x00,0x0E,0x44,
0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x70,0x00,0x01,0x90,0x00,0x03,0x10,0x00,0x04,0x10,0x00,0x18,0x10,0x00,0x1F,0xFE,0x00,
0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x1F,0x82,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x10,0xC2,0x00,0x10,0x44,0x00,0x10,0x38,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0xF8,0x00,0x06,0x84,0x00,0x08,0x82,0x00,0x18,0x82,0x00,0x10,0x82,0x00,0x10,0xC4,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x10,0x1E,0x00,0x10,0xE0,0x00,0x13,0x00,0x00,0x1C,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x3C,0x00,
0x19,0xC6,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x10,0xC2,0x00,0x18,0xC6,0x00,0x0F,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x82,0x00,0x08,0xC2,0x00,0x10,
0x42,0x00,0x10,0x44,0x00,0x10,0x44,0x00,0x08,0x58,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x06,0x00,0x03,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x03,0x07,0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x50,0x00,0x00,0x90,0x00,0x00,0x98,0x00,0x01,0x08,0x00,
0x01,0x08,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,
0x10,0x00,0x01,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x08,0x00,0x01,0x08,0x00,0x00,0x98,0x00,0x00,0x90,0x00,0x00,0x50,0x00,0x00,0x60,
0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x10,0x66,0x00,0x10,0x86,0x00,0x11,0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0xF8,0x00,0x06,0x06,0x00,0x0C,0x03,0x00,0x08,0x01,0x00,0x10,0xF1,0x80,0x11,0x08,0x80,0x12,0x04,0x80,0x12,0x04,0x80,0x12,0x04,0x80,0x0B,0xF8,0x80,0x0C,
0x04,0x00,0x06,0x0C,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x1E,0x00,0x00,0x70,0x00,0x03,0x90,0x00,0x0E,0x10,0x00,0x18,0x10,
0x00,0x0E,0x10,0x00,0x01,0xD0,0x00,0x00,0x78,0x00,0x00,0x0E,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x82,0x00,0x10,0x82,0x00,
0x10,0x82,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x09,0xC4,0x00,0x0F,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x04,0x08,0x00,0x08,0x04,0x00,0x10,
0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x02,0x00,0x10,0x02,
0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x18,0x06,0x00,0x08,0x04,0x00,0x06,0x18,0x00,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x82,0x00,
0x10,0x82,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x10,0x82,0x00,0x10,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,
0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x04,0x08,0x00,0x08,0x04,0x00,0x10,0x02,0x00,0x10,0x02,
0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x08,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x1F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x06,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x06,0x00,0x1F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x00,0x80,
0x00,0x01,0x40,0x00,0x03,0x20,0x00,0x06,0x30,0x00,0x0C,0x18,0x00,0x18,0x0C,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x00,0x02,0x00,
0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x18,0x00,0x00,0x06,0x00,0x00,0x01,
0x80,0x00,0x00,0x60,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x01,0x80,0x00,0x06,0x00,0x00,0x18,0x00,0x00,0x1F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x1F,0xFE,0x00,0x0C,0x00,0x00,0x06,0x00,0x00,0x03,0x00,0x00,0x00,0x80,0x00,0x00,0x60,0x00,0x00,0x30,0x00,0x00,0x0C,0x00,0x1F,0xFE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x03,0xF0,0x00,0x04,0x08,0x00,0x08,0x04,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x08,0x04,0x00,0x06,
0x18,0x00,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x08,0x80,
0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x04,0x08,0x00,0x08,0x04,0x00,0x10,0x02,0x00,0x10,0x02,0x00,0x10,0x03,0x00,0x10,0x03,0x80,
0x10,0x02,0x80,0x08,0x04,0x40,0x06,0x18,0x40,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x10,0x40,0x00,0x10,
0x40,0x00,0x10,0x70,0x00,0x08,0x98,0x00,0x07,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x06,0x00,0x19,0x02,0x00,0x10,0x82,0x00,0x10,0x82,
0x00,0x10,0x42,0x00,0x10,0x64,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x1F,0xFE,0x00,
0x10,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF8,0x00,0x00,0x0C,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,
0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x0C,0x00,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x1E,0x00,0x00,0x03,0xC0,0x00,0x00,0x70,
0x00,0x00,0x0C,0x00,0x00,0x06,0x00,0x00,0x1C,0x00,0x00,0xE0,0x00,0x07,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x01,0xF0,0x00,
0x00,0x1E,0x00,0x00,0x06,0x00,0x00,0x38,0x00,0x01,0xC0,0x00,0x0E,0x00,0x00,0x07,0x00,0x00,0x00,0xE0,0x00,0x00,0x1C,0x00,0x00,0x06,0x00,0x00,0x3C,0x00,0x07,
0xE0,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x06,0x00,0x0C,0x1C,0x00,0x06,0x30,0x00,0x01,0xC0,0x00,0x01,0xC0,0x00,0x03,0x30,
0x00,0x0C,0x18,0x00,0x18,0x06,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x0C,0x00,0x00,0x03,0x00,0x00,0x01,0x80,0x00,0x00,0x7E,0x00,
0x01,0x80,0x00,0x03,0x00,0x00,0x0C,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x06,0x00,0x10,0x1A,0x00,0x10,0x22,0x00,0x10,0xC2,0x00,0x11,
0x82,0x00,0x16,0x02,0x00,0x1C,0x02,0x00,0x18,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xC0,0x40,0x00,0x40,0x40,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
0x00,0x70,0x00,0x00,0x0E,0x00,0x00,0x01,0xC0,0x00,0x00,0x78,0x00,0x00,0x0E,0x00,0x00,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x40,
0x40,0x00,0x40,0x7F,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x03,0x00,0x00,0x0C,0x00,0x00,0x10,0x00,0x00,0x18,0x00,0x00,0x06,0x00,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,
0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x02,0x32,0x00,
0x02,0x22,0x00,0x02,0x22,0x00,0x03,0x22,0x00,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,
0x02,0x00,0x01,0x04,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x01,0x04,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x01,0x04,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x3F,0xFE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xF8,0x00,0x01,0x24,0x00,0x02,0x22,0x00,0x02,0x22,0x00,0x02,0x22,0x00,0x01,0x22,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,
0xFE,0x00,0x32,0x00,0x00,0x22,0x00,0x00,0x22,0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x40,0x01,0x04,0x40,0x02,0x02,0x40,0x02,0x02,
0x40,0x02,0x02,0x40,0x02,0x02,0x80,0x03,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,
0x01,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x13,0xFF,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x00,0x00,0x20,0x00,0x00,0x70,0x00,0x01,0x98,0x00,0x03,0x0C,0x00,0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x3F,0xFC,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x03,0x00,0x00,0x01,0xFE,0x00,
0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x03,0x00,0x00,0x01,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,
0x00,0x00,0x02,0x00,0x00,0x03,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x01,0x04,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,
0x00,0x02,0x02,0x00,0x01,0x04,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xC0,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,
0x01,0x04,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x01,0x04,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x02,0x02,0x00,0x03,
0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC2,
0x00,0x02,0x42,0x00,0x02,0x22,0x00,0x02,0x22,0x00,0x02,0x12,0x00,0x02,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x00,0x02,0x02,0x00,0x02,0x02,0x00,
0x02,0x02,0x00,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF8,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x03,
0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x03,0xC0,0x00,0x00,0x38,0x00,0x00,0x0E,0x00,0x00,0x06,0x00,0x00,0x38,0x00,0x01,0xC0,0x00,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x03,0xC0,0x00,0x00,0x38,0x00,0x00,0x06,0x00,0x00,0x0E,0x00,0x00,0x70,0x00,0x03,0x80,0x00,0x00,0xE0,0x00,
0x00,0x1C,0x00,0x00,0x06,0x00,0x00,0x38,0x00,0x01,0xE0,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x06,0x00,0x01,0x8C,0x00,0x00,
0xF0,0x00,0x00,0x60,0x00,0x00,0x98,0x00,0x03,0x0C,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x40,0x03,0xC0,0x40,0x00,0x70,0x40,0x00,0x0C,
0x80,0x00,0x07,0x00,0x00,0x3C,0x00,0x03,0xE0,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x06,0x00,0x02,0x0E,0x00,0x02,0x32,0x00,0x02,0x62,0x00,
0x02,0x82,0x00,0x03,0x02,0x00,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x60,0x00,0x1F,0x9F,0x80,0x20,0x00,0x40,0x20,0x00,0x40,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x40,0x20,0x00,0x40,0x1F,0xBF,0x80,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x40,0x00,0x00,0xFF,0xC0,0x01,0xFF,0xC0,0x03,0xFF,0xC0,0x07,0xFF,0xC0,0x0F,0xFF,0xC0,0x1F,0xFF,0xC0,0x3F,0xF0,0x00,0x7F,0xF0,0x00,0x3F,
0xF0,0x00,0x1F,0xFF,0xC0,0x3F,0xFF,0xC0,0x3F,0xFF,0xC0,0x3F,0xFF,0xC0,0x03,0xFF,0xC0,0x00,0xFF,0xC0,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,
0x00,0x07,0x1C,0x00,0x0C,0x06,0x00,0x18,0x03,0x00,0x10,0x01,0x00,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,0x30,0x01,0x80,
0x11,0x01,0x00,0x1B,0x03,0x00,0x0F,0x06,0x00,0x0F,0x1C,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x04,0x00,0x05,
0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,
0x00,0x04,0x04,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x00,
0x1F,0xF0,0x00,0x3F,0xFC,0x00,0x7C,0xFE,0x00,0x78,0x7F,0x80,0x78,0x7F,0xC0,0x78,0x7F,0x80,0x7C,0xFE,0x00,0x3F,0xFC,0x00,0x1F,0xF0,0x00,0x0F,0xC0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x04,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,
0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,
0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x38,0x00,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,0x06,0x00,0x00,
0x1C,0x00,0x00,0x38,0x00,0x00,0x70,0x00,0x00,0xE0,0x00,0x01,0x80,0x00,0x03,0x00,0x00,0x06,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x70,0x00,0x00,0xE8,0x00,0x00,0xA4,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,
0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x70,0x00,0x00,0x38,0x00,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,0x07,0x00,0x1F,0xFF,0x80,0x00,0x07,0x00,0x00,0x0E,
0x00,0x00,0x1C,0x00,0x00,0x38,0x00,0x00,0x70,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0xE0,0x00,
0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0C,0xE6,0x00,0x0E,0x4E,0x00,0x0F,0x1E,0x00,0x0F,0x1E,0x00,0x0E,0x4E,0x00,0x0C,0xE6,0x00,0x0F,
0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,
0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF8,0x00,0x07,0xBC,0x00,0x0C,0x06,0x00,0x18,0x03,0x00,0x00,0x01,0x80,0x00,0x01,0x80,
0x00,0x01,0x80,0x3F,0xC0,0x80,0x00,0x01,0x80,0x00,0x01,0x80,0x00,0x01,0x80,0x18,0x03,0x00,0x0C,0x06,0x00,0x07,0xBC,0x00,0x03,0xF8,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x07,0xC0,0x7E,0x0F,0xC0,0x7B,0x1E,0xC0,0x79,0xBC,0xC0,0x78,0xF8,0xC0,0x78,0x00,
0xC0,0x78,0xF8,0xC0,0x79,0xBC,0xC0,0x7B,0x1E,0xC0,0x7E,0x0F,0xC0,0x7C,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x1F,0xFF,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x04,0x04,0x00,0x07,0xFC,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
0x00,0x00,0xE0,0x00,0x01,0xF0,0x00,0x03,0xF8,0x00,0x07,0x5C,0x00,0x0E,0x4E,0x00,0x1C,0x47,0x00,0x18,0x43,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,
0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x04,0x00,0x0E,0x0E,0x00,0x07,0x1C,0x00,0x03,0xB8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x01,0xF0,0x00,0x03,0xB8,0x00,0x07,0x1C,0x00,0x0E,0x0E,0x00,0x04,0x04,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0C,0x00,
0x03,0x18,0x00,0x01,0xB0,0x00,0x00,0xE0,0x00,0x7F,0xFF,0xC0,0x30,0xE1,0x80,0x19,0xB3,0x00,0x0F,0x1E,0x00,0x06,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x07,0xFC,
0x00,0x07,0xFC,0x00,0x03,0xF8,0x00,0x03,0xF8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x03,
0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x1F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x18,0x43,0x00,0x1C,0x47,0x00,
0x0E,0x4E,0x00,0x07,0x5C,0x00,0x03,0xF8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,
0x04,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xD4,0x00,0x05,0x54,0x00,0x04,0xC4,0x00,0x01,0xF8,0x00,0x06,0x64,0x00,0x01,0x54,0x00,0x05,0x34,0x00,0x05,0x74,
0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,
0x01,0xC0,0x00,0x03,0x80,0x00,0x07,0x00,0x00,0x0E,0x00,0x00,0x1C,0x00,0x00,0x3F,0xFF,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,0x07,0x00,0x00,0x03,0x80,0x00,0x01,
0xC0,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x06,0x40,0x00,0x06,0xE0,0x00,0x0C,0xC0,
0x00,0x0D,0x98,0x00,0x09,0xB0,0x00,0x09,0x32,0x00,0x09,0x37,0x00,0x09,0x32,0x00,0x09,0xB0,0x00,0x0D,0x98,0x00,0x0C,0xC0,0x00,0x06,0xE0,0x00,0x06,0x40,0x00,
0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x0F,0xF0,0x00,0x18,0x38,0x00,0x30,0x18,0x00,0x30,0x0C,0x00,0x30,
0x0C,0x00,0x30,0x0C,0x00,0x30,0x1C,0x00,0x38,0x18,0x00,0x1C,0x78,0x00,0x0F,0xFC,0x00,0x03,0xCE,0x00,0x00,0x07,0x00,0x00,0x03,0x80,0x00,0x01,0x80,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x07,0x80,0x00,0x1F,0x80,0x00,0x7F,0x80,0x01,0xFF,0x80,0x03,0xFF,0x80,0x0F,0xFF,0x80,
0x3E,0x05,0x80,0x1F,0xFF,0x80,0x07,0xFF,0x80,0x01,0xFF,0x80,0x00,0x7F,0x80,0x00,0x3F,0x80,0x00,0x0F,0x80,0x00,0x03,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x02,0x08,0x00,0x0F,0x1E,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x7E,0x0F,0xC0,0x7E,0x0F,0xC0,0x7E,0x0F,0xC0,0x7E,0x0F,
0xC0,0x7E,0x0F,0xC0,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0x1E,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x0F,0xFE,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,
0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x04,0x00,0x04,0x04,
0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,
0x04,0x04,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,
0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const lui_bitmap_t BITMAP_lui_default = {.size_x=1388, .size_y=19, .payload=lui_default_payload};

static const _lui_glyph_t FONT_GLYPHS_lui_default[] = {
{ .character=32/* */, .width=3, .payload_index=0 }, 			{ .character=33/*!*/, .width=4, .payload_index=9 }, 
{ .character=34/*"*/, .width=6, .payload_index=21 }, 			{ .character=35/*#*/, .width=11, .payload_index=39 }, 
{ .character=36/*$*/, .width=9, .payload_index=72 }, 			{ .character=37/*%*/, .width=14, .payload_index=99 }, 
{ .character=38/*&*/, .width=12, .payload_index=141 }, 			{ .character=39/*'*/, .width=3, .payload_index=177 }, 
{ .character=40/*(*/, .width=6, .payload_index=186 }, 			{ .character=41/*)*/, .width=6, .payload_index=204 }, 
{ .character=42/***/, .width=9, .payload_index=222 }, 			{ .character=43/*+*/, .width=9, .payload_index=249 }, 
{ .character=44/*,*/, .width=4, .payload_index=276 }, 			{ .character=45/*-*/, .width=6, .payload_index=288 }, 
{ .character=46/*.*/, .width=4, .payload_index=306 }, 			{ .character=47/*/*/, .width=9, .payload_index=318 }, 
{ .character=48/*0*/, .width=9, .payload_index=345 }, 			{ .character=49/*1*/, .width=6, .payload_index=372 }, 
{ .character=50/*2*/, .width=8, .payload_index=390 }, 			{ .character=51/*3*/, .width=9, .payload_index=414 }, 
{ .character=52/*4*/, .width=10, .payload_index=441 }, 			{ .character=53/*5*/, .width=9, .payload_index=471 }, 
{ .character=54/*6*/, .width=9, .payload_index=498 }, 			{ .character=55/*7*/, .width=9, .payload_index=525 }, 
{ .character=56/*8*/, .width=9, .payload_index=552 }, 			{ .character=57/*9*/, .width=9, .payload_index=579 }, 
{ .character=58/*:*/, .width=4, .payload_index=606 }, 			{ .character=59/*;*/, .width=4, .payload_index=618 }, 
{ .character=60/*<*/, .width=10, .payload_index=630 }, 			{ .character=61/*=*/, .width=10, .payload_index=660 }, 
{ .character=62/*>*/, .width=10, .payload_index=690 }, 			{ .character=63/*?*/, .width=7, .payload_index=720 }, 
{ .character=64/*@*/, .width=15, .payload_index=741 }, 			{ .character=65/*A*/, .width=13, .payload_index=786 }, 
{ .character=66/*B*/, .width=10, .payload_index=825 }, 			{ .character=67/*C*/, .width=11, .payload_index=855 }, 
{ .character=68/*D*/, .width=11, .payload_index=888 }, 			{ .character=69/*E*/, .width=9, .payload_index=921 }, 
{ .character=70/*F*/, .width=9, .payload_index=948 }, 			{ .character=71/*G*/, .width=11, .payload_index=975 }, 
{ .character=72/*H*/, .width=11, .payload_index=1008 }, 			{ .character=73/*I*/, .width=3, .payload_index=1041 }, 
{ .character=74/*J*/, .width=9, .payload_index=1050 }, 			{ .character=75/*K*/, .width=10, .payload_index=1077 }, 
{ .character=76/*L*/, .width=9, .payload_index=1107 }, 			{ .character=77/*M*/, .width=14, .payload_index=1134 }, 
{ .character=78/*N*/, .width=11, .payload_index=1176 }, 			{ .character=79/*O*/, .width=13, .payload_index=1209 }, 
{ .character=80/*P*/, .width=10, .payload_index=1248 }, 			{ .character=81/*Q*/, .width=13, .payload_index=1278 }, 
{ .character=82/*R*/, .width=11, .payload_index=1317 }, 			{ .character=83/*S*/, .width=9, .payload_index=1350 }, 
{ .character=84/*T*/, .width=11, .payload_index=1377 }, 			{ .character=85/*U*/, .width=11, .payload_index=1410 }, 
{ .character=86/*V*/, .width=12, .payload_index=1443 }, 			{ .character=87/*W*/, .width=16, .payload_index=1479 }, 
{ .character=88/*X*/, .width=12, .payload_index=1527 }, 			{ .character=89/*Y*/, .width=11, .payload_index=1563 }, 
{ .character=90/*Z*/, .width=10, .payload_index=1596 }, 			{ .character=91/*[*/, .width=5, .payload_index=1626 }, 
{ .character=92/*\*/, .width=9, .payload_index=1641 }, 			{ .character=93/*]*/, .width=5, .payload_index=1668 }, 
{ .character=94/*^*/, .width=10, .payload_index=1683 }, 			{ .character=95/*_*/, .width=10, .payload_index=1713 }, 
{ .character=96/*`*/, .width=5, .payload_index=1743 }, 			{ .character=97/*a*/, .width=8, .payload_index=1758 }, 
{ .character=98/*b*/, .width=9, .payload_index=1782 }, 			{ .character=99/*c*/, .width=8, .payload_index=1809 }, 
{ .character=100/*d*/, .width=9, .payload_index=1833 }, 			{ .character=101/*e*/, .width=9, .payload_index=1860 }, 
{ .character=102/*f*/, .width=7, .payload_index=1887 }, 			{ .character=103/*g*/, .width=9, .payload_index=1908 }, 
{ .character=104/*h*/, .width=9, .payload_index=1935 }, 			{ .character=105/*i*/, .width=3, .payload_index=1962 }, 
{ .character=106/*j*/, .width=5, .payload_index=1971 }, 			{ .character=107/*k*/, .width=9, .payload_index=1986 }, 
{ .character=108/*l*/, .width=4, .payload_index=2013 }, 			{ .character=109/*m*/, .width=13, .payload_index=2025 }, 
{ .character=110/*n*/, .width=9, .payload_index=2064 }, 			{ .character=111/*o*/, .width=10, .payload_index=2091 }, 
{ .character=112/*p*/, .width=9, .payload_index=2121 }, 			{ .character=113/*q*/, .width=9, .payload_index=2148 }, 
{ .character=114/*r*/, .width=7, .payload_index=2175 }, 			{ .character=115/*s*/, .width=8, .payload_index=2196 }, 
{ .character=116/*t*/, .width=7, .payload_index=2220 }, 			{ .character=117/*u*/, .width=9, .payload_index=2241 }, 
{ .character=118/*v*/, .width=10, .payload_index=2268 }, 			{ .character=119/*w*/, .width=15, .payload_index=2298 }, 
{ .character=120/*x*/, .width=10, .payload_index=2343 }, 			{ .character=121/*y*/, .width=10, .payload_index=2373 }, 
{ .character=122/*z*/, .width=9, .payload_index=2403 }, 			{ .character=123/*{*/, .width=7, .payload_index=2430 }, 
{ .character=124/*|*/, .width=3, .payload_index=2451 }, 			{ .character=125/*}*/, .width=7, .payload_index=2460 }, 
{ .character=126/*~*/, .width=10, .payload_index=2481 }, 			
{ .character=1/*[01]home.png*/, .width=19, .payload_index=2511 }, 
{ .character=2/*[02]reload.png*/, .width=19, .payload_index=2568 }, 			{ .character=3/*[03]battery-half.png*/, .width=19, .payload_index=2625 }, 
{ .character=4/*[04]location.png*/, .width=19, .payload_index=2682 }, 			{ .character=5/*[05]battery-full.png*/, .width=19, .payload_index=2739 }, 
{ .character=6/*[06]checkmark.png*/, .width=19, .payload_index=2796 }, 			{ .character=7/*[07]return-down-back.png*/, .width=19, .payload_index=2853 }, 
{ .character=8/*[08]arrow-down.png*/, .width=19, .payload_index=2910 }, 			{ .character=9/*[09]backspace.png*/, .width=19, .payload_index=2967 }, 
{ .character=10/*[0A]pause.png*/, .width=19, .payload_index=3024 }, 			{ .character=11/*[0B]remove.png*/, .width=19, .payload_index=3081 }, 
{ .character=12/*[0C]power.png*/, .width=19, .payload_index=3138 }, 			{ .character=13/*[0D]hourglass.png*/, .width=19, .payload_index=3195 }, 
{ .character=14/*[0E]volume-medium.png*/, .width=19, .payload_index=3252 }, 			{ .character=15/*[0F]arrow-back.png*/, .width=19, .payload_index=3309 }, 
{ .character=16/*[10]close.png*/, .width=19, .payload_index=3366 }, 			{ .character=17/*[11]bluetooth.png*/, .width=19, .payload_index=3423 }, 
{ .character=18/*[12]play.png*/, .width=19, .payload_index=3480 }, 			{ .character=19/*[13]volume-off.png*/, .width=19, .payload_index=3537 }, 
{ .character=20/*[14]arrow-forward.png*/, .width=19, .payload_index=3594 }, 			{ .character=21/*[15]battery-charging.png*/, .width=19, .payload_index=3651 }, 
{ .character=22/*[16]arrow-up.png*/, .width=19, .payload_index=3708 }, 			{ .character=23/*[17]wifi.png*/, .width=19, .payload_index=3765 }, 
{ .character=24/*[18]search.png*/, .width=19, .payload_index=3822 }, 			{ .character=25/*[19]warning.png*/, .width=19, .payload_index=3879 }, 
{ .character=26/*[1A]settings.png*/, .width=19, .payload_index=3936 }, 			{ .character=27/*[1B]add.png*/, .width=19, .payload_index=3993 }, 
{ .character=28/*[1C]battery-dead.png*/, .width=19, .payload_index=4050 }, 			{ .character=29/*[1D]stop.png*/, .width=19, .payload_index=4107 },};

const lui_font_t FONT_lui_default = { .bitmap = &BITMAP_lui_default, .glyph_count = 124, .glyphs = FONT_GLYPHS_lui_default };

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */
