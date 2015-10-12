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

#include "py/runtime.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
} pyb_timer_obj_t;

STATIC const pyb_timer_obj_t pyb_timer_obj[] = {
    {{&pyb_timer_type}},
    {{&pyb_timer_type}},
    {{&pyb_timer_type}},
    {{&pyb_timer_type}}
};

#define NUM_TIMER MP_ARRAY_SIZE(pyb_timer_obj)
#define TIMER_ID(obj) ((obj) - &pyb_timer_obj[0])

void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;
    mp_printf(print, "TIMER(%u)", TIMER_ID(self));
}


STATIC void pyb_timer_init_helper(pyb_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int =-1} },
        { MP_QSTR_prescaler,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t freq = args[0].u_int;
    uint32_t presc = args[1].u_int;
    uint32_t period = args[2].u_int;

    mp_hal_stdout_tx_str("Init");
    char aux[100];
    sprintf(aux,"f:%d presc:0x%x period:%d",freq,presc,period);
    mp_hal_stdout_tx_str(aux);

    if(freq!=-1)
    {

    }

}


STATIC mp_obj_t pyb_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return  mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_init_obj, 1, pyb_timer_init);


STATIC mp_obj_t pyb_timer_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t timer_id = mp_obj_get_int(args[0]);

    if (timer_id >= NUM_TIMER) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "TIMER %d does not exist", timer_id));
    }

    if(n_args>2) {
    	//mp_hal_enableTimerAsTimer();
	//pyb_timer_init_helper(&pyb_timer_obj[timer_id], n_args-1, args+1, mp_map_t *kw_args);
    }
    return (mp_obj_t)&pyb_timer_obj[timer_id];
}







STATIC const mp_map_elem_t pyb_timer_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_timer_init_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_off), (mp_obj_t)&pyb_led_off_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_toggle), (mp_obj_t)&pyb_led_toggle_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_intensity), (mp_obj_t)&pyb_led_intensity_obj },

};

STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_t)&pyb_timer_locals_dict,
};
