## LameUI - a lightweight GUI library for embedded systems

### What?
LameUI is a super simple, platform independent UI library with lots of limitations, designed for embedded systems with low resources. It's under active development and I'm trying to add more features. Two important goals of LameUI are:
 - Be lightweight
 - Use minimum number of files

### Why?
Main reason - to challenge my skills and learn new things. It was started as a lockdown project and soon enough I abandoned it in an incomplete state as I lost interest. After 4 months I came back to it with a goal to at least make it work once. Future of this project is uncertain, but as long as it feels challenging, I'll keep adding/fixing stuffs.

### Who?
**Who will use it?**  
Probably no one. Unless you need a very simple and very lightweight UI library.

**Who is/are developing it?**  
I, alone.

## Demo:
This is a demo of LameUI running on a simulator I made with opengl so I can test it without needing to flash the microcontroller every time.

simulator: https://github.com/abhra0897/LameUI_simulation

![Example gif](lameUI_demo3.gif)

Keyboard and Textbox demo (Work in progress)

![demo gif](lameUI_demo_keyboard.gif)

## UI Elements
Following UI elements are implemented so far:

1. Label
2. Line chart
3. Button
4. Switch
5. Checkbox
6. Slider
7. List
8. Button grid
9. Keyboard
10. Textbox
11. Panel

## Usage
LameUI needs 3 callback functions from user to draw on display and read inputs.  

1. `draw_pixels_area_cb` to *Display Output*.  (**Mandatory**)
2. `render_complete_cb` to *Display Output*.   (**Optional**)
3. `read_touch_input_cb` to *Read Inputs*.     (**Optional**)

### Display Output
**1. `draw_pixels_area_cb`**  
Drawing to display is done by callback functions provided by the user.

```C
void lui_dispdrv_set_draw_pixels_area_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color));
```

**2. `render_complete_cb`**  
LameUI has no frame buffer of its own. Because it's mainly targetted towards MCUs with low RAM. As a result, user need to manage buffering if they need to. Sometimes buffering, and then flushing the entire buffer is faster than writing individual pixels each time.  

That's why LameUI calls `void (*render_complete_cb)();` when rendering is complete, given that user provided the callback function. In that callback function, user should flush the buffer. This is useful when user is buffering the draw calls.  

For setting the callback function, this function is used:

```C
void lui_dispdrv_set_render_complete_cb(lui_dispdrv_t* dispdrv, void (*render_complete_cb)());
```

Note: Commonly available lcd displays that we use with microcontrollers have built-in GRAM, hence, the `render_complete_cb()` is not mandatory for them.

### Read Inputs
**3. `read_touch_input_cb`**  

LameUI now only supports touch input. Support for D-Pad / Rotary encoder is in my mind, but that's not the priority for now.

```C
void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* touch_inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* touch_inputdata));
```

## Quick Start
This is a samll LameUI code and then all parts are analyzed.

```C
#include<stdio.h>
#include "LameUI/lame_ui.h"
//... other required includes ....

#define HOR_RES    320
#define VERT_RES   240

// [Mandatory] LameUI memory. Here we are giving it 2KB to work with.
uint8_t lameui_memory[2000];

// [Optional] LameUI does NOT use display buffer. This is used only if user wants 
// to buffer on their own.
// Uncomment below line to enable display buffering. Buffering is handled by user and NOT by LameUI.
//#define USE_BUFFERING
#ifdef USE_BUFERING
	#define DISPLAY_BUFF_SIZE    (HOR_RES * VERT_RES)	// Only full screen buffer is possible (no partial buffer)
	uint16_t display_buffer[DISPLAY_BUFF_SIZE];
	uint32_t display_buff_counter = 0;
#endif


//----------- callback function prototypes ------------
void my_draw_pixels_cb (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void my_render_cmplt_cb();
void my_input_read_cb (lui_touch_input_data_t *input);
void button_1_cb(lui_obj_t* button_obj);
//-----------------------------------------------------

int main ()
{
	// [Mandatory] At the starting, initialize LameUI with some working memory.
    // This memory is used to create objects
	lui_init(lameui_memory, sizeof(lameui_memory));

	//----------------------------------------------------------
	//[Mandatory] creating display driver object
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(my_display_driver, HOR_RES, VERT_RES);
	lui_dispdrv_set_draw_pixels_area_cb(my_display_driver, my_draw_pixels_cb);
    // Setting render complete callback is optional.
    // Only needed if user is buffering the draw calls.
	#ifdef USE_BUFERING
		lui_dispdrv_set_render_complete_cb(my_display_driver, my_render_cmplt_cb);
	#endif

    //[Optional] creating input device object
	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_device, my_input_read_cb);	

	//----------------------------------------------------------
	//[Mandatory] create and add a scene
    // There must be at least one scene. It is at the top of the hierarchy. All 
    // other objects will be under the hierarchy of a scene. There can be multiple 
    // scenes. A particular object can be under only one scene at a time.
	lui_obj_t* scene_1 = lui_scene_create();
    // [Mandatory] A scene must be set as active scene. Only the active scene
    // is rendered.
	lui_scene_set_active(scene_1);

    // Create a label
	lui_obj_t* label_1 = lui_label_create();
    // [Mandatory] An object must have a parent which is either a `scene` or a 
    // descendent of `scene`.
	lui_object_add_to_parent(label_1, scene_1);
	lui_object_set_position(label_1, 5, 50);
	lui_label_set_text(label_1, "This is Label 1. Below is Button 1");

	// Create a button
	lui_obj_t* button_1 = lui_button_create();
    // [Mandatory] Add a parent
	lui_object_add_to_parent(button_1, scene_1);
	lui_object_set_area(button_1, 110, 40);
	lui_object_set_position(button_1, 65, 100);
	lui_button_set_label_text(button_1, "Button 1");
	lui_object_set_callback(button_1, button_1_cb);

	while (1)
	{
		// [Mandatory] LameUI has no built-in timer. So, update LameUI periodically.
		// lui_update() function reads inputs, checks if anything needs to be drawn, and fires callbacks if and when needed.
		lui_update();
		sleep_ms(30);
	}

	return 0;
}

//----------- callback function definitions ------------

void my_draw_pixels_cb (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	// Assuming user has at least one of these functions:
	// 1. display_put_pixel(x, y, color);    // Draw a single pixel
	// 2. display_put_pixels(x1, y1, x2, y2, color)    // Fill an area with same color
	// 3. display_fill(color_buffer, x1, y1, x2, y2);    // Fill an area with diffrent colors from a buffer


	#ifdef USE_BUFERING
		// When using buffering, we'll put pixels inside a full-screen buffer.
		// Fill the display with buffer when either the buffer is full, or when
		// the `render_complete` callback is fired by LameUI.

		uint16_t temp_x = 0;
		uint16_t temp_y = 0;
		// Prepare the display buffer
		// After the loop ends, the prepared buffer is flushed
		for (temp_y = y; temp_y <= y + height - 1; temp_y++)
		{
			for (temp_x = x; temp_x <= x + width - 1; temp_x++)
			{
				display_buffer[HOR_RES * y + x] = color;
				
				// increase the buffer counter
				display_buff_counter++;

				// If size reached max buffer size, flush it now
				if (display_buff_counter >= DISPLAY_BUFF_SIZE)
				{
					display_fill(display_buffer, 0, 0, HOR_RES, VERT_RES)	// Filling an area with multiple colors from a buffer
					display_buff_counter = 0; //reset the counter
				}
			}
		}

	#else
		// When not using buffering, directly draw those pixels.
		// There are 2 ways. Uncomment the method user wants to use.

		//------------------ [ Method 1] -----------------------------------
		// Draw pixels one by 1. This is the easiest way to implement, but also slow.
		//------------------------------------------------------------------
		// for (uint16_t h = 0; h < hright; h++)
		// {
		// 	for (uint16_t w = 0; w < width; w++)
		// 	{
		// 		display_put_pixel(x, y, color);    // Filling a single pixel with a color
		// 	}
		// }
		
		//------------------ [ Method 2] -----------------------------------
		// Another even better way is to draw an area with same color, since most displays 
		// used in embedded systems support setting drawing area and then pass a color 
		// to fill that area. This saves rendering time.
		//------------------------------------------------------------------
		display_put_pixels(x, y, x + width - 1, y + height - 1, color);    // Filling an area with same color

	#endif
}

#ifdef USE_BUFERING
void my_render_cmplt_cb()
{
	display_fill(display_buffer, 0, 0, HOR_RES, VERT_RES)	// display_fill(buffer, x1, y1, x2, y2)
	display_buff_counter = 0; //reset the counter
}
#endif

void my_input_read_cb (lui_touch_input_data_t *input)
{
	// Assuming that user has a function display_get_touch() that returns a `touch_data_t` variable.
	// Also assuming that members of `touch_data_t` are: pressed(uint8_t), x(uint16_t), y(uint16_t).
	touch_data_t touch = display_get_touch();

	input->is_pressed = touch.pressed;	// `touch.pressed` should be 1 if touched, 0 if not touched
	if (input->is_pressed == 1)
	{
		input->y = touch.y;
		input->x = touch.x;
	}
	else
	{
		// IMPORTANT: User MUST set x and y as -1 when display is NOT touched.
		input->y = -1;
		input->x = -1;
	}
}

void button_1_cb(lui_obj_t* button_obj)
{
	// Get the event of callback
	int8_t event = lui_object_get_event(button_obj);
	if (event == -1)
		return;
	
	if (event == LUI_EVENT_PRESSED)
	{
		// Button is pressed. Do something
		fprintf(stderr, "Button 1 is pressed");
	}
	else if (event == LUI_EVENT_RELEASED)
	{
		// Button is released. Do something
		fprintf(stderr, "Button 1 is released");
	}
}


```

## API 
API Documentation: https://abhra0897.github.io/LameUI/  
*Note: Documentation is work in progress. It's incomplete as of now.*

## Simple Examples
Check example code fragments inside [lame_ui.h](lame_ui.h). Or visit [documentatin page](https://abhra0897.github.io/LameUI/html/html/examples_page.html).  

(for now, go to the Simulator repo and see the example there: https://github.com/abhra0897/LameUI_simulation/blob/experimental/src/main.c). That example has all the UI elements. Ignore the parts that are related to OpenGL.