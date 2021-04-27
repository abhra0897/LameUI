## LameUI - a lame & lightweight(?) non-buffered UI library for embedded systems

This is a useless readme, I'll make a proper once later.

## What?
LameUI is a super simple, platform independent UI library with lots and lots of limitations, designed for embedded systems with low resources. Though its under active development and I'm trying to remove those limitations. Even though I tried my best to keep it light weight, it became heavier than I wanted it to be.

## Why?
Main reason - to challenge my skills and learn new things. I also happened to need a bare-minimum UI library for my own project which I abandoned as I started to make this one instead. It was started as a lockdown project and soon enough I abandoned it in an incomplete state as I lost interest. After 4 months I came back to it with a goal to at least make it work once. Future of this project is uncertain, but as long as it feels challenging, I'll keep adding/fixing stuffs.

## Lame Demo:
This is a demo of LameUI running on a simulator I made with opengl so I can test it without needing to flash the microcontroller every time.

simulator: https://github.com/abhra0897/LameUI_simulation

![Example gif](lameUI_demo3.gif)

I currently don't have an lcd display in my stock, so, this emulator is the only way. :(

## UI Elements
Following UI elements are implemented so far:

- Button
- Switch
- Line chart
- Label
- Panel
- List
- Slider
- Checkbox


## Display Output
Drawing to display is done by call back functions provided by the user.

```C
void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), lui_dispdrv_t *dispdrv);
```

LameUI has no buffer of its own. Because it's mainly targetted towards MCUs with low RAM. As a result, user need to manage buffering if they need to. Sometimes buffering, and then flush the entire buffer is faster than writing individual pixels each time

That's why LameUI calls `void (*render_complete_cb)();` when rendering is complete, given that user provided the callback function. In that callback function, user should flush the buffer. For setting the callback function, this function is used:

```C
void lui_dispdrv_set_render_complete_cb(void (*render_complete_cb)(), lui_dispdrv_t *dispdrv)
```

Note: Commonly available lcd displays that we use with microcontrollers have built-in GRAM, hence, the `render_complete_cb()` is not mandatory for them.

## Read Inputs
LameUI now only supports touch input. Support for D-Pad / Rotary encoder is in my mind, but that's not the priority for now.

```C
1. void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(tLuiTouchInputData *inputdata), tLuiTouchInputDev *inputdev);
```

## Structure of LameUI
TBD

## Simple Example
TBD

(for now, go to the Simulator repo and see the example there: https://github.com/abhra0897/LameUI_simulation/blob/experimental/src/main.c). That example has all the UI elements. Ignore the parts that are related to OpenGL.

## API
Documentation is not prepared yet cause each day I'm changing the code. Also, probably no one else will use it. For now, read the header file for all the available functions.
