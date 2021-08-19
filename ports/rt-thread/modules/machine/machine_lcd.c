/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 SummerGift <SummerGift@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#if MICROPY_PY_MACHINE_LCD
#include <dfs_posix.h>
#include "machine_lcd.h"
#include <drv_lcd.h>

#define MAX_CO (2400 - 1)

typedef struct _machine_lcd_obj_t {
    mp_obj_base_t base;
} machine_lcd_obj_t;

STATIC void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, msg));
    }
}

/// \classmethod \constructor(skin_position)
///
/// Construct an LCD object.  
STATIC mp_obj_t machine_lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    
    // create lcd object
    machine_lcd_obj_t *lcd = m_new_obj(machine_lcd_obj_t);
    lcd->base.type = &machine_lcd_type;

    return MP_OBJ_FROM_PTR(lcd);
}

/// \method light(value)
///
/// Turn the backlight on/off.  True or 1 turns it on, False or 0 turns it off.
STATIC mp_obj_t machine_lcd_light(mp_obj_t self_in, mp_obj_t value) {
    if (mp_obj_is_true(value)) {
        lcd_display_on(); // set pin high to turn backlight on
    } else {
        lcd_display_off();// set pin low to turn backlight off
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_lcd_light_obj, machine_lcd_light);

/// \method fill(colour)
///
/// Fill the screen with the given colour.
///
STATIC mp_obj_t machine_lcd_fill(mp_obj_t self_in, mp_obj_t col_in) {
    int col = mp_obj_get_int(col_in);
    lcd_clear(col);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_lcd_fill_obj, machine_lcd_fill);

/// \method pixel(x, y, colour)
///
/// Set the pixel at `(x, y)` to the given colour.
///
STATIC mp_obj_t machine_lcd_pixel(size_t n_args, const mp_obj_t *args) {
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    
    error_check((x >= 0 && x <= MAX_CO) && (y >= 0 && y <= MAX_CO) , "The min/max X/Y coordinates is 0/239");

    int col = mp_obj_get_int(args[3]);
    lcd_draw_point_color(x, y, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_pixel_obj, 4, 4, machine_lcd_pixel);

/// \method text(str, x, y, size)
///
/// Draw the given text to the position `(x, y)` using the given size (16 24 32).
///
STATIC mp_obj_t machine_lcd_text(size_t n_args, const mp_obj_t *args) {
    size_t len;
    const char *data = mp_obj_str_get_data(args[1], &len);
    int x = mp_obj_get_int(args[2]);
    int y = mp_obj_get_int(args[3]);
    int size = mp_obj_get_int(args[4]);
    
    error_check((x >= 0 && x <= MAX_CO) && (y >= 0 && y <= MAX_CO) , "The min/max X/Y coordinates is 0/239");
    
    error_check(size == 16 || size == 24 || size == 32, "lcd only support font size 16 24 32");
    
    lcd_show_string(x, y, size, data);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_text_obj, 5, 5, machine_lcd_text);

/// \method line(x1, y1, x2, y2)
///
/// display a line on the lcd, from (x1, y1) to (x2, y2).
///
STATIC mp_obj_t machine_lcd_line(size_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[1]);
    int y1 = mp_obj_get_int(args[2]);
    int x2 = mp_obj_get_int(args[3]);
    int y2 = mp_obj_get_int(args[4]);
    
    error_check((x1 >= 0 && x1 <= MAX_CO) && (y1 >= 0 && y1 <= MAX_CO) , "The min/max X/Y coordinates is 0/239");
    error_check((x2 >= 0 && x2 <= MAX_CO) && (y2 >= 0 && y2 <= MAX_CO) , "The min/max X/Y coordinates is 0/239");

    lcd_draw_line(x1, y1, x2, y2);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_line_obj, 5, 5, machine_lcd_line);

/// \method rectangle(x1, y1, x2, y2)
///
/// display a rectangle on the lcd, from (x1, y1) to (x2, y2).
///
STATIC mp_obj_t machine_lcd_rectangle(size_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[1]);
    int y1 = mp_obj_get_int(args[2]);
    int x2 = mp_obj_get_int(args[3]);
    int y2 = mp_obj_get_int(args[4]);

    error_check((x1 >= 0 && x1 <= MAX_CO) && (y1 >= 0 && y1 <= MAX_CO) , "The min/max X/Y coordinates is 0/239");
    error_check((x2 >= 0 && x2 <= MAX_CO) && (y2 >= 0 && y2 <= MAX_CO) , "The min/max X/Y coordinates is 0/239");

    lcd_draw_rectangle(x1, y1, x2, y2);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_rectangle_obj, 5, 5, machine_lcd_rectangle);

/// \method circle(x1, y1, r)
///
/// display a circle on the lcd, center(x1, y1) R = r.
///
STATIC mp_obj_t machine_lcd_circle(size_t n_args, const mp_obj_t *args) {
    int x1 = mp_obj_get_int(args[1]);
    int y1 = mp_obj_get_int(args[2]);
    int r  = mp_obj_get_int(args[3]);

    error_check((x1 >= 0 && x1 <= MAX_CO) && (y1 >= 0 && y1 <= MAX_CO) , "The min/max X/Y coordinates is 0/239");

    lcd_draw_circle(x1, y1, r);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_circle_obj, 4, 4, machine_lcd_circle);

/// \method set_color(back, fore)
///
/// Set background color and foreground color.
///
STATIC mp_obj_t machine_lcd_set_color(size_t n_args, const mp_obj_t *args) {
    rt_uint16_t back = mp_obj_get_int(args[1]);
    rt_uint16_t fore = mp_obj_get_int(args[2]);

    lcd_set_color(back, fore);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_set_color_obj, 3, 3, machine_lcd_set_color);

/// \method show_image array
///
/// display the image on the lcd..
/// @param   x       x position
/// @param   y       y position
/// @param   length  length of image
/// @param   wide    wide of image
/// @param   p       image_array
STATIC mp_obj_t machine_lcd_show_image(size_t n_args, const mp_obj_t *args) {
    rt_uint16_t x = mp_obj_get_int(args[1]);
    rt_uint16_t y = mp_obj_get_int(args[2]);
    rt_uint16_t length = mp_obj_get_int(args[3]);
    rt_uint16_t wide = mp_obj_get_int(args[4]);

    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(args[5], &bufinfo, MP_BUFFER_READ)) 
    {
        lcd_show_image( x, y, length, wide, (const rt_uint8_t *)bufinfo.buf);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_show_image_obj, 6, 6, machine_lcd_show_image);

STATIC rt_uint16_t rgb888to565(rt_uint32_t RGB) 
{
     int R, G, B; 
     R = (RGB >> 19) & 0x1F; 
     G = (RGB >> 10) & 0x3F; 
     B = (RGB >> 3) & 0x1F; 
     return (R << 11) | (G << 5) | B; 
} 

/// \method show_image array
///
/// display the image on the lcd.
/// @param   x       x position
/// @param   y       y position
/// @param   file    bmp file pathname
STATIC mp_obj_t machine_lcd_show_bmp(size_t n_args, const mp_obj_t *args) {
    #define BMP_INFO_SIZE 54
    rt_uint16_t x = mp_obj_get_int(args[1]);
    rt_uint16_t y = mp_obj_get_int(args[2]);
    const char *pathname = mp_obj_str_get_str(args[3]);

    int fd, len;
    fd = open(pathname, O_RDONLY, 0);
    if (fd < 0)
    {
         mp_raise_OSError(MP_EINVAL);
    }

    void *bmp_info = rt_malloc(BMP_INFO_SIZE);
    if (bmp_info == RT_NULL)
    {
        mp_raise_OSError(MP_ENOMEM);
    }

    len = read(fd, bmp_info, BMP_INFO_SIZE);
    if (len < 0)
    {
        close(fd);
        mp_raise_OSError(MP_EINVAL);
    }

    rt_uint32_t width  = *(rt_uint32_t *)(bmp_info + 18);
    rt_uint32_t heigth = *(rt_uint32_t *)(bmp_info + 22);
    rt_uint16_t bit_count = *(rt_uint16_t *)(bmp_info + 28);
    
    if (bit_count != 32)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
              "bit count : %d, only support 32-bit bmp picture", bit_count));
    }

    void *image_buf = rt_malloc(2 * width);
    if (image_buf == RT_NULL)
    {
        mp_raise_OSError(MP_ENOMEM);
    }

    void *row_buf = rt_malloc(4 * width);
    if (row_buf == RT_NULL)
    {
        mp_raise_OSError(MP_ENOMEM);
    }

    int image_index, row_index;
    rt_uint16_t rgb565_temp;

    for(int i = 0; i < heigth; i++)
    {
        image_index = 0;
        row_index = 0;

        len = read(fd, row_buf, 4 * width);
        if (len < 0)
        {
            close(fd);
            mp_raise_OSError(MP_EINVAL);
        }

        while(row_index < (4 * width))
        {
            rgb565_temp = rgb888to565(*(rt_uint32_t *)(row_buf + row_index));
            *(rt_uint8_t *)(image_buf + image_index) = (rgb565_temp >> 8);
            *(rt_uint8_t *)(image_buf + image_index + 1) = rgb565_temp & 0xff;
            
            row_index   += 4;
            image_index += 2;
        }

        lcd_show_image( x, y--, width, 1, (const rt_uint8_t *)image_buf);
    }

    close(fd);
    rt_free(bmp_info);
    rt_free(image_buf);
    rt_free(row_buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lcd_show_bmp_obj, 4, 4, machine_lcd_show_bmp);

STATIC const mp_rom_map_elem_t machine_lcd_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_light), MP_ROM_PTR(&machine_lcd_light_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill),  MP_ROM_PTR(&machine_lcd_fill_obj)  },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&machine_lcd_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_text),  MP_ROM_PTR(&machine_lcd_text_obj)  },
    { MP_ROM_QSTR(MP_QSTR_line),  MP_ROM_PTR(&machine_lcd_line_obj)  },
    { MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&machine_lcd_rectangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&machine_lcd_circle_obj) }, 
    { MP_ROM_QSTR(MP_QSTR_set_color), MP_ROM_PTR(&machine_lcd_set_color_obj) }, 
    { MP_ROM_QSTR(MP_QSTR_show_image), MP_ROM_PTR(&machine_lcd_show_image_obj) }, 
    { MP_ROM_QSTR(MP_QSTR_show_bmp), MP_ROM_PTR(&machine_lcd_show_bmp_obj) }, 
    // color
    { MP_ROM_QSTR(MP_QSTR_WHITE), MP_ROM_INT(WHITE) },
    { MP_ROM_QSTR(MP_QSTR_BLACK), MP_ROM_INT(BLACK) },
    { MP_ROM_QSTR(MP_QSTR_BLUE), MP_ROM_INT(BLUE) },
    { MP_ROM_QSTR(MP_QSTR_BRED), MP_ROM_INT(BRED) },
    { MP_ROM_QSTR(MP_QSTR_GRED), MP_ROM_INT(GRED) },
    { MP_ROM_QSTR(MP_QSTR_GBLUE), MP_ROM_INT(GBLUE) },
    { MP_ROM_QSTR(MP_QSTR_RED), MP_ROM_INT(RED) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA), MP_ROM_INT(MAGENTA) },
    { MP_ROM_QSTR(MP_QSTR_GREEN), MP_ROM_INT(GREEN) },
    { MP_ROM_QSTR(MP_QSTR_CYAN), MP_ROM_INT(CYAN) },
    { MP_ROM_QSTR(MP_QSTR_YELLOW), MP_ROM_INT(YELLOW) },
    { MP_ROM_QSTR(MP_QSTR_BROWN), MP_ROM_INT(BROWN) },
    { MP_ROM_QSTR(MP_QSTR_BRRED), MP_ROM_INT(BRRED) },
    { MP_ROM_QSTR(MP_QSTR_GRAY), MP_ROM_INT(GRAY) },
    { MP_ROM_QSTR(MP_QSTR_GRAY175), MP_ROM_INT(GRAY175) },
    { MP_ROM_QSTR(MP_QSTR_GRAY151), MP_ROM_INT(GRAY151) },
    { MP_ROM_QSTR(MP_QSTR_GRAY187), MP_ROM_INT(GRAY187) },
    { MP_ROM_QSTR(MP_QSTR_GRAY240), MP_ROM_INT(GRAY240) },
};
STATIC MP_DEFINE_CONST_DICT(machine_lcd_locals_dict, machine_lcd_locals_dict_table);

const mp_obj_type_t machine_lcd_type = {
    { &mp_type_type },
    .name = MP_QSTR_LCD,
    .make_new = machine_lcd_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_lcd_locals_dict,
};

#endif // MICROPY_PY_MACHINE_LCD
