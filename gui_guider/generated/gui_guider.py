# Copyright 2022 NXP
# SPDX-License-Identifier: MIT
# The auto-generated can only be used on NXP devices

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct

lv.init()
SDL.init(w=800,h=480)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(800*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 800
disp_drv.ver_res = 480
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)



# create screen
screen = lv.obj()
screen.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# create style style_screen_main_main_default
style_screen_main_main_default = lv.style_t()
style_screen_main_main_default.init()
style_screen_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_main_main_default.set_bg_opa(0)

# add style for screen
screen.add_style(style_screen_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_Background
screen_Background = lv.img(screen)
screen_Background.set_pos(int(0),int(0))
screen_Background.set_size(800,480)
screen_Background.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Background.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1578916293.png','rb') as f:
        screen_Background_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1578916293.png')
    sys.exit()

screen_Background_img = lv.img_dsc_t({
  'data_size': len(screen_Background_img_data),
  'header': {'always_zero': 0, 'w': 800, 'h': 480, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_Background_img_data
})

screen_Background.set_src(screen_Background_img)
screen_Background.set_pivot(50,50)
screen_Background.set_angle(0)
# create style style_screen_background_main_main_default
style_screen_background_main_main_default = lv.style_t()
style_screen_background_main_main_default.init()
style_screen_background_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_background_main_main_default.set_img_recolor_opa(0)
style_screen_background_main_main_default.set_img_opa(255)

# add style for screen_Background
screen_Background.add_style(style_screen_background_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_Date
screen_Date = lv.label(screen)
screen_Date.set_pos(int(636),int(11))
screen_Date.set_size(155,31)
screen_Date.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Date.set_text("Sep 07, 2023")
screen_Date.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_date_main_main_default
style_screen_date_main_main_default = lv.style_t()
style_screen_date_main_main_default.init()
style_screen_date_main_main_default.set_radius(0)
style_screen_date_main_main_default.set_bg_color(lv.color_make(0x00,0x00,0x00))
style_screen_date_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_date_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_date_main_main_default.set_bg_opa(0)
style_screen_date_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_date_main_main_default.set_text_font(lv.font_montserratMedium_20)
except AttributeError:
    try:
        style_screen_date_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_screen_date_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_date_main_main_default.set_text_letter_space(2)
style_screen_date_main_main_default.set_text_line_space(0)
style_screen_date_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_date_main_main_default.set_pad_left(0)
style_screen_date_main_main_default.set_pad_right(0)
style_screen_date_main_main_default.set_pad_top(8)
style_screen_date_main_main_default.set_pad_bottom(0)

# add style for screen_Date
screen_Date.add_style(style_screen_date_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_Clock
screen_Clock = lv.label(screen)
screen_Clock.set_pos(int(0),int(437))
screen_Clock.set_size(149,31)
screen_Clock.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Clock.set_text("12:00 am")
screen_Clock.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_clock_main_main_default
style_screen_clock_main_main_default = lv.style_t()
style_screen_clock_main_main_default.init()
style_screen_clock_main_main_default.set_radius(0)
style_screen_clock_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_clock_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_clock_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_clock_main_main_default.set_bg_opa(0)
style_screen_clock_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_clock_main_main_default.set_text_font(lv.font_montserratMedium_20)
except AttributeError:
    try:
        style_screen_clock_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_screen_clock_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_clock_main_main_default.set_text_letter_space(2)
style_screen_clock_main_main_default.set_text_line_space(0)
style_screen_clock_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_clock_main_main_default.set_pad_left(0)
style_screen_clock_main_main_default.set_pad_right(0)
style_screen_clock_main_main_default.set_pad_top(8)
style_screen_clock_main_main_default.set_pad_bottom(0)

# add style for screen_Clock
screen_Clock.add_style(style_screen_clock_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_StationButton1
screen_StationButton1 = lv.imgbtn(screen)
screen_StationButton1.set_pos(int(384),int(388))
screen_StationButton1.set_size(70,65)
screen_StationButton1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-374002602.png','rb') as f:
        screen_StationButton1_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-374002602.png')
    sys.exit()

screen_StationButton1_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton1_imgReleased_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 65, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton1_imgReleased_data
})
screen_StationButton1.set_src(lv.imgbtn.STATE.RELEASED, None, screen_StationButton1_imgReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1703206741.png','rb') as f:
        screen_StationButton1_imgPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1703206741.png')
    sys.exit()

screen_StationButton1_imgPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton1_imgPressed_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 65, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton1_imgPressed_data
})
screen_StationButton1.set_src(lv.imgbtn.STATE.PRESSED, None, screen_StationButton1_imgPressed, None)


try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-374002602.png','rb') as f:
        screen_StationButton1_imgCheckedReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-374002602.png')
    sys.exit()

screen_StationButton1_imgCheckedReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton1_imgCheckedReleased_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 65, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton1_imgCheckedReleased_data
})
screen_StationButton1.set_src(lv.imgbtn.STATE.CHECKED_RELEASED, None, screen_StationButton1_imgCheckedReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1703206741.png','rb') as f:
        screen_StationButton1_imgCheckedPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1703206741.png')
    sys.exit()

screen_StationButton1_imgCheckedPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton1_imgCheckedPressed_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 65, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton1_imgCheckedPressed_data
})
screen_StationButton1.set_src(lv.imgbtn.STATE.CHECKED_PRESSED, None, screen_StationButton1_imgCheckedPressed, None)

screen_StationButton1.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_stationbutton1_main_main_default
style_screen_stationbutton1_main_main_default = lv.style_t()
style_screen_stationbutton1_main_main_default.init()
style_screen_stationbutton1_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_stationbutton1_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton1_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton1_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_stationbutton1_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_stationbutton1_main_main_default.set_img_recolor_opa(0)
style_screen_stationbutton1_main_main_default.set_img_opa(255)

# add style for screen_StationButton1
screen_StationButton1.add_style(style_screen_stationbutton1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationbutton1_main_main_pressed
style_screen_stationbutton1_main_main_pressed = lv.style_t()
style_screen_stationbutton1_main_main_pressed.init()
style_screen_stationbutton1_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton1_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton1_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton1_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton1_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton1_main_main_pressed.set_img_recolor_opa(0)
style_screen_stationbutton1_main_main_pressed.set_img_opa(255)

# add style for screen_StationButton1
screen_StationButton1.add_style(style_screen_stationbutton1_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_stationbutton1_main_main_checked
style_screen_stationbutton1_main_main_checked = lv.style_t()
style_screen_stationbutton1_main_main_checked.init()
style_screen_stationbutton1_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton1_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton1_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton1_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton1_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton1_main_main_checked.set_img_recolor_opa(0)
style_screen_stationbutton1_main_main_checked.set_img_opa(255)

# add style for screen_StationButton1
screen_StationButton1.add_style(style_screen_stationbutton1_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_StationButton2
screen_StationButton2 = lv.imgbtn(screen)
screen_StationButton2.set_pos(int(292),int(387))
screen_StationButton2.set_size(70,66)
screen_StationButton2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1391870670.png','rb') as f:
        screen_StationButton2_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1391870670.png')
    sys.exit()

screen_StationButton2_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton2_imgReleased_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 66, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton2_imgReleased_data
})
screen_StationButton2.set_src(lv.imgbtn.STATE.RELEASED, None, screen_StationButton2_imgReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-825887283.png','rb') as f:
        screen_StationButton2_imgPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-825887283.png')
    sys.exit()

screen_StationButton2_imgPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton2_imgPressed_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 66, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton2_imgPressed_data
})
screen_StationButton2.set_src(lv.imgbtn.STATE.PRESSED, None, screen_StationButton2_imgPressed, None)


try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1391870670.png','rb') as f:
        screen_StationButton2_imgCheckedReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1391870670.png')
    sys.exit()

screen_StationButton2_imgCheckedReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton2_imgCheckedReleased_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 66, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton2_imgCheckedReleased_data
})
screen_StationButton2.set_src(lv.imgbtn.STATE.CHECKED_RELEASED, None, screen_StationButton2_imgCheckedReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-825887283.png','rb') as f:
        screen_StationButton2_imgCheckedPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-825887283.png')
    sys.exit()

screen_StationButton2_imgCheckedPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton2_imgCheckedPressed_data),
  'header': {'always_zero': 0, 'w': 70, 'h': 66, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton2_imgCheckedPressed_data
})
screen_StationButton2.set_src(lv.imgbtn.STATE.CHECKED_PRESSED, None, screen_StationButton2_imgCheckedPressed, None)

screen_StationButton2.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_stationbutton2_main_main_default
style_screen_stationbutton2_main_main_default = lv.style_t()
style_screen_stationbutton2_main_main_default.init()
style_screen_stationbutton2_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_stationbutton2_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton2_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton2_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_stationbutton2_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_stationbutton2_main_main_default.set_img_recolor_opa(0)
style_screen_stationbutton2_main_main_default.set_img_opa(255)

# add style for screen_StationButton2
screen_StationButton2.add_style(style_screen_stationbutton2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationbutton2_main_main_pressed
style_screen_stationbutton2_main_main_pressed = lv.style_t()
style_screen_stationbutton2_main_main_pressed.init()
style_screen_stationbutton2_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton2_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton2_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton2_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton2_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton2_main_main_pressed.set_img_recolor_opa(0)
style_screen_stationbutton2_main_main_pressed.set_img_opa(255)

# add style for screen_StationButton2
screen_StationButton2.add_style(style_screen_stationbutton2_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_stationbutton2_main_main_checked
style_screen_stationbutton2_main_main_checked = lv.style_t()
style_screen_stationbutton2_main_main_checked.init()
style_screen_stationbutton2_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton2_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton2_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton2_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton2_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton2_main_main_checked.set_img_recolor_opa(0)
style_screen_stationbutton2_main_main_checked.set_img_opa(255)

# add style for screen_StationButton2
screen_StationButton2.add_style(style_screen_stationbutton2_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_StationButton3
screen_StationButton3 = lv.imgbtn(screen)
screen_StationButton3.set_pos(int(198),int(386))
screen_StationButton3.set_size(73,67)
screen_StationButton3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-988074009.png','rb') as f:
        screen_StationButton3_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-988074009.png')
    sys.exit()

screen_StationButton3_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton3_imgReleased_data),
  'header': {'always_zero': 0, 'w': 73, 'h': 67, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton3_imgReleased_data
})
screen_StationButton3.set_src(lv.imgbtn.STATE.RELEASED, None, screen_StationButton3_imgReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1089135334.png','rb') as f:
        screen_StationButton3_imgPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1089135334.png')
    sys.exit()

screen_StationButton3_imgPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton3_imgPressed_data),
  'header': {'always_zero': 0, 'w': 73, 'h': 67, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton3_imgPressed_data
})
screen_StationButton3.set_src(lv.imgbtn.STATE.PRESSED, None, screen_StationButton3_imgPressed, None)


try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-988074009.png','rb') as f:
        screen_StationButton3_imgCheckedReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-988074009.png')
    sys.exit()

screen_StationButton3_imgCheckedReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton3_imgCheckedReleased_data),
  'header': {'always_zero': 0, 'w': 73, 'h': 67, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton3_imgCheckedReleased_data
})
screen_StationButton3.set_src(lv.imgbtn.STATE.CHECKED_RELEASED, None, screen_StationButton3_imgCheckedReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1089135334.png','rb') as f:
        screen_StationButton3_imgCheckedPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1089135334.png')
    sys.exit()

screen_StationButton3_imgCheckedPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton3_imgCheckedPressed_data),
  'header': {'always_zero': 0, 'w': 73, 'h': 67, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton3_imgCheckedPressed_data
})
screen_StationButton3.set_src(lv.imgbtn.STATE.CHECKED_PRESSED, None, screen_StationButton3_imgCheckedPressed, None)

screen_StationButton3.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_stationbutton3_main_main_default
style_screen_stationbutton3_main_main_default = lv.style_t()
style_screen_stationbutton3_main_main_default.init()
style_screen_stationbutton3_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_stationbutton3_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton3_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton3_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton3_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_stationbutton3_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_stationbutton3_main_main_default.set_img_recolor_opa(0)
style_screen_stationbutton3_main_main_default.set_img_opa(255)

# add style for screen_StationButton3
screen_StationButton3.add_style(style_screen_stationbutton3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationbutton3_main_main_pressed
style_screen_stationbutton3_main_main_pressed = lv.style_t()
style_screen_stationbutton3_main_main_pressed.init()
style_screen_stationbutton3_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton3_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton3_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton3_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton3_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton3_main_main_pressed.set_img_recolor_opa(0)
style_screen_stationbutton3_main_main_pressed.set_img_opa(255)

# add style for screen_StationButton3
screen_StationButton3.add_style(style_screen_stationbutton3_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_stationbutton3_main_main_checked
style_screen_stationbutton3_main_main_checked = lv.style_t()
style_screen_stationbutton3_main_main_checked.init()
style_screen_stationbutton3_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton3_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton3_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton3_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton3_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton3_main_main_checked.set_img_recolor_opa(0)
style_screen_stationbutton3_main_main_checked.set_img_opa(255)

# add style for screen_StationButton3
screen_StationButton3.add_style(style_screen_stationbutton3_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_StationButton4
screen_StationButton4 = lv.imgbtn(screen)
screen_StationButton4.set_pos(int(479),int(386))
screen_StationButton4.set_size(65,63)
screen_StationButton4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp2031564764.png','rb') as f:
        screen_StationButton4_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp2031564764.png')
    sys.exit()

screen_StationButton4_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton4_imgReleased_data),
  'header': {'always_zero': 0, 'w': 65, 'h': 63, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton4_imgReleased_data
})
screen_StationButton4.set_src(lv.imgbtn.STATE.RELEASED, None, screen_StationButton4_imgReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-186193189.png','rb') as f:
        screen_StationButton4_imgPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-186193189.png')
    sys.exit()

screen_StationButton4_imgPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton4_imgPressed_data),
  'header': {'always_zero': 0, 'w': 65, 'h': 63, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton4_imgPressed_data
})
screen_StationButton4.set_src(lv.imgbtn.STATE.PRESSED, None, screen_StationButton4_imgPressed, None)


try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp2031564764.png','rb') as f:
        screen_StationButton4_imgCheckedReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp2031564764.png')
    sys.exit()

screen_StationButton4_imgCheckedReleased = lv.img_dsc_t({
  'data_size': len(screen_StationButton4_imgCheckedReleased_data),
  'header': {'always_zero': 0, 'w': 65, 'h': 63, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton4_imgCheckedReleased_data
})
screen_StationButton4.set_src(lv.imgbtn.STATE.CHECKED_RELEASED, None, screen_StationButton4_imgCheckedReleased, None)

try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-186193189.png','rb') as f:
        screen_StationButton4_imgCheckedPressed_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-186193189.png')
    sys.exit()

screen_StationButton4_imgCheckedPressed = lv.img_dsc_t({
  'data_size': len(screen_StationButton4_imgCheckedPressed_data),
  'header': {'always_zero': 0, 'w': 65, 'h': 63, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_StationButton4_imgCheckedPressed_data
})
screen_StationButton4.set_src(lv.imgbtn.STATE.CHECKED_PRESSED, None, screen_StationButton4_imgCheckedPressed, None)

screen_StationButton4.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_stationbutton4_main_main_default
style_screen_stationbutton4_main_main_default = lv.style_t()
style_screen_stationbutton4_main_main_default.init()
style_screen_stationbutton4_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_stationbutton4_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton4_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton4_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton4_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_stationbutton4_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_stationbutton4_main_main_default.set_img_recolor_opa(0)
style_screen_stationbutton4_main_main_default.set_img_opa(255)

# add style for screen_StationButton4
screen_StationButton4.add_style(style_screen_stationbutton4_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationbutton4_main_main_pressed
style_screen_stationbutton4_main_main_pressed = lv.style_t()
style_screen_stationbutton4_main_main_pressed.init()
style_screen_stationbutton4_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton4_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton4_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton4_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton4_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton4_main_main_pressed.set_img_recolor_opa(0)
style_screen_stationbutton4_main_main_pressed.set_img_opa(255)

# add style for screen_StationButton4
screen_StationButton4.add_style(style_screen_stationbutton4_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_stationbutton4_main_main_checked
style_screen_stationbutton4_main_main_checked = lv.style_t()
style_screen_stationbutton4_main_main_checked.init()
style_screen_stationbutton4_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_stationbutton4_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_stationbutton4_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_stationbutton4_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_stationbutton4_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_stationbutton4_main_main_checked.set_img_recolor_opa(0)
style_screen_stationbutton4_main_main_checked.set_img_opa(255)

# add style for screen_StationButton4
screen_StationButton4.add_style(style_screen_stationbutton4_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_StationsView
screen_StationsView = lv.tileview(screen)
screen_StationsView.set_pos(int(177),int(97))
screen_StationsView.set_size(388,252)
screen_StationsView.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_StationsView_StationTileview4 = screen_StationsView.add_tile(3 , 0, lv.DIR.LEFT)

# create screen_tittleLabel4
screen_tittleLabel4 = lv.label(screen_StationsView_StationTileview4)
screen_tittleLabel4.set_pos(int(110),int(-1))
screen_tittleLabel4.set_size(170,47)
screen_tittleLabel4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_tittleLabel4.set_text("Control Interface")
screen_tittleLabel4.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_tittlelabel4_main_main_default
style_screen_tittlelabel4_main_main_default = lv.style_t()
style_screen_tittlelabel4_main_main_default.init()
style_screen_tittlelabel4_main_main_default.set_radius(0)
style_screen_tittlelabel4_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel4_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel4_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tittlelabel4_main_main_default.set_bg_opa(0)
style_screen_tittlelabel4_main_main_default.set_text_color(lv.color_make(0xe0,0xb7,0x4f))
try:
    style_screen_tittlelabel4_main_main_default.set_text_font(lv.font_montserratMedium_18)
except AttributeError:
    try:
        style_screen_tittlelabel4_main_main_default.set_text_font(lv.font_montserrat_18)
    except AttributeError:
        style_screen_tittlelabel4_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_tittlelabel4_main_main_default.set_text_letter_space(2)
style_screen_tittlelabel4_main_main_default.set_text_line_space(0)
style_screen_tittlelabel4_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_tittlelabel4_main_main_default.set_pad_left(0)
style_screen_tittlelabel4_main_main_default.set_pad_right(0)
style_screen_tittlelabel4_main_main_default.set_pad_top(8)
style_screen_tittlelabel4_main_main_default.set_pad_bottom(0)

# add style for screen_tittleLabel4
screen_tittleLabel4.add_style(style_screen_tittlelabel4_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_CounterArc
screen_CounterArc = lv.arc(screen_StationsView_StationTileview4)
screen_CounterArc.set_pos(int(206),int(43))
screen_CounterArc.set_size(161,163)
screen_CounterArc.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_CounterArc.set_mode(lv.arc.MODE.NORMAL)
screen_CounterArc.set_range(0, 255)
screen_CounterArc.set_bg_angles(0, 360)
screen_CounterArc.set_angles(0, 200)
screen_CounterArc.set_rotation(90)
# create style style_screen_counterarc_main_main_default
style_screen_counterarc_main_main_default = lv.style_t()
style_screen_counterarc_main_main_default.init()
style_screen_counterarc_main_main_default.set_radius(6)
style_screen_counterarc_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_counterarc_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_counterarc_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_counterarc_main_main_default.set_bg_opa(0)
style_screen_counterarc_main_main_default.set_border_width(0)
style_screen_counterarc_main_main_default.set_pad_left(20)
style_screen_counterarc_main_main_default.set_pad_right(20)
style_screen_counterarc_main_main_default.set_pad_top(20)
style_screen_counterarc_main_main_default.set_pad_bottom(20)
style_screen_counterarc_main_main_default.set_arc_color(lv.color_make(0x5c,0x5c,0x5c))
style_screen_counterarc_main_main_default.set_arc_width(20)

# add style for screen_CounterArc
screen_CounterArc.add_style(style_screen_counterarc_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_counterarc_main_indicator_default
style_screen_counterarc_main_indicator_default = lv.style_t()
style_screen_counterarc_main_indicator_default.init()
style_screen_counterarc_main_indicator_default.set_arc_color(lv.color_make(0x80,0xA2,0x47))
style_screen_counterarc_main_indicator_default.set_arc_width(20)

# add style for screen_CounterArc
screen_CounterArc.add_style(style_screen_counterarc_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_counterarc_main_knob_default
style_screen_counterarc_main_knob_default = lv.style_t()
style_screen_counterarc_main_knob_default.init()
style_screen_counterarc_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_counterarc_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_counterarc_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_counterarc_main_knob_default.set_bg_opa(0)
style_screen_counterarc_main_knob_default.set_pad_all(20)

# add style for screen_CounterArc
screen_CounterArc.add_style(style_screen_counterarc_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_MinusButton
screen_MinusButton = lv.btn(screen_StationsView_StationTileview4)
screen_MinusButton.set_pos(int(217),int(198))
screen_MinusButton.set_size(61,45)
screen_MinusButton.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_MinusButton_label = lv.label(screen_MinusButton)
screen_MinusButton_label.set_text("-")
screen_MinusButton.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_MinusButton_label.align(lv.ALIGN.CENTER,0,0)
# create style style_screen_minusbutton_main_main_default
style_screen_minusbutton_main_main_default = lv.style_t()
style_screen_minusbutton_main_main_default.init()
style_screen_minusbutton_main_main_default.set_radius(5)
style_screen_minusbutton_main_main_default.set_bg_color(lv.color_make(0x5C,0x5C,0x5C))
style_screen_minusbutton_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_minusbutton_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_minusbutton_main_main_default.set_bg_opa(255)
style_screen_minusbutton_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_minusbutton_main_main_default.set_border_width(0)
style_screen_minusbutton_main_main_default.set_border_opa(255)
style_screen_minusbutton_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_minusbutton_main_main_default.set_text_font(lv.font_montserratMedium_50)
except AttributeError:
    try:
        style_screen_minusbutton_main_main_default.set_text_font(lv.font_montserrat_50)
    except AttributeError:
        style_screen_minusbutton_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_minusbutton_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)

# add style for screen_MinusButton
screen_MinusButton.add_style(style_screen_minusbutton_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_minusbutton_main_main_pressed
style_screen_minusbutton_main_main_pressed = lv.style_t()
style_screen_minusbutton_main_main_pressed.init()
style_screen_minusbutton_main_main_pressed.set_radius(5)
style_screen_minusbutton_main_main_pressed.set_bg_color(lv.color_make(0x85,0x85,0x85))
style_screen_minusbutton_main_main_pressed.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_minusbutton_main_main_pressed.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_minusbutton_main_main_pressed.set_bg_opa(255)
style_screen_minusbutton_main_main_pressed.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_minusbutton_main_main_pressed.set_border_width(0)
style_screen_minusbutton_main_main_pressed.set_border_opa(255)
style_screen_minusbutton_main_main_pressed.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_minusbutton_main_main_pressed.set_text_font(lv.font_montserratMedium_40)
except AttributeError:
    try:
        style_screen_minusbutton_main_main_pressed.set_text_font(lv.font_montserrat_40)
    except AttributeError:
        style_screen_minusbutton_main_main_pressed.set_text_font(lv.font_montserrat_16)

# add style for screen_MinusButton
screen_MinusButton.add_style(style_screen_minusbutton_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)


# create screen_PlusButton
screen_PlusButton = lv.btn(screen_StationsView_StationTileview4)
screen_PlusButton.set_pos(int(297),int(198))
screen_PlusButton.set_size(61,45)
screen_PlusButton.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_PlusButton_label = lv.label(screen_PlusButton)
screen_PlusButton_label.set_text("+")
screen_PlusButton.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_PlusButton_label.align(lv.ALIGN.CENTER,0,0)
# create style style_screen_plusbutton_main_main_default
style_screen_plusbutton_main_main_default = lv.style_t()
style_screen_plusbutton_main_main_default.init()
style_screen_plusbutton_main_main_default.set_radius(5)
style_screen_plusbutton_main_main_default.set_bg_color(lv.color_make(0x5C,0x5C,0x5C))
style_screen_plusbutton_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_plusbutton_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_plusbutton_main_main_default.set_bg_opa(255)
style_screen_plusbutton_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_plusbutton_main_main_default.set_border_width(0)
style_screen_plusbutton_main_main_default.set_border_opa(255)
style_screen_plusbutton_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_plusbutton_main_main_default.set_text_font(lv.font_montserratMedium_40)
except AttributeError:
    try:
        style_screen_plusbutton_main_main_default.set_text_font(lv.font_montserrat_40)
    except AttributeError:
        style_screen_plusbutton_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_plusbutton_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)

# add style for screen_PlusButton
screen_PlusButton.add_style(style_screen_plusbutton_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_plusbutton_main_main_pressed
style_screen_plusbutton_main_main_pressed = lv.style_t()
style_screen_plusbutton_main_main_pressed.init()
style_screen_plusbutton_main_main_pressed.set_radius(5)
style_screen_plusbutton_main_main_pressed.set_bg_color(lv.color_make(0x85,0x85,0x85))
style_screen_plusbutton_main_main_pressed.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_plusbutton_main_main_pressed.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_plusbutton_main_main_pressed.set_bg_opa(255)
style_screen_plusbutton_main_main_pressed.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_plusbutton_main_main_pressed.set_border_width(0)
style_screen_plusbutton_main_main_pressed.set_border_opa(255)
style_screen_plusbutton_main_main_pressed.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_plusbutton_main_main_pressed.set_text_font(lv.font_montserratMedium_40)
except AttributeError:
    try:
        style_screen_plusbutton_main_main_pressed.set_text_font(lv.font_montserrat_40)
    except AttributeError:
        style_screen_plusbutton_main_main_pressed.set_text_font(lv.font_montserrat_16)

# add style for screen_PlusButton
screen_PlusButton.add_style(style_screen_plusbutton_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)


# create screen_CounterLabel
screen_CounterLabel = lv.label(screen_StationsView_StationTileview4)
screen_CounterLabel.set_pos(int(260),int(102))
screen_CounterLabel.set_size(54,32)
screen_CounterLabel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_CounterLabel.set_text("115")
screen_CounterLabel.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_counterlabel_main_main_default
style_screen_counterlabel_main_main_default = lv.style_t()
style_screen_counterlabel_main_main_default.init()
style_screen_counterlabel_main_main_default.set_radius(0)
style_screen_counterlabel_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_counterlabel_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_counterlabel_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_counterlabel_main_main_default.set_bg_opa(0)
style_screen_counterlabel_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_counterlabel_main_main_default.set_text_font(lv.font_montserratMedium_24)
except AttributeError:
    try:
        style_screen_counterlabel_main_main_default.set_text_font(lv.font_montserrat_24)
    except AttributeError:
        style_screen_counterlabel_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_counterlabel_main_main_default.set_text_letter_space(2)
style_screen_counterlabel_main_main_default.set_text_line_space(0)
style_screen_counterlabel_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_counterlabel_main_main_default.set_pad_left(0)
style_screen_counterlabel_main_main_default.set_pad_right(0)
style_screen_counterlabel_main_main_default.set_pad_top(8)
style_screen_counterlabel_main_main_default.set_pad_bottom(0)

# add style for screen_CounterLabel
screen_CounterLabel.add_style(style_screen_counterlabel_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_BarIndicator
screen_BarIndicator = lv.bar(screen_StationsView_StationTileview4)
screen_BarIndicator.set_pos(int(74),int(73))
screen_BarIndicator.set_size(44,125)
screen_BarIndicator.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_BarIndicator.set_style_anim_time(1000, 0)
screen_BarIndicator.set_mode(lv.bar.MODE.NORMAL)
screen_BarIndicator.set_value(70, lv.ANIM.OFF)
# create style style_screen_barindicator_main_main_default
style_screen_barindicator_main_main_default = lv.style_t()
style_screen_barindicator_main_main_default.init()
style_screen_barindicator_main_main_default.set_radius(10)
style_screen_barindicator_main_main_default.set_bg_color(lv.color_make(0x5C,0x5C,0x5C))
style_screen_barindicator_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_barindicator_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_barindicator_main_main_default.set_bg_opa(255)

# add style for screen_BarIndicator
screen_BarIndicator.add_style(style_screen_barindicator_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_barindicator_main_indicator_default
style_screen_barindicator_main_indicator_default = lv.style_t()
style_screen_barindicator_main_indicator_default.init()
style_screen_barindicator_main_indicator_default.set_radius(10)
style_screen_barindicator_main_indicator_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_barindicator_main_indicator_default.set_bg_grad_color(lv.color_make(0x00,0x00,0x00))
style_screen_barindicator_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_barindicator_main_indicator_default.set_bg_opa(129)

# add style for screen_BarIndicator
screen_BarIndicator.add_style(style_screen_barindicator_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)


# create screen_SliderControl
screen_SliderControl = lv.slider(screen_StationsView_StationTileview4)
screen_SliderControl.set_pos(int(12),int(216))
screen_SliderControl.set_size(175,10)
screen_SliderControl.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_SliderControl.set_range(0, 100)
screen_SliderControl.set_value(50, False)

# create style style_screen_slidercontrol_main_main_default
style_screen_slidercontrol_main_main_default = lv.style_t()
style_screen_slidercontrol_main_main_default.init()
style_screen_slidercontrol_main_main_default.set_radius(50)
style_screen_slidercontrol_main_main_default.set_bg_color(lv.color_make(0x5C,0x5C,0x5C))
style_screen_slidercontrol_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_slidercontrol_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_slidercontrol_main_main_default.set_bg_opa(255)
style_screen_slidercontrol_main_main_default.set_outline_color(lv.color_make(0x5C,0x5C,0x5C))
style_screen_slidercontrol_main_main_default.set_outline_width(0)
style_screen_slidercontrol_main_main_default.set_outline_opa(0)

# add style for screen_SliderControl
screen_SliderControl.add_style(style_screen_slidercontrol_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_slidercontrol_main_indicator_default
style_screen_slidercontrol_main_indicator_default = lv.style_t()
style_screen_slidercontrol_main_indicator_default.init()
style_screen_slidercontrol_main_indicator_default.set_radius(50)
style_screen_slidercontrol_main_indicator_default.set_bg_color(lv.color_make(0xE0,0xB7,0x4F))
style_screen_slidercontrol_main_indicator_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_slidercontrol_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_slidercontrol_main_indicator_default.set_bg_opa(255)

# add style for screen_SliderControl
screen_SliderControl.add_style(style_screen_slidercontrol_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_slidercontrol_main_knob_default
style_screen_slidercontrol_main_knob_default = lv.style_t()
style_screen_slidercontrol_main_knob_default.init()
style_screen_slidercontrol_main_knob_default.set_radius(50)
style_screen_slidercontrol_main_knob_default.set_bg_color(lv.color_make(0xE0,0xB7,0x4F))
style_screen_slidercontrol_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_slidercontrol_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_slidercontrol_main_knob_default.set_bg_opa(144)

# add style for screen_SliderControl
screen_SliderControl.add_style(style_screen_slidercontrol_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_SliderPosition
screen_SliderPosition = lv.label(screen_StationsView_StationTileview4)
screen_SliderPosition.set_pos(int(68),int(41))
screen_SliderPosition.set_size(54,32)
screen_SliderPosition.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_SliderPosition.set_text("70")
screen_SliderPosition.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_sliderposition_main_main_default
style_screen_sliderposition_main_main_default = lv.style_t()
style_screen_sliderposition_main_main_default.init()
style_screen_sliderposition_main_main_default.set_radius(0)
style_screen_sliderposition_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sliderposition_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sliderposition_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_sliderposition_main_main_default.set_bg_opa(0)
style_screen_sliderposition_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_sliderposition_main_main_default.set_text_font(lv.font_montserratMedium_20)
except AttributeError:
    try:
        style_screen_sliderposition_main_main_default.set_text_font(lv.font_montserrat_20)
    except AttributeError:
        style_screen_sliderposition_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_sliderposition_main_main_default.set_text_letter_space(2)
style_screen_sliderposition_main_main_default.set_text_line_space(0)
style_screen_sliderposition_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_sliderposition_main_main_default.set_pad_left(0)
style_screen_sliderposition_main_main_default.set_pad_right(0)
style_screen_sliderposition_main_main_default.set_pad_top(8)
style_screen_sliderposition_main_main_default.set_pad_bottom(0)

# add style for screen_SliderPosition
screen_SliderPosition.add_style(style_screen_sliderposition_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_StationsView_StationTileview3 = screen_StationsView.add_tile(2 , 0, lv.DIR.LEFT | lv.DIR.RIGHT)

# create screen_TittleLabel3
screen_TittleLabel3 = lv.label(screen_StationsView_StationTileview3)
screen_TittleLabel3.set_pos(int(98),int(7))
screen_TittleLabel3.set_size(190,47)
screen_TittleLabel3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_TittleLabel3.set_text("CPU Performance")
screen_TittleLabel3.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_tittlelabel3_main_main_default
style_screen_tittlelabel3_main_main_default = lv.style_t()
style_screen_tittlelabel3_main_main_default.init()
style_screen_tittlelabel3_main_main_default.set_radius(0)
style_screen_tittlelabel3_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel3_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel3_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tittlelabel3_main_main_default.set_bg_opa(0)
style_screen_tittlelabel3_main_main_default.set_text_color(lv.color_make(0xe0,0xb7,0x4f))
try:
    style_screen_tittlelabel3_main_main_default.set_text_font(lv.font_montserratMedium_18)
except AttributeError:
    try:
        style_screen_tittlelabel3_main_main_default.set_text_font(lv.font_montserrat_18)
    except AttributeError:
        style_screen_tittlelabel3_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_tittlelabel3_main_main_default.set_text_letter_space(2)
style_screen_tittlelabel3_main_main_default.set_text_line_space(0)
style_screen_tittlelabel3_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_tittlelabel3_main_main_default.set_pad_left(0)
style_screen_tittlelabel3_main_main_default.set_pad_right(0)
style_screen_tittlelabel3_main_main_default.set_pad_top(8)
style_screen_tittlelabel3_main_main_default.set_pad_bottom(0)

# add style for screen_TittleLabel3
screen_TittleLabel3.add_style(style_screen_tittlelabel3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_LVGL
screen_LVGL = lv.arc(screen_StationsView_StationTileview3)
screen_LVGL.set_pos(int(0),int(53))
screen_LVGL.set_size(344,204)
screen_LVGL.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_LVGL.set_mode(lv.arc.MODE.NORMAL)
screen_LVGL.set_range(0, 100)
screen_LVGL.set_bg_angles(0, 0)
screen_LVGL.set_angles(180, 268)
screen_LVGL.set_rotation(0)
screen_LVGL.set_style_arc_rounded(False, lv.PART.INDICATOR | lv.STATE.DEFAULT)
screen_LVGL.set_style_arc_rounded(False, lv.PART.MAIN | lv.STATE.DEFAULT)
# create style style_screen_lvgl_main_main_default
style_screen_lvgl_main_main_default = lv.style_t()
style_screen_lvgl_main_main_default.init()
style_screen_lvgl_main_main_default.set_radius(6)
style_screen_lvgl_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_lvgl_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_lvgl_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_lvgl_main_main_default.set_bg_opa(0)
style_screen_lvgl_main_main_default.set_border_width(0)
style_screen_lvgl_main_main_default.set_pad_left(20)
style_screen_lvgl_main_main_default.set_pad_right(20)
style_screen_lvgl_main_main_default.set_pad_top(20)
style_screen_lvgl_main_main_default.set_pad_bottom(20)
style_screen_lvgl_main_main_default.set_arc_color(lv.color_make(0x4d,0x4d,0x4d))
style_screen_lvgl_main_main_default.set_arc_width(20)

# add style for screen_LVGL
screen_LVGL.add_style(style_screen_lvgl_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_lvgl_main_indicator_default
style_screen_lvgl_main_indicator_default = lv.style_t()
style_screen_lvgl_main_indicator_default.init()
style_screen_lvgl_main_indicator_default.set_arc_color(lv.color_make(0x62,0x8f,0xb5))
style_screen_lvgl_main_indicator_default.set_arc_width(10)

# add style for screen_LVGL
screen_LVGL.add_style(style_screen_lvgl_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_lvgl_main_knob_default
style_screen_lvgl_main_knob_default = lv.style_t()
style_screen_lvgl_main_knob_default.init()
style_screen_lvgl_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvgl_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvgl_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_lvgl_main_knob_default.set_bg_opa(0)
style_screen_lvgl_main_knob_default.set_pad_all(20)

# add style for screen_LVGL
screen_LVGL.add_style(style_screen_lvgl_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_HttpServer
screen_HttpServer = lv.arc(screen_StationsView_StationTileview3)
screen_HttpServer.set_pos(int(0),int(53))
screen_HttpServer.set_size(344,204)
screen_HttpServer.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_HttpServer.set_mode(lv.arc.MODE.NORMAL)
screen_HttpServer.set_range(0, 100)
screen_HttpServer.set_bg_angles(0, 0)
screen_HttpServer.set_angles(270, 358)
screen_HttpServer.set_rotation(0)
screen_HttpServer.set_style_arc_rounded(False, lv.PART.INDICATOR | lv.STATE.DEFAULT)
screen_HttpServer.set_style_arc_rounded(False, lv.PART.MAIN | lv.STATE.DEFAULT)
# create style style_screen_httpserver_main_main_default
style_screen_httpserver_main_main_default = lv.style_t()
style_screen_httpserver_main_main_default.init()
style_screen_httpserver_main_main_default.set_radius(6)
style_screen_httpserver_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_httpserver_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_httpserver_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_httpserver_main_main_default.set_bg_opa(0)
style_screen_httpserver_main_main_default.set_border_width(0)
style_screen_httpserver_main_main_default.set_pad_left(20)
style_screen_httpserver_main_main_default.set_pad_right(20)
style_screen_httpserver_main_main_default.set_pad_top(20)
style_screen_httpserver_main_main_default.set_pad_bottom(20)
style_screen_httpserver_main_main_default.set_arc_color(lv.color_make(0x4d,0x4d,0x4d))
style_screen_httpserver_main_main_default.set_arc_width(20)

# add style for screen_HttpServer
screen_HttpServer.add_style(style_screen_httpserver_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_httpserver_main_indicator_default
style_screen_httpserver_main_indicator_default = lv.style_t()
style_screen_httpserver_main_indicator_default.init()
style_screen_httpserver_main_indicator_default.set_arc_color(lv.color_make(0x80,0xa2,0x47))
style_screen_httpserver_main_indicator_default.set_arc_width(15)

# add style for screen_HttpServer
screen_HttpServer.add_style(style_screen_httpserver_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_httpserver_main_knob_default
style_screen_httpserver_main_knob_default = lv.style_t()
style_screen_httpserver_main_knob_default.init()
style_screen_httpserver_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_httpserver_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_httpserver_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_httpserver_main_knob_default.set_bg_opa(0)
style_screen_httpserver_main_knob_default.set_pad_all(20)

# add style for screen_HttpServer
screen_HttpServer.add_style(style_screen_httpserver_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_Idle
screen_Idle = lv.arc(screen_StationsView_StationTileview3)
screen_Idle.set_pos(int(0),int(53))
screen_Idle.set_size(344,204)
screen_Idle.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Idle.set_mode(lv.arc.MODE.NORMAL)
screen_Idle.set_range(0, 100)
screen_Idle.set_bg_angles(0, 0)
screen_Idle.set_angles(90, 178)
screen_Idle.set_rotation(0)
screen_Idle.set_style_arc_rounded(False, lv.PART.INDICATOR | lv.STATE.DEFAULT)
screen_Idle.set_style_arc_rounded(False, lv.PART.MAIN | lv.STATE.DEFAULT)
# create style style_screen_idle_main_main_default
style_screen_idle_main_main_default = lv.style_t()
style_screen_idle_main_main_default.init()
style_screen_idle_main_main_default.set_radius(6)
style_screen_idle_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_idle_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_idle_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_idle_main_main_default.set_bg_opa(0)
style_screen_idle_main_main_default.set_border_width(0)
style_screen_idle_main_main_default.set_pad_left(20)
style_screen_idle_main_main_default.set_pad_right(20)
style_screen_idle_main_main_default.set_pad_top(20)
style_screen_idle_main_main_default.set_pad_bottom(20)
style_screen_idle_main_main_default.set_arc_color(lv.color_make(0x4d,0x4d,0x4d))
style_screen_idle_main_main_default.set_arc_width(20)

# add style for screen_Idle
screen_Idle.add_style(style_screen_idle_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_idle_main_indicator_default
style_screen_idle_main_indicator_default = lv.style_t()
style_screen_idle_main_indicator_default.init()
style_screen_idle_main_indicator_default.set_arc_color(lv.color_make(0xE0,0xB7,0x4F))
style_screen_idle_main_indicator_default.set_arc_width(20)

# add style for screen_Idle
screen_Idle.add_style(style_screen_idle_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_idle_main_knob_default
style_screen_idle_main_knob_default = lv.style_t()
style_screen_idle_main_knob_default.init()
style_screen_idle_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idle_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idle_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_idle_main_knob_default.set_bg_opa(0)
style_screen_idle_main_knob_default.set_pad_all(20)

# add style for screen_Idle
screen_Idle.add_style(style_screen_idle_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_Other
screen_Other = lv.arc(screen_StationsView_StationTileview3)
screen_Other.set_pos(int(0),int(53))
screen_Other.set_size(344,204)
screen_Other.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Other.set_mode(lv.arc.MODE.NORMAL)
screen_Other.set_range(0, 100)
screen_Other.set_bg_angles(0, 0)
screen_Other.set_angles(0, 88)
screen_Other.set_rotation(0)
screen_Other.set_style_arc_rounded(False, lv.PART.INDICATOR | lv.STATE.DEFAULT)
screen_Other.set_style_arc_rounded(False, lv.PART.MAIN | lv.STATE.DEFAULT)
# create style style_screen_other_main_main_default
style_screen_other_main_main_default = lv.style_t()
style_screen_other_main_main_default.init()
style_screen_other_main_main_default.set_radius(6)
style_screen_other_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_other_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_other_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_other_main_main_default.set_bg_opa(0)
style_screen_other_main_main_default.set_border_width(0)
style_screen_other_main_main_default.set_pad_left(20)
style_screen_other_main_main_default.set_pad_right(20)
style_screen_other_main_main_default.set_pad_top(20)
style_screen_other_main_main_default.set_pad_bottom(20)
style_screen_other_main_main_default.set_arc_color(lv.color_make(0x4d,0x4d,0x4d))
style_screen_other_main_main_default.set_arc_width(20)

# add style for screen_Other
screen_Other.add_style(style_screen_other_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_other_main_indicator_default
style_screen_other_main_indicator_default = lv.style_t()
style_screen_other_main_indicator_default.init()
style_screen_other_main_indicator_default.set_arc_color(lv.color_make(0x64,0x64,0x64))
style_screen_other_main_indicator_default.set_arc_width(5)

# add style for screen_Other
screen_Other.add_style(style_screen_other_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_other_main_knob_default
style_screen_other_main_knob_default = lv.style_t()
style_screen_other_main_knob_default.init()
style_screen_other_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_other_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_other_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_other_main_knob_default.set_bg_opa(0)
style_screen_other_main_knob_default.set_pad_all(20)

# add style for screen_Other
screen_Other.add_style(style_screen_other_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_LVGLBullet
screen_LVGLBullet = lv.img(screen_StationsView_StationTileview3)
screen_LVGLBullet.set_pos(int(220),int(100))
screen_LVGLBullet.set_size(15,13)
screen_LVGLBullet.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_LVGLBullet.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-1985490499.png','rb') as f:
        screen_LVGLBullet_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-1985490499.png')
    sys.exit()

screen_LVGLBullet_img = lv.img_dsc_t({
  'data_size': len(screen_LVGLBullet_img_data),
  'header': {'always_zero': 0, 'w': 15, 'h': 13, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_LVGLBullet_img_data
})

screen_LVGLBullet.set_src(screen_LVGLBullet_img)
screen_LVGLBullet.set_pivot(50,50)
screen_LVGLBullet.set_angle(0)
# create style style_screen_lvglbullet_main_main_default
style_screen_lvglbullet_main_main_default = lv.style_t()
style_screen_lvglbullet_main_main_default.init()
style_screen_lvglbullet_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_lvglbullet_main_main_default.set_img_recolor_opa(0)
style_screen_lvglbullet_main_main_default.set_img_opa(255)

# add style for screen_LVGLBullet
screen_LVGLBullet.add_style(style_screen_lvglbullet_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_IdleBullet
screen_IdleBullet = lv.img(screen_StationsView_StationTileview3)
screen_IdleBullet.set_pos(int(220),int(190))
screen_IdleBullet.set_size(15,13)
screen_IdleBullet.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_IdleBullet.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-2126039109.png','rb') as f:
        screen_IdleBullet_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp-2126039109.png')
    sys.exit()

screen_IdleBullet_img = lv.img_dsc_t({
  'data_size': len(screen_IdleBullet_img_data),
  'header': {'always_zero': 0, 'w': 15, 'h': 13, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_IdleBullet_img_data
})

screen_IdleBullet.set_src(screen_IdleBullet_img)
screen_IdleBullet.set_pivot(50,50)
screen_IdleBullet.set_angle(0)
# create style style_screen_idlebullet_main_main_default
style_screen_idlebullet_main_main_default = lv.style_t()
style_screen_idlebullet_main_main_default.init()
style_screen_idlebullet_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_idlebullet_main_main_default.set_img_recolor_opa(0)
style_screen_idlebullet_main_main_default.set_img_opa(255)

# add style for screen_IdleBullet
screen_IdleBullet.add_style(style_screen_idlebullet_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_OtherBullet
screen_OtherBullet = lv.img(screen_StationsView_StationTileview3)
screen_OtherBullet.set_pos(int(220),int(160))
screen_OtherBullet.set_size(15,13)
screen_OtherBullet.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_OtherBullet.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp232267454.png','rb') as f:
        screen_OtherBullet_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp232267454.png')
    sys.exit()

screen_OtherBullet_img = lv.img_dsc_t({
  'data_size': len(screen_OtherBullet_img_data),
  'header': {'always_zero': 0, 'w': 15, 'h': 13, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_OtherBullet_img_data
})

screen_OtherBullet.set_src(screen_OtherBullet_img)
screen_OtherBullet.set_pivot(50,50)
screen_OtherBullet.set_angle(0)
# create style style_screen_otherbullet_main_main_default
style_screen_otherbullet_main_main_default = lv.style_t()
style_screen_otherbullet_main_main_default.init()
style_screen_otherbullet_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_otherbullet_main_main_default.set_img_recolor_opa(0)
style_screen_otherbullet_main_main_default.set_img_opa(255)

# add style for screen_OtherBullet
screen_OtherBullet.add_style(style_screen_otherbullet_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_HttpServerBullet
screen_HttpServerBullet = lv.img(screen_StationsView_StationTileview3)
screen_HttpServerBullet.set_pos(int(220),int(130))
screen_HttpServerBullet.set_size(15,13)
screen_HttpServerBullet.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_HttpServerBullet.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp91718844.png','rb') as f:
        screen_HttpServerBullet_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp91718844.png')
    sys.exit()

screen_HttpServerBullet_img = lv.img_dsc_t({
  'data_size': len(screen_HttpServerBullet_img_data),
  'header': {'always_zero': 0, 'w': 15, 'h': 13, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_HttpServerBullet_img_data
})

screen_HttpServerBullet.set_src(screen_HttpServerBullet_img)
screen_HttpServerBullet.set_pivot(50,50)
screen_HttpServerBullet.set_angle(0)
# create style style_screen_httpserverbullet_main_main_default
style_screen_httpserverbullet_main_main_default = lv.style_t()
style_screen_httpserverbullet_main_main_default.init()
style_screen_httpserverbullet_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_httpserverbullet_main_main_default.set_img_recolor_opa(0)
style_screen_httpserverbullet_main_main_default.set_img_opa(255)

# add style for screen_HttpServerBullet
screen_HttpServerBullet.add_style(style_screen_httpserverbullet_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_LVGLLabel
screen_LVGLLabel = lv.label(screen_StationsView_StationTileview3)
screen_LVGLLabel.set_pos(int(230),int(93))
screen_LVGLLabel.set_size(69,32)
screen_LVGLLabel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_LVGLLabel.set_text("LVGL:")
screen_LVGLLabel.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_lvgllabel_main_main_default
style_screen_lvgllabel_main_main_default = lv.style_t()
style_screen_lvgllabel_main_main_default.init()
style_screen_lvgllabel_main_main_default.set_radius(0)
style_screen_lvgllabel_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvgllabel_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvgllabel_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_lvgllabel_main_main_default.set_bg_opa(0)
style_screen_lvgllabel_main_main_default.set_text_color(lv.color_make(0xFF,0xFF,0xFF))
try:
    style_screen_lvgllabel_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_lvgllabel_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_lvgllabel_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_lvgllabel_main_main_default.set_text_letter_space(2)
style_screen_lvgllabel_main_main_default.set_text_line_space(0)
style_screen_lvgllabel_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_lvgllabel_main_main_default.set_pad_left(0)
style_screen_lvgllabel_main_main_default.set_pad_right(0)
style_screen_lvgllabel_main_main_default.set_pad_top(8)
style_screen_lvgllabel_main_main_default.set_pad_bottom(0)

# add style for screen_LVGLLabel
screen_LVGLLabel.add_style(style_screen_lvgllabel_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_HttpLabel
screen_HttpLabel = lv.label(screen_StationsView_StationTileview3)
screen_HttpLabel.set_pos(int(237),int(122))
screen_HttpLabel.set_size(64,32)
screen_HttpLabel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_HttpLabel.set_text("Server:")
screen_HttpLabel.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_httplabel_main_main_default
style_screen_httplabel_main_main_default = lv.style_t()
style_screen_httplabel_main_main_default.init()
style_screen_httplabel_main_main_default.set_radius(0)
style_screen_httplabel_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_httplabel_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_httplabel_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_httplabel_main_main_default.set_bg_opa(0)
style_screen_httplabel_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_httplabel_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_httplabel_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_httplabel_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_httplabel_main_main_default.set_text_letter_space(2)
style_screen_httplabel_main_main_default.set_text_line_space(0)
style_screen_httplabel_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_httplabel_main_main_default.set_pad_left(0)
style_screen_httplabel_main_main_default.set_pad_right(0)
style_screen_httplabel_main_main_default.set_pad_top(8)
style_screen_httplabel_main_main_default.set_pad_bottom(0)

# add style for screen_HttpLabel
screen_HttpLabel.add_style(style_screen_httplabel_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_OtherLabel
screen_OtherLabel = lv.label(screen_StationsView_StationTileview3)
screen_OtherLabel.set_pos(int(237),int(152))
screen_OtherLabel.set_size(62,32)
screen_OtherLabel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_OtherLabel.set_text("Other:")
screen_OtherLabel.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_otherlabel_main_main_default
style_screen_otherlabel_main_main_default = lv.style_t()
style_screen_otherlabel_main_main_default.init()
style_screen_otherlabel_main_main_default.set_radius(0)
style_screen_otherlabel_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_otherlabel_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_otherlabel_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_otherlabel_main_main_default.set_bg_opa(0)
style_screen_otherlabel_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_otherlabel_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_otherlabel_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_otherlabel_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_otherlabel_main_main_default.set_text_letter_space(2)
style_screen_otherlabel_main_main_default.set_text_line_space(0)
style_screen_otherlabel_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_otherlabel_main_main_default.set_pad_left(0)
style_screen_otherlabel_main_main_default.set_pad_right(0)
style_screen_otherlabel_main_main_default.set_pad_top(8)
style_screen_otherlabel_main_main_default.set_pad_bottom(0)

# add style for screen_OtherLabel
screen_OtherLabel.add_style(style_screen_otherlabel_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_IdleLabel
screen_IdleLabel = lv.label(screen_StationsView_StationTileview3)
screen_IdleLabel.set_pos(int(235),int(182))
screen_IdleLabel.set_size(48,32)
screen_IdleLabel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_IdleLabel.set_text("Idle:")
screen_IdleLabel.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_idlelabel_main_main_default
style_screen_idlelabel_main_main_default = lv.style_t()
style_screen_idlelabel_main_main_default.init()
style_screen_idlelabel_main_main_default.set_radius(0)
style_screen_idlelabel_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idlelabel_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idlelabel_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_idlelabel_main_main_default.set_bg_opa(0)
style_screen_idlelabel_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_idlelabel_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_idlelabel_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_idlelabel_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_idlelabel_main_main_default.set_text_letter_space(2)
style_screen_idlelabel_main_main_default.set_text_line_space(0)
style_screen_idlelabel_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_idlelabel_main_main_default.set_pad_left(0)
style_screen_idlelabel_main_main_default.set_pad_right(0)
style_screen_idlelabel_main_main_default.set_pad_top(8)
style_screen_idlelabel_main_main_default.set_pad_bottom(0)

# add style for screen_IdleLabel
screen_IdleLabel.add_style(style_screen_idlelabel_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_LVGLPercentage
screen_LVGLPercentage = lv.label(screen_StationsView_StationTileview3)
screen_LVGLPercentage.set_pos(int(290),int(92))
screen_LVGLPercentage.set_size(39,32)
screen_LVGLPercentage.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_LVGLPercentage.set_text("27%")
screen_LVGLPercentage.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_lvglpercentage_main_main_default
style_screen_lvglpercentage_main_main_default = lv.style_t()
style_screen_lvglpercentage_main_main_default.init()
style_screen_lvglpercentage_main_main_default.set_radius(0)
style_screen_lvglpercentage_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvglpercentage_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_lvglpercentage_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_lvglpercentage_main_main_default.set_bg_opa(0)
style_screen_lvglpercentage_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_lvglpercentage_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_lvglpercentage_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_lvglpercentage_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_lvglpercentage_main_main_default.set_text_letter_space(2)
style_screen_lvglpercentage_main_main_default.set_text_line_space(0)
style_screen_lvglpercentage_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_lvglpercentage_main_main_default.set_pad_left(0)
style_screen_lvglpercentage_main_main_default.set_pad_right(0)
style_screen_lvglpercentage_main_main_default.set_pad_top(8)
style_screen_lvglpercentage_main_main_default.set_pad_bottom(0)

# add style for screen_LVGLPercentage
screen_LVGLPercentage.add_style(style_screen_lvglpercentage_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_ServerPercentage
screen_ServerPercentage = lv.label(screen_StationsView_StationTileview3)
screen_ServerPercentage.set_pos(int(301),int(124))
screen_ServerPercentage.set_size(39,32)
screen_ServerPercentage.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_ServerPercentage.set_text("35%")
screen_ServerPercentage.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_serverpercentage_main_main_default
style_screen_serverpercentage_main_main_default = lv.style_t()
style_screen_serverpercentage_main_main_default.init()
style_screen_serverpercentage_main_main_default.set_radius(0)
style_screen_serverpercentage_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_serverpercentage_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_serverpercentage_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_serverpercentage_main_main_default.set_bg_opa(0)
style_screen_serverpercentage_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_serverpercentage_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_serverpercentage_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_serverpercentage_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_serverpercentage_main_main_default.set_text_letter_space(2)
style_screen_serverpercentage_main_main_default.set_text_line_space(0)
style_screen_serverpercentage_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_serverpercentage_main_main_default.set_pad_left(0)
style_screen_serverpercentage_main_main_default.set_pad_right(0)
style_screen_serverpercentage_main_main_default.set_pad_top(8)
style_screen_serverpercentage_main_main_default.set_pad_bottom(0)

# add style for screen_ServerPercentage
screen_ServerPercentage.add_style(style_screen_serverpercentage_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_OtherPercentage
screen_OtherPercentage = lv.label(screen_StationsView_StationTileview3)
screen_OtherPercentage.set_pos(int(297),int(152))
screen_OtherPercentage.set_size(39,32)
screen_OtherPercentage.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_OtherPercentage.set_text("16%")
screen_OtherPercentage.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_otherpercentage_main_main_default
style_screen_otherpercentage_main_main_default = lv.style_t()
style_screen_otherpercentage_main_main_default.init()
style_screen_otherpercentage_main_main_default.set_radius(0)
style_screen_otherpercentage_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_otherpercentage_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_otherpercentage_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_otherpercentage_main_main_default.set_bg_opa(0)
style_screen_otherpercentage_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_otherpercentage_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_otherpercentage_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_otherpercentage_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_otherpercentage_main_main_default.set_text_letter_space(2)
style_screen_otherpercentage_main_main_default.set_text_line_space(0)
style_screen_otherpercentage_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_otherpercentage_main_main_default.set_pad_left(0)
style_screen_otherpercentage_main_main_default.set_pad_right(0)
style_screen_otherpercentage_main_main_default.set_pad_top(8)
style_screen_otherpercentage_main_main_default.set_pad_bottom(0)

# add style for screen_OtherPercentage
screen_OtherPercentage.add_style(style_screen_otherpercentage_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_IdlePercentage
screen_IdlePercentage = lv.label(screen_StationsView_StationTileview3)
screen_IdlePercentage.set_pos(int(279),int(182))
screen_IdlePercentage.set_size(39,32)
screen_IdlePercentage.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_IdlePercentage.set_text("22%")
screen_IdlePercentage.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_idlepercentage_main_main_default
style_screen_idlepercentage_main_main_default = lv.style_t()
style_screen_idlepercentage_main_main_default.init()
style_screen_idlepercentage_main_main_default.set_radius(0)
style_screen_idlepercentage_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idlepercentage_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_idlepercentage_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_idlepercentage_main_main_default.set_bg_opa(0)
style_screen_idlepercentage_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_idlepercentage_main_main_default.set_text_font(lv.font_montserratMedium_14)
except AttributeError:
    try:
        style_screen_idlepercentage_main_main_default.set_text_font(lv.font_montserrat_14)
    except AttributeError:
        style_screen_idlepercentage_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_idlepercentage_main_main_default.set_text_letter_space(2)
style_screen_idlepercentage_main_main_default.set_text_line_space(0)
style_screen_idlepercentage_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_idlepercentage_main_main_default.set_pad_left(0)
style_screen_idlepercentage_main_main_default.set_pad_right(0)
style_screen_idlepercentage_main_main_default.set_pad_top(8)
style_screen_idlepercentage_main_main_default.set_pad_bottom(0)

# add style for screen_IdlePercentage
screen_IdlePercentage.add_style(style_screen_idlepercentage_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_StationsView_StationTileview2 = screen_StationsView.add_tile(1 , 0, lv.DIR.LEFT | lv.DIR.RIGHT)

# create screen_TempCelsius
screen_TempCelsius = lv.meter(screen_StationsView_StationTileview2)
screen_TempCelsius.set_pos(int(8),int(62))
screen_TempCelsius.set_size(166,166)
screen_TempCelsius.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_TempCelsius_scale_1 = screen_TempCelsius.add_scale()
screen_TempCelsius.set_scale_ticks(screen_TempCelsius_scale_1, 41, 10, 10,
    lv.color_make(0x4a, 0x44, 0x36))
screen_TempCelsius.set_scale_major_ticks(screen_TempCelsius_scale_1, 4, 3, 11,
	lv.color_make(0xe4, 0xbd, 0x49), 11)
screen_TempCelsius.set_scale_range(screen_TempCelsius_scale_1, 0, 100, 300, 90)
screen_TempCelsius_scale_1_needleLine_0 = screen_TempCelsius.add_needle_line(screen_TempCelsius_scale_1, 5,
    lv.color_make(0xE0, 0xB7, 0x4F),
    -10)
screen_TempCelsius.set_indicator_value(screen_TempCelsius_scale_1_needleLine_0, 10)
# create style style_screen_tempcelsius_main_main_default
style_screen_tempcelsius_main_main_default = lv.style_t()
style_screen_tempcelsius_main_main_default.init()
style_screen_tempcelsius_main_main_default.set_radius(200)
style_screen_tempcelsius_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_tempcelsius_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_tempcelsius_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tempcelsius_main_main_default.set_bg_opa(0)
style_screen_tempcelsius_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tempcelsius_main_main_default.set_border_width(0)
style_screen_tempcelsius_main_main_default.set_border_opa(0)

# add style for screen_TempCelsius
screen_TempCelsius.add_style(style_screen_tempcelsius_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_tempcelsius_main_ticks_default
style_screen_tempcelsius_main_ticks_default = lv.style_t()
style_screen_tempcelsius_main_ticks_default.init()
style_screen_tempcelsius_main_ticks_default.set_text_color(lv.color_make(0xE0,0xB7,0x4F))
try:
    style_screen_tempcelsius_main_ticks_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_tempcelsius_main_ticks_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_tempcelsius_main_ticks_default.set_text_font(lv.font_montserrat_16)

# add style for screen_TempCelsius
screen_TempCelsius.add_style(style_screen_tempcelsius_main_ticks_default, lv.PART.TICKS|lv.STATE.DEFAULT)


# create screen_TittleLabel2
screen_TittleLabel2 = lv.label(screen_StationsView_StationTileview2)
screen_TittleLabel2.set_pos(int(80),int(2))
screen_TittleLabel2.set_size(230,51)
screen_TittleLabel2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_TittleLabel2.set_text("Temperature Sensors")
screen_TittleLabel2.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_tittlelabel2_main_main_default
style_screen_tittlelabel2_main_main_default = lv.style_t()
style_screen_tittlelabel2_main_main_default.init()
style_screen_tittlelabel2_main_main_default.set_radius(0)
style_screen_tittlelabel2_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel2_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tittlelabel2_main_main_default.set_bg_opa(0)
style_screen_tittlelabel2_main_main_default.set_text_color(lv.color_make(0xe0,0xb7,0x4f))
try:
    style_screen_tittlelabel2_main_main_default.set_text_font(lv.font_montserratMedium_18)
except AttributeError:
    try:
        style_screen_tittlelabel2_main_main_default.set_text_font(lv.font_montserrat_18)
    except AttributeError:
        style_screen_tittlelabel2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_tittlelabel2_main_main_default.set_text_letter_space(2)
style_screen_tittlelabel2_main_main_default.set_text_line_space(0)
style_screen_tittlelabel2_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_tittlelabel2_main_main_default.set_pad_left(0)
style_screen_tittlelabel2_main_main_default.set_pad_right(0)
style_screen_tittlelabel2_main_main_default.set_pad_top(8)
style_screen_tittlelabel2_main_main_default.set_pad_bottom(0)

# add style for screen_TittleLabel2
screen_TittleLabel2.add_style(style_screen_tittlelabel2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_DCelsius
screen_DCelsius = lv.label(screen_StationsView_StationTileview2)
screen_DCelsius.set_pos(int(91),int(195))
screen_DCelsius.set_size(100,32)
screen_DCelsius.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_DCelsius.set_text("10 °C")
screen_DCelsius.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_dcelsius_main_main_default
style_screen_dcelsius_main_main_default = lv.style_t()
style_screen_dcelsius_main_main_default.init()
style_screen_dcelsius_main_main_default.set_radius(0)
style_screen_dcelsius_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_dcelsius_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_dcelsius_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_dcelsius_main_main_default.set_bg_opa(0)
style_screen_dcelsius_main_main_default.set_text_color(lv.color_make(0x7E,0xAF,0xD7))
try:
    style_screen_dcelsius_main_main_default.set_text_font(lv.font_montserratMedium_18)
except AttributeError:
    try:
        style_screen_dcelsius_main_main_default.set_text_font(lv.font_montserrat_18)
    except AttributeError:
        style_screen_dcelsius_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_dcelsius_main_main_default.set_text_letter_space(2)
style_screen_dcelsius_main_main_default.set_text_line_space(0)
style_screen_dcelsius_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_dcelsius_main_main_default.set_pad_left(0)
style_screen_dcelsius_main_main_default.set_pad_right(0)
style_screen_dcelsius_main_main_default.set_pad_top(8)
style_screen_dcelsius_main_main_default.set_pad_bottom(0)

# add style for screen_DCelsius
screen_DCelsius.add_style(style_screen_dcelsius_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_meter_1
screen_meter_1 = lv.meter(screen_StationsView_StationTileview2)
screen_meter_1.set_pos(int(196),int(62))
screen_meter_1.set_size(166,166)
screen_meter_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_meter_1_scale_1 = screen_meter_1.add_scale()
screen_meter_1.set_scale_ticks(screen_meter_1_scale_1, 41, 10, 10,
    lv.color_make(0x4a, 0x44, 0x36))
screen_meter_1.set_scale_major_ticks(screen_meter_1_scale_1, 4, 3, 11,
	lv.color_make(0xe4, 0xbd, 0x49), 11)
screen_meter_1.set_scale_range(screen_meter_1_scale_1, 0, 200, 300, 90)
screen_meter_1_scale_1_needleLine_0 = screen_meter_1.add_needle_line(screen_meter_1_scale_1, 5,
    lv.color_make(0xE0, 0xB7, 0x4F),
    -10)
screen_meter_1.set_indicator_value(screen_meter_1_scale_1_needleLine_0, 50)
# create style style_screen_meter_1_main_main_default
style_screen_meter_1_main_main_default = lv.style_t()
style_screen_meter_1_main_main_default.init()
style_screen_meter_1_main_main_default.set_radius(200)
style_screen_meter_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_meter_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_meter_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_meter_1_main_main_default.set_bg_opa(0)
style_screen_meter_1_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_meter_1_main_main_default.set_border_width(0)
style_screen_meter_1_main_main_default.set_border_opa(0)

# add style for screen_meter_1
screen_meter_1.add_style(style_screen_meter_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_meter_1_main_ticks_default
style_screen_meter_1_main_ticks_default = lv.style_t()
style_screen_meter_1_main_ticks_default.init()
style_screen_meter_1_main_ticks_default.set_text_color(lv.color_make(0xE0,0xB7,0x4F))
try:
    style_screen_meter_1_main_ticks_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_meter_1_main_ticks_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_meter_1_main_ticks_default.set_text_font(lv.font_montserrat_16)

# add style for screen_meter_1
screen_meter_1.add_style(style_screen_meter_1_main_ticks_default, lv.PART.TICKS|lv.STATE.DEFAULT)


# create screen_DFahrenheit
screen_DFahrenheit = lv.label(screen_StationsView_StationTileview2)
screen_DFahrenheit.set_pos(int(279),int(195))
screen_DFahrenheit.set_size(100,32)
screen_DFahrenheit.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_DFahrenheit.set_text("50 °F")
screen_DFahrenheit.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_dfahrenheit_main_main_default
style_screen_dfahrenheit_main_main_default = lv.style_t()
style_screen_dfahrenheit_main_main_default.init()
style_screen_dfahrenheit_main_main_default.set_radius(0)
style_screen_dfahrenheit_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_dfahrenheit_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_dfahrenheit_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_dfahrenheit_main_main_default.set_bg_opa(0)
style_screen_dfahrenheit_main_main_default.set_text_color(lv.color_make(0x7E,0xAF,0xD7))
try:
    style_screen_dfahrenheit_main_main_default.set_text_font(lv.font_montserratMedium_18)
except AttributeError:
    try:
        style_screen_dfahrenheit_main_main_default.set_text_font(lv.font_montserrat_18)
    except AttributeError:
        style_screen_dfahrenheit_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_dfahrenheit_main_main_default.set_text_letter_space(2)
style_screen_dfahrenheit_main_main_default.set_text_line_space(0)
style_screen_dfahrenheit_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_dfahrenheit_main_main_default.set_pad_left(0)
style_screen_dfahrenheit_main_main_default.set_pad_right(0)
style_screen_dfahrenheit_main_main_default.set_pad_top(8)
style_screen_dfahrenheit_main_main_default.set_pad_bottom(0)

# add style for screen_DFahrenheit
screen_DFahrenheit.add_style(style_screen_dfahrenheit_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_StationsView_StationTileview1 = screen_StationsView.add_tile(0, 0, lv.DIR.RIGHT)

# create screen_TittleLabel1
screen_TittleLabel1 = lv.label(screen_StationsView_StationTileview1)
screen_TittleLabel1.set_pos(int(79),int(-2))
screen_TittleLabel1.set_size(230,51)
screen_TittleLabel1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_TittleLabel1.set_text("Touch Sensing Input")
screen_TittleLabel1.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_tittlelabel1_main_main_default
style_screen_tittlelabel1_main_main_default = lv.style_t()
style_screen_tittlelabel1_main_main_default.init()
style_screen_tittlelabel1_main_main_default.set_radius(0)
style_screen_tittlelabel1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tittlelabel1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tittlelabel1_main_main_default.set_bg_opa(0)
style_screen_tittlelabel1_main_main_default.set_text_color(lv.color_make(0xe0,0xb7,0x4f))
try:
    style_screen_tittlelabel1_main_main_default.set_text_font(lv.font_montserratMedium_19)
except AttributeError:
    try:
        style_screen_tittlelabel1_main_main_default.set_text_font(lv.font_montserrat_19)
    except AttributeError:
        style_screen_tittlelabel1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_tittlelabel1_main_main_default.set_text_letter_space(2)
style_screen_tittlelabel1_main_main_default.set_text_line_space(0)
style_screen_tittlelabel1_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_tittlelabel1_main_main_default.set_pad_left(0)
style_screen_tittlelabel1_main_main_default.set_pad_right(0)
style_screen_tittlelabel1_main_main_default.set_pad_top(8)
style_screen_tittlelabel1_main_main_default.set_pad_bottom(0)

# add style for screen_TittleLabel1
screen_TittleLabel1.add_style(style_screen_tittlelabel1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_TSISliderPos
screen_TSISliderPos = lv.arc(screen_StationsView_StationTileview1)
screen_TSISliderPos.set_pos(int(69),int(42))
screen_TSISliderPos.set_size(402,232)
screen_TSISliderPos.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_TSISliderPos.set_mode(lv.arc.MODE.NORMAL)
screen_TSISliderPos.set_range(0, 100)
screen_TSISliderPos.set_bg_angles(180, 360)
screen_TSISliderPos.set_angles(180, 270)
screen_TSISliderPos.set_rotation(0)
# create style style_screen_tsisliderpos_main_main_default
style_screen_tsisliderpos_main_main_default = lv.style_t()
style_screen_tsisliderpos_main_main_default.init()
style_screen_tsisliderpos_main_main_default.set_radius(6)
style_screen_tsisliderpos_main_main_default.set_bg_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_tsisliderpos_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_tsisliderpos_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tsisliderpos_main_main_default.set_bg_opa(0)
style_screen_tsisliderpos_main_main_default.set_border_width(0)
style_screen_tsisliderpos_main_main_default.set_pad_left(20)
style_screen_tsisliderpos_main_main_default.set_pad_right(20)
style_screen_tsisliderpos_main_main_default.set_pad_top(20)
style_screen_tsisliderpos_main_main_default.set_pad_bottom(20)
style_screen_tsisliderpos_main_main_default.set_arc_color(lv.color_make(0x5c,0x5c,0x5c))
style_screen_tsisliderpos_main_main_default.set_arc_width(20)

# add style for screen_TSISliderPos
screen_TSISliderPos.add_style(style_screen_tsisliderpos_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_tsisliderpos_main_indicator_default
style_screen_tsisliderpos_main_indicator_default = lv.style_t()
style_screen_tsisliderpos_main_indicator_default.init()
style_screen_tsisliderpos_main_indicator_default.set_arc_color(lv.color_make(0x62,0x8f,0xb5))
style_screen_tsisliderpos_main_indicator_default.set_arc_width(20)

# add style for screen_TSISliderPos
screen_TSISliderPos.add_style(style_screen_tsisliderpos_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# create style style_screen_tsisliderpos_main_knob_default
style_screen_tsisliderpos_main_knob_default = lv.style_t()
style_screen_tsisliderpos_main_knob_default.init()
style_screen_tsisliderpos_main_knob_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tsisliderpos_main_knob_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tsisliderpos_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_tsisliderpos_main_knob_default.set_bg_opa(0)
style_screen_tsisliderpos_main_knob_default.set_pad_all(20)

# add style for screen_TSISliderPos
screen_TSISliderPos.add_style(style_screen_tsisliderpos_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)


# create screen_Position
screen_Position = lv.label(screen_StationsView_StationTileview1)
screen_Position.set_pos(int(150),int(122))
screen_Position.set_size(76,32)
screen_Position.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Position.set_text("50%")
screen_Position.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_position_main_main_default
style_screen_position_main_main_default = lv.style_t()
style_screen_position_main_main_default.init()
style_screen_position_main_main_default.set_radius(0)
style_screen_position_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_position_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_position_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_position_main_main_default.set_bg_opa(0)
style_screen_position_main_main_default.set_text_color(lv.color_make(0x7E,0xAF,0xD7))
try:
    style_screen_position_main_main_default.set_text_font(lv.font_montserratMedium_25)
except AttributeError:
    try:
        style_screen_position_main_main_default.set_text_font(lv.font_montserrat_25)
    except AttributeError:
        style_screen_position_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_position_main_main_default.set_text_letter_space(2)
style_screen_position_main_main_default.set_text_line_space(0)
style_screen_position_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_position_main_main_default.set_pad_left(0)
style_screen_position_main_main_default.set_pad_right(0)
style_screen_position_main_main_default.set_pad_top(8)
style_screen_position_main_main_default.set_pad_bottom(0)

# add style for screen_Position
screen_Position.add_style(style_screen_position_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_Elect1
screen_Elect1 = lv.label(screen_StationsView_StationTileview1)
screen_Elect1.set_pos(int(35),int(180))
screen_Elect1.set_size(100,32)
screen_Elect1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Elect1.set_text("Electrode A")
screen_Elect1.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_elect1_main_main_default
style_screen_elect1_main_main_default = lv.style_t()
style_screen_elect1_main_main_default.init()
style_screen_elect1_main_main_default.set_radius(0)
style_screen_elect1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_elect1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_elect1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_elect1_main_main_default.set_bg_opa(0)
style_screen_elect1_main_main_default.set_text_color(lv.color_make(0xFF,0xFF,0xFF))
try:
    style_screen_elect1_main_main_default.set_text_font(lv.font_montserratMedium_13)
except AttributeError:
    try:
        style_screen_elect1_main_main_default.set_text_font(lv.font_montserrat_13)
    except AttributeError:
        style_screen_elect1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_elect1_main_main_default.set_text_letter_space(2)
style_screen_elect1_main_main_default.set_text_line_space(0)
style_screen_elect1_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_elect1_main_main_default.set_pad_left(0)
style_screen_elect1_main_main_default.set_pad_right(0)
style_screen_elect1_main_main_default.set_pad_top(8)
style_screen_elect1_main_main_default.set_pad_bottom(0)

# add style for screen_Elect1
screen_Elect1.add_style(style_screen_elect1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_Elect2
screen_Elect2 = lv.label(screen_StationsView_StationTileview1)
screen_Elect2.set_pos(int(243),int(179))
screen_Elect2.set_size(100,32)
screen_Elect2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_Elect2.set_text("Electrode B")
screen_Elect2.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_elect2_main_main_default
style_screen_elect2_main_main_default = lv.style_t()
style_screen_elect2_main_main_default.init()
style_screen_elect2_main_main_default.set_radius(0)
style_screen_elect2_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_elect2_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_elect2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_elect2_main_main_default.set_bg_opa(0)
style_screen_elect2_main_main_default.set_text_color(lv.color_make(0xFF,0xFF,0xFF))
try:
    style_screen_elect2_main_main_default.set_text_font(lv.font_montserratMedium_13)
except AttributeError:
    try:
        style_screen_elect2_main_main_default.set_text_font(lv.font_montserrat_13)
    except AttributeError:
        style_screen_elect2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_elect2_main_main_default.set_text_letter_space(2)
style_screen_elect2_main_main_default.set_text_line_space(0)
style_screen_elect2_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_elect2_main_main_default.set_pad_left(0)
style_screen_elect2_main_main_default.set_pad_right(0)
style_screen_elect2_main_main_default.set_pad_top(8)
style_screen_elect2_main_main_default.set_pad_bottom(0)

# add style for screen_Elect2
screen_Elect2.add_style(style_screen_elect2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_E1Value
screen_E1Value = lv.label(screen_StationsView_StationTileview1)
screen_E1Value.set_pos(int(32),int(207))
screen_E1Value.set_size(100,32)
screen_E1Value.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_E1Value.set_text("0")
screen_E1Value.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_e1value_main_main_default
style_screen_e1value_main_main_default = lv.style_t()
style_screen_e1value_main_main_default.init()
style_screen_e1value_main_main_default.set_radius(0)
style_screen_e1value_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_e1value_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_e1value_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_e1value_main_main_default.set_bg_opa(0)
style_screen_e1value_main_main_default.set_text_color(lv.color_make(0xFF,0xFF,0xFF))
try:
    style_screen_e1value_main_main_default.set_text_font(lv.font_montserratMedium_16)
except AttributeError:
    try:
        style_screen_e1value_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_e1value_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_e1value_main_main_default.set_text_letter_space(2)
style_screen_e1value_main_main_default.set_text_line_space(0)
style_screen_e1value_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_e1value_main_main_default.set_pad_left(0)
style_screen_e1value_main_main_default.set_pad_right(0)
style_screen_e1value_main_main_default.set_pad_top(8)
style_screen_e1value_main_main_default.set_pad_bottom(0)

# add style for screen_E1Value
screen_E1Value.add_style(style_screen_e1value_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_E2Value
screen_E2Value = lv.label(screen_StationsView_StationTileview1)
screen_E2Value.set_pos(int(245),int(209))
screen_E2Value.set_size(100,32)
screen_E2Value.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_E2Value.set_text("0")
screen_E2Value.set_long_mode(lv.label.LONG.WRAP)
# create style style_screen_e2value_main_main_default
style_screen_e2value_main_main_default = lv.style_t()
style_screen_e2value_main_main_default.init()
style_screen_e2value_main_main_default.set_radius(0)
style_screen_e2value_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_e2value_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_e2value_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_e2value_main_main_default.set_bg_opa(0)
style_screen_e2value_main_main_default.set_text_color(lv.color_make(0xFF,0xFF,0xFF))
try:
    style_screen_e2value_main_main_default.set_text_font(lv.font_montserratMedium_16)
except AttributeError:
    try:
        style_screen_e2value_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_e2value_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_e2value_main_main_default.set_text_letter_space(2)
style_screen_e2value_main_main_default.set_text_line_space(0)
style_screen_e2value_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_e2value_main_main_default.set_pad_left(0)
style_screen_e2value_main_main_default.set_pad_right(0)
style_screen_e2value_main_main_default.set_pad_top(8)
style_screen_e2value_main_main_default.set_pad_bottom(0)

# add style for screen_E2Value
screen_E2Value.add_style(style_screen_e2value_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationsview_main_main_default
style_screen_stationsview_main_main_default = lv.style_t()
style_screen_stationsview_main_main_default.init()
style_screen_stationsview_main_main_default.set_radius(0)
style_screen_stationsview_main_main_default.set_bg_color(lv.color_make(0x00,0x00,0x00))
style_screen_stationsview_main_main_default.set_bg_grad_color(lv.color_make(0xf6,0xf6,0xf6))
style_screen_stationsview_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_stationsview_main_main_default.set_bg_opa(0)

# add style for screen_StationsView
screen_StationsView.add_style(style_screen_stationsview_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_stationsview_main_scrollbar_default
style_screen_stationsview_main_scrollbar_default = lv.style_t()
style_screen_stationsview_main_scrollbar_default.init()
style_screen_stationsview_main_scrollbar_default.set_radius(0)
style_screen_stationsview_main_scrollbar_default.set_bg_color(lv.color_make(0xea,0xef,0xf3))
style_screen_stationsview_main_scrollbar_default.set_bg_opa(255)

# add style for screen_StationsView
screen_StationsView.add_style(style_screen_stationsview_main_scrollbar_default, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)


# create screen_EmergencyStop
screen_EmergencyStop = lv.img(screen)
screen_EmergencyStop.set_pos(int(126),int(510))
screen_EmergencyStop.set_size(534,224)
screen_EmergencyStop.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_EmergencyStop.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1908568471.png','rb') as f:
        screen_EmergencyStop_img_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\IndustrialPanelv1\\generated\\mPythonImages\\mp1908568471.png')
    sys.exit()

screen_EmergencyStop_img = lv.img_dsc_t({
  'data_size': len(screen_EmergencyStop_img_data),
  'header': {'always_zero': 0, 'w': 534, 'h': 224, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_EmergencyStop_img_data
})

screen_EmergencyStop.set_src(screen_EmergencyStop_img)
screen_EmergencyStop.set_pivot(50,50)
screen_EmergencyStop.set_angle(0)
# create style style_screen_emergencystop_main_main_default
style_screen_emergencystop_main_main_default = lv.style_t()
style_screen_emergencystop_main_main_default.init()
style_screen_emergencystop_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_emergencystop_main_main_default.set_img_recolor_opa(0)
style_screen_emergencystop_main_main_default.set_img_opa(255)

# add style for screen_EmergencyStop
screen_EmergencyStop.add_style(style_screen_emergencystop_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)



def screen_StationButton1_pressed_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_StationButton1.add_event_cb(lambda e: screen_StationButton1_pressed_1_event_cb(e,screen_StationsView), lv.EVENT.PRESSED, None)


def screen_StationButton2_pressed_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_StationButton2.add_event_cb(lambda e: screen_StationButton2_pressed_1_event_cb(e,screen_StationsView), lv.EVENT.PRESSED, None)


def screen_StationButton3_pressed_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_StationButton3.add_event_cb(lambda e: screen_StationButton3_pressed_1_event_cb(e,screen_StationsView), lv.EVENT.PRESSED, None)


def screen_StationButton4_pressed_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_StationButton4.add_event_cb(lambda e: screen_StationButton4_pressed_1_event_cb(e,screen_StationsView), lv.EVENT.PRESSED, None)


def screen_MinusButton_clicked_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_MinusButton.add_event_cb(lambda e: screen_MinusButton_clicked_1_event_cb(e,screen_StationsView), lv.EVENT.CLICKED, None)


def screen_SliderControl_value_changed_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_SliderControl.add_event_cb(lambda e: screen_SliderControl_value_changed_1_event_cb(e,screen_StationsView), lv.EVENT.VALUE_CHANGED, None)


def screen_PlusButton_clicked_1_event_cb(e,screen_StationsView):
    src = e.get_target()
    code = e.get_code()
screen_PlusButton.add_event_cb(lambda e: screen_PlusButton_clicked_1_event_cb(e,screen_StationsView), lv.EVENT.CLICKED, None)



# content from custom.py

# Load the default screen
lv.scr_load(screen)

while SDL.check():
    time.sleep_ms(5)
