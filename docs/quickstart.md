## Usage {#quickstart_page}
LameUI needs 2 callback functions from user to draw on display and read inputs.  

1. `draw_pixels_buff_cb` to *Display Output*.  (**Mandatory**)
2. `read_touch_input_cb` to *Read Inputs*.     (**Optional**)

### **1. `draw_pixels_buff_cb`**  [ Display Output ]
Drawing to display is done by callback functions provided by the user. Register this callback using:

```C
void lui_dispdrv_set_draw_disp_buff_cb(lui_dispdrv_t* dispdrv, void (*draw_pixels_buff_cb)(uint16_t* disp_buff, lui_area_t* area));
```
Note: `lui_dispdrv_set_disp_buff(lui_dispdrv_t* dispdrv, uint16_t* disp_buff, uint16_t size_in_px)` must be called to set the display buffer. Else, `draw_pixels_buff_cb()` callback won't work.

### **2. `read_touch_input_cb`** [ Read Inputs ]
 

LameUI now only supports touch input. Support for D-Pad / Rotary encoder is in my mind, but that's not the priority for now. Register this callback using: 

```C
void lui_touch_inputdev_set_read_input_cb(lui_touch_input_dev_t* touch_inputdev, void (*read_touch_input_cb)(lui_touch_input_data_t* touch_inputdata));
```

## Quick Start

**[ Part 1 ]**

This is a very simple example using TFT_eSPI library on Arduino framework. In this example we will only see how to register a display driver and a touch input device. No UI widgets are added here. For a detailed example, see next part.

```C
// This example uses TFT_eSPI library on Arduino framework
// Hardware: [MCU = ESP32, Display: ILI9341, Touch IC = XPT2046]

#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
// ... [ Include LameUI library and other required headers too] ...

#define HOR_RES		320
#define VERT_RES	240
#define DISP_BUFF_PX_CNT (HOR_RES * 10)

uint16_t disp_buffer[DISP_BUFF_PX_CNT];

TFT_eSPI tft = TFT_eSPI();  // Invoke library
uint8_t lui_memory[4000];

void draw_disp_buff_cb(uint16_t* disp_buff, lui_area_t* area);
{
    tft.setAddrWindow(area->x, area->y, area->w, area->h);
    tft.pushColors(disp_buff, (area->w * area->h), 1);
}
void read_touch_input_cb(lui_touch_input_data_t* inputdata)
{
    uint16_t x = 0, y = 0; // To store the touch coordinates
    
    // Pressed will be set true is there is a valid touch on the screen
    bool pressed = tft.getTouch(&x, &y);
    inputdata->is_pressed = pressed;
    if (pressed)
    {
        inputdata->x = x;
        inputdata->y = y;
    }
    else
    {
        inputdata->x = -1;
        inputdata->y = -1;
    }
}

void setup(void) 
{	
    // Initialize tft
    tft.init();
    tft.setTouch(touch_cal_data);
    
    // Initialize LameUI with some memory
    lui_init(lui_memory, sizeof(lui_memory));
    
    // Create a display driver object
    lui_dispdrv_t* display_driver = lui_dispdrv_create_and_register();
    lui_dispdrv_set_resolution(display_driver, HOR_RES, VERT_RES);
    lui_dispdrv_set_disp_buff(my_display_driver, disp_buffer, DISP_BUFF_PX_CNT);
	lui_dispdrv_set_draw_disp_buff_cb(my_display_driver, draw_disp_buff_cb);
    
    // Create touch input device
    lui_touch_input_dev_t* input_device = lui_touch_inputdev_create_and_register();
    lui_touch_inputdev_set_read_input_cb(input_device, read_touch_input_cb);
    

    // ... [Add scene (mandatory) and other widgets] ...
}

void loop()
{
    // Must update the UI periodically
    lui_update();

    // ... [Do other stuffs] ...
}
```

**[ Part 2 ]**

This example shows how to create a simple text label and button. 

```C
#include<stdio.h>
#include "LameUI/lame_ui.h"
//... other required includes ....

#define HOR_RES    320
#define VERT_RES   240

// [Mandatory] Display buffer. Buffer pixel count must be multiple of Horizontal Resolution
#define DISP_BUFF_PX_CNT (HOR_RES * 10)
uint16_t disp_buffer[DISP_BUFF_PX_CNT];
// [Mandatory] LameUI memory. Here we are giving it 2KB to work with.
uint8_t lameui_memory[2000];


//----------- callback function prototypes ------------
void my_draw_disp_buff_cb (uint16_t* disp_buff, lui_area_t* area);
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
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create_and_register();
	lui_dispdrv_set_resolution(my_display_driver, HOR_RES, VERT_RES);
	lui_dispdrv_set_disp_buff(my_display_driver, disp_buffer, DISP_BUFF_PX_CNT);
	lui_dispdrv_set_draw_disp_buff_cb(my_display_driver, my_draw_disp_buff_cb);

    //[Optional] creating input device object
	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create_and_register();
	lui_touch_inputdev_set_read_input_cb(my_input_device, my_input_read_cb);	

	//----------------------------------------------------------
	//[Mandatory] create and add a scene
    // There must be at least one scene. It is at the top of the hierarchy. All 
    // other objects will be under the hierarchy of a scene. There can be multiple 
    // scenes. A particular object can be under only one scene at a time.
	lui_obj_t* scene_1 = lui_scene_create();
    // [Mandatory] A scene must be set as an active scene. Only the active scene
    // is rendered.
	lui_scene_set_active(scene_1);

    // Create a label
	lui_obj_t* label_1 = lui_label_create();
    // [Mandatory] An object must have a parent which is either a `scene` or a 
    // descendent of `scene`.
	lui_object_add_to_parent(label_1, scene_1);
	lui_object_set_position(label_1, 5, 50);
	lui_label_set_text(label_1, "This is Label 1. Below is Button 1");

	// Create a button (and directly add to parent)
	lui_obj_t* button_1 = lui_button_create_and_add(scene_1);
	lui_object_set_area(button_1, 110, 40);
	lui_object_set_position(button_1, 65, 100);
	lui_button_set_label_text(button_1, "Button 1");
	lui_object_set_callback(button_1, button_1_cb);

	while (1)
	{
		// LameUI has no built-in timer. So, update LameUI periodically.
		// lui_update() function reads inputs, checks if anything needs to be drawn, and fires callbacks if and when needed.
		lui_update();
		sleep_ms(15);
	}

	return 0;
}

//----------- callback function definitions ------------

void my_draw_disp_buff_cb(uint16_t* disp_buff, lui_area_t* area);
{
	uint16_t x = area->x;	// start x
	uint16_t y = area->y;	// start y
	uint16_t w = area->w;	// width
	uint16_t h = area->h;	// height
    display_draw_buffer(disp_buff, x, y, w, h);
}

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
