/*
 * lame_ui.c
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 16-May-2020
 *      Author: rik
 */

#include "lame_ui.h"

//Global variables
uint8_t lui_scene_total = 0;
tLuiDispDrv *lui_disp_drv;
tLuiScene *scenes[MAX_SCENES];



/*-------------------------------------------------------------------------------
 * 				LUI_LABEL related functions
 *-------------------------------------------------------------------------------
 */


/*
 * Render a label. This label may or maynot be a part of any scene.
 * The entire scene is not updated.
 */
void lui_label_draw (tLuiLabel *lui_label)
{
	uint16_t x_temp = lui_label->x;
	uint16_t y_temp = lui_label->y;

	const tImage *img = {0};
	uint16_t width = 0, height = 0;

	tFont *temp_font = lui_label->font;
	// If label has no font set for it, check the global set font in scene
	// If no scene present, i.e., parent_scene_index == -1, return
	// If scene is there but scene has no font, return
	// So, if label has no parent scene, it must have its own font to be rendered
	if (temp_font == NULL)
	{
		// If no parent scene (i.e. parent_scene_index = -1), return
		if (lui_label->parent_index == -1)
			return;
		// If the parent scene also has no font set, return.
		if (scenes[lui_label->parent_index]->font == NULL)
			return;

		// Otherwise use the scene's font (global font)
		temp_font = scenes[lui_label->parent_index]->font;
	}

	// Draw the label background color
	lui_draw_rect_fill(lui_label->x, lui_label->y, lui_label->width, lui_label->height, lui_label->bg_color);

	// Scan chars one by one from the string
	//char
	for (uint16_t char_cnt = 0; *(lui_label->text+char_cnt) != '\0'; char_cnt++)
	{
		if (*(lui_label->text+char_cnt) == '\n')
		{
			x_temp = lui_label->x;					//go to first col
			y_temp += (temp_font->chars[0].image->height);	//go to next row (row height = height of space)
		}

		else if (*(lui_label->text+char_cnt) == '\t')
		{
			x_temp += 4 * (temp_font->chars[0].image->height);	//Skip 4 spaces (width = width of space)
		}
		else
		{
			// Find the glyph for the char from the font
			for (uint8_t i = 0; i < temp_font->length; i++)
			{
				if (temp_font->chars[i].code == *(lui_label->text+char_cnt))
				{
					img = temp_font->chars[i].image;
					break;
				}
			}
			width = img->width;
			height = img->height;


			// check if not enough space available at the right side
			if (x_temp + width > lui_label->x + lui_label->width)
			{
				x_temp = lui_label->x;					//go to first col
				y_temp += height;	//go to next row

				// check if not enough space available at the bottom
				if(y_temp + height > lui_label->y + lui_label->height - 1)
					return;
			}

			lui_draw_char(x_temp, y_temp, lui_label->color, img);

			x_temp += width;		//next char position
		}
	}
}


/*
 * Initialize a label with default values
 */
tLuiLabel lui_label_create()
{
	tLuiLabel initial_label;

	initial_label.x = 0;
	initial_label.y = 0;
	initial_label.width = 50;
	initial_label.height = 20;
	initial_label.text = "";
	initial_label.font = NULL;
	initial_label.color = DEFAULT_TEXT_FORECOLOR;
	initial_label.bg_color = DEFAULT_TEXT_BGCOLOR;

	initial_label.index = -1;
	initial_label.parent_index = -1;

	return initial_label;
}


/*
 * Add a label to a scene.
 * One label can be added only to one scene
 * To add it to other scene, remove it first
 */

void lui_label_add_to_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene)
{
	// If the scene index is -1, return
	if (lui_scene->index == -1)
		return;
    //add the ui element with a new index to scene only if the parent index is -1 (i.e., no parent)
    if (lui_lbl->parent_index == -1)
    {
		lui_lbl->index = lui_scene->label_total;
		lui_lbl->parent_index = lui_scene->index;
		lui_scene->lui_label[lui_scene->label_total] = lui_lbl;
		lui_scene->label_total++;
    }
}



/*
 * Remove an existing label from a scene
 */
void lui_label_remove_from_scene(tLuiLabel *lui_lbl, tLuiScene *lui_scene)
{
    // If scene index is -1, return
	if (lui_scene->index == -1)
    	return;
	//Remove ui element only if it's parent index matches with scene's index
    if (lui_lbl->parent_index == lui_scene->index)
    {
        for (int i = lui_lbl->index; i < lui_scene->label_total; i++)
        {
        	lui_scene->lui_label[i]->index -= 1;
        }
        lui_scene->label_total--;
        lui_lbl->index = -1;
        lui_lbl->parent_index = -1;
    }
}


void lui_label_set_position(uint16_t x, uint16_t y, tLuiLabel *lui_lbl)
{
	if (lui_lbl->parent_index == -1)
		return;
	if (lui_lbl->x == x && lui_lbl->y == y)
		return;
	lui_lbl->x = x;
	lui_lbl->y = y;
	scenes[lui_lbl->parent_index]->needs_refresh = 1;
}


void lui_label_set_area(uint16_t width, uint16_t height, tLuiLabel *lui_lbl)
{
	if (lui_lbl->parent_index == -1)
		return;
	if (lui_lbl->width == width && lui_lbl->height == height)
		return;

	if (lui_lbl->width < width && lui_lbl->height < height)
		lui_lbl->needs_refresh = 1;
	else
		scenes[lui_lbl->parent_index]->needs_refresh = 1;
	
	lui_lbl->width = width;
	lui_lbl->height = height;
}


void lui_label_set_font(const tFont *font, tLuiLabel *lui_lbl)
{
	if (lui_lbl->parent_index == -1)
		return;
	lui_lbl->font = font;
	scenes[lui_lbl->parent_index]->needs_refresh = 1;
}


void lui_label_set_text(const char *text, tLuiLabel *lui_lbl)
{
	if (lui_lbl->parent_index == -1)
		return;
	lui_lbl->text = text;
	lui_lbl->needs_refresh = 1;
}

void lui_label_set_colors(uint16_t text_color, uint16_t bg_color, tLuiLabel *lui_lbl)
{
	if (lui_lbl->parent_index == -1)
		return;
	if (lui_lbl->color == text_color && lui_lbl->bg_color == bg_color)
		return;
	lui_lbl->needs_refresh = 1;
	lui_lbl->color = text_color;
	lui_lbl->bg_color = bg_color;
}



/*-------------------------------------------------------------------------------
 * 				LUI_LINE_CHART related functions
 *-------------------------------------------------------------------------------
 */

/*
* Draw a line chart
*/
void lui_linechart_draw(tLuiLineChart *p_lui_line_chart)
{
	uint16_t temp_x = p_lui_line_chart->x;
	uint16_t temp_y = p_lui_line_chart->y;
	uint16_t width = p_lui_line_chart->width;
	uint16_t height = p_lui_line_chart->height;
	uint16_t bg_color = p_lui_line_chart->bg_color;
	uint16_t line_color = p_lui_line_chart->color;
	uint16_t data_points = p_lui_line_chart->data.points;

	double mapped_data[lui_disp_drv->display_hor_res * 2];

	double x_data_min_new = temp_x;
	double x_data_max_new = temp_x + width;
	//[0][0] element of 2D array is x_min
	// address of [0][0] = base address
	double x_data_min_old = *(p_lui_line_chart->data.source);
	//[n][0] element of 2D array is x_max
	//address of [n][0] = base address + (n*2) - 2
	double x_data_max_old = *(p_lui_line_chart->data.source + (data_points*2) - 2);


	double y_data_min_new = temp_y + height;
	double y_data_max_new = temp_y;
	double y_data_min_old;
	double y_data_max_old;

	// If data auto-scale is enabled, find out the Y max and min value
	if (p_lui_line_chart->data.auto_scale == 1)
	{
		// Initially, Max and Min both are set to the first Y value of the source array
		double y_max = *(p_lui_line_chart->data.source + 1);
		double y_min = *(p_lui_line_chart->data.source + 1);

		// Now compare max and min with y values from source array to find the maximum and minimum
		for (uint16_t i = 1; i < data_points; i++)
		{
			double y_val = *(p_lui_line_chart->data.source + (i*2) + 1);
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
		y_data_min_old = p_lui_line_chart->data.y_min_value;
		y_data_max_old = p_lui_line_chart->data.y_max_value;
	}

	// Set font for using in chart
	tFont *temp_font = p_lui_line_chart->font;
	// If object has no font set for it, check the global set font in scene
	if (temp_font == NULL)
	{
		// Find object's parent scene's index
		uint8_t parent_scene_index = p_lui_line_chart->parent_index;

		// If the parent scene also has no font set, return.
		if (scenes[parent_scene_index]->font == NULL)
			return;

		// Otherwise use the scene's font (global font)
		temp_font = scenes[parent_scene_index]->font;
	}


	// Draw the chart background
	lui_draw_rect_fill(temp_x, temp_y, width, height, bg_color);

	// Draw the scale numbers

	// Draw the chart grid if needed
	if (p_lui_line_chart->grid.is_grid)
	{
		uint16_t hor_grid_spacing = height / (p_lui_line_chart->grid.hor_count + 1);
		uint16_t vert_grid_spacing = width / (p_lui_line_chart->grid.vert_count + 1);

		// Draw the vertical grids from left to right
		for (int i = 1; i <= p_lui_line_chart->grid.vert_count; i++)
		{
			lui_draw_line(temp_x + (i * vert_grid_spacing), temp_y, temp_x + (i * vert_grid_spacing), temp_y + height, 1, p_lui_line_chart->grid.color);
		}

		// Draw the horizontal grids from bottom to top
		uint16_t y_bottom = temp_y + height;
		for (int i = 1; i <= p_lui_line_chart->grid.hor_count; i++)
		{
			lui_draw_line(temp_x, y_bottom - (i * hor_grid_spacing), temp_x + width, y_bottom - (i * hor_grid_spacing), 1, p_lui_line_chart->grid.color);
		}
	}


	// Map all the point values to pixel co-ordinate values
	for (int i = 0; i < data_points; i++)
	{
		double x_data_old = *(p_lui_line_chart->data.source + (i*2));
		double y_data_old = *(p_lui_line_chart->data.source + (i*2) + 1);
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
			lui_disp_drv->draw_pixel_cb(current_x, current_y, line_color);
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
	if (p_lui_line_chart->border.is_border)
	{
		uint16_t border_color = p_lui_line_chart->border.color;
		lui_draw_rect(temp_x, temp_y, width, height, 1, border_color);
	}
}


/*
 * Initialize a line chart with default values
 */
tLuiLineChart lui_linechart_create()
{
	tLuiLineChart initial_line_chart;

	double tmp_data[] = {0};
	initial_line_chart.data.source = tmp_data;
	initial_line_chart.data.y_max_value = 0;
	initial_line_chart.data.y_min_value = lui_disp_drv->display_vert_res;
	initial_line_chart.data.points = 0;
	initial_line_chart.data.auto_scale = 1;

	initial_line_chart.x = 0;
	initial_line_chart.y = 0;
	initial_line_chart.width = 100;
	initial_line_chart.height = 100;

	initial_line_chart.color = DEFAULT_LINE_CHART_FORECOLOR;
	
	initial_line_chart.border.is_border = 1;
	initial_line_chart.border.color = DEFAULT_LINE_CHART_FORECOLOR;

	initial_line_chart.grid.color = DEFAULT_LINE_CHART_FORECOLOR;
	initial_line_chart.grid.hor_count = 5;
	initial_line_chart.grid.vert_count = 5;
	initial_line_chart.grid.is_grid = 1;

	initial_line_chart.bg_color = DEFAULT_LINE_CHART_BGCOLOR;

	initial_line_chart.font = NULL;

	initial_line_chart.index = -1;
	initial_line_chart.parent_index = -1;

	return initial_line_chart;
}


/*
 * Add a line chart to a scene.
 * A line chart can be added only to one scene
 * To add it to other scene, remove it first
 */
void lui_linechart_add_to_scene(tLuiLineChart *p_lui_line_chart, tLuiScene *p_lui_scene)
{
	// If scene's index is -1, return
	if (p_lui_scene->index == -1)
		return;
	//add the ui element with a new index to scene only if the parent index is -1 (i.e., no parent)
    if (p_lui_line_chart->parent_index == -1)
    {
    	p_lui_line_chart->index = p_lui_scene->line_chart_total;
    	p_lui_line_chart->parent_index = p_lui_scene->index;
    	p_lui_scene->lui_line_chart[p_lui_scene->line_chart_total] = p_lui_line_chart;
    	p_lui_scene->line_chart_total++;
    }
}


/*
 * Remove an existing line chart from a scene
 */
void lui_linechart_remove_from_scene(tLuiLineChart *p_lui_line_chart, tLuiScene *p_lui_scene)
{
	// If object's or scene's index is -1, return
	if (p_lui_line_chart->index == -1 || p_lui_scene->index == -1)
		return;
	//Remove ui element only if it's parent index matches with scene's index
    if (p_lui_line_chart->parent_index == p_lui_scene->index)
    {
        for (int i = p_lui_line_chart->index; i < p_lui_scene->line_chart_total; i++)
        {
        	p_lui_scene->lui_line_chart[i]->index -= 1;
        }
        p_lui_scene->line_chart_total--;
        p_lui_line_chart->parent_index = -1;
        p_lui_line_chart->index = -1;
    }
}

void lui_linechart_set_position(uint16_t x, uint16_t y, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->x == x && lui_chart->y == y)
		return;
	scenes[lui_chart->parent_index]->needs_refresh = 1;
	lui_chart->x = x;
	lui_chart->y = y;
}

void lui_linechart_set_area(uint16_t width, uint16_t height, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->width == width && lui_chart->height == height)
		return;
	if (lui_chart->width < width && lui_chart->height < height)
		lui_chart->needs_refresh = 1;
	else
		scenes[lui_chart->parent_index]->needs_refresh = 1;	
	lui_chart->width = width;
	lui_chart->height = height;
}

void lui_linechart_set_border(uint16_t color, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->border.color == color)
		return;	
	lui_chart->needs_refresh = 1;
	lui_chart->border.color = color;
}

void lui_linechart_set_border_visible(uint8_t state, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->border.is_border == state)
		return;	
	lui_chart->needs_refresh = 1;
	lui_chart->border.is_border = state;
}

void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->grid.color == color && lui_chart->grid.hor_count == hor_lines && lui_chart->grid.vert_count == vert_lines)
		return;	
	lui_chart->needs_refresh = 1;
	lui_chart->grid.color = color;
	lui_chart->grid.hor_count = hor_lines;
	lui_chart->grid.vert_count = vert_lines;
}

void lui_linechart_set_grid_visible(uint8_t state, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->grid.is_grid == state)
		return;	
	lui_chart->needs_refresh = 1;
	lui_chart->grid.is_grid = state;
}

void lui_linechart_set_colors(uint16_t line_color, uint16_t bg_color, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->color == line_color && lui_chart->bg_color == bg_color)
		return;	
	lui_chart->needs_refresh = 1;
	lui_chart->color = line_color;
	lui_chart->bg_color = bg_color;
}

void lui_linechart_set_data_range(uint16_t y_min, uint16_t y_max, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->data.y_max_value == y_max && lui_chart->data.y_min_value == y_min)
		return;
	lui_chart->needs_refresh = 1;
	lui_chart->data.y_max_value = y_max;
	lui_chart->data.y_min_value = y_min;
}

void lui_linechart_set_data_source(double *source, uint16_t points, tLuiLineChart *lui_chart)
{
	if (lui_chart->parent_index == -1)
		return;
	if (lui_chart->data.points == points)
		return;
	lui_chart->needs_refresh = 1;
	lui_chart->data.source = source;
	lui_chart->data.points = points;
}


/*-------------------------------------------------------------------------------
 * 				LUI_BUTTON related functions
 *-------------------------------------------------------------------------------
 */
/*
 * Draw a button
 */
void lui_button_draw(tLuiButton *lui_btn)
{

	uint16_t temp_x = lui_btn->x;
	uint16_t temp_y = lui_btn->y;
	uint16_t btn_height = lui_btn->height;
	uint16_t btn_width = lui_btn->width;


	uint8_t str_width_height[2];

	// Draw the button's body color
	lui_draw_rect_fill(temp_x, temp_y, btn_width, btn_height, lui_btn->color);

	// Draw the button label (text)
	// Text will be in the miidle of the button.
	// So, at first we need to calculate its position

	get_string_dimension(lui_btn->label.text, lui_btn->label.font, str_width_height);

	str_width_height[0] > btn_width ? btn_width : str_width_height[0];
	str_width_height[1] > btn_height ? btn_height : str_width_height[1];

	temp_x = temp_x + (btn_width - str_width_height[0]) / 2;
	temp_y = temp_y + (btn_height - str_width_height[1]) / 2;

	tLuiLabel btn_label = lui_label_create();
	btn_label.text = lui_btn->label.text;
	btn_label.color = lui_btn->label.color;
	btn_label.x = temp_x;
	btn_label.y = temp_y;
	btn_label.width = str_width_height[0];
	btn_label.height = str_width_height[1];
	btn_label.font = lui_btn->label.font;
	lui_label_draw(&btn_label);
}


/*
 * Create a button with default variables
 */
tLuiButton lui_button_create()
{
	tLuiButton initial_button;

	initial_button.x = 0;
	initial_button.y = 0;
	initial_button.width = 0;
	initial_button.height = 0;
	initial_button.color = 0xFFFF;
	initial_button.pressed_color = 0xCE59; //grey
	initial_button.selection_color = 0xFF40; //Yellow
	initial_button.is_pressed = 0;
	initial_button.is_selected = 0;

	initial_button.label.text = "";
	initial_button.label.color = 0x0000; //black
	initial_button.label.font = NULL;

	initial_button.index = -1;
	initial_button.parent_index = -1;

	return  initial_button;
}


/*
 * Add a button to a scene
 * A button can only be added to one scene
 * To add it to other scene, remove it first
 */
void lui_button_add_to_scene(tLuiButton *lui_btn, tLuiScene *lui_scene)
{
	// If the scene index is -1, return
	if (lui_scene->index == -1)
		return;
    //add the ui element with a new index to scene only if the parent index is -1 (i.e., no parent)
    if (lui_btn->parent_index == -1)
    {
		lui_btn->index = lui_scene->button_total;
		lui_btn->parent_index = lui_scene->index;
		lui_scene->lui_button[lui_scene->button_total] = lui_btn;
		lui_scene->button_total++;
    }
}


/*
 * Remove an existing button from a scene
 */
void lui_button_remove_from_scene(tLuiButton *lui_btn, tLuiScene *lui_scene)
{
    // If scene index is -1, return
	if (lui_scene->index == -1)
    	return;
	//Remove ui element only if it's parent index matches with scene's index
    if (lui_btn->parent_index == lui_scene->index)
    {
        for (int i = lui_btn->index; i < lui_scene->button_total; i++)
        {
        	lui_scene->lui_button[i]->index -= 1;
        }
        lui_scene->button_total--;
        lui_btn->index = -1;
        lui_btn->parent_index = -1;
    }
}




/*-------------------------------------------------------------------------------
 * 				LUI_SCENE related functions
 *-------------------------------------------------------------------------------
 */

/*
 * Create a empty scene with default values and return the scene variable
 */
tLuiScene lui_scene_create()
{
	tLuiScene initial_scene;

	initial_scene.label_total = 0;
	initial_scene.line_chart_total = 0;
	initial_scene.button_total = 0;

	initial_scene.font = NULL;
	initial_scene.background.color = 0x00;
	initial_scene.background.image.dataSize = 0;

	initial_scene.index = -1;

	return initial_scene;
}


/*
 * add a scene in the global scene array
 */
void lui_scene_add(tLuiScene *lui_scene)
{
	//If scene index is -1, i.e., scene is not added already, then add it in the global "scenes" array
	if (lui_scene->index == -1)
	{
		lui_scene->index = lui_scene_total;
		scenes[lui_scene->index] = lui_scene;
		lui_scene_total++;
	}
}


/*
 * Update the entire scene that contains some UI elements
 */
void lui_scene_render(tLuiScene *lui_scene)
{
	// If scene is created but not not added, i.e., index is -1, return.
	if (lui_scene->index == -1)
		return;

	// If no callback function (for drawing) is provided by user, return
	if (lui_disp_drv->draw_pixel_cb == NULL || lui_disp_drv->draw_pixels_area_cb == NULL)
		return;

    uint8_t temp_label_total = lui_scene->label_total;
    uint8_t temp_button_total = lui_scene->button_total;
    uint8_t temp_line_chart_total = lui_scene->line_chart_total;

	uint16_t background_width = lui_disp_drv->display_hor_res;
	uint16_t background_height = lui_disp_drv->display_vert_res;
	uint16_t background_color = lui_scene->background.color;


	// Reading input only if user provided input reading callback function
	if (lui_disp_drv->read_input_cb != NULL)
	{
		tLuiInputDev input_data;
		lui_disp_drv->read_input_cb(&input_data);
		//printf("\nX position: %d, Y position: %d, Is pressed: %d\n", input_data.position.x, input_data.position.y, input_data.is_pressed);
	}


	// Render background first
	lui_disp_drv->draw_pixels_area_cb(0, 0, background_width, background_height, background_color);


	// Render all the buttons
    for (int i = 0; i < temp_button_total; i++)
    {
    	lui_button_draw(lui_scene->lui_button[i]);
    }

    // Render all the labels
    for (int i = 0; i < temp_label_total; i++)
    {
    	lui_label_draw(lui_scene->lui_label[i]);
    }

    //Render all the line charts
    for (int i = 0; i < temp_line_chart_total; i++)
    {
    	lui_linechart_draw(lui_scene->lui_line_chart[i]);
    }
}



/*------------------------------------------------------------------------------
 * 			DISPLAY DRIVER Callback Functions and Display Properties
 *------------------------------------------------------------------------------
 */

tLuiDispDrv lui_dispdrv_create()
{
	tLuiDispDrv initial_disp_drv;

	initial_disp_drv.draw_pixel_cb = NULL;
	initial_disp_drv.draw_pixels_area_cb = NULL;
	initial_disp_drv.read_input_cb = NULL;
	initial_disp_drv.display_hor_res = 320;		//horizontal 320px default
	initial_disp_drv.display_vert_res = 240;	//vertical 240px default

	return initial_disp_drv;
}


void lui_dispdrv_register (tLuiDispDrv *disp_drv)
{
	lui_disp_drv = disp_drv;
}



/*------------------------------------------------------------------------------
 * 			PRIVATE FUNCTIONS. TO BE ONLY USED BY THIS LIBRARY
 * 			These functions are helper functions
 *------------------------------------------------------------------------------
 */


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
				lui_disp_drv->draw_pixel_cb(temp_x, temp_y, fore_color);
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
			lui_disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);
		else
			lui_disp_drv->draw_pixel_cb(x, y, color);
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
			lui_disp_drv->draw_pixels_area_cb(x, y, line_width, line_width, color);
		else
			lui_disp_drv->draw_pixel_cb(x, y, color);

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
	lui_disp_drv->draw_pixels_area_cb(x, y, w, h, color);
}


/*
 * Map a range of data to a new range of data
 */
double lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min)
{
    double new_val = ((((old_val - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min);
    return new_val;
}