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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "microbitdisplay.h"
#include "microbitimage.h"
#include "softpwm.h"
#include "ticker.h"
extern uint32_t ticks;

STATIC mp_obj_t microbit_reset_(void) {
    NVIC_SystemReset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(microbit_reset_obj, microbit_reset_);

STATIC mp_obj_t microbit_sleep(mp_obj_t ms_in) {
    mp_int_t ms;
    if (mp_obj_is_integer(ms_in)) {
        ms = mp_obj_get_int(ms_in);
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    } else {
        ms = (mp_int_t)mp_obj_get_float(ms_in);
#endif
    }
    if (ms > 0) {
        mp_hal_delay_ms(ms);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_sleep_obj, microbit_sleep);

STATIC mp_obj_t microbit_running_time(void) {
    return MP_OBJ_NEW_SMALL_INT(ticks);
}
MP_DEFINE_CONST_FUN_OBJ_0(microbit_running_time_obj, microbit_running_time);

static const monochrome_5by5_t panic = SMALL_IMAGE(
    1,1,0,1,1,
    1,1,0,1,1,
    0,0,0,0,0,
    0,1,1,1,0,
    1,0,0,0,1
);

STATIC mp_obj_t microbit_panic(mp_uint_t n_args, const mp_obj_t *args) {
    while(true) {
        microbit_display_show(&microbit_display_obj, (microbit_image_obj_t*)&panic);
        mp_hal_delay_ms(1000);
        char num[4];
        int code;
        if (n_args) {
            code = mp_obj_get_int(args[0]);
        } else {
            code = 0;
        }
        num[2] = code%10 + '0';
        code /= 10;
        num[1] = code%10 + '0';
        code /= 10;
        num[0] = code%10 + '0';
        for (int i = 0; i < 3; i++) {
            microbit_display_show(&microbit_display_obj, microbit_image_for_char(num[i]));
            mp_hal_delay_ms(1000);
        }
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(microbit_panic_obj, 0, 1, microbit_panic);

STATIC mp_obj_t microbit_temperature(void) {
    int temp;
    NRF_TEMP->TASKS_START = 1;
    while (NRF_TEMP->EVENTS_DATARDY == 0);
    NRF_TEMP->EVENTS_DATARDY = 0;
    temp = NRF_TEMP->TEMP;
    NRF_TEMP->TASKS_STOP = 1;
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    return mp_obj_new_float(temp/4.0);
#else
    return mp_obj_new_int(temp/4);
#endif
}
MP_DEFINE_CONST_FUN_OBJ_0(microbit_temperature_obj, microbit_temperature);

static mp_obj_t microbit_module_init(void) {
    softpwm_init();
    ticker_init(microbit_display_tick);
    ticker_start();
    pwm_start();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(microbit_module___init___obj, microbit_module_init);

STATIC const mp_rom_map_elem_t microbit_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&microbit_module___init___obj) },

    { MP_ROM_QSTR(MP_QSTR_Image), MP_ROM_PTR(&microbit_image_type) },

    { MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&microbit_display_obj) },
/*    { MP_OBJ_NEW_QSTR(MP_QSTR_button_a), (mp_obj_t)&microbit_button_a_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_button_b), (mp_obj_t)&microbit_button_b_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accelerometer), (mp_obj_t)&microbit_accelerometer_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_compass), (mp_obj_t)&microbit_compass_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_i2c), (mp_obj_t)&microbit_i2c_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_uart), (mp_obj_t)&microbit_uart_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_spi), (mp_obj_t)&microbit_spi_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_reset), (mp_obj_t)&microbit_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&microbit_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_running_time), (mp_obj_t)&microbit_running_time_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_panic), (mp_obj_t)&microbit_panic_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_temperature), (mp_obj_t)&microbit_temperature_obj },
    
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin0), (mp_obj_t)&microbit_p0_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin1), (mp_obj_t)&microbit_p1_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin2), (mp_obj_t)&microbit_p2_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin3), (mp_obj_t)&microbit_p3_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin4), (mp_obj_t)&microbit_p4_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin5), (mp_obj_t)&microbit_p5_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin6), (mp_obj_t)&microbit_p6_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin7), (mp_obj_t)&microbit_p7_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin8), (mp_obj_t)&microbit_p8_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin9), (mp_obj_t)&microbit_p9_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin10), (mp_obj_t)&microbit_p10_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin11), (mp_obj_t)&microbit_p11_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin12), (mp_obj_t)&microbit_p12_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin13), (mp_obj_t)&microbit_p13_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin14), (mp_obj_t)&microbit_p14_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin15), (mp_obj_t)&microbit_p15_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin16), (mp_obj_t)&microbit_p16_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin19), (mp_obj_t)&microbit_p19_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin20), (mp_obj_t)&microbit_p20_obj },
*/
};

STATIC MP_DEFINE_CONST_DICT(microbit_module_globals, microbit_module_globals_table);

const mp_obj_module_t microbit_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&microbit_module_globals,
};
