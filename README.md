## LameUI - a lame & lightweight non-buffered UI library for embedded systems

This is a useless readme, I'll make a proper once later.

## What?
LameUI is a super simple, platform independent UI library with lots and lots of limitations, designed for embedded systems with low resources. Though its under active development and I'm trying to remove those limitations. For now it supports only three UI elements. Completely unsuitable for any kind of application.

## Why?
Main reason - to challenge my skills and learn new things. I also happened to need a bare-minimum UI library for my own project which I abandoned as I started to make this one instead. It was started as a lockdown project and soon enough I abandoned it in an incomplete state as I lost interest. After 4 months I came back to it with a goal to at least make it work once. Future of this project is uncertain, but as long as it feels challenging, I'll keep adding/fixing stuffs.

## Lame Demo:
This is a demo of LameUI running on a simulator I made with opengl so I can test it without needing to flash the microcontroller every time.

simulator: https://github.com/abhra0897/LameUI_simulation

![Example gif](lameUi_demo1.gif)

The UI library is not primarily designed to work with OpenGL. So, rendering is slow in the simulator. The gif recorder made it look even slower, hah!

## UI Elements
Following UI elements are implemented so far:
- Button
- Switch
- Line chart
- Label
Next step is adding a slider.

## Display Output
Drawing to display is done by call back functions provided by the user. 2 callback functions are used. One for drawing a single pixel, another is drawing for an area of pixels.

```C
1. void lui_dispdrv_set_draw_pixel_cb(void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color), tLuiDispDrv *dispdrv)
2. void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), tLuiDispDrv *dispdrv)
```

LameUI is a non-buffered UI library, i.e., it has no buffer of its own. Because it's mainly targetted towards MCUs with low RAM. As a result, user need to manage buffering if they need to. Sometimes buffering, and then flush the entire buffer is faster than writing individual pixels each time. I'll add a "flush_ready" flag soon, so, if user wants to do buffering, they'll know when to flush the buffer.

## Read Inputs
LameUI natively supports two different types of input devices - touch input, D-pad input. Inputs are read using callback functions supplied by the user.

```C
1. void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *inputdata), tLuiTouchInputDev *inputdev);
2. void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(tLuiDpadInputData *dpad_inputdata), tLuiDpadInputDev *dpad_inputdev);
```

## Structure of LameUI
The concept of LameUI is like this - there is an element called **scene** which is the parent of all other elements. That is, a scene contains many different UI elements. There can be more than one scene in an application. Each UI element has a property to store their parent scene's ID. For interacting with hardware devices (display and inputs) call-backs are used. Element **tLuiDispDrv** handles the display driver, element **tLuiTouchInputDev** handles touch inputs, and element **tLuiDpadInputDev** handles D-Pad inputs. User first registers these 3 elements and add a scene. Then user adds required UI elemets (buttons etc) to that scene. It looks like this:

- **tLuiDispDrv** disp_drv (mandatory)
- **tLuiTouchInputDev** touch_input
- **tLuiDpadInputDev** dpad_input
- **tLuiScene** scene1 (mandatory)
    - **tLuiButton** button1
    - **tLuiLineChart** graph1
    - **tLuiLabel** label1
- **tLuiScene** scene2
    - **tLuiButton** button2
    - **tLuiLabel** label2

## Simple Example
Here is a simple example of how to add a label with text "*How is world?*". 
Note: Configuring the display driver, input devices, and preparing scene are one time processes. Once done, you need not do them again each time you add a UI element like button, label etc.

```C
//----------------------------------------------------------
//create and register a dsiplay driver
tLuiDispDrv *my_display_driver = lui_dispdrv_create();
lui_dispdrv_register(my_display_driver);
lui_dispdrv_set_resolution(240, 320, my_display_driver);
lui_dispdrv_set_draw_pixel_cb(my_set_pixel, my_display_driver);
lui_dispdrv_set_draw_pixels_area_cb(my_set_pixel_area, my_display_driver);

//----------------------------------------------------------
//create and register touch/mouse input
tLuiTouchInputDev *my_input_device = lui_touch_inputdev_create();
lui_touch_inputdev_register(my_input_device);
lui_touch_inputdev_set_read_input_cb(my_input_read, my_input_device);	

//----------------------------------------------------------
//create and add scene
tLuiScene *scn_one = lui_scene_create();
lui_scene_set_bg_color(LUI_RGB(0, 0, 0), scn_one);
lui_scene_set_font(&font_microsoft_16, scn_one);

//----------------------------------------------------------
//create a label
g_lbl1 = lui_label_create();
lui_label_add_to_scene(g_lbl1, scn_one);
lui_label_set_text("Hello World", g_lbl1);
lui_label_set_position(52, 0, g_lbl1);
lui_label_set_area(190, 20, g_lbl1);
lui_label_set_colors(LUI_RGB(0, 250, 0), LUI_RGB(60, 60, 60), g_lbl1);
//lui_label_set_font(&font_microsoft_16, g_lbl1);

//---------------Render the scene----------------------------
lui_scene_render(scn_one);
```

## API
Documentation is not prepared yet cause each day I'm changing the code. Also, probably no one else will use it. For now, read the header file for all the available functions.
