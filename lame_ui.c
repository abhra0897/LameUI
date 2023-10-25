/*
 *  Created on: 02-Apr-2020
 */
/**
 * @file lame_ui.c
 * @author Avra Mitra
 * @brief Source FIle of LameUI GUI library. Must include lame_ui.h. No other file is mandatory.
 * @version 2.0
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2020-2023
 * 
 */

#include "lame_ui.h"


/*-------------------------------------
 * 			Global variables
 *-------------------------------------
 */
static uint8_t g_lui_needs_render = 0;
static _lui_mem_block_t g_lui_mem_block;
static _lui_main_t* g_lui_main;
/*-------------------------------------
 * 				End
 *-------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				Main functions
 *-------------------------------------------------------------------------------
 */
int8_t lui_init(uint8_t mem_block[], uint32_t size)
{
	_lui_mem_init(mem_block, size);
	g_lui_main = (_lui_main_t* )_lui_mem_alloc(sizeof(_lui_main_t));
	if (g_lui_main == NULL)
		return -1;

	// 	g_lui_main->scenes = {NULL};
	g_lui_main->default_font = &LUI_DEFAULT_FONT;
	g_lui_main->disp_drv = NULL;
	g_lui_main->touch_input_dev = NULL;
	g_lui_main->last_touch_data.x = -1;
	g_lui_main->last_touch_data.y = -1;
	g_lui_main->last_touch_data.is_pressed = 0;
	g_lui_main->input_event_clicked = 0;
	g_lui_main->input_state_pressed = 0;
	g_lui_main->total_scenes = 0;
	g_lui_main->active_scene = NULL;
	g_lui_main->active_obj = NULL;
	g_lui_main->total_created_objects = 0;

	return 0;
}

int8_t lui_set_default_font(const lui_font_t* font)
{
	if (font == NULL)
		return -1;

	g_lui_main->default_font = font;
	return 0;
}

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
	if (g_lui_needs_render)
	{
		_lui_object_render_parent_with_children( g_lui_main->active_scene);
	}

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

void lui_label_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LABEL) < 0)
		return;

	if (!(obj->visible))
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);

	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_area_t lbl_area = {
		.x = obj->x,
		.y = obj->y,
		.w = obj->common_style.width,
		.h = obj->common_style.height
	};

	uint16_t bg_color = obj->common_style.bg_color;
	const lui_bitmap_t* parent_bmp = NULL;
	lui_bitmap_mono_pal_t* mono_palette = NULL;
	lui_area_t bmp_crop;
	if (obj->parent && lbl->style.is_transparent_bg)
	{
		bg_color = obj->parent->common_style.bg_color;
		/* NOTE: panel and scene both have same first items in the struct. So even if
		 * the parent is scene, we can use lui_panel_t for casting.
		 */
		lui_panel_t* panel = (lui_panel_t* )(obj->parent->obj_main_data);
		if (panel->bg_image)
		{
			parent_bmp = panel->bg_image;
			mono_palette = &panel->img_pal;
			bmp_crop.x = obj->x - obj->parent->x;
			bmp_crop.y = obj->y - obj->parent->y;
			bmp_crop.w = lbl_area.w;
			bmp_crop.h = lbl_area.h;

			// if (panel->image_crop)
			// {
			// 	bmp_crop.x += panel->image_crop->x;
			// 	bmp_crop.y += panel->image_crop->y;
			// }
		}
	}
	lui_gfx_draw_string_advanced(
		lbl->text, 
		&lbl_area, 
		lbl->style.text_color, 
		bg_color, 
		parent_bmp,
		mono_palette, 
		&bmp_crop, 
		1, 
		lbl->font);

	// Draw the label border if needed
	if (obj->common_style.border_width)
	{
		lui_gfx_draw_rect(
			obj->x, obj->y, obj->common_style.width,
		    obj->common_style.height, obj->common_style.border_width,
		    obj->common_style.border_color);
	}

}

lui_obj_t* lui_label_create(void)
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_label_t* initial_label =  (lui_label_t* )_lui_mem_alloc(sizeof(*initial_label));
	if (initial_label == NULL)
		return NULL;
	
	initial_label->text = "";
	initial_label->font = g_lui_main->default_font;
	initial_label->style.text_color = LUI_STYLE_LABEL_TEXT_COLOR;
	initial_label->style.is_transparent_bg = 1;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// objeect type
	obj->obj_type = LUI_OBJ_LABEL;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_LABEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_LABEL_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_LABEL_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_LABEL_WIDTH;
	obj->common_style.height = LUI_STYLE_LABEL_HEIGHT;

	obj->obj_main_data = (void* )initial_label;

	return obj;
}

lui_obj_t* lui_label_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(label, obj_parent)
}

void lui_label_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LABEL) < 0)
		return;
	if (font == NULL)
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	lbl->font = font;
	_lui_object_set_need_refresh(obj->parent);
}

void lui_label_set_text(lui_obj_t* obj, const char* text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LABEL) < 0)
		return;
	
	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	lbl->text = (char*)text;

	_lui_object_set_need_refresh(obj);
}

void lui_label_set_text_color(lui_obj_t* obj, uint16_t text_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LABEL) < 0)
		return;

	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	if (lbl->style.text_color == text_color)
		return;
	lbl->style.text_color = text_color;
	_lui_object_set_need_refresh(obj);
}

void lui_label_set_bg_transparent(lui_obj_t* obj, uint8_t is_transparent)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LABEL) < 0)
		return;

	lui_label_t* lbl = (lui_label_t* )(obj->obj_main_data);
	if (lbl->style.is_transparent_bg == is_transparent)
		return;
	lbl->style.is_transparent_bg = is_transparent ? 1 : 0;
	_lui_object_set_need_refresh(obj);
}

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_LINE_CHART related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_LINECHART)

/*
* Draw a line chart
*/
void lui_linechart_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
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

	double mapped_data[g_lui_main->disp_drv->display_hor_res * 2];

	double x_data_min_new = obj->x;
	double x_data_max_new = obj->x + width - 1;
	//[0][0] element of 2D array is x_min
	// address of [0][0] = base address
	double x_data_min_old = *(chart->data.source);
	//[n][0] element of 2D array is x_max
	//address of [n][0] = base address + (n*2) - 2
	double x_data_max_old = *(chart->data.source + (data_points*2) - 2);


	double y_data_min_new = obj->y + height - 1;
	double y_data_max_new = obj->y;
	double y_data_min_old;
	double y_data_max_old;

	// If data auto-scale is enabled, find out the Y max and min value
	if (chart->data.auto_scale == 1)
	{
		// Initially, Max and Min both are set to the first Y value of the source array
		double y_max = *(chart->data.source + 1);
		double y_min = y_max;

		// Now compare max and min with y values from source array to find the maximum and minimum
		for (uint16_t i = 1; i < data_points; i++)
		{
			double y_val = *(chart->data.source + (i*2) + 1);
			if (y_max < y_val)
				y_max = y_val;
			if (y_min > y_val)
				y_min = y_val;
		}
		if (y_max == y_min)
		{
			y_max = _LUI_MAX(y_max, y_max + 1);	// preventing overflow
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
	for (uint16_t i = 0; i < data_points; i++)
	{
		double x_data_old = *(chart->data.source + (i*2));
		double y_data_old = *(chart->data.source + (i*2) + 1);
		// Mapping range of x values
		mapped_data[i*2] = _lui_map_range(x_data_old, x_data_max_old, x_data_min_old, x_data_max_new, x_data_min_new);

		// Mapping range of y values
		mapped_data[i*2 + 1] = _lui_map_range(y_data_old, y_data_max_old, y_data_min_old, y_data_max_new, y_data_min_new);
	}


	lui_area_t clip_win = {temp_x, temp_y, width, height};
	// Now draw the lines using the mapped points to make the graph
	for (uint16_t i = 0; i < data_points - 1; i++)
	{
		uint32_t i_cur_x = (i * 2), i_cur_y = (i * 2 + 1);
		uint32_t i_nxt_x = (i * 2 + 2), i_nxt_y = (i * 2 + 3);
		if (chart->style.draw_mode & LUI_LINECHART_DRAW_MODE_LINE)
		{
			if (chart->data.auto_scale)
			{
				lui_gfx_draw_line_clipped(
					mapped_data[i_cur_x], mapped_data[i_cur_y],
					mapped_data [i_nxt_x], mapped_data [i_nxt_y],
					&clip_win, chart->style.line_width, line_color);
			}
			else
			{
				double current[2] = {mapped_data[i_cur_x], mapped_data[i_cur_y]};
				double next[2] = {mapped_data [i_nxt_x], mapped_data [i_nxt_y]};
				// modifies `current` and `next`
				uint8_t flag_accept = _lui_clip_line(current, next, &clip_win);
				if (flag_accept)
				{
					lui_gfx_draw_line_clipped(
						current[0], current[1], next[0], next[1],
						&clip_win, chart->style.line_width, line_color);
				}
				else
					continue;
			}
		}
		// Draw point only if point draw mode enabled and points are within the clip area/window
		if (chart->style.draw_mode & LUI_LINECHART_DRAW_MODE_POINT)
		{
			if (mapped_data[i_cur_x] >= clip_win.x && mapped_data[i_cur_x] <= clip_win.x + clip_win.w - 1 &&
				mapped_data[i_cur_y] >= clip_win.y && mapped_data[i_cur_y] <= clip_win.y + clip_win.h - 1)
			{
				double px = mapped_data[i_cur_x] - chart->style.point_width / 2;
				double py = mapped_data[i_cur_y] - chart->style.point_width / 2;
				lui_gfx_draw_rect_fill_clipped(px, py, chart->style.point_width, chart->style.point_width, &clip_win, chart->style.point_color);
			}

			// draw the last remaining point
			if (i == chart->data.points - 2 &&
				mapped_data[i_nxt_x] >= clip_win.x && mapped_data[i_nxt_x] <= clip_win.x + clip_win.w - 1 &&
				mapped_data[i_nxt_y] >= clip_win.y && mapped_data[i_nxt_y] <= clip_win.y + clip_win.h - 1)
			{
				double px = mapped_data[i_nxt_x] - chart->style.point_width / 2.;
				double py = mapped_data[i_nxt_y] - chart->style.point_width / 2.;
				lui_gfx_draw_rect_fill_clipped(px, py, chart->style.point_width, chart->style.point_width, &clip_win, chart->style.point_color);
			}
		}

	}

	// Draw the chart border if needed
	if (obj->common_style.border_width)
	{
		lui_gfx_draw_rect(temp_x, temp_y, width, height, obj->common_style.border_width, obj->common_style.border_color);
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

	lui_chart_t* initial_line_chart =  (lui_chart_t* )_lui_mem_alloc(sizeof(*initial_line_chart));
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
	initial_line_chart->style.line_width = 2;
	initial_line_chart->style.point_width = 7;
	initial_line_chart->style.point_color = LUI_STYLE_LINECHART_POINT_COLOR;
	initial_line_chart->style.draw_mode = LUI_LINECHART_DRAW_MODE_LINE | LUI_LINECHART_DRAW_MODE_POINT;
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
	obj->common_style.border_width = LUI_STYLE_LINECHART_BORDER_THICKNESS;
	obj->common_style.bg_color = LUI_STYLE_LINECHART_BG_COLOR;
	obj->common_style.height = LUI_STYLE_LINECHART_HEIGHT;
	obj->common_style.width = LUI_STYLE_LINECHART_WIDTH;
	
	obj->obj_main_data = (void* )initial_line_chart;

	return obj;
}

lui_obj_t* lui_linechart_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(linechart, obj_parent)
}

void lui_linechart_set_grid_count(lui_obj_t* obj, uint16_t hor_lines, uint16_t vert_lines)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
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
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.grid_color == color)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_color = color;
}

void lui_linechart_set_grid_visible(lui_obj_t* obj, uint8_t state)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.grid_visible == state)
		return;	
	_lui_object_set_need_refresh(obj);
	chart->style.grid_visible = state;
}

void lui_linechart_set_line_color(lui_obj_t* obj, uint16_t line_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.line_color == line_color)
		return;	
	chart->style.line_color = line_color;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_line_width(lui_obj_t* obj, uint8_t line_width)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.line_width == line_width)
		return;
	chart->style.line_width = _LUI_MAX(line_width, 1);
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_point_color(lui_obj_t* obj, uint16_t point_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.point_color == point_color)
		return;
	chart->style.point_color = point_color;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_point_width(lui_obj_t* obj, uint8_t point_width)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;

	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.point_width == point_width)
		return;
	chart->style.point_width = _LUI_MAX(point_width, 1);
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_draw_mode(lui_obj_t* obj, uint8_t mode_flag)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	if (mode_flag == 0 || mode_flag > 3)
		return;
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->style.draw_mode == mode_flag)
		return;
	chart->style.draw_mode = mode_flag;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_data_auto_scale(lui_obj_t* obj, uint8_t auto_scale)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (auto_scale == chart->data.auto_scale)
		return;
	chart->data.auto_scale = auto_scale ? 1 : 0;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_data_range(lui_obj_t* obj, double y_min, double y_max)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	if (chart->data.y_max_value == y_max && chart->data.y_min_value == y_min && chart->data.auto_scale == 0)
		return;

	chart->data.y_max_value = y_max;
	chart->data.y_min_value = y_min;
	chart->data.auto_scale = 0;
	_lui_object_set_need_refresh(obj);
}

void lui_linechart_set_data_source(lui_obj_t* obj, double *source, uint16_t points)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LINECHART) < 0)
		return;
	
	lui_chart_t* chart = (lui_chart_t* )(obj->obj_main_data);
	// if (chart->data.points == points)
	// 	return;
	_lui_object_set_need_refresh(obj);
	chart->data.source = source;
	chart->data.points = points;
}

#endif

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
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);

	uint8_t padding = 2;
	uint16_t temp_x = obj->x;
	uint16_t temp_y = obj->y;
	uint16_t btn_height = obj->common_style.height;
	uint16_t btn_width = obj->common_style.width;
	
	uint16_t str_dim[2];

	/* If button's background is transparent, draw parent's bg color or parent's bitmap as bg */
	if (obj->parent && btn->style.is_transparent_bg && obj->state != LUI_STATE_SELECTED)
	{
		/* NOTE: panel and scene both have same first items in the struct. So even if
		* the parent is scene, we can use lui_panel_t for casting.
		*/
		lui_panel_t* panel = (lui_panel_t* )(obj->parent->obj_main_data);
		if (panel->bg_image)
		{
			lui_area_t bmp_crop;
			const lui_bitmap_t* bg_bmp = panel->bg_image;
			bmp_crop.x = temp_x - obj->parent->x;
			bmp_crop.y = temp_y - obj->parent->y;
			bmp_crop.w = btn_width;
			bmp_crop.h = btn_height;

			lui_gfx_draw_bitmap(bg_bmp, &panel->img_pal, temp_x, temp_y, &bmp_crop);
		}
		else
		{
			lui_gfx_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, obj->parent->common_style.bg_color);
		}
	}
	/* Else draw the button's bg color depending on its current state */
	else
	{
		uint16_t btn_color = obj->common_style.bg_color;
		if (obj->state == LUI_STATE_SELECTED)
			btn_color = btn->style.selection_color;
		else if (obj->state == LUI_STATE_PRESSED || (btn->is_checkable && obj->value))
			btn_color = btn->style.pressed_color;
		// else if (btn->state == LUI_STATE_IDLE)
		// 	btn_color = btn->color;

		lui_gfx_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, btn_color);
	}

	/* Draw background bitmap if not NULL */
	if (btn->img_idle || btn->img_pressed)
	{
		lui_area_t crop = {
			.x = 0,
			.y = 0,
			.w = obj->common_style.width,
			.h = obj->common_style.height
		};
		if ((obj->state == LUI_STATE_PRESSED || (btn->is_checkable && obj->value)) && 
			btn->img_pressed)
		{
			lui_gfx_draw_bitmap(
				btn->img_pressed,
				&btn->img_press_pal,
				temp_x, temp_y, 
				&crop);
		}
		else if (obj->state == LUI_STATE_IDLE && btn->img_idle)
		{
			lui_gfx_draw_bitmap(
				btn->img_idle, 
				&btn->img_idle_pal,
				temp_x, temp_y, 
				&crop);
		}
	}
	
	
	/* Draw the button label (text) if not NULL */
	if (btn->label.text && btn->label.text[0] != '\0')
	{
		uint16_t lbl_color = btn->style.label_color;
		const char* lbl_txt = btn->label.text;
		if (obj->state == LUI_STATE_PRESSED || (btn->is_checkable && obj->value))
		{
			lbl_color = btn->style.label_pressed_color;
			lbl_txt = btn->label.text_pressed;
			if (lbl_txt == NULL || lbl_txt[0] == '\0')
				return;
		}

		lui_gfx_get_string_dimension(lbl_txt, btn->label.font, btn_width, str_dim);

		str_dim[0] = str_dim[0] > btn_width ? btn_width : str_dim[0];
		str_dim[1] = str_dim[1] > btn_height ? btn_height : str_dim[1];

		temp_y = temp_y + (btn_height - str_dim[1]) / 2;
		if (btn->label.text_align == LUI_ALIGN_CENTER)
		{
			temp_x = temp_x + (btn_width - str_dim[0]) / 2;
		}
		else if (btn->label.text_align == LUI_ALIGN_RIGHT)
		{
			temp_x = temp_x + (btn_width - str_dim[0]) - padding;
		}
		else
		{
			temp_x = temp_x + padding;
		}

		lui_area_t btn_lbl_area = {
			.x = temp_x,
			.y = temp_y,
			.w = str_dim[0],
			.h = str_dim[1]
		};
		lui_gfx_draw_string_advanced(lbl_txt, &btn_lbl_area, lbl_color, 0, NULL, NULL, NULL, 0, btn->label.font);
	}
	
	/* Finally Draw the border if needed */
	if (obj->common_style.border_width)
	{
		lui_gfx_draw_rect(
		    obj->x, obj->y, btn_width, btn_height,
		    obj->common_style.border_width,
		    obj->common_style.border_color);
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

	lui_button_t* initial_button =  (lui_button_t* )_lui_mem_alloc(sizeof(*initial_button));
	if (initial_button == NULL)
		return NULL;

	initial_button->style.pressed_color = LUI_STYLE_BUTTON_PRESSED_COLOR;
	initial_button->style.selection_color = LUI_STYLE_BUTTON_SELECTION_COLOR;
	initial_button->style.label_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->style.label_pressed_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->style.is_transparent_bg = 0;

	initial_button->img_idle = NULL;
	initial_button->img_pressed = NULL;
	initial_button->img_idle_pal.fore_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->img_idle_pal.back_color = LUI_STYLE_BUTTON_BG_COLOR;
	initial_button->img_idle_pal.is_backgrnd = 1;
	initial_button->img_press_pal.fore_color = LUI_STYLE_BUTTON_LABEL_COLOR;
	initial_button->img_press_pal.back_color = LUI_STYLE_BUTTON_BG_COLOR;
	initial_button->img_press_pal.is_backgrnd = 1;

	initial_button->is_checkable = 0;
	
	initial_button->label.text = NULL;
	initial_button->label.text_pressed = NULL;
	initial_button->label.font = g_lui_main->default_font;
	initial_button->label.text_align = LUI_ALIGN_CENTER;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_BUTTON;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_BUTTON_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_BUTTON_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_BUTTON_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_BUTTON_WIDTH;
	obj->common_style.height = LUI_STYLE_BUTTON_HEIGHT;
	
	obj->obj_main_data = (void* )initial_button;

	return  obj;
}

lui_obj_t* lui_button_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(button, obj_parent)
}

void lui_button_set_label_texts(lui_obj_t* obj, const char* idle_text, const char* pressed_text)
{
	lui_button_set_label_text(obj, idle_text);
	lui_button_set_label_text_pressed(obj, pressed_text);
}

void lui_button_set_label_text(lui_obj_t* obj, const char* text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->label.text = text;
	btn->label.text_pressed = text;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_text_pressed(lui_obj_t* obj, const char* pressed_text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->label.text_pressed = pressed_text;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_align(lui_obj_t *obj, uint8_t alignment)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->label.text_align != alignment)
	{
		btn->label.text_align = alignment;
		_lui_object_set_need_refresh(obj);
	}
}

void lui_button_set_label_colors(lui_obj_t* obj, uint16_t idle_color, uint16_t pressed_color)
{
	lui_button_set_label_color(obj, idle_color);
	lui_button_set_label_color_pressed(obj, pressed_color);
}

void lui_button_set_label_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.label_color == color)
		return;
	btn->style.label_color = color;
	btn->style.label_pressed_color = color;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_color_pressed(lui_obj_t* obj, uint16_t pressed_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.label_pressed_color == pressed_color)
		return;
	btn->style.label_pressed_color = pressed_color;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_label_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->label.font = (lui_font_t* )font;
	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);
}

void lui_button_set_bitmap_images(lui_obj_t* obj, const lui_bitmap_t* idle_bitmap, const lui_bitmap_t* pressed_bitmap)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (idle_bitmap && pressed_bitmap)
		btn->style.is_transparent_bg = 1;
	else
		btn->style.is_transparent_bg = 0;
	btn->img_idle = idle_bitmap;
	btn->img_pressed = pressed_bitmap;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_bitmap_images_mono_palette(lui_obj_t* obj, lui_bitmap_mono_pal_t* idle_palette, lui_bitmap_mono_pal_t* press_palette)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);

	if (idle_palette)
		btn->img_idle_pal = *idle_palette;
	if (press_palette)
		btn->img_press_pal = *press_palette;

	_lui_object_set_need_refresh(obj);
}

void lui_button_set_value(lui_obj_t* obj, uint8_t value)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (obj->value == value || !btn->is_checkable)
		return;
	obj->value = value ? 1 : 0;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_checked(lui_obj_t* obj)
{
	lui_button_set_value(obj, 1);
}

void lui_button_set_unchecked(lui_obj_t* obj)
{
	lui_button_set_value(obj, 0);
}

void lui_button_set_checkable(lui_obj_t* obj, uint8_t is_checkable)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	btn->is_checkable = is_checkable ? 1 : 0;
}

uint8_t lui_button_get_checkable(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return 0;

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	return btn->is_checkable;
}

uint8_t lui_button_get_check_value(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return 0;
	return obj->value;
}

void lui_button_set_extra_colors(lui_obj_t* obj, uint16_t pressed_color, uint16_t selection_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;
	
	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.pressed_color == pressed_color && btn->style.selection_color == selection_color)
		return;
	btn->style.pressed_color = pressed_color;
	btn->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

void lui_button_set_bg_transparent(lui_obj_t* obj, uint8_t is_transparent)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BUTTON) < 0)
		return;

	lui_button_t* btn = (lui_button_t* )(obj->obj_main_data);
	if (btn->style.is_transparent_bg == is_transparent)
		return;
	btn->style.is_transparent_bg = is_transparent;
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

#if defined(LUI_USE_LIST)

void lui_list_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	if (!(obj->visible))
		return;
	
	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);

	if (list->is_dropdown && !list->is_expanded)
		return;

	/* Draw bg to clear old elements */
	lui_gfx_draw_rect_fill(
		obj->x, 
		obj->y, 
		obj->common_style.width, 
		obj->common_style.height, 
		obj->common_style.bg_color);

	uint8_t lim = list->page_first_item_index + list->items_per_page;
	for (uint8_t i = list->page_first_item_index; i < lim; i++)
	{
		if (i == list->items_cnt)
			break;
		// uint16_t item_bg_color = obj->common_style.bg_color;
		// if (obj->state == LUI_STATE_SELECTED)
		// 	btn_color = btn->style.selection_color;
		// else if (obj->state == LUI_STATE_PRESSED)
		// 	btn_color = btn->style.pressed_color;
		// else if (btn->state == LUI_STATE_IDLE)
		// 	btn_color = btn->color;

		/* Convert local coordinates of item to global coordinates */
		uint16_t x = obj->x + list->items[i]->area.x1;
		uint16_t y = obj->y + list->items[i]->area.y1;
		uint16_t w = list->items[i]->area.x2 - list->items[i]->area.x1 + 1;
		uint16_t h = list->items[i]->area.y2 - list->items[i]->area.y1 + 1;

		// TODO: Make pressed color changeable
		if (i == list->selected_item_index)
			lui_gfx_draw_rect_fill(x, y, w, h, LUI_STYLE_LIST_ITEM_PRESSED_COLOR);

		if (list->style.item_has_border == 1)
			lui_gfx_draw_rect(x, y, w, h+1, 1, list->style.item_border_color);
		
		// Draw the text
		uint16_t dim[2];
		lui_gfx_get_string_dimension(list->items[i]->text, list->font, w, dim);

		dim[0] = dim[0] > w ? w : dim[0];
		dim[1] = dim[1] > h ? h : dim[1];

		y = y + ((h - dim[1]) / 2);
		uint8_t padding = 2;
		if (list->text_align == LUI_ALIGN_CENTER)
			x = x + (w - dim[0]) / 2;
		else if (list->text_align == LUI_ALIGN_RIGHT)
			x = x + (w - dim[0]) - padding;
		else
			x = x + padding;

		lui_area_t lst_item_area = {
			.x = x,
			.y = y,
			.w = dim[0],
			.h = dim[1]
		};

		lui_gfx_draw_string_advanced(
			list->items[i]->text,
			&lst_item_area,
			list->style.item_label_color,
			0,
			NULL,
			NULL,
			NULL,
			0,
			list->font);
	}
	if (obj->common_style.border_width)
		lui_gfx_draw_rect(
		    obj->x, obj->y,  obj->common_style.width,
		    obj->common_style.height,
		    obj->common_style.border_width,
		    obj->common_style.border_color);

}

lui_obj_t* lui_list_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_list_t* initial_list =  (lui_list_t* )_lui_mem_alloc(sizeof(*initial_list));
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
	initial_list->selected_item_index = 0; // unused
	initial_list->items_per_page = 0;
	initial_list->item_min_height = 0;
	initial_list->font = g_lui_main->default_font;
	initial_list->text_align = LUI_ALIGN_LEFT;
	initial_list->is_dropdown = 0;
	initial_list->is_expanded = 1;
	initial_list->max_items = 0;
	initial_list->items_cnt = 0;
	initial_list->items = NULL;

	initial_list->style.item_has_border = LUI_STYLE_LIST_ITEM_BORDER_THICKNESS;
	initial_list->style.item_label_color = LUI_STYLE_LIST_ITEM_LABEL_COLOR;
	initial_list->style.item_border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;

	// set common styles for list object
	obj->common_style.bg_color = LUI_STYLE_LIST_ITEM_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_LIST_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_LIST_BORDER_THICKNESS;
	obj->common_style.height = LUI_STYLE_LIST_HEIGHT;
	obj->common_style.width = LUI_STYLE_LIST_WIDTH;

	// create navigation buttons
	_lui_list_add_nav_buttons(obj);

	return obj;
}

lui_obj_t* lui_list_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(list, obj_parent)
}

void lui_list_prepare(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	// list has no item (button), so return
	// if (obj->children_count == 0)
	// 	return;

	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
	
	uint16_t item_h_padding = 10;
	/* usable height depends on navigation buttons' existence
	if no nav button (for single page list), total height is usable */
	if (list->item_min_height < list->font->bitmap->size_y)
		list->item_min_height = list->font->bitmap->size_y;
	uint16_t list_usable_height = obj->common_style.height;
	uint8_t item_height = list->item_min_height + item_h_padding;
	if (list->is_dropdown)
		list_usable_height = list_usable_height - item_height; // 1 row is used by `expand` button. So, subtract it

	/* buttons_per_page is calculated excluding the nav buttons. Only items are considered */
	list->items_per_page = list_usable_height / item_height;
	/* when item per page is higher than total item count, we need more than 1 page
	whenever more than 1 page is needed, nav buttons are added.
	so, we calculate the usable_height and recalculate items per page accordingly.
	 */
	if (list->items_per_page < list->items_cnt)
	{
		list_usable_height = list_usable_height - item_height; // leaving some space for < and > nav buttons (they're in same row)
		list->items_per_page -= 1;
	}
	// if items per page is less than or equal to total items, we need only 1 page
	// so, set item heights in such way that they fill the entire height
	else
	{
		list->items_per_page = list->items_cnt;
	}
	// item_height = list_usable_height / list->items_per_page;
	
	// fast way to round up a/b: (a+b-1)/b
	list->page_count = (list->items_cnt +  list->items_per_page - 1) / list->items_per_page;
	list->current_page_index = 0;
	list->page_first_item_index = (list->current_page_index * list->items_per_page);
	
	// set x, y coordinates and height, width of all the list items.
	// NOTE: Item (x, y) coordinates are local coordinates with respect to the List
	uint8_t item_pos = 0; 
	uint16_t item_start_y = 0;
	if (list->is_dropdown)
		item_start_y += item_height;
	for (uint8_t i = 0; i < list->items_cnt; i++)
	{
		struct _lui_list_item* item = list->items[i];
		item->area.x1 = 0;
		// starting offset is set by item_pos and btn_per_page. Taking remainder of item_pos/btn_per_page so that 
		// every time item_pos becomes big enough to come to next page, the offset becomes 0
		item->area.y1 = item_start_y + ((item_pos % list->items_per_page) * item_height);
		item->area.x2 = item->area.x1 + obj->common_style.width - 1;
		item->area.y2 = item->area.y1 + item_height - 1;
		item_pos++;
	}
	

	// navigation button x,y, w, h set.
	lui_obj_t* nav_prev = obj->first_child;
	lui_obj_t* nav_nxt = nav_prev->next_sibling;
	lui_obj_t* nav_expand = nav_nxt->next_sibling;
	lui_obj_t* nav_text = nav_expand->next_sibling;

	nav_nxt->x = obj->x + (obj->common_style.width / 2) + 0; // index 1 = nav button nxt
	nav_prev->x = obj->x + 0;	// index 0 = nav button prev
	nav_nxt->y = nav_prev->y = obj->y + item_start_y + list_usable_height;
	nav_nxt->common_style.width = nav_prev->common_style.width = (obj->common_style.width / 2) - 0; // 4 is just margin
	nav_nxt->common_style.height = nav_prev->common_style.height = item_height;
	((lui_button_t* )nav_nxt->obj_main_data)->label.font = list->font;
	((lui_button_t* )nav_prev->obj_main_data)->label.font = list->font;

	nav_text->x = obj->x;
	nav_text->y = obj->y;
	nav_text->common_style.width = obj->common_style.width - (list->font->bitmap->size_y + 4);
	nav_text->common_style.height = item_height;
	nav_expand->x = obj->x + nav_text->common_style.width;
	nav_expand->y = obj->y;
	nav_expand->common_style.width = list->font->bitmap->size_y + 4; //obj->common_style.width - nav_text->common_style.width;
	nav_expand->common_style.height = item_height;
	((lui_button_t* )nav_expand->obj_main_data)->label.font = list->font;
	((lui_button_t* )nav_text->obj_main_data)->label.font = list->font;
	lui_button_set_label_text(nav_text, list->items[list->selected_item_index]->text);
	
	// both nav buttons won't be rendered unless the list is multi page
	// if list page count is more than 0, draw the nav button
	nav_nxt->visible = 0;
	nav_prev->visible = 0;
	if (list->page_count > 0 && (list->is_expanded || !list->is_dropdown))
	{
		// draw "next" or "prev" button only if there's a next or previous page
		if (list->current_page_index + 1 < list->page_count)
			nav_nxt->visible = 1;
		if (list->current_page_index - 1 >= 0)
			nav_prev->visible = 1;
	}

	if (list->is_dropdown)
		nav_expand->visible = nav_text->visible = 1;
	else
		nav_expand->visible = nav_text->visible = 0;

	// whenever prepare function is called, list (and all children of it) will be redrawn
	_lui_object_set_need_refresh(obj);
}

int8_t lui_list_set_max_items_count(lui_obj_t* obj, uint8_t max_items_cnt)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;
	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (list->items != NULL)
		return -1;

	list->items = (struct _lui_list_item**)_lui_mem_alloc(sizeof(*(list->items)) * max_items_cnt);
	if (list->items == NULL)
		return -1;
	list->max_items = max_items_cnt;
	return 0;
	
}

int8_t lui_list_add_item(lui_obj_t* obj, const char* text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (list->items_cnt == list->max_items)
		return -1;

	struct _lui_list_item* item =  (struct _lui_list_item* )_lui_mem_alloc(sizeof(*item));
	if (item == NULL)
		return -1;
	
	item->text = (char*)text;
	list->items[list->items_cnt] = item;
	list->items_cnt++;

	_lui_object_set_need_refresh(obj);

	return  0;
}

int8_t lui_list_remove_item(lui_obj_t* obj, uint8_t item_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;

	if (item_index >= list->items_cnt)
		return -1;

	for (uint16_t i = item_index; i < list->items_cnt - 1; i++)
		list->items[item_index] = list->items[item_index + 1];
	
	list->items_cnt--;
	_lui_object_set_need_refresh(obj);
	return 0;
}

int8_t lui_list_remove_all(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	list->items_cnt = 0;
	_lui_object_set_need_refresh(obj);
	return 0;
}

int16_t lui_list_get_selected_item_index(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	return ((lui_list_t* )(obj->obj_main_data))->selected_item_index;
}

int16_t lui_list_set_selected_item_index(lui_obj_t* obj, uint8_t item_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (item_index >= list->items_cnt)
		return -1;

	list->selected_item_index = item_index;
	return 0;
	/* Must call list_prepare() after calling this */
}

const char* lui_list_get_item_text(lui_obj_t* obj, uint8_t item_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return NULL;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (item_index >= list->items_cnt)
		return NULL;
	return list->items[item_index]->text;
}

int8_t lui_list_set_item_text(lui_obj_t* obj, const char* text, uint8_t item_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (item_index >= list->items_cnt)
		return -1;

	list->items[item_index]->text = text;
	return 0;

	/* Must call list_prepare() after calling this */
}

int8_t lui_list_set_dropdown_mode(lui_obj_t* obj, uint8_t is_dropdown)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (list->is_dropdown == is_dropdown)
		return -1;
	list->is_dropdown = is_dropdown;
	if (is_dropdown)
	{
		list->is_expanded = 0;
		_lui_object_set_need_refresh(obj->parent);
	}
	else
	{
		list->is_expanded = 1;
		_lui_object_set_need_refresh(obj);
	}
	return 0;

	/* Must call list_prepare() after calling this */
}

int8_t lui_list_set_dropdown_expand(lui_obj_t* obj, uint8_t is_expanded)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return -1;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	if (!list->is_dropdown)
		return -1;
	if (list->is_expanded == is_expanded)
		return -1;
	list->is_expanded = is_expanded;
	if (is_expanded)
	{
		obj->value = obj->layer;	// Hack: Storing layer data temporarily in value property
		lui_object_set_layer(obj, LUI_LAYER_SYSTEM);
	}
	else
	{
		lui_object_set_layer(obj, obj->value);
	}
	return 0;

	/* Must call list_prepare() after calling this */
}

void lui_list_set_item_min_height(lui_obj_t* obj, uint8_t height)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;
	lui_list_t* list = (lui_list_t* )obj->obj_main_data;

	if (height >= list->font->bitmap->size_y)
	{
		list->item_min_height = height;
		_lui_object_set_need_refresh(obj);
	}
	// this min_height is applied to all items in `prepare` func
}

void lui_list_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;
	if (font == NULL)
		return;
	
	lui_list_t* list = (lui_list_t* )obj->obj_main_data;
	list->font = font;

	// parent needs refresh (along with all its children)
	_lui_object_set_need_refresh(obj->parent);

	// this font is set to all items in `prepare` func
}

void lui_list_set_text_align(lui_obj_t *obj, uint8_t alignment)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;
	
	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
	if (list->text_align != alignment)
	{
		list->text_align = alignment;
		_lui_object_set_need_refresh(obj);
	}

	// this alignment is set to all items in `prepare` func
}

void lui_list_set_text_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;
	
	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
	if (list->style.item_label_color != color)
	{
		list->style.item_label_color = color;
		_lui_object_set_need_refresh(obj);
	}
}

void lui_list_set_item_border(lui_obj_t* obj, uint8_t has_border, uint16_t border_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
	if (list->style.item_has_border != has_border ||
		list->style.item_border_color != border_color)
	{
		list->style.item_has_border = has_border;
		list->style.item_border_color = border_color;
		_lui_object_set_need_refresh(obj);
	}
}

void lui_list_set_nav_btn_label_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_button_set_label_color(obj->first_child, color);
	lui_button_set_label_color(obj->first_child->next_sibling, color);
}

void lui_list_set_nav_btn_bg_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_object_set_bg_color(obj->first_child, color);
	lui_object_set_bg_color(obj->first_child->next_sibling, color);
}

void lui_list_set_nav_btn_extra_colors(lui_obj_t* obj, uint16_t pressed_color, uint16_t selection_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_button_set_extra_colors(obj->first_child, pressed_color, selection_color);
	lui_button_set_extra_colors(obj->first_child->next_sibling, pressed_color, selection_color);
}

void lui_list_set_nav_btn_label_text(lui_obj_t* obj, const char* btn_prev_text, const char* btn_nxt_text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_button_set_label_text(obj->first_child, btn_prev_text);
	lui_button_set_label_text(obj->first_child->next_sibling, btn_nxt_text);
}

void lui_list_set_nav_btn_border_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_object_set_border_color(obj->first_child, color);
	lui_object_set_border_color(obj->first_child->next_sibling, color);
}

void lui_list_set_page_index(lui_obj_t* obj, uint8_t index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_LIST) < 0)
		return;

	lui_list_t* list = (lui_list_t* )obj->obj_main_data;

	if (index >= list->page_count || index == list->current_page_index)
		return;
	list->current_page_index = index;
	_lui_object_set_need_refresh(obj);

	list->page_first_item_index = (list->current_page_index * list->items_per_page);

	// navigation button x,y, w, h set.
	lui_obj_t* nav_prev = obj->first_child;
	lui_obj_t* nav_nxt = nav_prev->next_sibling;
	// if list page count is more than 0, draw the nav button
	lui_object_set_visibility(nav_nxt , 0);
	lui_object_set_visibility(nav_prev , 0);
	if (list->page_count > 0 && (list->is_expanded || !list->is_dropdown))
	{
		// draw "next" or "prev" button only if there's a next or previous page
		if (list->current_page_index + 1 < list->page_count)
			lui_object_set_visibility(nav_nxt , 1);
		if (list->current_page_index - 1 >= 0)
			lui_object_set_visibility(nav_prev , 1);
			
	}
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
	lui_button_set_label_text(obj_nav_btn_prev, LUI_ICON_CARET_BACK);
	lui_button_set_label_text( obj_nav_btn_nxt, LUI_ICON_CARET_FORWARD);
	// set nav button styles (nav button height and width are calculated by `prepare` function)
	lui_button_set_label_color(obj_nav_btn_prev, LUI_STYLE_LIST_NAV_LABEL_COLOR);
	lui_button_set_label_color(obj_nav_btn_nxt, LUI_STYLE_LIST_NAV_LABEL_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_prev, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_nxt, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	obj_nav_btn_nxt->common_style.border_width = obj_nav_btn_prev->common_style.border_width = 0;
	obj_nav_btn_nxt->common_style.border_color = obj_nav_btn_prev->common_style.border_color = LUI_STYLE_LIST_ITEM_BORDER_COLOR;
	obj_nav_btn_nxt->common_style.bg_color = obj_nav_btn_prev->common_style.bg_color = LUI_STYLE_LIST_NAV_BG_COLOR;
	obj_nav_btn_prev->obj_event_cb = obj_nav_btn_nxt->obj_event_cb = _lui_list_nav_btn_cb;

	_lui_list_add_button_obj(obj, obj_nav_btn_prev);
	_lui_list_add_button_obj(obj, obj_nav_btn_nxt);

	/* For handling `dropdown` functionalities */
	/* `nav_btn_expand` button is the dropdown arrow,
		`nav_btn_text` button is the selected item text button
		Clicking on either of them will expand/contract the list
	 */
	lui_obj_t* obj_nav_btn_expand = lui_button_create();
	lui_obj_t* obj_nav_btn_text = lui_button_create();
	lui_button_set_label_text(obj_nav_btn_expand, LUI_ICON_CARET_DOWN);
	lui_button_set_label_color(obj_nav_btn_expand, LUI_STYLE_LIST_NAV_LABEL_COLOR);
	lui_button_set_label_color(obj_nav_btn_text, LUI_STYLE_LIST_ITEM_LABEL_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_expand, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	lui_button_set_extra_colors(obj_nav_btn_text, LUI_STYLE_LIST_NAV_PRESSED_COLOR, LUI_STYLE_LIST_NAV_SELECTION_COLOR);
	lui_button_set_label_align(obj_nav_btn_text, LUI_ALIGN_LEFT);
	obj_nav_btn_expand->common_style.border_width = 0;
	obj_nav_btn_text->common_style.border_width = 1;
	obj_nav_btn_text->common_style.border_color = LUI_STYLE_LIST_BORDER_COLOR;
	obj_nav_btn_expand->common_style.bg_color = LUI_STYLE_LIST_NAV_BG_COLOR;
	obj_nav_btn_text->common_style.bg_color = LUI_STYLE_LIST_ITEM_BG_COLOR;
	obj_nav_btn_expand->obj_event_cb = obj_nav_btn_text->obj_event_cb = _lui_list_nav_btn_cb;
	_lui_list_add_button_obj(obj, obj_nav_btn_expand);
	_lui_list_add_button_obj(obj, obj_nav_btn_text);
	
}

void _lui_list_nav_btn_cb(lui_obj_t* obj_nav_btn)
{
	uint8_t event = lui_object_get_event(obj_nav_btn);
	lui_list_t* list = (lui_list_t* )obj_nav_btn->parent->obj_main_data;
	lui_obj_t* prev_btn = obj_nav_btn->parent->first_child;
	lui_obj_t* next_btn = prev_btn->next_sibling;
	lui_obj_t* expand_btn = next_btn->next_sibling;
	lui_obj_t* expand_btn2 = expand_btn->next_sibling;
	uint8_t index = list->current_page_index;

	if (event == LUI_EVENT_PRESSED)
	{
		/* first child is nav_prev btn */
		if (obj_nav_btn == prev_btn && list->current_page_index > 0)
		{
			index--;
			lui_list_set_page_index(obj_nav_btn->parent, index);
		}
		/* 2nd child is nav_nxt button */
		else if (obj_nav_btn == next_btn && list->current_page_index < list->page_count - 1)
		{
			index++;
			lui_list_set_page_index(obj_nav_btn->parent, index);
		}
		else if (obj_nav_btn == expand_btn || obj_nav_btn == expand_btn2)
		{
			list->is_expanded = !list->is_expanded;
			lui_object_set_visibility(next_btn , 0);
			lui_object_set_visibility(prev_btn , 0);
			if (list->page_count > 0 && (list->is_expanded || !list->is_dropdown))
			{
				// draw "next" or "prev" button only if there's a next or previous page
				if (list->current_page_index + 1 < list->page_count)
					lui_object_set_visibility(next_btn, 1);
				if (list->current_page_index - 1 >= 0)
					lui_object_set_visibility(prev_btn, 1);
			}
			if (list->is_expanded)
			{
				lui_button_set_label_text(expand_btn, LUI_ICON_CARET_UP);
				obj_nav_btn->parent->value = obj_nav_btn->parent->layer;	// Hack: Using value property to store the layer data temporarily
				lui_object_set_layer(obj_nav_btn->parent, LUI_LAYER_SYSTEM);
			}
			else
			{
				lui_button_set_label_text(expand_btn, LUI_ICON_CARET_DOWN);
				lui_object_set_layer(obj_nav_btn->parent, obj_nav_btn->value);
			}
		}
		
	}
}

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_SWITCH related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_SWITCH)

void lui_switch_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SWITCH) < 0)
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
	if (obj->common_style.border_width)
		lui_gfx_draw_rect(
		    temp_x, temp_y, temp_width, temp_height,
		    obj->common_style.border_width,
		    obj->common_style.border_color);	// switch border

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


	lui_switch_t* initial_switch =  (lui_switch_t* )_lui_mem_alloc(sizeof(*initial_switch));
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
	obj->common_style.border_width = LUI_STYLE_SWITCH_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_SWITCH_WIDTH;
	obj->common_style.height = LUI_STYLE_SWITCH_HEIGHT;
	
	obj->obj_main_data = (void* )initial_switch;

	return  obj;
}

lui_obj_t* lui_switch_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(switch, obj_parent)
}

void lui_switch_set_extra_colors(lui_obj_t* obj, uint16_t knob_off_color, uint16_t knob_on_color, uint16_t selection_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SWITCH) < 0)
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
	if (_lui_verify_obj(obj, LUI_OBJ_SWITCH) < 0)
		return -1;
	
	return obj->value;
}

void lui_switch_set_value(lui_obj_t* obj, uint8_t value)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SWITCH) < 0)
		return;
	
	if (obj->value == value)
		return;
	obj->value = value ? 1 : 0;
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

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_CHECKBOX related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_CHECKBOX)

void lui_checkbox_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;

	if (!(obj->visible))
		return;
	
	lui_checkbox_t* chkbox = (lui_checkbox_t* )obj->obj_main_data;
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
	uint16_t side = obj->common_style.height;
	lui_gfx_draw_rect_fill(obj->x, obj->y, side, side, bg_color);
	// draw the tick mark if needed
	if (obj->value == 1)
	{
		uint16_t point_1_x = obj->x + (side * .2), point_1_y = obj->y + (side * .55);
		uint16_t point_2_x = obj->x + (side* .4), point_2_y = obj->y + (side * .75);
		uint16_t point_3_x = obj->x + (side * .75), point_3_y = obj->y + (side * .3);
		
		lui_gfx_draw_line(point_1_x, point_1_y, point_2_x, point_2_y, 2, chkbox->style.tick_color);
		lui_gfx_draw_line(point_2_x, point_2_y, point_3_x, point_3_y, 2, chkbox->style.tick_color);
	}

	// draw the border if needed
	if (obj->common_style.border_width)
	{
		lui_gfx_draw_rect(
			obj->x, obj->y, side, side,
			obj->common_style.border_width,
			obj->common_style.border_color);
	}

	/* Draw label if any */
	if (chkbox->label.text)
	{
		uint16_t lbl_bg_color = ~(chkbox->label.style.text_color); // using inverted text color as bg
		uint16_t dim[2] = {0, 0};	// x, y
		lui_gfx_get_string_dimension(chkbox->label.text, chkbox->label.font, g_lui_main->disp_drv->display_hor_res - obj->x + side + 2, dim);
		lui_area_t chkbx_txt_area = {
			.x = obj->x + side + 2,
			.y = obj->y + 1,
			.w = dim[0],
			.h = dim[1]
		};

		lui_area_t bitmap_crop_area = {
			.x = chkbx_txt_area.x - obj->parent->x,
			.y = chkbx_txt_area.y - obj->parent->y,
			.w = chkbx_txt_area.w,
			.h = chkbx_txt_area.h
		};
		const lui_bitmap_t* bg_img = NULL;
		lui_bitmap_mono_pal_t* mono_palette = NULL;
		if (obj->parent)
		{
			lbl_bg_color = obj->parent->common_style.bg_color;
			/* As panel and scene both have same first elements in the struct, 
			 * we can use panel even for scene 
			 */
			lui_panel_t* panel = (lui_panel_t*)(obj->parent->obj_main_data);
			bg_img = panel->bg_image;
			mono_palette = &panel->img_pal;
		}
		lui_gfx_draw_string_advanced(
			chkbox->label.text,
			&chkbx_txt_area,
			chkbox->label.style.text_color, 
			lbl_bg_color,
			bg_img,
			mono_palette,
			&bitmap_crop_area,
			1,
			chkbox->label.font);
	}
}

lui_obj_t* lui_checkbox_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_checkbox_t* initial_chkbox =  (lui_checkbox_t* )_lui_mem_alloc(sizeof(*initial_chkbox));
	if (initial_chkbox == NULL)
		return NULL;
	
	initial_chkbox->style.bg_checked_color = LUI_STYLE_CHECKBOX_BG_CHECKED_COLOR;
	initial_chkbox->style.selection_color = LUI_STYLE_CHECKBOX_SELECTION_COLOR;
	initial_chkbox->style.tick_color = LUI_STYLE_CHECKBOX_TICK_COLOR;
	initial_chkbox->label.font = g_lui_main->default_font;
	initial_chkbox->label.text = NULL;
	initial_chkbox->label.style.text_color = LUI_STYLE_CHECKBOX_LABEL_COLOR;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_CHECKBOX;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_CHECKBOX_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_CHECKBOX_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_CHECKBOX_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_CHECKBOX_WIDTH;	 // Not needed for now. Still present.
	obj->common_style.height = g_lui_main->default_font->bitmap->size_y > LUI_STYLE_CHECKBOX_HEIGHT ? g_lui_main->default_font->bitmap->size_y : LUI_STYLE_CHECKBOX_HEIGHT;
	
	obj->obj_main_data = (void* )initial_chkbox;

	return  obj;
}

lui_obj_t* lui_checkbox_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(checkbox, obj_parent)
}

void lui_checkbox_set_extra_colors(lui_obj_t* obj, uint16_t bg_checked_color, uint16_t tick_color, uint16_t selection_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;
	
	lui_checkbox_t* chkbox = (lui_checkbox_t* )obj->obj_main_data;

	if (chkbox->style.bg_checked_color == bg_checked_color && chkbox->style.tick_color == tick_color && chkbox->style.selection_color == selection_color)
		return;
	chkbox->style.bg_checked_color = bg_checked_color;
	chkbox->style.tick_color = tick_color;
	chkbox->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_checkbox_get_value(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return -1;
	
	return obj->value;
}

void lui_checkbox_set_value(lui_obj_t* obj, uint8_t value)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;
	
	if (obj->value == value)
		return;
	obj->value = value ? 1 : 0;
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

void lui_checkbox_set_label_text(lui_obj_t* obj, const char* text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;

	((lui_checkbox_t* )obj->obj_main_data)->label.text = (char*)text;
	uint16_t dim[2];
	lui_gfx_get_string_dimension(
		text, 
		((lui_checkbox_t* )obj->obj_main_data)->label.font,
		g_lui_main->disp_drv->display_hor_res - obj->x,
		dim);
	obj->common_style.width = dim[0] + obj->common_style.height;
	_lui_object_set_need_refresh(obj);
}

void lui_checkbox_set_label_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;

	((lui_checkbox_t* )obj->obj_main_data)->label.font = font;
	uint16_t dim[2];
	lui_gfx_get_string_dimension(
		((lui_checkbox_t* )obj->obj_main_data)->label.text,
		font,
		g_lui_main->disp_drv->display_hor_res - obj->x,
		dim);
	obj->common_style.height = font->bitmap->size_y > LUI_STYLE_CHECKBOX_HEIGHT ? font->bitmap->size_y : LUI_STYLE_CHECKBOX_HEIGHT;
	obj->common_style.width = dim[0] + obj->common_style.height;
	_lui_object_set_need_refresh(obj->parent);
}

void lui_checkbox_set_label_color(lui_obj_t* obj, uint16_t color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_CHECKBOX) < 0)
		return;
	if (((lui_checkbox_t* )obj->obj_main_data)->label.style.text_color == color)
		return;

	((lui_checkbox_t* )obj->obj_main_data)->label.style.text_color = color;
	_lui_object_set_need_refresh(obj);
}

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_SLIDER related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_SLIDER)

void lui_slider_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;

	if (!(obj->visible))
		return;
	
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	uint16_t knob_color = slider->style.knob_color;
	if (obj->state == LUI_STATE_SELECTED || obj->state == LUI_STATE_PRESSED)
	{
		knob_color = slider->style.selection_color;
	}

	uint8_t is_hor = obj->common_style.width >= obj->common_style.height;
	/* For horizontal */
	if (is_hor)
	{
		/* draw the filled region (left) first */
		lui_gfx_draw_rect_fill(obj->x, obj->y, slider->knob_center_rel_d, obj->common_style.height, slider->style.bg_filled_color);
		/* draw the remaining region (right)  */
		lui_gfx_draw_rect_fill(obj->x + slider->knob_center_rel_d, obj->y, obj->common_style.width - slider->knob_center_rel_d, obj->common_style.height, obj->common_style.bg_color);
	}
	/* For vertical */
	else
	{
		/* draw the remaining region (top) first */
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, slider->knob_center_rel_d, obj->common_style.bg_color);
		/* draw the filled region (bottom) */
		lui_gfx_draw_rect_fill(obj->x, obj->y + slider->knob_center_rel_d, obj->common_style.width, obj->common_style.height - slider->knob_center_rel_d, slider->style.bg_filled_color);
	}
	
	// draw the knob
	if (slider->knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT)
	{	
		/* For horizontal */
		if (is_hor)
			lui_gfx_draw_rect_fill(obj->x + slider->knob_center_rel_d - (slider->style.knob_width / 2), obj->y, slider->style.knob_width, obj->common_style.height, knob_color);
		/* For vertical */
		else
			lui_gfx_draw_rect_fill(obj->x, obj->y + slider->knob_center_rel_d - (slider->style.knob_width / 2), obj->common_style.width, slider->style.knob_width, knob_color);

	}
	else if (slider->knob_type == LUI_SLIDER_KNOB_TYPE_TEXT)
	{
		if (slider->show_value || slider->custom_text)
		{
			char s[64];
			if (slider->custom_text && slider->show_value)
				snprintf(s, 64, "%d %s", obj->value, slider->custom_text);
			else if (slider->show_value)
				snprintf(s, 64, "%d", obj->value);
			else
				snprintf(s, 64, "%s", slider->custom_text);
			uint16_t dim[2];
			lui_gfx_get_string_dimension(s, slider->font, obj->common_style.width, dim);
			uint16_t txt_x = 0, txt_y = 0;
			if (slider->is_progress_bar)
			{
				txt_x = obj->x + (obj->common_style.width - dim[0])/2;
				txt_y = obj->y + (obj->common_style.height - dim[1])/2;
			}
			else
			{
				if (is_hor)
				{
					txt_x = obj->x + slider->knob_center_rel_d;
					if (dim[0] < slider->knob_center_rel_d)
						txt_x = txt_x - dim[0];
				}
				else
				{
					txt_y = obj->y + slider->knob_center_rel_d;
					if (dim[1] < slider->knob_center_rel_d)
						txt_y = txt_y + dim[1];
				}
			}

			lui_area_t txt_area = {
				.x = txt_x,
				.y = txt_y,
				.w = dim[0],
				.h = dim[1]
			};
			lui_gfx_draw_string_advanced(s, &txt_area, slider->style.knob_color, 0, NULL, NULL, NULL, 0, slider->font);
		}
	}


	// draw the border if needed
	if (obj->common_style.border_width)
	{
		lui_gfx_draw_rect(
			obj->x, obj->y, obj->common_style.width,
			obj->common_style.height,
			obj->common_style.border_width,
			obj->common_style.border_color);
	}
}

lui_obj_t* lui_slider_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;


	lui_slider_t* initial_slider =  (lui_slider_t* )_lui_mem_alloc(sizeof(*initial_slider));
	if (initial_slider == NULL)
		return NULL;
	
	initial_slider->style.bg_filled_color = LUI_STYLE_SLIDER_BG_FILLED_COLOR;
	initial_slider->style.knob_color = LUI_STYLE_SLIDER_KNOB_COLOR;
	initial_slider->style.selection_color = LUI_STYLE_SLIDER_SELECTION_COLOR;
	initial_slider->style.knob_width = LUI_STYLE_SLIDER_KNOB_WIDTH;
	initial_slider->range_min = 0;
	initial_slider->range_max = 100;
	initial_slider->knob_center_rel_d = LUI_SLIDER_KNOB_TYPE_DEFAULT ? LUI_STYLE_SLIDER_KNOB_WIDTH / 2 : 0;
	initial_slider->knob_type = LUI_SLIDER_KNOB_TYPE_DEFAULT;
	initial_slider->font = g_lui_main->default_font;
	initial_slider->custom_text = NULL;
	initial_slider->show_value = 0;
	initial_slider->is_progress_bar = 0;

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_SLIDER;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_SLIDER_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_SLIDER_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_SLIDER_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_SLIDER_WIDTH;
	obj->common_style.height = LUI_STYLE_SLIDER_HEIGHT;

	obj->obj_main_data = (void* )initial_slider;

	return  obj;
}

lui_obj_t* lui_slider_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(slider, obj_parent)
}

void lui_slider_set_extra_colors(lui_obj_t* obj, uint16_t knob_color, uint16_t bg_filled_color, uint16_t selection_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;
	
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;

	if (slider->style.knob_color == knob_color && slider->style.bg_filled_color == bg_filled_color && slider->style.selection_color == selection_color)
		return;
	slider->style.knob_color = knob_color;
	slider->style.bg_filled_color = bg_filled_color;
	slider->style.selection_color = selection_color;
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_show_value(lui_obj_t* obj, uint8_t show_val)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;
	
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	if (slider->show_value == show_val)
		return;

	slider->show_value = show_val;
	if (show_val)
	{
		lui_slider_set_knob_type(obj, LUI_SLIDER_KNOB_TYPE_TEXT);
		return;
	}
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_value(lui_obj_t* obj, int16_t value)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;
	
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	
	
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
	

	/* calculate knob's center x position relative to the slider, when value of slider is manually set by user (y is always same) */
	uint16_t knob_w = slider->knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT ? slider->style.knob_width : 0;
	if (obj->common_style.width > obj->common_style.height)
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, obj->common_style.width - (knob_w / 2), (knob_w / 2));
	else
		/* For vertical slider, relation between y-axis and value is inverse. So, we swap new max and min */
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, (knob_w / 2), obj->common_style.height - (knob_w / 2));

	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_range(lui_obj_t* obj, int16_t range_min, int16_t range_max)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;
	
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;

	if (range_min == slider->range_min && range_max == slider->range_max)
		return;
	
	slider->range_max = range_max;
	slider->range_min = range_min;

	// limiting within max and min
	obj->value = obj->value > range_max ? range_max : (obj->value < range_min ? range_min : obj->value);

	// calculate knob's center x position relative to the slider, when value of slider is manually set by user (y is always same)
	uint16_t knob_w = slider->knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT ? slider->style.knob_width : 0;
	if (obj->common_style.width > obj->common_style.height)
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, obj->common_style.width - (knob_w / 2), (knob_w / 2));
	else
		/* For vertical slider, relation between y-axis and value is inverse. So, we swap new max and min */
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, (knob_w / 2), obj->common_style.height - (knob_w / 2));

	_lui_object_set_need_refresh(obj);
}

int16_t lui_slider_get_value(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return -1;
	
	return obj->value;
}

int16_t lui_slider_get_min_value(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return -1;
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;

	return slider->range_min;
}

int16_t lui_slider_get_max_value(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return -1;
	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	
	return slider->range_max;
}

void lui_slider_set_progress_bar(lui_obj_t* obj, uint8_t is_progress_bar)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;

	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	if (slider->is_progress_bar == is_progress_bar)
		return;

	slider->is_progress_bar = is_progress_bar;
	if (is_progress_bar)
	{
		obj->enabled = 0;
		lui_slider_set_knob_type(obj, LUI_SLIDER_KNOB_TYPE_TEXT);
		slider->show_value = 1;
	}
	else
	{
		obj->enabled = 1;
		lui_slider_set_knob_type(obj, LUI_SLIDER_KNOB_TYPE_DEFAULT);
		slider->show_value = 0;
	}
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_text(lui_obj_t* obj, const char* custom_text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;

	((lui_slider_t* )obj->obj_main_data)->custom_text = (char*)custom_text;
	_lui_object_set_need_refresh(obj);
}

void lui_slider_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return;
	if (font == NULL)
		return;

	((lui_slider_t* )obj->obj_main_data)->font = font;
	obj->common_style.height = font->bitmap->size_y > LUI_STYLE_SLIDER_HEIGHT ? font->bitmap->size_y : LUI_STYLE_SLIDER_HEIGHT;
	_lui_object_set_need_refresh(obj);

	return;
}

int8_t lui_slider_set_knob_type(lui_obj_t* obj, uint8_t knob_type)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SLIDER) < 0)
		return -1;
	if (knob_type > LUI_SLIDER_KNOB_TYPE_TEXT)
		return -1;

	lui_slider_t* slider = (lui_slider_t* )obj->obj_main_data;
	if (slider->knob_type == knob_type)
		return -1;
	if (slider->is_progress_bar && knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT)
		return -1;
	slider->knob_type = knob_type;
	uint16_t knob_w = slider->knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT ? slider->style.knob_width : 0;
	if (obj->common_style.width > obj->common_style.height)
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, obj->common_style.width - (knob_w / 2), (knob_w / 2));
	else
		/* For vertical slider, relation between y-axis and value is inverse. So, we swap new max and min */
		slider->knob_center_rel_d = _lui_map_range(obj->value, slider->range_max, slider->range_min, (knob_w / 2), obj->common_style.height - (knob_w / 2));
	_lui_object_set_need_refresh(obj);

	return 0;
}

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */


/*-------------------------------------------------------------------------------
 * 				LUI_BTNGRID related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_BUTTONGRID)

void lui_btngrid_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
		
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
			if (!(btngrid->btn_properties[i] & LUI_BTNGRID_MASK_BTN_HIDDEN))
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
					if (btngrid->btn_properties[i] & LUI_BTNGRID_MASK_BTN_CHECKABLE)
					{ 
						if (btngrid->btn_properties[i] & LUI_BTNGRID_MASK_BTN_CHECKED)
						{
							btn_color = btngrid->style.btn_pressed_color;
						}
					}
				}

				btn_width = btngrid->btn_area[i].x2 - btngrid->btn_area[i].x1 + 1;
				btn_height = btngrid->btn_area[i].y2 - btngrid->btn_area[i].y1 + 1;
				lui_gfx_draw_rect_fill(btngrid->btn_area[i].x1, btngrid->btn_area[i].y1, btn_width, btn_height, btn_color);

				if (obj->common_style.border_width)
				{
					lui_gfx_draw_rect(
						btngrid->btn_area[i].x1, btngrid->btn_area[i].y1,
						btn_width, btn_height,
						obj->common_style.border_width,
						obj->common_style.border_color);
				}

				uint16_t str_width_height[2];
				lui_gfx_get_string_dimension(btngrid->texts[j], btngrid->font, btn_width, str_width_height);

				str_width_height[0] = str_width_height[0] > btn_width ? btn_width : str_width_height[0];
				str_width_height[1] = str_width_height[1] > btn_height ? btn_height : str_width_height[1];

				uint16_t temp_x = btngrid->btn_area[i].x1 + (btn_width - str_width_height[0]) / 2;
				uint16_t temp_y = btngrid->btn_area[i].y1 + (btn_height - str_width_height[1]) / 2;

				lui_area_t btngrd_btn_area = {
					.x = temp_x,
					.y = temp_y,
					.w = str_width_height[0],
					.h = str_width_height[1]
				};
				lui_gfx_draw_string_advanced(btngrid->texts[j], &btngrd_btn_area, btngrid->style.btn_label_color, 0, NULL, NULL, NULL, 0, btngrid->font);
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


	lui_btngrid_t* initial_btngrid =  (lui_btngrid_t* )_lui_mem_alloc(sizeof(*initial_btngrid));
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
	#if defined(LUI_USE_KEYBOARD)
	initial_btngrid->kb_data = NULL;
	#endif

	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_BTNGRID;
	obj->enabled = 1;
	// object common style
	obj->common_style.bg_color = LUI_STYLE_BTNGRID_BASE_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_BTNGRID_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_BTNGRID_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_BTNGRID_WIDTH;
	obj->common_style.height = LUI_STYLE_BTNGRID_HEIGHT;

	obj->obj_main_data = (void* )initial_btngrid;

	return  obj;
}

lui_obj_t* lui_btngrid_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(btngrid, obj_parent)
}

void lui_btngrid_set_textmap(lui_obj_t* obj, const char* texts[])
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;

	if (texts == NULL)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
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
			btngrid->btn_area = (_lui_area_priv_t* )_lui_mem_alloc(buttons * sizeof(_lui_area_priv_t));
			if (btngrid->btn_area == NULL)
				return;
			btngrid->btn_properties = (uint8_t* )_lui_mem_alloc(buttons * sizeof(uint8_t));
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
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	if (properties == NULL)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
	if (btngrid->btn_properties == NULL)
		return;
	
	memcpy(btngrid->btn_properties, properties, btngrid->btn_cnt);
	btngrid->needs_full_render = 1;
	_lui_object_set_need_refresh(obj);
	_lui_btngrid_calc_btn_area(obj);
}

void lui_btngrid_set_btn_property_bits(lui_obj_t* obj, uint16_t btn_index, uint8_t property_byte)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

	if (btngrid->btn_properties == NULL || btn_index >= btngrid->btn_cnt)
		return;
	if (btngrid->btn_properties[btn_index] == property_byte)
		return;
	if ((property_byte & LUI_BTNGRID_MASK_BTN_WIDTH_UNIT) == 0)
		return;

	btngrid->btn_properties[btn_index] = property_byte;
	_lui_btngrid_calc_btn_area(obj);

	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void lui_btngrid_set_btn_text(lui_obj_t* obj, uint8_t btn_index, char* text)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

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
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~LUI_BTNGRID_MASK_BTN_WIDTH_UNIT;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property | (width_unit & LUI_BTNGRID_MASK_BTN_WIDTH_UNIT));
}

void lui_btngrid_set_btn_hidden(lui_obj_t* obj, uint16_t btn_index, uint8_t hidden)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~LUI_BTNGRID_MASK_BTN_HIDDEN;
	property = hidden ? (property | LUI_BTNGRID_MASK_BTN_HIDDEN) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
}

void lui_btngrid_set_btn_checkable(lui_obj_t* obj, uint16_t btn_index, uint8_t checkable)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~LUI_BTNGRID_MASK_BTN_CHECKABLE;
	property = checkable ? (property | LUI_BTNGRID_MASK_BTN_CHECKABLE) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
	if (checkable)
	{
		((lui_btngrid_t* )(obj->obj_main_data))->needs_full_render = 0; /* if something is set to checkable, no need to redarw entire btngrid */
	}
}

void lui_btngrid_set_btn_checked(lui_obj_t* obj, uint16_t btn_index, uint8_t checked)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	uint8_t property = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index] & ~LUI_BTNGRID_MASK_BTN_CHECKED;
	property = checked ? (property | LUI_BTNGRID_MASK_BTN_CHECKED) : property;
	lui_btngrid_set_btn_property_bits(obj, btn_index, property);
}

int16_t lui_btngrid_get_acive_btn_index(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return -1;
	
	return ((lui_btngrid_t* )(obj->obj_main_data))->active_btn_index;
}

const char* lui_btngrid_get_btn_text(lui_obj_t* obj, uint16_t btn_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return NULL;
	
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

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

int8_t lui_btngrid_get_btn_check_status(lui_obj_t* obj, uint8_t btn_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return -1;
	
	uint8_t props = ((lui_btngrid_t* )(obj->obj_main_data))->btn_properties[btn_index];
	return  (props & LUI_BTNGRID_MASK_BTN_CHECKED) ? 1 : 0;
}

void lui_btngrid_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	if (font == NULL)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
	btngrid->font = font;
	_lui_object_set_need_refresh(obj);
	btngrid->needs_full_render = 1;
}

void lui_btngrid_set_extra_colors(lui_obj_t* obj, uint16_t btn_color, uint16_t label_color, uint16_t btn_pressed_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

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
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
	
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

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
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;
		
	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
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
			units_in_row += (btngrid->btn_properties[unit_index++] & LUI_BTNGRID_MASK_BTN_WIDTH_UNIT);
			++btns_in_row;
			++i;          
		}
		
		float raw_width = (float)(obj->common_style.width) / (float)units_in_row;

		w = 0.0;
		h += raw_height;
		for (int j = 0; j < btns_in_row; j++)
		{
			_lui_area_priv_t area;
			float this_btn_w = raw_width * (float)(btngrid->btn_properties[btn_index] & LUI_BTNGRID_MASK_BTN_WIDTH_UNIT);
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

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_KEYBOARD related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_KEYBOARD) && defined(LUI_USE_BUTTONGRID)

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

	lui_keyboard_t* initial_kb = (lui_keyboard_t* )_lui_mem_alloc(sizeof(*initial_kb));
	if (initial_kb == NULL)
		return NULL;
	initial_kb->target_txtbox = NULL;
	initial_kb->keyboard_mode = LUI_KEYBOARD_MODE_TXT_LOWER;
	
	lui_obj_t* obj_btngrid = lui_btngrid_create();
	if (obj_btngrid == NULL)
		return NULL;
	((lui_btngrid_t* )(obj_btngrid->obj_main_data))->kb_data = initial_kb;
	uint16_t h = 0.5 * (float)g_lui_main->disp_drv->display_vert_res;
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

lui_obj_t* lui_keyboard_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(keyboard, obj_parent)
}

void lui_keyboard_sys_cb(lui_obj_t* obj_sender)
{
	if (_lui_verify_obj(obj_sender, LUI_OBJ_BTNGRID) < 0)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj_sender->obj_main_data);
	if (btngrid->kb_data->target_txtbox == NULL)
		return;

	int16_t active_btn_id = btngrid->active_btn_index;
	const char* btn_text = lui_keyboard_get_key_text(obj_sender, active_btn_id);
	if (active_btn_id == -1)
		return;


	uint16_t caret_index = lui_textbox_get_caret_index(btngrid->kb_data->target_txtbox);
	if (strcmp(btn_text, LUI_ICON_CHECKMARK) == 0)
	{
		btngrid->kb_data->target_txtbox->state = LUI_STATE_IDLE;
		btngrid->kb_data->target_txtbox->event = LUI_EVENT_EXITED;
		if (btngrid->kb_data->target_txtbox->obj_event_cb)
			btngrid->kb_data->target_txtbox->obj_event_cb(btngrid->kb_data->target_txtbox);
	}
	else if (strcmp(btn_text, LUI_ICON_CLOSE) == 0)
	{
		lui_textbox_t* txtbox = (lui_textbox_t* )(btngrid->kb_data->target_txtbox->obj_main_data);
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
		// lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, ++caret_index);
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
		g_lui_needs_render = 1;
	}
	else
	{
		lui_textbox_insert_char(btngrid->kb_data->target_txtbox, btn_text[0]);
		// lui_textbox_set_caret_index(btngrid->kb_data->target_txtbox, lui_textbox_get_caret_index(btngrid->kb_data->target_txtbox) + 1);
	}

}

void lui_keyboard_set_mode(lui_obj_t* obj, uint8_t mode)
{
	if (_lui_verify_obj(obj, LUI_OBJ_BTNGRID) < 0)
		return;

	lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);

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
	g_lui_needs_render = 1;


}

void lui_keyboard_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	lui_btngrid_set_font(obj, font);
}

const char* lui_keyboard_get_key_text(lui_obj_t* obj, uint8_t btn_index)
{
	return lui_btngrid_get_btn_text(obj, btn_index);
}

void lui_keyboard_set_target_txtbox(lui_obj_t* obj_kb, lui_obj_t* obj_txtbox)
{
	if (_lui_verify_obj(obj_kb, LUI_OBJ_BTNGRID) < 0)
		return;
	// type check for textbox
	if (obj_txtbox != NULL && obj_txtbox->obj_type != LUI_OBJ_TEXTBOX)
		return;

	lui_btngrid_t* btngrid_kb = (lui_btngrid_t* )(obj_kb->obj_main_data);
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

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_TEXTBOX related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_TEXTBOX)

void lui_textbox_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	if (!(obj->visible))
		return;
	
	// if no display driver is registered, return
	if (_lui_disp_drv_check() == 0)
		return;

	
	lui_textbox_t* txtbox = (lui_textbox_t* )(obj->obj_main_data);
	
	if (txtbox->needs_full_render == 1)
	{
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);
		if (obj->common_style.border_width)
		{
			lui_gfx_draw_rect(
				obj->x, obj->y, obj->common_style.width,
				obj->common_style.height,
				obj->common_style.border_width,
				obj->common_style.border_color);
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

	lui_area_t txtbx_area = {
		.x = obj->x + pad,
		.y = obj->y + pad,
		.w = obj->common_style.width - 2*pad,
		.h = obj->common_style.height - 2*pad
	};
	lui_gfx_draw_string_advanced(
		txtbox->text_buffer, 
		&txtbx_area, 
		txtbox->style.text_color, 
		obj->common_style.bg_color,
		NULL,
		NULL,
		NULL,
		1, 
		txtbox->font);
	

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
			const _lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(txtbox->text_buffer[i], txtbox->font);
			uint8_t glyph_h = txtbox->font->bitmap->size_y;
			uint8_t glyph_w = 0;
			uint8_t glyph_xadv = 0;
// 			uint8_t glyph_xoff = 0; // not needed here

			if (glyph == NULL)
			{
				glyph_w = txtbox->font->bitmap->size_y / 2;
				glyph_xadv = glyph_w;
			}
			/* Width of space is not correct in older font maps, so we calc w based on h */
			else if (glyph->character == ' ' && glyph->x_adv == 0)
			{
				glyph_w = txtbox->font->bitmap->size_y / 4;
				glyph_xadv = glyph_w;
			}
			else
			{
				glyph_w = glyph->width;
				glyph_xadv = _LUI_MAX(glyph->x_adv, glyph_w);	// becasue in some rare cases x_adv = 0
// 				glyph_xoff = glyph->x_off; // not needed here
			}

			// check if not enough space available at the right side
			if (caret_x + glyph_xadv > obj->x + obj->common_style.width - pad)
			{
				caret_x = obj->x + pad;			//go to first col
				caret_y += glyph_h;					//go to next row
			}
			// check if not enough space available at the bottom
			if(caret_y + glyph_h > obj->y + obj->common_style.height - pad)
			{
				break;
			}
			caret_x += glyph_xadv;		//next char position
		}
	}

	/* Whyy???*/
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


	lui_textbox_t* initial_textbox =  (lui_textbox_t* )_lui_mem_alloc(sizeof(*initial_textbox));
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
	obj->common_style.border_width = LUI_STYLE_TEXTBOX_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_TEXTBOX_WIDTH;
	obj->common_style.height = LUI_STYLE_TEXTBOX_HEIGHT;

	obj->obj_main_data = (void* )initial_textbox;

	return  obj;
}

lui_obj_t* lui_textbox_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(textbox, obj_parent)
}

void lui_textbox_enter_edit_mode(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	obj->state = LUI_STATE_ENTERED;
	obj->event = LUI_EVENT_ENTERED;
	if (obj->obj_event_cb)
		obj->obj_event_cb(obj);
	_lui_object_set_need_refresh(obj);
}

void lui_textbox_exit_edit_mode(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	obj->state = LUI_STATE_IDLE;
	obj->event = LUI_EVENT_EXITED;
	if (obj->obj_event_cb)
		obj->obj_event_cb(obj);
	_lui_object_set_need_refresh(obj);
}

void lui_textbox_set_caret_index(lui_obj_t* obj, uint16_t caret_index)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
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
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return 0;
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	return txtbox->caret_index;
}

int8_t lui_textbox_insert_char(lui_obj_t* obj, char c)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return -1;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return -1;
	
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	if (txtbox->used_chars + 1 > txtbox->max_len)
		return -1;
	for (int32_t i = txtbox->used_chars - 1; i >= txtbox->caret_index; i--)
	{
		txtbox->text_buffer[i + 1] = txtbox->text_buffer[i];
	}
	txtbox->text_buffer[txtbox->caret_index] = c;
	++(txtbox->used_chars);
	/* Increase caret index after inserting char */
	++(txtbox->caret_index);
	if (txtbox->caret_index > txtbox->used_chars - 1)
		txtbox->caret_index = txtbox->used_chars - 1;

	_lui_object_set_need_refresh(obj);
	return 0;
}

int8_t lui_textbox_delete_char(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return -1;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return -1;
	
	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	if (txtbox->used_chars <= 1)
		return -1;
	for (int32_t i = txtbox->caret_index; i < txtbox->used_chars; i++)
	{
		txtbox->text_buffer[i] = txtbox->text_buffer[i + 1];
	}
	--(txtbox->used_chars);

	_lui_object_set_need_refresh(obj);
	return 0;
}

int8_t lui_textbox_insert_string(lui_obj_t* obj, char* str, uint16_t len)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return -1;
	if (((lui_textbox_t* )obj->obj_main_data)->text_buffer == NULL)
		return -1;
	for (uint16_t i = 0; i < len; i++)
	{
		if (str[i] == '\0')
			return 0;
		if (lui_textbox_insert_char(obj, str[i]) < 0)
			return -1;
	}
	_lui_object_set_need_refresh(obj);
	return 0;
}

void lui_textbox_set_text_buffer(lui_obj_t* obj, char* text_buffer, uint16_t buff_size)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	lui_textbox_t* txtbox = (lui_textbox_t* )obj->obj_main_data;
	txtbox->text_buffer = text_buffer;
	txtbox->max_len = buff_size;
	txtbox->text_buffer[0] = '\0';
	txtbox->used_chars = 1;
	txtbox->caret_index = 0;

	_lui_object_set_need_refresh(obj);
}

void lui_textbox_clear_text_buffer(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	lui_textbox_t* txtbox = (lui_textbox_t* )(obj->obj_main_data);
	txtbox->used_chars = 1;
	txtbox->text_buffer[0] = '\0';
	txtbox->caret_index = 0;
	_lui_object_set_need_refresh(obj);
}

void lui_textbox_set_text_color(lui_obj_t* obj, uint16_t text_color)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;

	lui_textbox_t* txtbox = (lui_textbox_t* )(obj->obj_main_data);
	txtbox->style.text_color = text_color;
	_lui_object_set_need_refresh(obj);
	txtbox->needs_full_render = 1;
}

void lui_textbox_set_font(lui_obj_t* obj, const lui_font_t* font)
{
	if (_lui_verify_obj(obj, LUI_OBJ_TEXTBOX) < 0)
		return;
	if (font == NULL)
		return;

	lui_textbox_t* txtbox = (lui_textbox_t* )(obj->obj_main_data);
	txtbox->font = font;
	_lui_object_set_need_refresh(obj);
	txtbox->needs_full_render = 1;
}

#endif

/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */

/*-------------------------------------------------------------------------------
 * 				LUI_PANEL related functions
 *-------------------------------------------------------------------------------
 */

#if defined(LUI_USE_PANEL)

/*
 * Initialize a label with default values
 */
lui_obj_t* lui_panel_create()
{
	// if total created objects become more than max allowed objects, don't create the object
	if ( g_lui_main->total_created_objects + 1 > LUI_MAX_OBJECTS)
		return NULL;
	 g_lui_main->total_created_objects++;

	lui_panel_t* initial_panel = (lui_panel_t* )_lui_mem_alloc(sizeof(*initial_panel));
	if (initial_panel == NULL)
		return NULL;
	initial_panel->layout.type = LUI_LAYOUT_NONE;
	initial_panel->layout.dim = 0;
	initial_panel->layout.pad_x = 2;
	initial_panel->layout.pad_y = 2;
	initial_panel->bg_image = NULL;
	/* Image palette for mono 1-bpp bitmap image */
	initial_panel->img_pal.fore_color = LUI_RGB(255, 2555, 255);
	initial_panel->img_pal.back_color = 0;
	initial_panel->img_pal.is_backgrnd = 1;
	lui_obj_t* obj = _lui_object_create();
	if (obj == NULL)
		return NULL;
	// object type
	obj->obj_type = LUI_OBJ_PANEL;
	obj->enabled = 1;
	// object comon style
	obj->common_style.bg_color = LUI_STYLE_PANEL_BG_COLOR;
	obj->common_style.border_color = LUI_STYLE_PANEL_BORDER_COLOR;
	obj->common_style.border_width = LUI_STYLE_PANEL_BORDER_THICKNESS;
	obj->common_style.width = LUI_STYLE_PANEL_WIDTH;
	obj->common_style.height = LUI_STYLE_PANEL_HEIGHT;
	obj->obj_main_data = (void* )initial_panel;	// will add panel specific main data later

	return obj;
}

lui_obj_t* lui_panel_create_and_add(lui_obj_t* obj_parent)
{
	_LUI_CREATE_AND_ADD(panel, obj_parent)
}

void lui_panel_draw(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_PANEL) < 0)
		return;

	if (!(obj->visible))
		return;
	
	lui_panel_t* panel = (lui_panel_t* )(obj->obj_main_data);
	if (panel->bg_image)
	{
		lui_area_t crop = {
			.x = 0,
			.y = 0,
			.w = obj->common_style.width,
			.h = obj->common_style.height
		};
		lui_gfx_draw_bitmap(
			panel->bg_image, 
			&panel->img_pal,
			obj->x, obj->y, 
			&crop);
	}
	else
	{
		/* Else, just draw background color */
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width,  obj->common_style.height, obj->common_style.bg_color);
	}

	/* Draw optional border */
	if (obj->common_style.border_width)
		lui_gfx_draw_rect(
			obj->x, obj->y, obj->common_style.width,
			obj->common_style.height,
			obj->common_style.border_width,
			obj->common_style.border_color);
}

void lui_panel_set_bitmap_image(lui_obj_t* obj, const lui_bitmap_t* bitmap)
{
	/* As panel and scene both have same first elements in the struct, we can re-use scene's function here */
	lui_scene_set_bitmap_image(obj, bitmap);
}

void lui_panel_set_bitmap_image_mono_palette(lui_obj_t* obj, lui_bitmap_mono_pal_t* palette)
{
	lui_scene_set_bitmap_image_mono_palette(obj, palette);
}

int8_t lui_panel_layout_set_properties(lui_obj_t* obj, uint8_t type, uint8_t pad_x, uint8_t pad_y)
{
	if (_lui_verify_obj(obj, LUI_OBJ_PANEL) < 0)
		return -1;
	return _lui_layout_set_properties(obj, type, pad_x, pad_y);
}

int8_t  lui_panel_layout_calculate(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_PANEL) < 0)
		return -1;
	return _lui_layout_calculate(obj);
}



#endif

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

	lui_scene_t* initial_scene =  (lui_scene_t* )_lui_mem_alloc(sizeof(*initial_scene));
	if (initial_scene == NULL)
		return NULL;

	initial_scene->layout.type = LUI_LAYOUT_NONE;
	initial_scene->layout.dim = 0;
	initial_scene->layout.pad_x = 2;
	initial_scene->layout.pad_y = 2;
	initial_scene->font = g_lui_main->default_font;
	initial_scene->bg_image = NULL;
	/* Color palette for mono 1-bpp bitmap image */
	initial_scene->img_pal.fore_color = LUI_RGB(255, 2555, 255);
	initial_scene->img_pal.back_color = 0;
	initial_scene->img_pal.is_backgrnd = 1;
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
	if (_lui_verify_obj(obj, LUI_OBJ_SCENE) < 0)
		return;

	if (!(obj->visible))
		return;

	lui_scene_t* scene = (lui_scene_t* )(obj->obj_main_data);
	if (scene->bg_image)
	{
		lui_area_t crop = {
			.x = 0,
			.y = 0,
			.w = obj->common_style.width,
			.h = obj->common_style.height
		};
		lui_gfx_draw_bitmap(
			scene->bg_image, 
			&scene->img_pal,
			obj->x, obj->y, 
			&crop);
	}
	else
		lui_gfx_draw_rect_fill(obj->x, obj->y, obj->common_style.width, obj->common_style.height, obj->common_style.bg_color);
}

void lui_scene_set_bitmap_image(lui_obj_t* obj, const lui_bitmap_t* bitmap)
{
	if (obj == NULL)
		return;
	
	// type check
	/* NOTE: panel and scene both have same first elements in the struct. So, we can use
	 * this function for panel too. :)
	 */
	if (obj->obj_type != LUI_OBJ_SCENE && obj->obj_type != LUI_OBJ_PANEL)
		return;

	lui_scene_t* scene = (lui_scene_t* )(obj->obj_main_data);
	scene->bg_image = bitmap;
	_lui_object_set_need_refresh(obj);
}

void lui_scene_set_bitmap_image_mono_palette(lui_obj_t* obj, lui_bitmap_mono_pal_t* palette)
{
	if (obj == NULL)
		return;
	
	// type check
	/* NOTE: panel and scene both have same first elements in the struct. So, we can use
	 * this function for panel too. :)
	 */
	if (obj->obj_type != LUI_OBJ_SCENE && obj->obj_type != LUI_OBJ_PANEL)
		return;

	lui_scene_t* scene = (lui_scene_t* )(obj->obj_main_data);

	if (palette)
		scene->img_pal = *palette;

	_lui_object_set_need_refresh(obj);
}


// void lui_scene_set_font(lui_obj_t* obj_scene, const lui_font_t* font)
//		return;
	
// 	lui_scene_t* scene = (lui_scene_t* )(obj_scene->obj_main_data);
// 	scene->font = font;
	
// 	_lui_object_set_need_refresh(obj_scene); 
// } {
// 	if (obj_scene == NULL)
// 		return;
// 	if (font == NULL)
// 		return;
// 	// type check
// 	if (obj_scene->obj_type != LUI_OBJ_SCENE)
// 		return;
	
// 	lui_scene_t* scene = (lui_scene_t* )(obj_scene->obj_main_data);
// 	scene->font = font;
	
// 	_lui_object_set_need_refresh(obj_scene); 
// }

void lui_scene_set_active(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SCENE) < 0)
		return;
	
	g_lui_main->active_scene = obj;
	_lui_object_set_need_refresh(obj);
}

int8_t lui_scene_layout_set_properties(lui_obj_t* obj, uint8_t type, uint8_t pad_x, uint8_t pad_y)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SCENE) < 0)
		return -1;
	return _lui_layout_set_properties(obj, type, pad_x, pad_y);
}

int8_t  lui_scene_layout_calculate(lui_obj_t* obj)
{
	if (_lui_verify_obj(obj, LUI_OBJ_SCENE) < 0)
		return -1;
	return _lui_layout_calculate(obj);
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
	lui_obj_t* obj =  (lui_obj_t* )_lui_mem_alloc(sizeof(*obj));
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
	obj->parent = NULL;
	obj->first_child = NULL;
	obj->next_sibling = NULL;
	obj->children_count = 0;
	g_lui_needs_render = 1;
	return obj;
}

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
	lui_object_set_layer(obj_child,obj_child->layer);
	obj_parent->children_count++;
	_lui_object_set_need_refresh(obj_child);
}

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

lui_obj_t* lui_object_get_child(lui_obj_t* obj_parent, uint16_t child_index)
{
	if (obj_parent == NULL)
		return NULL;
	if (obj_parent->children_count == 0 || child_index >= obj_parent->children_count)
		return NULL;

	lui_obj_t* child = obj_parent->first_child;

	for (uint16_t i = 0; i < child_index; i++)
	{
		child = child->next_sibling;
	}

	return child;
}

void lui_object_get_position_rel(lui_obj_t* obj, uint16_t pos[2])
{
	if (obj == NULL)
		return;
	if (obj->parent == NULL)
	{
		pos[0] = 0;
		pos[1] = 0;
	}
	else
	{
		pos[0] = obj->x - obj->parent->x;
		pos[1] = obj->y - obj->parent->y;
	}
}

void lui_object_get_position_abs(lui_obj_t* obj, uint16_t pos[2])
{
	if (obj == NULL)
		return;
	pos[0] = obj->x;
	pos[1] = obj->y;
}

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

			child->x = child->x + (obj->x - obj_old_x); // offset the child (current obj) based on parent
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

void lui_object_set_x_pos(lui_obj_t* obj, uint16_t x)
{
	if (obj == NULL)
		return;
	lui_object_set_position(obj, x, obj->y);
}

void lui_object_set_y_pos(lui_obj_t* obj, uint16_t y)
{
	if (obj == NULL)
		return;
	lui_object_set_position(obj, obj->x, y);
}

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

	#if defined(LUI_USE_BUTTONGRID)
	/* If object is an button grid, we need to recalculate the layout when area is changed
	 * But, we must check if text map is not null.
	 */
	if (obj->obj_type == LUI_OBJ_BTNGRID && ((lui_btngrid_t* )(obj->obj_main_data))->texts != NULL)
	{
		_lui_btngrid_calc_btn_area(obj);
	}
	#endif

}

void lui_object_set_width(lui_obj_t* obj, uint16_t width)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(obj, width, obj->common_style.height);
}

void lui_object_set_height(lui_obj_t* obj, uint16_t height)
{
	if (obj == NULL)
		return;	
	lui_object_set_area(obj, obj->common_style.width, height);
}

void lui_object_set_border_color(lui_obj_t* obj, uint16_t border_color)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_color == border_color)
		return;
	obj->common_style.border_color = border_color;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_border_visibility(lui_obj_t* obj, uint8_t is_visible)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_width == is_visible)
		return;
	obj->common_style.border_width = (is_visible == 0) ? 0 : 1;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_border_width(lui_obj_t* obj, uint8_t width)
{
	if (obj == NULL)
		return;
	if (obj->common_style.border_width == width)
		return;
	obj->common_style.border_width = width;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_bg_color(lui_obj_t* obj, uint16_t bg_color)
{
	if (obj == NULL)
		return;
	if (obj->common_style.bg_color == bg_color)
		return;
	obj->common_style.bg_color = bg_color;
	_lui_object_set_need_refresh(obj);
}

void lui_object_set_callback(lui_obj_t* obj, void (*obj_event_cb)(lui_obj_t* ))
{
	if (obj == NULL)
		return;
	obj->obj_event_cb = obj_event_cb;
}

int8_t lui_object_get_state(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	return obj->state;
}

int8_t lui_object_get_event(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	return obj->event;
}

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
			#if defined(LUI_USE_BUTTONGRID)
			((lui_btngrid_t* )(obj->obj_main_data))->needs_full_render = 1;
			#endif
		}
		else if (obj->obj_type == LUI_OBJ_TEXTBOX)
		{
			#if defined(LUI_USE_TEXTBOX)
			((lui_textbox_t* )(obj->obj_main_data))->needs_full_render = 1;
			#endif
		}
	}
	else
	{
		_lui_object_set_need_refresh(obj->parent);
	}
}

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

void lui_object_set_layer(lui_obj_t* obj, uint8_t layer_index)
{
	if (obj == NULL)
		return;
	if (obj->obj_type == LUI_OBJ_SCENE)
		return;

	int16_t layer_diff = layer_index - obj->layer;
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

			child->layer = child->layer + layer_diff;

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

int16_t lui_object_get_type(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;

	return obj->obj_type;
}

int16_t lui_object_get_layer(lui_obj_t* obj)
{
	if (obj == NULL)
		return -1;
	
	return obj->layer;
}

int _lui_obj_layer_cmprtr(const void* p1, const void* p2)
{
	uint8_t l1 = (*((lui_obj_t** )p1))->layer;
	uint8_t l2 = (*((lui_obj_t** )p2))->layer;
	uint8_t t1 = (*((lui_obj_t** )p1))->obj_type;
	uint8_t t2 = (*((lui_obj_t** )p2))->obj_type;
	int16_t ret = 0;
	if (l1 == l2)
	{
		if (t1 == LUI_OBJ_PANEL && t2 != LUI_OBJ_PANEL)
			ret = -1;
		else if (t2 == LUI_OBJ_PANEL && t1 != LUI_OBJ_PANEL)
			ret = 1;
	}
	else
		ret = (l1 - l2);

	return ret;
}



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
	
	g_lui_needs_render = 0;
}

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
				#ifdef LUI_USE_PANEL
				lui_panel_draw(obj);
				#endif
				break;
			case LUI_OBJ_BUTTON:
				lui_button_draw(obj);
				break;
			case LUI_OBJ_SWITCH:
				#ifdef LUI_USE_SWITCH
				lui_switch_draw(obj);
				#endif
				break;
			case LUI_OBJ_CHECKBOX:
				#ifdef LUI_USE_CHECKBOX
				lui_checkbox_draw(obj);
				#endif
				break;
			case LUI_OBJ_SLIDER:
				#ifdef LUI_USE_SLIDER
				lui_slider_draw(obj);
				#endif
				break;
			case LUI_OBJ_LABEL:
				lui_label_draw(obj);
				break;
			case LUI_OBJ_LINECHART:
				#ifdef LUI_USE_LINECHART
				lui_linechart_draw(obj);
				#endif
				break;
			case LUI_OBJ_LIST:
				#ifdef LUI_USE_LIST
				lui_list_draw(obj);
				#endif
				break;
			case LUI_OBJ_BTNGRID:
				#ifdef LUI_USE_BUTTONGRID
				lui_btngrid_draw(obj);
				#endif
				break;
			case LUI_OBJ_TEXTBOX:
				#ifdef LUI_USE_TEXTBOX
				lui_textbox_draw(obj);
				#endif
				break;
			default:
				break;
		}

		obj->needs_refresh = 0;	// drawing is done, so set need_refresh back to 0
	}
}

void _lui_object_set_need_refresh(lui_obj_t* obj)
{
	if (obj == NULL)
		return;
	/* Object's visibility is 0, return */
	if (!obj->visible)
		return;

	g_lui_needs_render = 1;
	/* already flag is 1, no need to waste time in loop. Return. */
	if (obj->needs_refresh == 1)
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
				#if defined(LUI_USE_BUTTONGRID)
				((lui_btngrid_t* )(child->obj_main_data))->needs_full_render = 1;
				#endif
			}
			else if (child->obj_type == LUI_OBJ_TEXTBOX)
			{
				#if defined(LUI_USE_TEXTBOX)
				((lui_textbox_t* )(child->obj_main_data))->needs_full_render = 1;
				#endif
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
		/* Why??!! */
		input_dev_type = LUI_INPUT_TYPE_TOUCH;
	}
	else
	{
		return NULL;
	}

	uint8_t scan_all_objs_flag = 0;
	lui_obj_t* obj_caused_cb = NULL;
	lui_obj_t* last_active_obj =  g_lui_main->active_obj;
	// lui_scene_t* scene_main_data = (lui_scene_t* )( g_lui_main->active_scene->obj_main_data);
	lui_touch_input_data_t input_touch_data;
	uint8_t input_is_pressed = 0;
	
	
	if (input_dev_type == LUI_INPUT_TYPE_TOUCH)
	{
		g_lui_main->touch_input_dev->read_touch_input_cb(&input_touch_data);
		input_is_pressed = input_touch_data.is_pressed;

		if (g_lui_main->last_touch_data.x == input_touch_data.x &&
			g_lui_main->last_touch_data.y == input_touch_data.y &&
			g_lui_main->last_touch_data.is_pressed == input_touch_data.is_pressed)
		{
			return NULL;
		}
		g_lui_main->last_touch_data.x = input_touch_data.x;
		g_lui_main->last_touch_data.y = input_touch_data.y;
		g_lui_main->last_touch_data.is_pressed = input_touch_data.is_pressed;
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
			if (child->layer == LUI_LAYER_POPUP)
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
		if (popup_layer_exists && obj_arr[arr_counter]->layer < LUI_LAYER_POPUP)
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
		#if defined(LUI_USE_BUTTONGRID)
		if (obj->obj_type == LUI_OBJ_BTNGRID)
		{
			lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
			for (uint16_t i = 0; i < btngrid->btn_cnt; i++)
			{
				if (input->x >= btngrid->btn_area[i].x1 && 
					input->x <  btngrid->btn_area[i].x2 &&
					input->y >= btngrid->btn_area[i].y1 &&
					input->y <  btngrid->btn_area[i].y2)
				{
					if (!(btngrid->btn_properties[i] & LUI_BTNGRID_MASK_BTN_DISABLED) &&
						!(btngrid->btn_properties[i] & LUI_BTNGRID_MASK_BTN_HIDDEN))
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
		#endif
		{
			#if defined(LUI_USE_LIST)
			if (obj->obj_type == LUI_OBJ_LIST)
			{
				lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
				if (list->is_expanded)
				{
					is_active = 1;
					uint8_t lim = list->page_first_item_index + list->items_per_page;
					
					for (uint8_t i = list->page_first_item_index; i < lim; i++)
					{
						if (i == list->items_cnt)
							break;
						/* Convert items' local coordinates to global */
						uint16_t x1, y1, x2, y2;
						x1 = list->items[i]->area.x1 + obj->x;
						x2 = list->items[i]->area.x2 + obj->x;
						y1 = list->items[i]->area.y1 + obj->y;
						y2 = list->items[i]->area.y2 + obj->y;

						if (input->x >= x1 && 
							input->x <  x2 &&
							input->y >= y1 &&
							input->y <  y2)
						{
							list->selected_item_index = i;
							g_lui_main->active_obj = obj;
							is_active = 1;
							break;
						}
					}
				}
			}
			else
			#endif
			{
				g_lui_main->active_obj = obj;
				is_active = 1;
			}
		}
	}

	if (is_active == 0)
	{
		// in case input is not on "obj" and previous "active_obj" is same as "obj",
		// set "active_obj" to NULL.
		if (g_lui_main->active_obj == obj)
		{
			g_lui_main->active_obj = NULL;
			#if defined(LUI_USE_BUTTONGRID)
			if (obj->obj_type == LUI_OBJ_BTNGRID)
			{
				((lui_btngrid_t* )(obj->obj_main_data))->active_btn_index = -1;
			}
			#endif
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
		/* if pressed, then....well, then state = PRESSED */
		if (input->is_pressed == 1)
		{
			new_state = LUI_STATE_PRESSED;
		}	
		/* else not pressed, state = SELECTED */
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
	/* when input is touch input and not mouse pointer, `SELECTION_LOST` event should become `RELEASED`  */
	if ((input->x == -1 || input->y == -1) && obj->event == LUI_EVENT_SELECTION_LOST)
		obj->event = LUI_EVENT_RELEASED;


	if (obj->event != LUI_EVENT_NONE)
	{
		obj->state = new_state;
		#if defined(LUI_USE_BUTTONGRID)
		if (obj->obj_type == LUI_OBJ_BTNGRID)
		{
			lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
			btngrid->btn_properties[btngrid->active_btn_index] |= (new_state & LUI_BTNGRID_MASK_BTN_CHECKED);
		}
		#endif

		#if defined(LUI_USE_LIST)
		if (obj->obj_type == LUI_OBJ_LIST)
		{
			lui_list_t* list = (lui_list_t* )(obj->obj_main_data);
			if (list->is_dropdown)
			{
				lui_obj_t* nav_btn_text = lui_object_get_child(obj, 3);
				lui_button_set_label_text(nav_btn_text, list->items[list->selected_item_index]->text);
			}
		}
		#endif

		#if defined(LUI_USE_PANEL)
		if (obj->obj_type != LUI_OBJ_PANEL)
			_lui_object_set_need_refresh(obj);
		#endif

	}

	/* Special case for TextBox. When clicked on a textbox, the state becomes ENTERED
	To EXIT the state, the close or ok button from keyboard must be pressed */
	if (obj->obj_type == LUI_OBJ_TEXTBOX)
	{
		#if defined(LUI_USE_TEXTBOX)
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
		#endif
	}

	/* Special case for checkable buttons, switch, and checkbox: if event is LUI_EVENT_RELEASED, 
	then set event to LUI_EVENT_VALUE_CHANGED, and then set the value to `value` property */
	else if (obj->obj_type == LUI_OBJ_SWITCH ||
		obj->obj_type == LUI_OBJ_CHECKBOX ||
		(obj->obj_type == LUI_OBJ_BUTTON && ((lui_button_t*)(obj->obj_main_data))->is_checkable))
	{
		if (obj->event == LUI_EVENT_RELEASED || obj->event == LUI_EVENT_SELECTION_LOST)
		{
			obj->event = LUI_EVENT_VALUE_CHANGED;	// for checkable items, being pressed means being toggled, thus value changed
			obj->value = obj->value ? 0 : 1;	// toggle the value (1->0 or 0->1)
			obj->needs_refresh = 1;
			g_lui_needs_render = 1;
		}
	}

	/*
	 * Special case for slider: If knob is kept pressed and if input pos is not same as knob's current position,
	 * set new position to knob  and value to slider, also set VALUE_CHANGED event 
	 */
	else if (obj->obj_type == LUI_OBJ_SLIDER)
	{
		#if defined(LUI_USE_SLIDER)
		lui_slider_t* slider = (lui_slider_t* )(obj->obj_main_data);
		uint8_t is_hor = obj->common_style.width >= obj->common_style.height;
		uint8_t is_pos_changed = 0;
		if (is_hor)
			is_pos_changed = input->x != (obj->x + slider->knob_center_rel_d);
		else
			is_pos_changed = input->y != (obj->y + slider->knob_center_rel_d);

		if (obj->state == LUI_STATE_PRESSED && is_pos_changed)
		{
			uint16_t knob_w_by_2 = slider->knob_type == LUI_SLIDER_KNOB_TYPE_DEFAULT ? slider->style.knob_width / 2 : 0;
			
			if (is_hor)
			{
				uint16_t max_knob_center_actual_x = obj->x + obj->common_style.width - knob_w_by_2;
				uint16_t min_knob_center_actual_x = obj->x + knob_w_by_2;

				// cap to minimum/maximum allowed position to prevent the knob from going out of boundary
				if (input->x > max_knob_center_actual_x)
					slider->knob_center_rel_d = max_knob_center_actual_x - obj->x;
				else if (input->x < min_knob_center_actual_x)
					slider->knob_center_rel_d = min_knob_center_actual_x - obj->x;
				else
					slider->knob_center_rel_d = input->x - obj->x;
				
				obj->value = _lui_map_range(slider->knob_center_rel_d, obj->common_style.width - (slider->style.knob_width / 2), (slider->style.knob_width / 2), slider->range_max, slider->range_min);
			}
			else
			{
				uint16_t max_knob_center_actual_y = obj->y + obj->common_style.height - knob_w_by_2;
				uint16_t min_knob_center_actual_y = obj->y + knob_w_by_2;

				// cap to minimum/maximum allowed position to prevent the knob from going out of boundary
				if (input->y > max_knob_center_actual_y)
					slider->knob_center_rel_d = max_knob_center_actual_y - obj->y;
				else if (input->y < min_knob_center_actual_y)
					slider->knob_center_rel_d = min_knob_center_actual_y - obj->y;
				else
					slider->knob_center_rel_d = input->y - obj->y;
				
				/* Since the relationship between y-axis pos and value is invserse, we switch max and min for new range */
				obj->value = _lui_map_range(slider->knob_center_rel_d, obj->common_style.height - (slider->style.knob_width / 2), (slider->style.knob_width / 2), slider->range_min, slider->range_max);
			}
			obj->event = LUI_EVENT_VALUE_CHANGED;
			obj->needs_refresh = 1;
			g_lui_needs_render = 1;
		}
		#endif
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
		#if defined(LUI_USE_BUTTONGRID)
		static int16_t last_active_btn = -1;
		lui_btngrid_t* btngrid = (lui_btngrid_t* )(obj->obj_main_data);
		if (btngrid->active_btn_index != last_active_btn)
		{
			uint8_t tmp_new_state, tmp_old_state;
			if (btngrid->active_btn_index == -1)
			{
				tmp_new_state = LUI_STATE_IDLE;
				tmp_old_state = old_state;
			}
			else
			{
				tmp_new_state = new_state;
				tmp_old_state = LUI_STATE_IDLE;
			}
			obj->event = _lui_get_event_against_state(tmp_new_state, tmp_old_state);
			/* when input is touch input and not mouse pointer, `SELECTION_LOST` event should become `RELEASED`  */
			if ((input->x == -1 || input->y == -1) && obj->event == LUI_EVENT_SELECTION_LOST)
				obj->event = LUI_EVENT_RELEASED;
		}

		if (obj->event == LUI_EVENT_PRESSED)
		{
			uint8_t is_checkable = btngrid->btn_properties[btngrid->active_btn_index] & LUI_BTNGRID_MASK_BTN_CHECKABLE;
			if (is_checkable)
			{
				obj->event = LUI_EVENT_CHECK_CHANGED;	
				//uint8_t prop = btngrid->btn_properties[btngrid->active_btn_index];
				btngrid->btn_properties[btngrid->active_btn_index] ^= LUI_BTNGRID_MASK_BTN_CHECKED;	// toggle check state of the active button
				//prop = btngrid->btn_properties[btngrid->active_btn_index];
			}
		}	

		if (obj->event != LUI_EVENT_NONE)
		{
			obj->needs_refresh = 1;
			g_lui_needs_render = 1;
		}
		last_active_btn = btngrid->active_btn_index;
		#endif
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
	lui_dispdrv_t* initial_disp_drv =  (lui_dispdrv_t* )_lui_mem_alloc(sizeof(*initial_disp_drv));
	if (initial_disp_drv == NULL)
		return NULL;

	initial_disp_drv->draw_pixels_buff_cb = NULL;
	initial_disp_drv->disp_buff = NULL;
	initial_disp_drv->disp_buff_sz_px = 0;
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

lui_dispdrv_t* lui_dispdrv_create_and_register()
{
	lui_dispdrv_t* dd = lui_dispdrv_create();
	g_lui_main->disp_drv = dd;
	return dd;
}

void lui_dispdrv_set_resolution(lui_dispdrv_t* dispdrv, uint16_t hor_res, uint16_t vert_res)
{
	if (dispdrv == NULL)
		return;
	dispdrv->display_hor_res = hor_res;
	dispdrv->display_vert_res = vert_res;
}

void lui_dispdrv_set_draw_disp_buff_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_buff_cb)(uint16_t* disp_buff, lui_area_t* area))
{
	if (dispdrv == NULL)
		return;
	dispdrv->draw_pixels_buff_cb = draw_pixels_buff_cb;
}

int8_t lui_dispdrv_set_disp_buff(lui_dispdrv_t* dispdrv, uint16_t* disp_buff, uint32_t size_in_px)
{
	if (dispdrv == NULL)
		return -1;
	if (disp_buff == NULL)
		return -1;
	dispdrv->disp_buff = disp_buff;
	dispdrv->disp_buff_sz_px = size_in_px;
	return 0;
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
	lui_touch_input_dev_t* initial_touch_inputdev =  (lui_touch_input_dev_t* )_lui_mem_alloc(sizeof(*initial_touch_inputdev));
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

lui_touch_input_dev_t* lui_touch_inputdev_create_and_register()
{
	lui_touch_input_dev_t* tid = lui_touch_inputdev_create();
	g_lui_main->touch_input_dev = tid;
	return tid;
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

void lui_gfx_draw_string_advanced(const char* str, lui_area_t* area, uint16_t fore_color, uint16_t bg_color, const lui_bitmap_t* bg_bitmap, lui_bitmap_mono_pal_t* palette, lui_area_t* bitmap_crop, uint8_t is_bg, const lui_font_t* font)
{
	if (str == NULL || area == NULL)
		return;

	uint16_t x_temp = area->x;
	uint16_t y_temp = area->y;
    const _lui_glyph_t* glyph;
	if (area->w == 0 || area->h == 0)
	{
		uint16_t max_w = area->w == 0 ? g_lui_main->disp_drv->display_hor_res - area->x : area->w;
		uint16_t max_h = area->h == 0 ? g_lui_main->disp_drv->display_vert_res - area->y : area->h;
		uint16_t dim[2] = {0, 0};	// x, y

		lui_gfx_get_string_dimension(str, font, max_w, dim);
		dim[1] = dim[1] > max_h ? max_h : dim[1];

		area->w = area->w == 0 ? dim[0] : area->w;
		area->h = area->h == 0 ? dim[1] : area->h;
	}
	/* If the calling function didn't know crop area and set it to 0,
	   here we set them to objects's w and h. Don't worry if the actual 
	   bitmap is smaller than the set crop size, `lui_gfx_draw_bitmap()` 
	   takes care of it.*/
	if (bitmap_crop)
	{
		if (!bitmap_crop->w )	bitmap_crop->w = area->w;
		if (!bitmap_crop->h )	bitmap_crop->h = area->h;
	}
	if (is_bg)
	{
		if (bg_bitmap)
			lui_gfx_draw_bitmap(bg_bitmap, palette, area->x, area->y, bitmap_crop);
		else
			lui_gfx_draw_rect_fill(area->x, area->y, area->w, area->h, bg_color);
	}
	
	// Scan chars one by one from the string
	//char
	while (*str != '\0')
	{
		if (*str == '\n')
		{
			x_temp = area->x;					//go to first col
			y_temp += (font->bitmap->size_y);	//go to next row (row height = height of space)
		}
		else
		{
			uint8_t glyph_width = 0;
			uint8_t glyph_xadv = 0;
			uint8_t glyph_xoff = 0;

            glyph = _lui_gfx_get_glyph_from_char(*str, font);

			if (glyph == NULL)
			{
				glyph_width = font->bitmap->size_y / 2;
				glyph_xadv = glyph_width;
			}
			/* Width of space is not correct in older font maps, so we calc w based on h */
			else if (glyph->character == ' ' && glyph->x_adv == 0)
			{
				glyph_width = font->bitmap->size_y / 4;
				glyph_xadv = glyph_width;
			}
			else
			{
				glyph_width = glyph->width;
				glyph_xadv = _LUI_MAX(glyph->x_adv, glyph_width);	// becasue in some rare cases x_adv = 0
				glyph_xoff = glyph->x_off;
			}

			// check if not enough space available at the right side
			if (x_temp + glyph_xoff + glyph_width > area->x + area->w)
			{
				x_temp = area->x;	//go to first col
				y_temp += font->bitmap->size_y;	//go to next row
			}

			/* check if not enough space available at the bottom */
			if(y_temp + font->bitmap->size_y > area->y + area->h)
				return;

			_lui_gfx_render_char_glyph(x_temp, y_temp, fore_color, 0, 0, glyph, font);
			x_temp += glyph_xadv;		//next char position
		}

        str++;
	}
}

void lui_gfx_draw_string_simple(const char* str, uint16_t x, uint16_t y, uint16_t fore_color, const lui_font_t* font)
{	
	lui_area_t str_area = {
		.x = x,
		.y = y,
		.w = 0,
		.h = 0
	};
	lui_gfx_draw_string_advanced(str, &str_area, fore_color, 0, NULL, NULL, NULL, 0, font);
}

void lui_gfx_draw_char(char c, uint16_t x, uint16_t y, uint16_t fore_color, uint16_t bg_color, uint8_t is_bg, const lui_font_t* font)
{
	if (c == '\0')
		return;
	const _lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(c, font);
	_lui_gfx_render_char_glyph(x, y, fore_color, bg_color, is_bg, glyph, font);
}

/*
 * Brehensen's algorithm is used.
 * Draw line between ANY two points.
 * Not necessarily start points has to be less than end points.
 */
void lui_gfx_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color)
{
	lui_gfx_draw_line_clipped(x0, y0, x1, y1, NULL, line_width, color);
}

void lui_gfx_draw_line_clipped(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color)
{
	/*
	* Brehensen's algorithm is used.
	* Not necessarily start points has to be less than end points.
	*/
	if (x0 == x1)	//vertical line
	{
		lui_gfx_draw_rect_fill_clipped(x0, (y0 < y1 ? y0 : y1), (uint16_t)line_width, (uint16_t)abs(y1 - y0 + 1), clip_area, color);
	}
	else if (y0 == y1)		//horizontal line
	{
		lui_gfx_draw_rect_fill_clipped((x0 < x1 ? x0 : x1), y0, (uint16_t)abs(x1 - x0 + 1), (uint16_t)line_width, clip_area, color);
	}
	else
	{
		if (abs(y1 - y0) < abs(x1 - x0))
		{
			if (x0 > x1)
			{
				_LUI_SWAP(uint16_t, x0, x1);
				_LUI_SWAP(uint16_t, y0, y1);
			}
			_lui_gfx_plot_line_low(x0, y0, x1, y1, clip_area, line_width, color);
		}

		else
		{
			if (y0 > y1)
			{
				_LUI_SWAP(uint16_t, x0, x1);
				_LUI_SWAP(uint16_t, y0, y1);
			}
			_lui_gfx_plot_line_high(x0, y0, x1, y1, clip_area, line_width, color) ;
		}
	}
}

//TODO: draw rect clipped
/*
 * Draw a rectangle with a given color and line width
 */
void lui_gfx_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color)
{
	uint16_t x_new = x+w-1;
	uint16_t y_new = y+h-1;
	lui_gfx_draw_line(x, y, x_new, y, line_width, color);  // TL->TR
	lui_gfx_draw_line(x_new-line_width+1, y, x_new-line_width+1, y_new, line_width, color);  // TR->BR
	lui_gfx_draw_line(x, y_new-line_width+1, x_new, y_new-line_width+1, line_width, color);  // BL->BR
	lui_gfx_draw_line(x, y, x, y_new, line_width, color);  // TL->BL
}

/*
 * Fill a rectangular area with a color
 */
void lui_gfx_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	lui_gfx_draw_rect_fill_clipped(x, y, w, h, NULL, color);
}

void lui_gfx_draw_rect_fill_clipped(uint16_t x, uint16_t y, uint16_t w, uint16_t h, lui_area_t* clip_area, uint16_t color)
{
	uint16_t buff_index = 0;
	uint16_t buff_h = 0;
	uint16_t px_cnt = 0;
	uint16_t tmp_x = 0, tmp_y = 0;

	if (clip_area)
	{
		// Totally outside
		if (x > clip_area->x + clip_area->w - 1 || y > clip_area->y + clip_area->h + 1 ||
			x + w - 1 < clip_area->x || y + h - 1 < clip_area->y)
			return;
		// Clip left
		if (x < clip_area->x)
		{
			w = w - (clip_area->x - x);
			x = clip_area->x;
		}
		// Clip top
		if (y < clip_area->y)
		{
			h = h - (clip_area->y - y);
			y = clip_area->y;
		}
		// Clip right
		if (x + w > clip_area->x + clip_area->w)
			w = clip_area->x + clip_area->w - x;
		// Clip bottom
		if (y + h > clip_area->y + clip_area->h)
			h = clip_area->y + clip_area->h - y;
	}

	for (tmp_y = y; tmp_y < y + h; ++tmp_y)
	{
		for (tmp_x = x; tmp_x < x + w; ++tmp_x)
		{
			g_lui_main->disp_drv->disp_buff[buff_index++] = color;
		}
		++buff_h;
		px_cnt += w;
		if (px_cnt + w >= g_lui_main->disp_drv->disp_buff_sz_px)
		{
			lui_area_t area = {
				.x = x,
				.y = (tmp_y + 1) - buff_h,
				.w = w,
				.h = buff_h,
			};
			g_lui_main->disp_drv->draw_pixels_buff_cb(g_lui_main->disp_drv->disp_buff, &area);
			buff_h = 0;
			px_cnt = 0;
			buff_index = 0;
		}
	}
	if (px_cnt)
	{
		lui_area_t area = {
			.x = x,
			.y = tmp_y - buff_h,
			.w = w,
			.h = buff_h,
		};
		g_lui_main->disp_drv->draw_pixels_buff_cb(g_lui_main->disp_drv->disp_buff, &area);
	}
}

uint16_t lui_gfx_get_font_height(const lui_font_t* font)
{
	if (font == NULL)
		return 0;

	return font->bitmap->size_y;
}

/*
 * Get the width and height of a string (in pixels).
 * Width: by adding up the width of each glyph (representing a character)
 * Height: Height of any glyph (representing a character)
 */
void lui_gfx_get_string_dimension(const char* str, const lui_font_t* font, uint16_t max_w, uint16_t str_dim[2])
{
	str_dim[0] = 0;	// -> width
	str_dim[1] = 0;	// -> height

	if (str == NULL)
		return;

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
            const _lui_glyph_t* glyph = _lui_gfx_get_glyph_from_char(*str, font);
			uint8_t glyph_width = 0;
			uint8_t glyph_xadv = 0;
			uint8_t glyph_xoff = 0;

			if (glyph == NULL)
			{
				glyph_width = font->bitmap->size_y / 2;
				glyph_xadv = glyph_width;
			}
			/* Width of space is not correct in older font maps, so we calc w based on h */
			else if (glyph->character == ' ' && glyph->x_adv == 0)
			{
				glyph_width = font->bitmap->size_y / 4;
				glyph_xadv = glyph_width;
			}
			else
			{
				glyph_width = glyph->width;
				glyph_xadv = _LUI_MAX(glyph->x_adv, glyph_width);	// becasue in some rare cases x_adv = 0
				glyph_xoff = glyph->x_off;
			}

            // Add width of glyphs
            if (temp_w + glyph_xadv > max_w)
			{
				if (temp_w + glyph_xoff + glyph_width > max_w)
				{
					temp_h += font->bitmap->size_y;
					temp_w = 0;
					needs_wrap = 1;
				}
				else
				{
					temp_w += glyph_xoff + glyph_width;
				}
			}
            else
			{
				temp_w += glyph_xadv;
			}
            temp_w_highest = temp_w_highest < temp_w ? temp_w : temp_w_highest;
		}

        str++;
	}
	str_dim[0] = needs_wrap ? max_w : temp_w_highest;
	str_dim[1] = temp_h; 
}


uint16_t lui_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
	return LUI_RGB(red, green, blue);
}

const _lui_glyph_t* _lui_gfx_get_glyph_from_char(char c, const lui_font_t* font)
{
	const _lui_glyph_t* glyph = NULL;
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

	uint16_t temp_x = x + glyph->x_off;
	uint16_t temp_y = y;

	uint16_t width = 0;
	uint16_t index_offset = 0;
	lui_area_t disp_area = {
		.x = 0,
		.y = 0,
		.w = 1,
		.h = 1,
	};

	/* Width of space is not correct in older font maps, so we calc w based on h */
	if (glyph->character == ' ' && glyph->x_adv == 0)
		width = font->bitmap->size_y / 4;
	else
		width = glyph->width;
	index_offset = glyph->payload_index;//((height / 8) + (height % 8 ? 1 : 0)) * x_pos;

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
			disp_area.x = temp_x;
			disp_area.y = temp_y;
			/**
			 * Nasty hack. Since width of space is calculated from height, 
			 * we can't render space from bitmap buffer. Hence, we just skip
			 * rendering forecolor for space.
			 */
            if (bit && glyph->character != ' ')	
            {
				g_lui_main->disp_drv->draw_pixels_buff_cb(&fore_color, &disp_area);
            }
            else
            {
                if (is_bg)
                    g_lui_main->disp_drv->draw_pixels_buff_cb(&bg_color, &disp_area);
            }
            ++bit_counter;
            ++temp_y;
        }
        ++index_offset;
        ++temp_x;
        temp_y = y;
    }
}

void lui_gfx_draw_bitmap(const lui_bitmap_t* bitmap, lui_bitmap_mono_pal_t* palette, uint16_t x, uint16_t y, lui_area_t* crop_area)
{
	if (bitmap == NULL)
		return;
	if (bitmap->bpp != 1 && bitmap->bpp != 8 && bitmap->bpp != 16)
		return;
		
	uint16_t color = 0;
	uint16_t tmp_x = x;
	uint16_t tmp_y = y;
	uint8_t mask = 0x80;
    uint8_t bit_counter = 0;
	uint32_t byte_offset = 0;
	uint16_t width = bitmap->size_x;
	uint16_t height = bitmap->size_y;
	uint32_t stride = 0;

	/* NOTE: Cropping supports only 8bpp and 16bpp bitmaps. NOT for 1-bpp mono */
	if (crop_area && bitmap->bpp != 1)
	{
		/* Crop area start pos can't be higher than bitmap dimension itself */
		if (crop_area->x > bitmap->size_x || crop_area->y > bitmap->size_y)
			return;
		_LUI_BOUNDS(crop_area->w, 1, bitmap->size_x - crop_area->x);
		_LUI_BOUNDS(crop_area->h, 1, bitmap->size_y - crop_area->y);

		width = crop_area->w;
		height = crop_area->h;
		uint32_t px_offset = bitmap->size_x * crop_area->y + crop_area->x; // Initial pixel offsets for cropping
		byte_offset = px_offset * (bitmap->bpp / 8);	// Initial bytes offset
		uint32_t px_skip = bitmap->size_x - crop_area->w;	// pixels to skip for cropping, in a loop
		stride = px_skip * (bitmap->bpp / 8);	// Bytes to skip, in a loop
	}
	/* For 1-bpp, we must go to next byte when 1 column is finished. */
	else if (bitmap->bpp == 1)
	{
		stride = 1;	
	}

	lui_area_t disp_area = {
		.x = 0,
		.y = 0,
		.w = 1,
		.h = 1,
	};
	uint16_t buff_index = 0;
	uint16_t buff_h = 0;
	uint16_t px_cnt = 0;
	uint16_t mono_fcol = palette ? palette->fore_color : 0xFFFF;
	uint16_t mono_bcol = palette ? palette->back_color : 0;
	uint16_t mono_transparent = bitmap->bpp == 1 ? (palette ? !palette->is_backgrnd : 0) : 0; // code fart
	for (uint16_t h = 0; h < height; h++)
	{
		bit_counter = 0;
		for (uint16_t w = 0; w < width; w++)
		{
			
			if (bitmap->bpp == 1)
			{
				if (bit_counter >= 8)   
				{
					++byte_offset;
					bit_counter = 0;
				}
				uint8_t bit = mask & (bitmap->payload[byte_offset] << bit_counter);
				color = bit ? mono_fcol : mono_bcol;
				++bit_counter;
			}
			else if (bitmap->bpp == 8)
			{
				color = LUI_RGB(bitmap->payload[byte_offset], bitmap->payload[byte_offset], bitmap->payload[byte_offset]);
				byte_offset += 1;
			}
			else if (bitmap->bpp == 16)
			{
				color = (uint16_t)(bitmap->payload[byte_offset]) << 8 | (uint16_t)(bitmap->payload[byte_offset+1]);
				byte_offset += 2;
			}
			else if (bitmap->bpp == 32)
			{
				/* 32bpp not supported yet. Only 16-bit colors are supported now */
				// offset += 3;
			}
			
			/* If image is mono and it has no bg, we won't buffer it. We'll draw it px by px */
			if (mono_transparent)
			{	
				if (color == mono_fcol)
				{
					disp_area.x = tmp_x;
					disp_area.y = tmp_y;
					g_lui_main->disp_drv->draw_pixels_buff_cb(&color, &disp_area);
				}
			}
			else
			{
				g_lui_main->disp_drv->disp_buff[buff_index++] = color;
			}
            ++tmp_x;
		}
		byte_offset += stride;	// Skip bytes in case of cropping
        tmp_x = x;
		++tmp_y;
		
		/* Below section is only for 8-bpp, 16-bpp, and non-transparent 1-bpp */
		if (!mono_transparent)
		{
			++buff_h;
			px_cnt += width;
			if (px_cnt + width > g_lui_main->disp_drv->disp_buff_sz_px)
			{
				disp_area.x = x;
				disp_area.y = tmp_y - buff_h;
				disp_area.w = width;
				disp_area.h = buff_h;
				g_lui_main->disp_drv->draw_pixels_buff_cb(g_lui_main->disp_drv->disp_buff, &disp_area);
				buff_h = 0;
				px_cnt = 0;
				buff_index = 0;
			}
		}
	}

	if (px_cnt)
	{
		disp_area.x = x;
		disp_area.y = tmp_y - buff_h;
		disp_area.w = width;
		disp_area.h = buff_h;
		g_lui_main->disp_drv->draw_pixels_buff_cb(g_lui_main->disp_drv->disp_buff, &disp_area);
	}
}

/*
 * When dy < 0
 * It's called only by line_draw function. Not for user
 */
// TODO: Make thick line render more efficient
void _lui_gfx_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color)
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
	lui_area_t disp_area = 
	{
		.x = 0,
		.y = 0,
		.w = 1,
		.h = 1,
	};
	while (x <= x1)
	{
		disp_area.x = x;
		disp_area.y = y;

		for (int8_t i = -line_width / 2; i <= line_width / 2; i++)
		{
			disp_area.y = _LUI_MAX((int32_t)y + i, 0);
			if (clip_area)
			{
				if (disp_area.x >= clip_area->x + clip_area->w || disp_area.x < clip_area->x ||
					disp_area.y >= clip_area->y + clip_area->h || disp_area.y < clip_area->y )
					{
						continue;
					}
			}
			g_lui_main->disp_drv->draw_pixels_buff_cb(&color, &disp_area);
		}

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
void _lui_gfx_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, lui_area_t* clip_area, uint8_t line_width, uint16_t color)
{
	fflush(stderr);
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
	lui_area_t disp_area =
	{
		.x = 0,
		.y = 0,
		.w = 1,
		.h = 1,
	};
	while (y <= y1)
	{
		disp_area.x = x;
		disp_area.y = y;

		// Draw multiple pixels vertically for line width
		for (int8_t i = -line_width / 2; i <= line_width / 2; i++)
		{
			disp_area.x = _LUI_MAX((int32_t)x + i, 0);
			if (clip_area)
			{
				if (disp_area.x >= clip_area->x + clip_area->w || disp_area.x < clip_area->x ||
					disp_area.y >= clip_area->y + clip_area->h || disp_area.y < clip_area->y )
					{
						continue;
					}
			}
			g_lui_main->disp_drv->draw_pixels_buff_cb(&color, &disp_area);
		}

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

// const lui_font_t* _lui_get_font_from_active_scene()
// {
// 	if ( g_lui_main->active_scene == NULL)
// 		return NULL;
// 	lui_scene_t* act_scene = (lui_scene_t* )( g_lui_main->active_scene->obj_main_data);
// 	return (act_scene->font);
// }

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

uint8_t _lui_calc_clip_region_code(double x, double y, const lui_area_t* clip_win)
{
	uint8_t rcode = 0;
	if (x < clip_win->x)    rcode |= (1 << 1);  //LEFT
	else if (x > clip_win->x + clip_win->w - 1)    rcode |= (1 << 2);	//RIGHT

	if (y < clip_win->y)	rcode |= (1 << 4);  //TOP
	else if (y > clip_win->y + clip_win->h - 1)	rcode |= (1 << 3);  //BOTTOM

	return rcode;

}

/**
 * https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
 * Returns: 0-> unaccepted, 1-> accepted unclipped, 3-> accepted clipped
 */
uint8_t _lui_clip_line(double* point_0, double* point_1, const lui_area_t* clip_win)
{
	if (!point_0 || !point_1 || !clip_win)	return 0;

	// rcode bits
	const uint8_t INSIDE = (1<<0), LEFT = (1<<1), RIGHT = (1<<2), BOTTOM = (1<<3), TOP = (1<<4);
	double x0 = point_0[0], y0 = point_0[1];
	double x1 = point_1[0], y1 = point_1[1];

	double xmin = clip_win->x, xmax = clip_win->x + clip_win->w - 1;
	// ymin and ymax are swapped because y=0 of screen is at top
	double ymax = clip_win->y, ymin = clip_win->y + clip_win->h - 1;

	uint8_t rcode_0 = _lui_calc_clip_region_code(x0, y0, clip_win);
	uint8_t rcode_1 = _lui_calc_clip_region_code(x1, y1, clip_win);

	(void)INSIDE;	// suppress `unused ` warning
	uint8_t flag_accept = 0;
	while(1)
	{
		if (!(rcode_0 | rcode_1))
		{
			flag_accept = 1;
			break;
		}
		else if (rcode_0 & rcode_1)
		{
			break;
		}
		else
		{
			uint8_t rcode = rcode_0 ? rcode_0 : rcode_1; // at least one code is non-zero, pick that one
			double x, y;

			// Now find the intersection point;
			// use formulas:
			//   slope = (y1 - y0) / (x1 - x0)
			//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
			//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
			// No need to worry about divide-by-zero because, in each case, the
			// outcode bit being tested guarantees the denominator is non-zero

			if (rcode & TOP)  // point is above the clip window
			{
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			}
			else if (rcode & BOTTOM)  // point is below the clip window
			{
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			}
			else if (rcode & RIGHT)  // point is to the right of clip window
			{
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			}
			else if (rcode & LEFT)  // point is to the left of clip window
			{
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (rcode == rcode_0)
			{
				x0 = x;
				y0 = y;
				rcode_0 = _lui_calc_clip_region_code(x0, y0, clip_win);
			}
			else
			{
				x1 = x;
				y1 = y;
				rcode_1 = _lui_calc_clip_region_code(x1, y1, clip_win);
			}
		}
	}
	point_0[0] = x0;
	point_0[1] = y0;
	point_1[0] = x1;
	point_1[1] = y1;
	return flag_accept;
}

int8_t _lui_verify_obj(lui_obj_t* obj, uint8_t obj_type)
{
	if (obj == NULL)
		return -1;
	if (obj->obj_type != obj_type)
		return -1;
	return 0;
}

int8_t _lui_layout_set_properties(lui_obj_t* obj, uint8_t layout_type, uint8_t pad_x, uint8_t pad_y)
{
	if (layout_type == LUI_LAYOUT_VERTICAL || layout_type == LUI_LAYOUT_HORIZONTAL)
	{
		struct _lui_layout_s* layout;
		if (obj->obj_type == LUI_OBJ_SCENE)
			layout = &((lui_scene_t*)(obj->obj_main_data))->layout;
		else
			layout = &((lui_panel_t*)(obj->obj_main_data))->layout;

		layout->type = layout_type;
		layout->pad_x = pad_x;
		layout->pad_y = pad_y;
		return 0;
	}
	else
		return -1;
}

int8_t _lui_layout_calculate(lui_obj_t* obj)
{
	struct _lui_layout_s* layout;
	uint16_t x, y;
	lui_obj_t* child;

	if (obj->obj_type == LUI_OBJ_SCENE)
		layout = &((lui_scene_t*)(obj->obj_main_data))->layout;
	else
		layout = &((lui_panel_t*)(obj->obj_main_data))->layout;

	if (layout->type == LUI_LAYOUT_VERTICAL)
	{
		for (uint8_t i = 0; i < obj->children_count; ++i)
		{
			child = lui_object_get_child(obj, i);
			x = layout->pad_x;
			y = layout->dim + layout->pad_y;
			layout->dim = y + child->common_style.height;
			lui_object_set_position(child, x, y);
		}
	}
	else if (layout->type == LUI_LAYOUT_HORIZONTAL)
	{
		for (uint8_t i = 0; i < obj->children_count; ++i)
		{
			child = lui_object_get_child(obj, i);
			x = layout->dim + layout->pad_x;
			y = layout->pad_y;
			layout->dim = x + child->common_style.width;
			lui_object_set_position(child, x, y);
		}
	}
	else
	{
		// Not implemented
		return -69;
	}
	return 0;
}

/*
 * Check if display driver is 
 * registered by the stupid user  
 */
uint8_t _lui_disp_drv_check()
{
	// if no display driver is registered, return
	if ( g_lui_main->disp_drv == NULL)
		return 0;
	
	// If no buffer or no callback function (for drawing) is provided by user, return
	else if (g_lui_main->disp_drv->draw_pixels_buff_cb == NULL || g_lui_main->disp_drv->disp_buff == NULL)
		return 0;
	else
		return 1;
}

void _lui_mem_init(uint8_t mem_block[], uint32_t size)
{
	g_lui_mem_block.mem_block = mem_block;
	g_lui_mem_block.block_max_sz = size;
	g_lui_mem_block.mem_allocated = 0;
}

void* _lui_mem_alloc(uint16_t element_size)
{
	if (g_lui_mem_block.mem_allocated + element_size > g_lui_mem_block.block_max_sz)
		return NULL;
	uint8_t* nxt_addr = g_lui_mem_block.mem_block + g_lui_mem_block.mem_allocated;
	g_lui_mem_block.mem_allocated += element_size;
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
/* Create fonts and bitmap images using: https://github.com/abhra0897/LameUI_font_maker */
/* Default Font data. DON'T EDIT!*/
/* "ubuntu_regular_17" */
static const uint8_t default_ubuntu_regular_17_payload[4260] ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x20,0x00,0x04,0x3C,0x00,0x07,0xE0,0x00,0x3C,0x20,0x00,0x04,0x20,0x00,0x04,0x3C,0x00,0x07,0xE0,0x00,0x3C,0x20,
0x00,0x04,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x08,0x00,0x09,0x04,0x00,0x11,0x04,0x00,0x70,0x87,0x00,0x10,0x84,0x00,0x10,0x44,0x00,0x08,0x48,0x00,
0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x21,0x00,0x00,0x21,0x04,0x00,0x21,0x08,0x00,0x1E,0x30,0x00,0x00,0x40,0x00,0x01,0x80,0x00,0x02,
0x00,0x00,0x0C,0x78,0x00,0x10,0x84,0x00,0x20,0x84,0x00,0x00,0x84,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x1C,0x88,0x00,0x23,0x04,
0x00,0x20,0x84,0x00,0x21,0x44,0x00,0x22,0x24,0x00,0x1C,0x18,0x00,0x00,0x28,0x00,0x00,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x0C,0x0C,0x00,0x30,0x03,0x00,0x40,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x80,0x30,0x03,0x00,0x0C,
0x0C,0x00,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x04,0x80,0x00,0x07,0x00,0x00,0x3C,0x00,0x00,0x07,0x00,0x00,0x04,0x80,0x00,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x03,0xF8,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x07,0x00,0x00,0x38,0x00,0x00,0xC0,0x00,0x07,0x00,0x00,0x38,0x00,0x00,0x40,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x10,0x08,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x10,0x08,0x00,0x0F,0xF0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x08,0x00,0x00,0x10,0x00,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x0C,0x00,0x10,0x14,0x00,0x20,
0x24,0x00,0x20,0x44,0x00,0x20,0x84,0x00,0x11,0x04,0x00,0x0E,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x08,0x00,0x20,0x04,0x00,0x21,0x04,0x00,0x21,0x04,
0x00,0x21,0x04,0x00,0x22,0x84,0x00,0x12,0x88,0x00,0x0C,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0xA0,0x00,0x03,0x20,0x00,0x04,0x20,0x00,
0x08,0x20,0x00,0x10,0x20,0x00,0x3F,0xFC,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x3F,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,
0x04,0x00,0x21,0x04,0x00,0x20,0x88,0x00,0x20,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x0D,0x08,0x00,0x11,0x04,0x00,0x11,0x04,0x00,0x21,0x04,
0x00,0x21,0x04,0x00,0x20,0x88,0x00,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x1C,0x00,0x20,0xE0,0x00,0x23,0x00,0x00,
0x24,0x00,0x00,0x28,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x70,0x00,0x12,0x88,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,
0x04,0x00,0x12,0x88,0x00,0x0C,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x11,0x04,0x00,0x20,0x84,0x00,0x20,0x84,0x00,0x20,0x88,0x00,0x20,0x88,
0x00,0x10,0xB0,0x00,0x0F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x06,0x0F,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xA0,0x00,0x00,0xA0,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x01,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x08,
0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x01,0x10,0x00,0x00,0xA0,0x00,0x00,0xA0,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
0x20,0x00,0x00,0x20,0xCC,0x00,0x21,0x00,0x00,0x22,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x0C,0x0C,0x00,0x10,0x02,0x00,0x11,
0xE2,0x00,0x22,0x11,0x00,0x24,0x09,0x00,0x24,0x09,0x00,0x24,0x09,0x00,0x24,0x09,0x00,0x17,0xF1,0x00,0x10,0x08,0x00,0x0C,0x08,0x00,0x03,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x18,0x00,0x00,0xE0,0x00,0x03,0x20,0x00,0x0C,0x20,0x00,0x30,0x20,0x00,0x0C,0x20,0x00,0x03,0x20,0x00,0x00,0xE0,0x00,
0x00,0x18,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x13,0x04,0x00,0x0C,
0x88,0x00,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x08,0x10,0x00,0x10,0x08,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,
0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,
0x20,0x04,0x00,0x10,0x08,0x00,0x08,0x10,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x21,
0x04,0x00,0x21,0x04,0x00,0x20,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x21,0x00,0x00,0x20,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x08,0x10,0x00,0x10,0x08,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,
0x10,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x3F,
0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,
0x00,0x00,0x08,0x00,0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x01,0x00,0x00,0x02,0x80,0x00,0x04,0x40,0x00,0x08,0x40,0x00,0x08,0x20,0x00,
0x10,0x10,0x00,0x20,0x08,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,
0x04,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x3F,0x00,0x00,0x10,0x00,0x00,0x0C,0x00,0x00,0x03,0x00,0x00,0x00,0xE0,0x00,0x00,0x10,
0x00,0x00,0x60,0x00,0x03,0x80,0x00,0x0C,0x00,0x00,0x10,0x00,0x00,0x3E,0x00,0x00,0x01,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x10,0x00,0x00,
0x0C,0x00,0x00,0x02,0x00,0x00,0x01,0x80,0x00,0x00,0x60,0x00,0x00,0x10,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x08,0x10,0x00,0x10,
0x08,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x10,0x08,0x00,0x08,0x10,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x3F,0xFC,0x00,0x20,0x40,0x00,0x20,0x40,0x00,0x20,0x40,0x00,0x20,0x40,0x00,0x10,0x80,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,
0x08,0x10,0x00,0x10,0x08,0x00,0x20,0x04,0x00,0x20,0x04,0x00,0x20,0x06,0x00,0x20,0x05,0x00,0x20,0x04,0x80,0x10,0x08,0x80,0x08,0x10,0x80,0x07,0xE0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x00,0x20,0x80,0x00,0x20,0x80,0x00,0x20,0x80,0x00,0x20,0x80,0x00,0x20,0xC0,0x00,0x11,0x20,0x00,0x0E,0x18,0x00,0x00,0x04,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x08,0x00,0x12,0x04,0x00,0x21,0x04,0x00,0x21,0x04,0x00,0x20,0x84,0x00,0x10,0x48,0x00,0x00,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x3F,0xFC,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xF0,0x00,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x08,0x00,0x3F,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x1C,0x00,0x00,0x03,0x00,0x00,0x00,0xE0,0x00,0x00,0x18,0x00,0x00,0x04,0x00,0x00,0x18,0x00,0x00,0xE0,0x00,0x03,0x00,0x00,
0x1C,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x0F,0x00,0x00,0x00,0xF0,0x00,0x00,0x0C,0x00,0x00,0x30,0x00,0x01,0xC0,0x00,0x06,
0x00,0x00,0x18,0x00,0x00,0x07,0x00,0x00,0x01,0xC0,0x00,0x00,0x30,0x00,0x00,0x0C,0x00,0x00,0xF0,0x00,0x0F,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x20,0x04,0x00,0x10,0x08,0x00,0x08,0x30,0x00,0x06,0x40,0x00,0x01,0x80,0x00,0x01,0x80,0x00,0x06,0x40,0x00,0x08,0x30,0x00,0x10,0x08,0x00,0x20,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x0C,0x00,0x00,0x02,0x00,0x00,0x01,0x00,0x00,0x00,0xFC,0x00,0x01,0x00,0x00,0x02,0x00,0x00,0x0C,
0x00,0x00,0x10,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x0C,0x00,0x20,0x14,0x00,0x20,0x64,0x00,0x20,0x84,0x00,0x23,0x04,0x00,0x24,0x04,
0x00,0x28,0x04,0x00,0x30,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x80,0x40,0x00,0x80,0x40,0x00,0x80,0x40,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x40,0x00,0x00,0x38,0x00,0x00,0x07,0x00,0x00,0x00,0xC0,0x00,0x00,0x38,0x00,0x00,0x07,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x80,0x40,
0x00,0x80,0x40,0x00,0x80,0x7F,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x02,0x00,0x00,0x0C,0x00,0x00,0x10,0x00,0x00,0x20,0x00,0x00,0x10,0x00,
0x00,0x0C,0x00,0x00,0x02,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x78,0x00,0x04,0x84,0x00,0x04,0x84,0x00,0x04,0x84,0x00,0x04,0x84,0x00,0x04,0x84,0x00,0x03,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFC,0x00,0x02,0x04,
0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x02,0x08,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x02,0x08,0x00,
0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x02,0x08,0x00,0x04,0x04,0x00,0x04,
0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x02,0x04,0x00,0x7F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x02,0x48,0x00,0x04,0x44,0x00,0x04,0x44,
0x00,0x04,0x44,0x00,0x04,0x44,0x00,0x02,0x44,0x00,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x00,0x24,0x00,0x00,0x44,0x00,0x00,0x44,0x00,0x00,
0x44,0x00,0x00,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x02,0x08,0x80,0x04,0x04,0x80,0x04,0x04,0x80,0x04,0x04,0x80,0x04,0x04,0x80,0x04,
0x09,0x00,0x07,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFC,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x02,0x00,
0x00,0x01,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x67,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x67,0xFF,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFC,0x00,0x00,0x40,0x00,0x00,0xA0,0x00,0x01,0x20,0x00,0x01,0x10,0x00,0x02,0x08,0x00,0x04,0x08,0x00,0x00,0x04,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xF8,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,
0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x03,0xFC,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x01,0xFC,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x01,0xFC,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xF0,0x00,0x02,0x08,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x02,0x08,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x07,0xFF,0x80,0x04,0x08,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x02,0x08,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0xF0,0x00,0x02,0x08,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x08,0x00,0x07,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xFC,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x00,0x04,0x84,0x00,0x04,0x44,
0x00,0x04,0x44,0x00,0x04,0x24,0x00,0x04,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF8,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x00,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x07,0xFC,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x03,0x80,0x00,0x00,0x60,0x00,0x00,0x18,0x00,0x00,0x04,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x03,0x80,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x01,0xC0,0x00,0x00,0x30,0x00,0x00,0x0C,0x00,0x00,0x30,0x00,0x01,0xC0,0x00,0x06,0x00,0x00,0x01,0xC0,0x00,
0x00,0x30,0x00,0x00,0x0C,0x00,0x00,0x30,0x00,0x01,0xC0,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x02,0x08,0x00,0x01,0x10,0x00,0x00,
0xA0,0x00,0x00,0x40,0x00,0x00,0xA0,0x00,0x01,0x10,0x00,0x02,0x08,0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x80,0x03,0x00,0x80,0x00,0xC0,
0x80,0x00,0x31,0x00,0x00,0x0E,0x00,0x00,0x38,0x00,0x00,0xC0,0x00,0x03,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x0C,0x00,0x04,0x14,0x00,
0x04,0x64,0x00,0x04,0x84,0x00,0x05,0x04,0x00,0x06,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x3F,0x7F,0x00,0x40,0x00,0x80,0x40,
0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x80,0x40,0x00,0x80,0x3F,0x7F,0x00,0x00,0x80,0x00,0x00,0x80,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0xFF,0xC0,0x01,0xFF,0xC0,0x03,0xFF,0xC0,0x07,0xFF,0xC0,0x0F,0xFF,0xC0,0x1F,0xFF,0xC0,0x3F,0xF0,0x00,0x7F,
0xF0,0x00,0x3F,0xF0,0x00,0x1F,0xFF,0xC0,0x3F,0xFF,0xC0,0x3F,0xFF,0xC0,0x3F,0xFF,0xC0,0x03,0xFF,0xC0,0x00,0xFF,0xC0,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0xF0,0x00,0x07,0x1C,0x00,0x0C,0x06,0x00,0x18,0x03,0x00,0x10,0x01,0x00,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,0x20,0x00,0x80,
0x30,0x01,0x80,0x11,0x01,0x00,0x1B,0x03,0x00,0x0F,0x06,0x00,0x0F,0x1C,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x01,0xF0,0x00,0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x1F,0xFF,
0x00,0x3F,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0F,0xC0,0x00,0x1F,0xF0,0x00,0x3F,0xFC,0x00,0x7C,0xFE,0x00,0x78,0x7F,0x80,0x78,0x7F,0xC0,0x78,0x7F,0x80,0x7C,0xFE,0x00,0x3F,0xFC,0x00,0x1F,0xF0,0x00,0x0F,
0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x04,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,
0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x07,0xFC,0x00,
0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x38,0x00,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,
0x06,0x00,0x00,0x1C,0x00,0x00,0x38,0x00,0x00,0x70,0x00,0x00,0xE0,0x00,0x01,0x80,0x00,0x03,0x00,0x00,0x06,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x70,0x00,0x00,0xE8,0x00,0x00,0xA4,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,
0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x70,0x00,0x00,0x38,0x00,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,0x07,0x00,0x1F,0xFF,0x80,0x00,0x07,
0x00,0x00,0x0E,0x00,0x00,0x1C,0x00,0x00,0x38,0x00,0x00,0x70,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,
0x00,0xE0,0x00,0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0C,0xE6,0x00,0x0E,0x4E,0x00,0x0F,0x1E,0x00,0x0F,0x1E,0x00,0x0E,0x4E,0x00,0x0C,
0xE6,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,
0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF8,0x00,0x07,0xBC,0x00,0x0C,0x06,0x00,0x18,0x03,0x00,0x00,0x01,0x80,
0x00,0x01,0x80,0x00,0x01,0x80,0x3F,0xC0,0x80,0x00,0x01,0x80,0x00,0x01,0x80,0x00,0x01,0x80,0x18,0x03,0x00,0x0C,0x06,0x00,0x07,0xBC,0x00,0x03,0xF8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0C,0x00,0x00,0x1C,0x00,0x00,0x3C,0x00,0x00,0x7C,0x00,0x00,0xFC,0x00,0x03,0xFC,
0x00,0x07,0xFC,0x00,0x03,0xFC,0x00,0x00,0xFC,0x00,0x00,0x7C,0x00,0x00,0x3C,0x00,0x00,0x1C,0x00,0x00,0x0C,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x1F,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x04,0x04,0x00,0x07,0xFC,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x40,0x00,0x00,0xE0,0x00,0x01,0xF0,0x00,0x03,0xF8,0x00,0x07,0x5C,0x00,0x0E,0x4E,0x00,0x1C,0x47,0x00,0x18,0x43,0x00,0x00,0x40,0x00,0x00,0x40,0x00,
0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x04,0x04,0x00,0x0E,0x0E,0x00,0x07,0x1C,0x00,0x03,0xB8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x01,0xF0,0x00,0x03,0xB8,0x00,0x07,0x1C,0x00,0x0E,0x0E,
0x00,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x0C,0x00,0x03,0x18,0x00,0x01,0xB0,0x00,0x00,0xE0,0x00,0x7F,0xFF,0xC0,0x30,0xE1,0x80,0x19,0xB3,0x00,0x0F,0x1E,0x00,0x06,0x0C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0x80,0x1F,0xFF,
0x00,0x0F,0xFE,0x00,0x07,0xFC,0x00,0x03,0xF8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x00,0xE0,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x01,
0xF0,0x00,0x03,0xF8,0x00,0x07,0xFC,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x1F,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x18,0x43,0x00,
0x1C,0x47,0x00,0x0E,0x4E,0x00,0x07,0x5C,0x00,0x03,0xF8,0x00,0x01,0xF0,0x00,0x00,0xE0,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xFC,0x00,0x04,0x04,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x05,0xD4,0x00,0x05,0x54,0x00,0x04,0xC4,0x00,0x01,0xF8,0x00,0x06,0x64,0x00,0x01,0x54,0x00,0x05,0x34,
0x00,0x05,0x74,0x00,0x05,0xF4,0x00,0x05,0xF4,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xC0,0x00,0x01,0xC0,0x00,0x03,0x80,0x00,0x07,0x00,0x00,0x0E,0x00,0x00,0x1C,0x00,0x00,0x3F,0xFF,0x00,0x1C,0x00,0x00,0x0E,0x00,0x00,0x07,0x00,0x00,0x03,
0x80,0x00,0x01,0xC0,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x06,0x40,0x00,0x06,0xE0,
0x00,0x0C,0xC0,0x00,0x0D,0x98,0x00,0x09,0xB0,0x00,0x09,0x32,0x00,0x09,0x37,0x00,0x09,0x32,0x00,0x09,0xB0,0x00,0x0D,0x98,0x00,0x0C,0xC0,0x00,0x06,0xE0,0x00,
0x06,0x40,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x0F,0xF0,0x00,0x18,0x38,0x00,0x30,0x18,0x00,0x30,
0x0C,0x00,0x30,0x0C,0x00,0x30,0x0C,0x00,0x30,0x1C,0x00,0x38,0x18,0x00,0x1C,0x78,0x00,0x0F,0xFC,0x00,0x03,0xCE,0x00,0x00,0x07,0x00,0x00,0x03,0x80,0x00,0x01,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x07,0x80,0x00,0x1F,0x80,0x00,0x7F,0x80,0x01,0xFF,0x80,0x03,0xFF,0x80,
0x0F,0xFF,0x80,0x3E,0x05,0x80,0x1F,0xFF,0x80,0x07,0xFF,0x80,0x01,0xFF,0x80,0x00,0x7F,0x80,0x00,0x3F,0x80,0x00,0x0F,0x80,0x00,0x03,0x80,0x00,0x00,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x02,0x08,0x00,0x0F,0x1E,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x7E,0x0F,0xC0,0x7E,0x0F,0xC0,0x7E,0x0F,
0xC0,0x7E,0x0F,0xC0,0x7E,0x0F,0xC0,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0x1E,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x0F,0xFE,0x00,0x00,0x40,0x00,0x00,
0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0x00,0x04,0x04,
0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,0x04,0x04,0x00,
0x04,0x04,0x00,0x04,0x04,0x00,0x07,0xFC,0x00,0x07,0xFC,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x06,0x00,0x00,0x07,
0x00,0x00,0x07,0x80,0x00,0x07,0xC0,0x00,0x07,0xE0,0x00,0x07,0xF8,0x00,0x07,0xFC,0x00,0x07,0xF8,0x00,0x07,0xE0,0x00,0x07,0xC0,0x00,0x07,0x80,0x00,0x07,0x00,
0x00,0x06,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const lui_bitmap_t BITMAP_default_ubuntu_regular_17 = {.size_x=1420, .size_y=19, .payload=default_ubuntu_regular_17_payload};

static const _lui_glyph_t FONT_GLYPHS_default_ubuntu_regular_17[] = {
{ .character=32/* */, .width=3, .payload_index=0 }, 			{ .character=33/*!*/, .width=3, .payload_index=9 }, 
{ .character=34/*"*/, .width=6, .payload_index=18 }, 			{ .character=35/*#*/, .width=11, .payload_index=36 }, 
{ .character=36/*$*/, .width=10, .payload_index=69 }, 			{ .character=37/*%*/, .width=15, .payload_index=99 }, 
{ .character=38/*&*/, .width=12, .payload_index=144 }, 			{ .character=39/*'*/, .width=3, .payload_index=180 }, 
{ .character=40/*(*/, .width=6, .payload_index=189 }, 			{ .character=41/*)*/, .width=6, .payload_index=207 }, 
{ .character=42/***/, .width=9, .payload_index=225 }, 			{ .character=43/*+*/, .width=9, .payload_index=252 }, 
{ .character=44/*,*/, .width=4, .payload_index=279 }, 			{ .character=45/*-*/, .width=6, .payload_index=291 }, 
{ .character=46/*.*/, .width=3, .payload_index=309 }, 			{ .character=47/*/*/, .width=9, .payload_index=318 }, 
{ .character=48/*0*/, .width=10, .payload_index=345 }, 			{ .character=49/*1*/, .width=6, .payload_index=375 }, 
{ .character=50/*2*/, .width=9, .payload_index=393 }, 			{ .character=51/*3*/, .width=10, .payload_index=420 }, 
{ .character=52/*4*/, .width=10, .payload_index=450 }, 			{ .character=53/*5*/, .width=10, .payload_index=480 }, 
{ .character=54/*6*/, .width=10, .payload_index=510 }, 			{ .character=55/*7*/, .width=10, .payload_index=540 }, 
{ .character=56/*8*/, .width=10, .payload_index=570 }, 			{ .character=57/*9*/, .width=10, .payload_index=600 }, 
{ .character=58/*:*/, .width=3, .payload_index=630 }, 			{ .character=59/*;*/, .width=4, .payload_index=639 }, 
{ .character=60/*<*/, .width=9, .payload_index=651 }, 			{ .character=61/*=*/, .width=10, .payload_index=678 }, 
{ .character=62/*>*/, .width=10, .payload_index=708 }, 			{ .character=63/*?*/, .width=8, .payload_index=738 }, 
{ .character=64/*@*/, .width=15, .payload_index=762 }, 			{ .character=65/*A*/, .width=13, .payload_index=807 }, 
{ .character=66/*B*/, .width=10, .payload_index=846 }, 			{ .character=67/*C*/, .width=12, .payload_index=876 }, 
{ .character=68/*D*/, .width=11, .payload_index=912 }, 			{ .character=69/*E*/, .width=9, .payload_index=945 }, 
{ .character=70/*F*/, .width=8, .payload_index=972 }, 			{ .character=71/*G*/, .width=11, .payload_index=996 }, 
{ .character=72/*H*/, .width=10, .payload_index=1029 }, 			{ .character=73/*I*/, .width=3, .payload_index=1059 }, 
{ .character=74/*J*/, .width=9, .payload_index=1068 }, 			{ .character=75/*K*/, .width=11, .payload_index=1095 }, 
{ .character=76/*L*/, .width=9, .payload_index=1128 }, 			{ .character=77/*M*/, .width=15, .payload_index=1155 }, 
{ .character=78/*N*/, .width=10, .payload_index=1200 }, 			{ .character=79/*O*/, .width=13, .payload_index=1230 }, 
{ .character=80/*P*/, .width=9, .payload_index=1269 }, 			{ .character=81/*Q*/, .width=13, .payload_index=1296 }, 
{ .character=82/*R*/, .width=11, .payload_index=1335 }, 			{ .character=83/*S*/, .width=9, .payload_index=1368 }, 
{ .character=84/*T*/, .width=11, .payload_index=1395 }, 			{ .character=85/*U*/, .width=10, .payload_index=1428 }, 
{ .character=86/*V*/, .width=13, .payload_index=1458 }, 			{ .character=87/*W*/, .width=17, .payload_index=1497 }, 
{ .character=88/*X*/, .width=12, .payload_index=1548 }, 			{ .character=89/*Y*/, .width=13, .payload_index=1584 }, 
{ .character=90/*Z*/, .width=10, .payload_index=1623 }, 			{ .character=91/*[*/, .width=6, .payload_index=1653 }, 
{ .character=92/*\*/, .width=9, .payload_index=1671 }, 			{ .character=93/*]*/, .width=6, .payload_index=1698 }, 
{ .character=94/*^*/, .width=11, .payload_index=1716 }, 			{ .character=95/*_*/, .width=10, .payload_index=1749 }, 
{ .character=96/*`*/, .width=5, .payload_index=1779 }, 			{ .character=97/*a*/, .width=9, .payload_index=1794 }, 
{ .character=98/*b*/, .width=10, .payload_index=1821 }, 			{ .character=99/*c*/, .width=9, .payload_index=1851 }, 
{ .character=100/*d*/, .width=10, .payload_index=1878 }, 			{ .character=101/*e*/, .width=10, .payload_index=1908 }, 
{ .character=102/*f*/, .width=8, .payload_index=1938 }, 			{ .character=103/*g*/, .width=10, .payload_index=1962 }, 
{ .character=104/*h*/, .width=10, .payload_index=1992 }, 			{ .character=105/*i*/, .width=3, .payload_index=2022 }, 
{ .character=106/*j*/, .width=6, .payload_index=2031 }, 			{ .character=107/*k*/, .width=10, .payload_index=2049 }, 
{ .character=108/*l*/, .width=5, .payload_index=2079 }, 			{ .character=109/*m*/, .width=15, .payload_index=2094 }, 
{ .character=110/*n*/, .width=10, .payload_index=2139 }, 			{ .character=111/*o*/, .width=10, .payload_index=2169 }, 
{ .character=112/*p*/, .width=10, .payload_index=2199 }, 			{ .character=113/*q*/, .width=10, .payload_index=2229 }, 
{ .character=114/*r*/, .width=8, .payload_index=2259 }, 			{ .character=115/*s*/, .width=8, .payload_index=2283 }, 
{ .character=116/*t*/, .width=7, .payload_index=2307 }, 			{ .character=117/*u*/, .width=10, .payload_index=2328 }, 
{ .character=118/*v*/, .width=11, .payload_index=2358 }, 			{ .character=119/*w*/, .width=15, .payload_index=2391 }, 
{ .character=120/*x*/, .width=11, .payload_index=2436 }, 			{ .character=121/*y*/, .width=11, .payload_index=2469 }, 
{ .character=122/*z*/, .width=8, .payload_index=2502 }, 			{ .character=123/*{*/, .width=7, .payload_index=2526 }, 
{ .character=124/*|*/, .width=3, .payload_index=2547 }, 			{ .character=125/*}*/, .width=7, .payload_index=2556 }, 
{ .character=126/*~*/, .width=10, .payload_index=2577 }, 			
{ .character=1/*[01]home.png*/, .width=19, .payload_index=2607 }, 
{ .character=2/*[02]reload.png*/, .width=19, .payload_index=2664 }, 			{ .character=3/*[03]caret-back.png*/, .width=19, .payload_index=2721 }, 
{ .character=4/*[04]location.png*/, .width=19, .payload_index=2778 }, 			{ .character=5/*[05]battery-full.png*/, .width=19, .payload_index=2835 }, 
{ .character=6/*[06]checkmark.png*/, .width=19, .payload_index=2892 }, 			{ .character=7/*[07]return-down-back.png*/, .width=19, .payload_index=2949 }, 
{ .character=8/*[08]arrow-down.png*/, .width=19, .payload_index=3006 }, 			{ .character=9/*[09]backspace.png*/, .width=19, .payload_index=3063 }, 
{ .character=10/*[0A]pause.png*/, .width=19, .payload_index=3120 }, 			{ .character=11/*[0B]remove.png*/, .width=19, .payload_index=3177 }, 
{ .character=12/*[0C]power.png*/, .width=19, .payload_index=3234 }, 			{ .character=13/*[0D]caret-up.png*/, .width=19, .payload_index=3291 }, 
{ .character=14/*[0E]volume-medium.png*/, .width=19, .payload_index=3348 }, 			{ .character=15/*[0F]arrow-back.png*/, .width=19, .payload_index=3405 }, 
{ .character=16/*[10]close.png*/, .width=19, .payload_index=3462 }, 			{ .character=17/*[11]bluetooth.png*/, .width=19, .payload_index=3519 }, 
{ .character=18/*[12]caret-forward.png*/, .width=19, .payload_index=3576 }, 			{ .character=19/*[13]volume-off.png*/, .width=19, .payload_index=3633 }, 
{ .character=20/*[14]arrow-forward.png*/, .width=19, .payload_index=3690 }, 			{ .character=21/*[15]battery-charging.png*/, .width=19, .payload_index=3747 }, 
{ .character=22/*[16]arrow-up.png*/, .width=19, .payload_index=3804 }, 			{ .character=23/*[17]wifi.png*/, .width=19, .payload_index=3861 }, 
{ .character=24/*[18]search.png*/, .width=19, .payload_index=3918 }, 			{ .character=25/*[19]warning.png*/, .width=19, .payload_index=3975 }, 
{ .character=26/*[1A]settings.png*/, .width=19, .payload_index=4032 }, 			{ .character=27/*[1B]add.png*/, .width=19, .payload_index=4089 }, 
{ .character=28/*[1C]battery-dead.png*/, .width=19, .payload_index=4146 }, 			{ .character=29/*[1D]caret-down.png*/, .width=19, .payload_index=4203 },};

const lui_font_t LUI_DEFAULT_FONT = { .bitmap = &BITMAP_default_ubuntu_regular_17, .glyph_count = 124, .glyphs = FONT_GLYPHS_default_ubuntu_regular_17 };


/*-------------------------------------------------------------------------------
 * 							END
 *-------------------------------------------------------------------------------
 */
