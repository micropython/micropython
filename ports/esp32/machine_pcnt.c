/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Till Harbaum
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
#include "py/mphal.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_PCNT

#include "driver/pcnt.h"

typedef struct _mpcnt_obj_t {
  mp_obj_base_t base;
  mp_uint_t unit_id;
  mp_obj_t irq_handler;
  mp_int_t event_pending;
} mpcnt_obj_t;

STATIC mp_obj_t mpcnt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {
  
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_uint_t unit_id =  mp_obj_get_int(args[0]);

    if( /* unit_id < PCNT_UNIT_0 || */ unit_id >= PCNT_UNIT_MAX )
        mp_raise_ValueError(MP_ERROR_TEXT("unit out of range"));

    mpcnt_obj_t *self = m_new_obj_with_finaliser(mpcnt_obj_t);
    self->base.type = &machine_pcnt_type;
    self->unit_id = unit_id;
    self->irq_handler = MP_OBJ_NULL;
    self->event_pending = false;

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mpcnt_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PCNT(%u)", self->unit_id);
}

STATIC mp_obj_t mpcnt_deinit(mp_obj_t self_in) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // disconnect any pin that may be connected
    pcnt_set_pin(self->unit_id, 0, PCNT_PIN_NOT_USED, PCNT_PIN_NOT_USED);
    pcnt_set_pin(self->unit_id, 1, PCNT_PIN_NOT_USED, PCNT_PIN_NOT_USED);

    // uninstall interrupt handlers
    pcnt_isr_handler_remove(self->unit_id);

    // don't deinstall the service handler itself as other
    // instances may still be using it
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_deinit_obj, mpcnt_deinit);

STATIC mp_obj_t mpcnt_counter_value(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int16_t count;
    
    // if True was given as arg and event is pending then return None
    if ( (n_args == 2) && mp_obj_get_int(args[1]) && self->event_pending )
      return mp_const_none;
    
    check_esp_err(pcnt_get_counter_value(self->unit_id, &count));
    return mp_obj_new_int(count);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_counter_value_obj, 1, 2, mpcnt_counter_value);

STATIC mp_obj_t mpcnt_counter_clear(mp_obj_t self_in) {
    check_esp_err(pcnt_counter_clear(((mpcnt_obj_t *)MP_OBJ_TO_PTR(self_in))->unit_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_counter_clear_obj, mpcnt_counter_clear);

STATIC mp_obj_t mpcnt_counter_resume(mp_obj_t self_in) {
    check_esp_err(pcnt_counter_resume(((mpcnt_obj_t *)MP_OBJ_TO_PTR(self_in))->unit_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_counter_resume_obj, mpcnt_counter_resume);

STATIC mp_obj_t mpcnt_counter_pause(mp_obj_t self_in) {
    check_esp_err(pcnt_counter_pause(((mpcnt_obj_t *)MP_OBJ_TO_PTR(self_in))->unit_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_counter_pause_obj, mpcnt_counter_pause);

STATIC mp_obj_t mpcnt_filter_enable(mp_obj_t self_in) {
    check_esp_err(pcnt_filter_enable(((mpcnt_obj_t *)MP_OBJ_TO_PTR(self_in))->unit_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_filter_enable_obj, mpcnt_filter_enable);

STATIC mp_obj_t mpcnt_filter_disable(mp_obj_t self_in) {
    check_esp_err(pcnt_filter_disable(((mpcnt_obj_t *)MP_OBJ_TO_PTR(self_in))->unit_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpcnt_filter_disable_obj, mpcnt_filter_disable);

STATIC mp_obj_t mpcnt_filter_value(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        uint16_t value;
        check_esp_err(pcnt_get_filter_value(self->unit_id, &value));
	return mp_obj_new_int(value);
    }
    uint16_t value = mp_obj_get_int(args[1]);
    check_esp_err(pcnt_set_filter_value(self->unit_id, value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_filter_value_obj, 1, 2, mpcnt_filter_value);

STATIC void mpcnt_isr_handler(void *arg) {
    mpcnt_obj_t *self = arg;
    self->event_pending = true;
    mp_sched_schedule(self->irq_handler, MP_OBJ_FROM_PTR(self));
    mp_hal_wake_main_task_from_isr();
}

STATIC mp_obj_t mpcnt_irq(mp_obj_t self_in, mp_obj_t handler_in) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    pcnt_isr_service_install(0);              // Ignore failure as service may already be installed
    pcnt_isr_handler_remove(self->unit_id);   // Ignore failure as no handler may be installed yet.
    
    self->irq_handler = handler_in;
    check_esp_err(pcnt_isr_handler_add(self->unit_id, mpcnt_isr_handler, (void *)self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mpcnt_irq_obj, mpcnt_irq);

STATIC mp_obj_t mpcnt_event_enable(mp_obj_t self_in, mp_obj_t value_in) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int value = mp_obj_get_int(value_in);
    check_esp_err(pcnt_event_enable(self->unit_id, value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mpcnt_event_enable_obj, mpcnt_event_enable);

STATIC mp_obj_t mpcnt_event_disable(mp_obj_t self_in, mp_obj_t value_in) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int value = mp_obj_get_int(value_in);
    check_esp_err(pcnt_event_disable(self->unit_id, value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mpcnt_event_disable_obj, mpcnt_event_disable);

STATIC mp_obj_t mpcnt_event_status(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t status;

    // if True was given as arg, then internally ack a pending event
    if ( (n_args == 2) && mp_obj_get_int(args[1]) )
      self->event_pending = false;
    
    // pcnt_get_event_status has been introduced in 4.3
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 3, 0)
    status = PCNT.status_unit[self->unit_id].val;
    PCNT.int_clr.val = BIT(self->unit_id);
#else
    check_esp_err(pcnt_get_event_status(self->unit_id, &status));
#endif
    return mp_obj_new_int(status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_event_status_obj, 1, 2, mpcnt_event_status);

STATIC mp_obj_t mpcnt_event_value(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    pcnt_evt_type_t type = mp_obj_get_int(args[1]);
    if (n_args == 2) {
        int16_t value;
        check_esp_err(pcnt_get_event_value(self->unit_id, type, &value));
	return mp_obj_new_int(value);
    }
    int16_t value = mp_obj_get_int(args[2]);
    check_esp_err(pcnt_set_event_value(self->unit_id, type, value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_event_value_obj, 2, 3, mpcnt_event_value);

STATIC int _mpcnt_map_pin(mp_obj_t pin) {
  if(pin == mp_const_none) return PCNT_PIN_NOT_USED;
  return machine_pin_get_id(pin);
}

STATIC mp_obj_t mpcnt_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mpcnt_obj_t *self = pos_args[0];

    enum { ARG_sig_pin, ARG_channel, ARG_ctrl_pin, ARG_pos_mode, ARG_neg_mode,
	   ARG_lctrl_mode, ARG_hctrl_mode, ARG_counter_l_lim, ARG_counter_h_lim };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,        MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_channel,    MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_ctrl,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_pos_mode,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PCNT_COUNT_INC} }, // Count up on the positive edge
        { MP_QSTR_neg_mode,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PCNT_COUNT_DIS} }, // Keep the counter value on the negative edge
        { MP_QSTR_lctrl_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PCNT_MODE_KEEP} }, //  Keep the primary counter mode if low
        { MP_QSTR_hctrl_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PCNT_MODE_KEEP} }, //  Keep the primary counter mode if high
        { MP_QSTR_counter_l_lim,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -100} },
        { MP_QSTR_counter_h_lim,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int =  100} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = _mpcnt_map_pin(args[ARG_sig_pin].u_obj),
        .ctrl_gpio_num = _mpcnt_map_pin(args[ARG_ctrl_pin].u_obj),
        .channel = PCNT_CHANNEL_0 + args[ARG_channel].u_int,
        .unit = self->unit_id,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = args[ARG_pos_mode].u_int,
        .neg_mode = args[ARG_neg_mode].u_int,
        // What to do when control input is low or high?
        .lctrl_mode = args[ARG_lctrl_mode].u_int,
        .hctrl_mode = args[ARG_hctrl_mode].u_int,
        // Set the maximum and minimum limit values to watch
        .counter_h_lim = args[ARG_counter_h_lim].u_int,
        .counter_l_lim = args[ARG_counter_l_lim].u_int,
    };
    
    /* Initialize PCNT unit */
    check_esp_err(pcnt_unit_config(&pcnt_config));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mpcnt_config_obj, 1, mpcnt_config);

STATIC mp_obj_t mpcnt_set_pin(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    pcnt_channel_t channel = mp_obj_get_int(args[1]);
    int pulse_io = _mpcnt_map_pin(args[2]);
    int ctrl_io = _mpcnt_map_pin(args[3]);
    check_esp_err(pcnt_set_pin(self->unit_id, channel, pulse_io, ctrl_io));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_set_pin_obj, 4, 4, mpcnt_set_pin);
  
STATIC mp_obj_t mpcnt_set_mode(size_t n_args, const mp_obj_t *args) {
    mpcnt_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    pcnt_channel_t channel = mp_obj_get_int(args[1]);
    pcnt_count_mode_t pos_mode = mp_obj_get_int(args[2]);
    pcnt_count_mode_t neg_mode = mp_obj_get_int(args[3]);
    pcnt_ctrl_mode_t hctrl_mode = mp_obj_get_int(args[4]);
    pcnt_ctrl_mode_t lctrl_mode = mp_obj_get_int(args[5]);
    check_esp_err(pcnt_set_mode(self->unit_id, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mpcnt_set_mode_obj, 6, 6, mpcnt_set_mode);

STATIC const mp_rom_map_elem_t mpcnt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_MODE_KEEP), MP_ROM_INT(PCNT_MODE_KEEP) },
    { MP_ROM_QSTR(MP_QSTR_MODE_REVERSE), MP_ROM_INT(PCNT_MODE_REVERSE) },
    { MP_ROM_QSTR(MP_QSTR_MODE_DISABLE), MP_ROM_INT(PCNT_MODE_DISABLE) },
    
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mpcnt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mpcnt_deinit_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&mpcnt_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pin), MP_ROM_PTR(&mpcnt_set_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mode), MP_ROM_PTR(&mpcnt_set_mode_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_COUNT_DIS), MP_ROM_INT(PCNT_COUNT_DIS) },
    { MP_ROM_QSTR(MP_QSTR_COUNT_INC), MP_ROM_INT(PCNT_COUNT_INC) },
    { MP_ROM_QSTR(MP_QSTR_COUNT_DEC), MP_ROM_INT(PCNT_COUNT_DEC) },
    { MP_ROM_QSTR(MP_QSTR_counter_value), MP_ROM_PTR(&mpcnt_counter_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_counter_pause), MP_ROM_PTR(&mpcnt_counter_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_counter_resume), MP_ROM_PTR(&mpcnt_counter_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_counter_clear), MP_ROM_PTR(&mpcnt_counter_clear_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_filter_enable), MP_ROM_PTR(&mpcnt_filter_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_filter_disable), MP_ROM_PTR(&mpcnt_filter_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_filter_value), MP_ROM_PTR(&mpcnt_filter_value_obj) },
        
    { MP_ROM_QSTR(MP_QSTR_EVT_THRES_0), MP_ROM_INT(PCNT_EVT_THRES_0) },
    { MP_ROM_QSTR(MP_QSTR_EVT_THRES_1), MP_ROM_INT(PCNT_EVT_THRES_1) },
    { MP_ROM_QSTR(MP_QSTR_EVT_L_LIM), MP_ROM_INT(PCNT_EVT_L_LIM) },
    { MP_ROM_QSTR(MP_QSTR_EVT_H_LIM), MP_ROM_INT(PCNT_EVT_H_LIM) },
    { MP_ROM_QSTR(MP_QSTR_EVT_ZERO), MP_ROM_INT(PCNT_EVT_ZERO) },							    
    { MP_ROM_QSTR(MP_QSTR_event_enable), MP_ROM_PTR(&mpcnt_event_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_event_disable), MP_ROM_PTR(&mpcnt_event_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_event_status), MP_ROM_PTR(&mpcnt_event_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_event_value), MP_ROM_PTR(&mpcnt_event_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&mpcnt_irq_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mpcnt_locals_dict, mpcnt_locals_dict_table);

const mp_obj_type_t machine_pcnt_type = {
    { &mp_type_type },
    .name = MP_QSTR_PCNT,
    .print = mpcnt_print,
    .make_new = mpcnt_make_new,
    .locals_dict = (mp_obj_t)&mpcnt_locals_dict,
};

#endif // MICROPY_PY_MACHINE_PCNT
