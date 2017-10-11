/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Mark Shannon
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

#include "py/runtime.h"
#include "microbitconstimage.h"

const mp_obj_tuple_t microbit_const_image_all_clocks_tuple_obj = {
    {&mp_type_tuple},
    .len = 12,
    .items = {
        (mp_obj_t)&microbit_const_image_clock12_obj,
        (mp_obj_t)&microbit_const_image_clock1_obj,
        (mp_obj_t)&microbit_const_image_clock2_obj,
        (mp_obj_t)&microbit_const_image_clock3_obj,
        (mp_obj_t)&microbit_const_image_clock4_obj,
        (mp_obj_t)&microbit_const_image_clock5_obj,
        (mp_obj_t)&microbit_const_image_clock6_obj,
        (mp_obj_t)&microbit_const_image_clock7_obj,
        (mp_obj_t)&microbit_const_image_clock8_obj,
        (mp_obj_t)&microbit_const_image_clock9_obj,
        (mp_obj_t)&microbit_const_image_clock10_obj,
        (mp_obj_t)&microbit_const_image_clock11_obj
    }
};

const mp_obj_tuple_t microbit_const_image_all_arrows_tuple_obj = {
    {&mp_type_tuple},
    .len = 8,
    .items = {
        (mp_obj_t)&microbit_const_image_arrow_n_obj,
        (mp_obj_t)&microbit_const_image_arrow_ne_obj,
        (mp_obj_t)&microbit_const_image_arrow_e_obj,
        (mp_obj_t)&microbit_const_image_arrow_se_obj,
        (mp_obj_t)&microbit_const_image_arrow_s_obj,
        (mp_obj_t)&microbit_const_image_arrow_sw_obj,
        (mp_obj_t)&microbit_const_image_arrow_w_obj,
        (mp_obj_t)&microbit_const_image_arrow_nw_obj
    }
};
