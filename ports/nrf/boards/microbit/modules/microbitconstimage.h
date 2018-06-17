/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#ifndef __MICROPY_INCLUDED_MICROBIT_CONSTIMAGE_H__
#define __MICROPY_INCLUDED_MICROBIT_CONSTIMAGE_H__

typedef struct _image_tuple_12 {
    mp_obj_base_t base;
    size_t len;
    mp_rom_obj_t items[12];
} image_tuple_12_t;

typedef struct _image_tuple_8 {
    mp_obj_base_t base;
    size_t len;
    mp_rom_obj_t items[8];
} image_tuple_8_t;

extern const image_tuple_12_t microbit_const_image_all_clocks_tuple_obj;
extern const image_tuple_8_t microbit_const_image_all_arrows_tuple_obj;
extern const mp_obj_type_t microbit_const_image_type;
extern const struct _monochrome_5by5_t microbit_const_image_heart_obj;
extern const struct _monochrome_5by5_t microbit_const_image_heart_small_obj;
extern const struct _monochrome_5by5_t microbit_const_image_happy_obj;
extern const struct _monochrome_5by5_t microbit_const_image_smile_obj;
extern const struct _monochrome_5by5_t microbit_const_image_sad_obj;
extern const struct _monochrome_5by5_t microbit_const_image_confused_obj;
extern const struct _monochrome_5by5_t microbit_const_image_angry_obj;
extern const struct _monochrome_5by5_t microbit_const_image_asleep_obj;
extern const struct _monochrome_5by5_t microbit_const_image_surprised_obj;
extern const struct _monochrome_5by5_t microbit_const_image_silly_obj;
extern const struct _monochrome_5by5_t microbit_const_image_fabulous_obj;
extern const struct _monochrome_5by5_t microbit_const_image_meh_obj;
extern const struct _monochrome_5by5_t microbit_const_image_yes_obj;
extern const struct _monochrome_5by5_t microbit_const_image_no_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock12_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock1_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock2_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock3_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock4_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock5_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock6_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock7_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock8_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock9_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock10_obj;
extern const struct _monochrome_5by5_t microbit_const_image_clock11_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_n_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_ne_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_e_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_se_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_s_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_sw_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_w_obj;
extern const struct _monochrome_5by5_t microbit_const_image_arrow_nw_obj;
extern const struct _monochrome_5by5_t microbit_const_image_triangle_obj;
extern const struct _monochrome_5by5_t microbit_const_image_triangle_left_obj;
extern const struct _monochrome_5by5_t microbit_const_image_chessboard_obj;
extern const struct _monochrome_5by5_t microbit_const_image_diamond_obj;
extern const struct _monochrome_5by5_t microbit_const_image_diamond_small_obj;
extern const struct _monochrome_5by5_t microbit_const_image_square_obj;
extern const struct _monochrome_5by5_t microbit_const_image_square_small_obj;
extern const struct _monochrome_5by5_t microbit_const_image_rabbit;
extern const struct _monochrome_5by5_t microbit_const_image_cow;
extern const struct _monochrome_5by5_t microbit_const_image_music_crotchet_obj;
extern const struct _monochrome_5by5_t microbit_const_image_music_quaver_obj;
extern const struct _monochrome_5by5_t microbit_const_image_music_quavers_obj;
extern const struct _monochrome_5by5_t microbit_const_image_pitchfork_obj;
extern const struct _monochrome_5by5_t microbit_const_image_xmas_obj;
extern const struct _monochrome_5by5_t microbit_const_image_pacman_obj;
extern const struct _monochrome_5by5_t microbit_const_image_target_obj;
extern const struct _monochrome_5by5_t microbit_const_image_tshirt_obj;
extern const struct _monochrome_5by5_t microbit_const_image_rollerskate_obj;
extern const struct _monochrome_5by5_t microbit_const_image_duck_obj;
extern const struct _monochrome_5by5_t microbit_const_image_house_obj;
extern const struct _monochrome_5by5_t microbit_const_image_tortoise_obj;
extern const struct _monochrome_5by5_t microbit_const_image_butterfly_obj;
extern const struct _monochrome_5by5_t microbit_const_image_stickfigure_obj;
extern const struct _monochrome_5by5_t microbit_const_image_ghost_obj;
extern const struct _monochrome_5by5_t microbit_const_image_sword_obj;
extern const struct _monochrome_5by5_t microbit_const_image_giraffe_obj;
extern const struct _monochrome_5by5_t microbit_const_image_skull_obj;
extern const struct _monochrome_5by5_t microbit_const_image_umbrella_obj;
extern const struct _monochrome_5by5_t microbit_const_image_snake_obj;

#endif // __MICROPY_INCLUDED_MICROBIT_CONSTIMAGE_H__
