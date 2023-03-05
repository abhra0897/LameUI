# LameUI Font Maker {#font_maker_instr_page}

[The **LameUI_font_maker** repository](https://github.com/abhra0897/LameUI_font_maker/tree/lameui_1.0) is for creating fonts to use with [LameUI GUI library v1.0](https://github.com/abhra0897/LameUI/tree/v1.0). It consists of two tools:

1. **font2image:** This is a GUI tool written in C++. This is a modified version of https://github.com/andryblack/fontbuilder. font2image creates a `.png` image containing all the characters and a `.json` description file. These files are processed by `lameui_fontgen.py` in the next step.
2. **lameui_fontgen.py:** This is a CLI tool written in Python. lameui_fontgen.py takes `.png` and `.json` files created by `font2image` as input and process them to create `.c`. and `.h` font files for LameUI.

## Requirements:
1. Qt5
2. Python3
3. Python PIL library
4. make, qmake
5. gcc

## Steps:

Following steps are used to create fonts for LameUI.

### Part 1: font2image

Use font2image to create `.png` and `.json` files from a `.ttf/.otf` font file. font2image has multiple tabs. Follow below instructions sequentially.

If font2image is not already compiled, compile and run using below commands. The output binary is created inside **font_tools**(./font_tools/) folder.

```
qmake -o Makefile font2image.pro
make -j
cd font_tools
./font2image
```

After running font2image, follow these steps:

[**Font Tab**]

1. Select `Fonts directory` and choose `Font family`, `Size`, and `Style` of font.
2. Set `DPI` of font or keep it to default.
3. Set `Hinting` type from dropdown. Switch to `Font image preview` tab to the right pane to visualize how the font will look. Most of the time `Hinting: Disabled` gives suitable result.
4. Change sliders of `Faux bold` and `Faux italic` to make font bold or italic.
5. Keep `Width` and `Height` to 100%.

[**Characters Tab**]

6. Enter the characters that should be in the generated font. Keep it to default for most use cases.

[**Layout Tab**]

7. Select `Padding` offsets. This is the spacing between 2 characters. Recommended to keep it to default value (1).

[**Output Tab**]

8. Choose output directory. Recommended directory is **lameui_fonts**(./font_tools/lameui_fonts/). But it can be any directory too.
9. Set file name or keep it to default (recommended).
10. Click on `Write Font` button. Now this will generate a `.PNG` file and a `json` file in current directory.



### Part 2: lameui_fontgen.py

Use lameui_fontgen.py to convert those generated `.PNG` and `json` file pair to `.c` and `.h` file. It also adds png icons to the font. Icons are in [icons](./font_tools/icons/) folder. 

Read more about icons [here (1)](https://github.com/abhra0897/LameUI_font_maker/blob/master/font_tools/icons/README.md), [here (2)](https://github.com/abhra0897/LameUI_font_maker/blob/master/font_tools/icons/sys_icons/README.md), and [here (3)](https://github.com/abhra0897/LameUI_font_maker/blob/master/font_tools/icons/usr_icons/README.md).

Now follow these steps:

1. Run lameui_fontgen.py with:  

    `python lameui_fontgen.py <font_png_file> <font_json_file> <system_icons_folder> <user_icons_folder> <C_files_output_folder>`

    - `<font_png_file>`: .PNG file created by font2image
    - `<font_json_file>`: .json file created by font2image
    - `<system_icons_folder>`: folder where system icons are stored. Recommended: **sys_icons**(./font_tools/icons/sys_icons/).
    - `<user_icons_folder>`: folder where user icons are stored. Recommended: **usr_icons**(./font_tools/icons/usr_icons/).
    - `<C_files_output_folder>`: Folder where output `.c`. and `.h` files will be stored. Recommended: **lameui_fonts**(./font_tools/lameui_fonts/).

    Example command: 

    `python lameui_fontgen.py ubuntu_bold_16.PNG ubuntu_bold_16.json icons/sys_icons/ icons/usr_icons/ lameui_fonts/`

2. Use generated `.c`. and `.h` font files in LameUI project by including the header and compiling the c file.
