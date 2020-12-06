## LameUI - a lame & lightweight non-buffered UI library for embedded systems

This is a useless readme, I'll make a proper once later.

## What?
LameUI is a super simple, platform independent UI library with lots and lots of limitations, designed for embedded systems with low resources. Though its under active development and I'm trying to remove those limitations. For now it supports only three UI elements. Completely unsuitable for any kind of application.

## Why?
Main reason - to challenge my skills and learn new things. I also happened to need a bare-minimum UI library for my own project which I abandoned as I started to make this one instead. It was started as a lockdown project and soon enough I abandoned it in an incomplete state as I lost interest. After 4 months I came back to it with a goal to at least make it work once. Future of this project is uncertain, but as long as it feels challenging, I'll keep adding/fixing stuffs.


## UI Elements
Following UI elements are implemented so far:
- Button
- Line chart
- Label
Next step is adding a slider.

## Display Output
Drawing to display is done by call back functions provided by the user. 2 callback functions are used. One for drawing a single pixel, another is drawing for an area of pixels.

```C
1. void lui_dispdrv_set_draw_pixel_cb(void (*draw_pixel_cb)(uint16_t x, uint16_t y, uint16_t color));
2. void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color));
```

LameUI is a non-buffered UI library, i.e., it has no buffer of its own. Because it's mainly targetted towards MCUs with low RAM. As a result, user need to manage buffering if they need to. Sometimes buffering, and then flush the entire buffer is faster than writing individual pixels each time. I'll add a "flush_ready" flag soon, so, if user wants to do buffering, they'll know when to flush the buffer.

## Read Inputs
LameUI natively supports two different types of input devices - touch input, D-pad input. Inputs are read using callback functions supplied by the user.

```C
1. void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *input));
2. void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(tLuiDpadInputData *input));
```

## Structure of LameUI
The concept of LameUI is like this - there is an element called **scene** which is the parent of all other elements. That is, a scene contains many different UI elements. There can be more than one scene in an application. Each UI element has a property to store their parent scene's ID. For interacting with hardware devices (display and inputs) call backs are used. Element **tLuiDispDrv** handles the display driver, element **tLuiTouchInputDev** handles touch inputs, and element **tLuiDpadInputDev** handles D-Pad inputs. User first registers these 3 elements and add a scene. Then user adds required UI elemets (buttons etc) to that scene. It looks like this:

- **tLuiDispDrv** disp_drv (mandatory)
- **tLuiTouchInputDev** touch_input (optional)
- **tLuiDpadInputDev** dpad_input (optional)
- **tLuiScene** scene1
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
//------------------Configure Display driver----------------------------------------
tLuiDispDrv my_display_driver = lui_dispdrv_create();
lui_dispdrv_register(&my_display_driver);
lui_dispdrv_set_resolution(240, 320);   // set display resolution
lui_dispdrv_set_draw_pixel_cb(my_set_pixel_function);   // set callback function for drawing a pixel
lui_dispdrv_set_draw_pixels_area_cb(my_set_pixel_area_function); // set callback function for drawing an area

//----------------Prepare the scene------------------------------------------
tLuiScene scn_one = lui_scene_create();
lui_scene_add(&scn_one);
lui_scene_set_bg_color(0xFFFF, scn_one);    // set scene background color as white
lui_scene_set_font(&font_microsoft_16, &scn_one);   // set scene font

//---------------Prepare the label and add it to the scene------------------------------
tLuiLabel lbl1 = lui_label_create();
lui_label_add_to_scene(&lbl1, &g_scn_one);
lui_label_set_text("How is the world?", &lbl1);
lui_label_set_position(0, 0, &lbl1);
lui_label_set_area(200, 50, &lbl1);
lui_label_set_colors(0x0000, 0xFFFF, &lbl1);

//---------------Render the label-------------------------------------------
lui_scene_render(&scn_one);
```