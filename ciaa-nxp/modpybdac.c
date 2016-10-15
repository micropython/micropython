
/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
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
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_dac_obj_t {
    mp_obj_base_t base;
} pyb_dac_obj_t;

STATIC const pyb_dac_obj_t pyb_dac_obj[] = {
    {{&pyb_dac_type}},
};


void pyb_dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "DAC 1");
}

STATIC mp_obj_t pyb_dac_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_int_t dac_id = mp_obj_get_int(args[0]);

    if (dac_id!=1) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "DAC %d does not exist", dac_id));
    }
    return (mp_obj_t)&pyb_dac_obj[0];
}


mp_obj_t pyb_dac_write(mp_obj_t self_in, mp_obj_t valObj) {
    uint16_t value = (uint16_t) mp_obj_get_int(valObj);
    int32_t r = mp_hal_writeDAC(value);
    if(r==0)
        return mp_const_true;
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_write_obj, pyb_dac_write);



const uint16_t NOISE_TABLE[256] ={ 892,351,115,611,678,365,86,777,403,722,735,1005,501,134,751,694,955,647,472,375,100,1008,61,821,418,587,927,110,632,639,727,212,318,552,613,386,302,818,405,881,986,432,1016,205,198,737,955,260,984,801,729,151,768,379,422,786,41,419,748,844,659,560,321,647,797,835,752,23,760,166,400,625,344,693,993,129,611,159,765,653,991,660,299,576,1019,427,835,705,921,779,965,776,646,865,983,125,228,770,689,366,343,935,527,711,671,278,339,690,992,323,484,435,580,333,421,388,759,630,568,400,268,144,728,470,65,828,311,25,630,488,355,769,7,597,244,397,703,985,523,875,758,279,555,25,206,971,401,421,345,982,819,402,780,532,465,279,989,85,249,829,776,346,878,582,850,685,910,130,767,445,647,485,378,381,919,690,708,230,730,998,829,448,965,471,417,569,535,457,588,431,652,510,467,229,376,948,41,358,865,150,987,508,494,593,975,961,694,48,643,813,508,626,993,316,272,230,857,747,94,38,315,484,890,94,172,279,342,627,465,449,355,32,483,847,365,216,425,912,814,500,665,393,136,94,55,574,845,10,329,516,282,122,231,610,155,263 };
const uint16_t TRIANGLE_TABLE[256] = {0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,256,264,272,280,288,296,304,312,320,328,336,344,352,360,368,376,384,392,400,408,416,424,432,440,448,456,464,472,480,488,496,504,512,520,528,536,544,552,560,568,576,584,592,600,608,616,624,632,640,648,656,664,672,680,688,696,704,712,720,728,736,744,752,760,768,776,784,792,800,808,816,824,832,840,848,856,864,872,880,888,896,904,912,920,928,936,944,952,960,968,976,984,992,1000,1008,1016,1023,1016,1008,1000,992,984,976,968,960,952,944,936,928,920,912,904,896,888,880,872,864,856,848,840,832,824,816,808,800,792,784,776,768,760,752,744,736,728,720,712,704,696,688,680,672,664,656,648,640,632,624,616,608,600,592,584,576,568,560,552,544,536,528,520,512,504,496,488,480,472,464,456,448,440,432,424,416,408,400,392,384,376,368,360,352,344,336,328,320,312,304,296,288,280,272,264,256,248,240,232,224,216,208,200,192,184,176,168,160,152,144,136,128,120,112,104,96,88,80,72,64,56,48,40,32,24,16,8};

mp_obj_t pyb_dac_noise(mp_obj_t self_in, mp_obj_t valFreq) {
    uint32_t freq = (uint32_t) mp_obj_get_int(valFreq);

    mp_hal_setSampleRateDAC(freq);
    mp_hal_writeDMADAC((uint16_t*)NOISE_TABLE, 256, 1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_noise_obj, pyb_dac_noise);


mp_obj_t pyb_dac_triangle(mp_obj_t self_in, mp_obj_t valFreq) {
    uint32_t freq = (uint32_t) mp_obj_get_int(valFreq);

    mp_hal_setSampleRateDAC(freq);
    mp_hal_writeDMADAC((uint16_t*)TRIANGLE_TABLE, 256, 1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_triangle_obj, pyb_dac_triangle);




STATIC const mp_arg_t pyb_dac_write_timed_args[] = {
    { MP_QSTR_data,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_freq,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};
#define PYB_DAC_WRITE_TIMED_NUM_ARGS MP_ARRAY_SIZE(pyb_dac_write_timed_args)

STATIC mp_obj_t pyb_dac_write_timed_helper(const pyb_dac_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    // parse args
    mp_arg_val_t vals[PYB_DAC_WRITE_TIMED_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_DAC_WRITE_TIMED_NUM_ARGS, pyb_dac_write_timed_args, vals);

    // Data
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(vals[0].u_obj, &bufinfo, MP_BUFFER_READ);

    // freq
    uint32_t freq = vals[1].u_int;

    // mode
    uint8_t mode = vals[2].u_int;

    mp_hal_setSampleRateDAC(freq);
    int32_t r = mp_hal_writeDMADAC((uint16_t*)bufinfo.buf, bufinfo.len/2, mode);

    return MP_OBJ_NEW_SMALL_INT(r);
}

STATIC mp_obj_t pyb_dac_write_timed(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_dac_write_timed_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_dac_write_timed_obj, 1, pyb_dac_write_timed);



















STATIC const mp_map_elem_t pyb_dac_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&pyb_dac_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_noise), (mp_obj_t)&pyb_dac_noise_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_triangle), (mp_obj_t)&pyb_dac_triangle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_timed), (mp_obj_t)&pyb_dac_write_timed_obj },


    // Class Constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_NORMAL),         MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CIRCULAR),        MP_OBJ_NEW_SMALL_INT(1) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_dac_locals_dict, pyb_dac_locals_dict_table);

const mp_obj_type_t pyb_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .print = pyb_dac_print,
    .make_new = pyb_dac_make_new,
    .locals_dict = (mp_obj_t)&pyb_dac_locals_dict,
};
