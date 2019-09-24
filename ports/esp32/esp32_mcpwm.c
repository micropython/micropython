#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"

#include <stdio.h>
#include "driver/mcpwm.h"
#include "mphalport.h"
#include "esp_err.h"

#define MCPWM_CHANNEL_MAX (MCPWM_TIMER_MAX * MCPWM_UNIT_MAX)

typedef struct _mcpwm_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    gpio_num_t pin_a;
    gpio_num_t pin_b;

    // Derived values for easier IDF API calls.
    mcpwm_unit_t unit;
    mcpwm_timer_t timer;
    mcpwm_io_signals_t sig_a;
    mcpwm_io_signals_t sig_b;
} mcpwm_obj_t;

// Forward dec'l
extern const mp_obj_type_t esp32_mcpwm_type;
STATIC mp_obj_t _mcpwm_deinit(mp_obj_t self_in);
STATIC mp_obj_t _mcpwm_duty(size_t n_args, const mp_obj_t *args);
STATIC mp_obj_t _mcpwm_freq(size_t n_args, const mp_obj_t *args);

STATIC mp_obj_t mcpwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, true);

    int block_id = mp_obj_get_int(args[0]);
    if (block_id >= MCPWM_CHANNEL_MAX) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "bad timer id requested: %d (0~5 allowed)", block_id));
    }

    // create PWM object from the given timer block
    mcpwm_obj_t *self = m_new_obj(mcpwm_obj_t);
    self->base.type = &esp32_mcpwm_type;
    self->id = block_id;

    self->timer = self->id % MCPWM_TIMER_MAX;
    self->unit = self->id / MCPWM_TIMER_MAX;
    self->sig_a = self->timer * 2;  // even numbers -> A operators
    self->sig_b = self->timer * 2 + 1;  // odd numbers -> B operators

    mcpwm_config_t config = {
        .frequency = 1000,  // frequency = 1kHz
        .cmpr_a = 50,       // duty A = 50%
        .cmpr_b = 20,       // duty B = 20%
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0
    };

    mcpwm_init(self->unit, self->timer, &config);
    mp_obj_t self_out = MP_OBJ_FROM_PTR(self);

    return self_out;
}


STATIC void mcpwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mcpwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "MCPWM(ID=%u. ", self->id);
    mp_printf(print, "pin=%u, ", self->pin_a);
    float duty = mcpwm_get_duty(self->unit, self->timer, MCPWM_OPR_A);
    int freq = mcpwm_get_frequency(self->unit, self->timer);
    mp_printf(print, "freq=%uHz, duty=%.1f%%", freq, duty);
    mp_printf(print, ")");
}


// BIND
STATIC mp_obj_t _mcpwm_bind(mp_obj_t self_in, mp_obj_t pin) {
    mcpwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_num_t pin_id = machine_pin_get_id(pin);
    mcpwm_gpio_init(self->unit, self->sig_a, pin_id);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mcpwm_bind_obj,  _mcpwm_bind);


// DEINIT
STATIC mp_obj_t _mcpwm_deinit(mp_obj_t self_in) {
    mcpwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mcpwm_stop(self->unit, self->timer);
    //mcpwm_set_signal_low(self->unit, self->timer, MCPWM_OPR_A);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_deinit_obj, _mcpwm_deinit);


// FREQ
STATIC mp_obj_t _mcpwm_freq(size_t n_args, const mp_obj_t *args) {
    mcpwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(mcpwm_get_frequency(self->unit, self->timer));
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    mcpwm_set_frequency(self->unit, self->timer, tval);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcpwm_freq_obj, 1, 2, _mcpwm_freq);


// DUTY
STATIC mp_obj_t _mcpwm_duty(size_t n_args, const mp_obj_t *args) {
    mcpwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        return mp_obj_new_float(mcpwm_get_duty(self->unit, self->timer, MCPWM_OPR_A));
    }

    // set
    float tval = mp_obj_get_float(args[1]);
    mcpwm_set_duty(self->unit, self->timer, MCPWM_OPR_A, tval);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcpwm_duty_obj, 1, 2, _mcpwm_duty);


STATIC const mp_rom_map_elem_t mcpwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&mcpwm_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mcpwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&mcpwm_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&mcpwm_freq_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mcpwm_locals_dict, mcpwm_locals_dict_table);

const mp_obj_type_t esp32_mcpwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_MCPWM,
    .print = mcpwm_print,
    .make_new = mcpwm_make_new,
    .locals_dict = (mp_obj_dict_t *)&mcpwm_locals_dict,
};
