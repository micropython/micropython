/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Thorsten von Eicken
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
#include "driver/pcnt.h"
#include "esp_err.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

// Forward dec'l
extern const mp_obj_type_t machine_counter_type;

typedef struct _esp32_counter_obj_t {
    mp_obj_base_t base;
    gpio_num_t pin;
    uint8_t active;  // object refers to an init'ed and not deinit'ed counter
    uint8_t running; // running = !paused
    uint8_t unit;
    uint8_t dir_up;  // true=up, false=down
    uint8_t edges;   // 1=rising, 2=falling, 3=both
    uint16_t limit;  // logical limit (negated for down-counting)
} esp32_counter_obj_t;

// Which unit/(channel) has which GPIO pin assigned?
// (-1 if not assigned)
STATIC gpio_num_t unit_gpio[PCNT_UNIT_MAX];

#define DIR_UP       (PCNT_COUNT_INC)
#define DIR_DOWN     (PCNT_COUNT_DEC)
#define EDGE_RISING  (1)
#define EDGE_FALLING (2)
#define EDGE_BOTH    (3)
#define TRIG_ZERO    (1)
#define TRIG_LIMIT   (2)

/******************************************************************************/

// MicroPython bindings for COUNTER

STATIC void esp32_counter_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "COUNTER(%u", self->pin);
    if (self->active) {
        int16_t v = 12345;
        pcnt_get_counter_value(self->unit, &v);
        if (!self->dir_up) v += self->limit; // adjust to limit..0 interval
        mp_printf(print, ", %s, value=%u, limit=%u", self->running ? "running" : "paused",
                (uint16_t)v, self->limit);
        static char *(edges[]) = {"??", "rising", "falling", "both"};
        mp_printf(print, ", dir=%s, edge=%s, unit=%d", self->dir_up ? "up":"down",
                edges[self->edges], self->unit);

    } else {
        mp_printf(print, ", deinitialized");
    }
    mp_printf(print, ")");
}

STATIC void esp32_counter_init_helper(esp32_counter_obj_t *self,
        size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_direction, ARG_edge, ARG_limit, ARG_reset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_direction, MP_ARG_INT, {.u_int = DIR_UP} },
        { MP_QSTR_edge, MP_ARG_INT, {.u_int = EDGE_RISING} },
        { MP_QSTR_limit, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_reset, MP_ARG_BOOL, {.u_bool = 1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Find a free COUNTER unit, also spot if our pin is already mentioned.
    int unit = 0;
    int avail = -1;
    while (unit < PCNT_UNIT_MAX) {
        if (unit_gpio[unit] == self->pin) { break; }
        if ((avail == -1) && (unit_gpio[unit] == -1)) { avail = unit; }
        unit++;
    }
    if (unit >= PCNT_UNIT_MAX) {
        if (avail == -1) { mp_raise_ValueError("out of COUNTER units"); }
        unit = avail;
    }
    self->unit = unit;

    // config counter unit
    self->active = 1;
    self->running = 1;
    pcnt_config_t cfg = {
        .unit = unit,
        .channel = 0,
        .pulse_gpio_num = self->pin,
        .ctrl_gpio_num = -1, // not yet supported
        .counter_h_lim = 0,
        .counter_l_lim = 0,
        .pos_mode = PCNT_COUNT_DIS, // positive edge is no-op
        .neg_mode = PCNT_COUNT_DIS, // negative edge is no-op
        .lctrl_mode = PCNT_MODE_KEEP, // ctrl pin is no-op
        .hctrl_mode = PCNT_MODE_KEEP, // ctrl pin is no-op
    };
    // figure direction and edges
    self->dir_up = args[ARG_direction].u_int == PCNT_COUNT_INC;
    self->edges = args[ARG_edge].u_int & EDGE_BOTH;
    pcnt_count_mode_t mode = self->dir_up ? PCNT_COUNT_INC : PCNT_COUNT_DEC;
    if (args[ARG_edge].u_int & EDGE_RISING) { cfg.pos_mode = mode; }
    if (args[ARG_edge].u_int & EDGE_FALLING) { cfg.neg_mode = mode; }
    // limit up or down
    self->limit = args[ARG_limit].u_int;
    if (self->limit == 0) self->limit = 0x7fff;
    if (self->dir_up) { cfg.counter_h_lim = self->limit; }
    else { cfg.counter_l_lim = -self->limit; }

    //printf("Counter dir_up=%d edges=%d limit=%d\n", self->dir_up, self->edges, self->limit);
    if (pcnt_unit_config(&cfg) != ESP_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "Counter not supported on pin %d", self->pin));
    }
    pcnt_counter_resume(unit); // apparently it starts paused...
    unit_gpio[unit] = self->pin;

    if (args[ARG_reset].u_bool) pcnt_counter_clear(self->unit);
}

STATIC bool counter_inited = false;

STATIC mp_obj_t esp32_counter_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);

    // create counter object from the given pin
    esp32_counter_obj_t *self = m_new_obj(esp32_counter_obj_t);
    self->base.type = &machine_counter_type;
    self->pin = pin_id;
    self->active = 0;
    self->unit = -1;

    // start the counter subsystem if it's not already running
    if (!counter_inited) {
        // Initial condition: no units assigned
        for (int x = 0; x < PCNT_UNIT_MAX; ++x) {
            unit_gpio[x] = -1;
        }
        counter_inited = true;
    }

    // perform init with any keyword args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    esp32_counter_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t esp32_counter_init(size_t n_args,
        const mp_obj_t *args, mp_map_t *kw_args) {
    esp32_counter_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(esp32_counter_init_obj, 1, esp32_counter_init);

STATIC mp_obj_t esp32_counter_deinit(mp_obj_t self_in) {
    esp32_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int unit = self->unit;

    // Valid unit?
    if ((unit >= 0) && (unit < PCNT_UNIT_MAX)) {
        // Mark it unused, and tell the hardware to stop
        unit_gpio[unit] = -1;
        pcnt_counter_pause(self->unit);
        pcnt_intr_disable(self->unit);
        self->active = 0;
        self->unit = -1;
        //gpio_matrix_out(self->pin, SIG_GPIO_OUT_IDX, false, false); // is this needed???
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_counter_deinit_obj, esp32_counter_deinit);

STATIC mp_obj_t esp32_counter_pause(mp_obj_t arg0) {
    esp32_counter_obj_t *self = MP_OBJ_TO_PTR(arg0);
    if (self->active) {
        pcnt_counter_pause(self->unit);
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Unit inactive"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_counter_pause_obj, esp32_counter_pause);

STATIC mp_obj_t esp32_counter_resume(mp_obj_t arg0) {
    esp32_counter_obj_t *self = MP_OBJ_TO_PTR(arg0);
    if (self->active) {
        pcnt_counter_resume(self->unit);
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Unit inactive"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_counter_resume_obj, esp32_counter_resume);

STATIC mp_obj_t esp32_counter_value(size_t n_args, const mp_obj_t *args) {
    esp32_counter_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->active) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Unit inactive"));
        return mp_const_none;
    }
    if (n_args == 1) {
        // get
        int16_t v = 12345;
        pcnt_get_counter_value(self->unit, &v);
        if (!self->dir_up) v += self->limit; // adjust to limit..0 interval
        return MP_OBJ_NEW_SMALL_INT((uint16_t)v);
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    if (self->dir_up && tval != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                    "Can only set value to zero"));
    } else if (!self->dir_up && tval != self->limit) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                    "Can only set value to limit (%u)", self->limit));
    } else {
        pcnt_counter_clear(self->unit);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_counter_value_obj, 1, 2, esp32_counter_value);

STATIC const mp_rom_map_elem_t esp32_counter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&esp32_counter_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_counter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&esp32_counter_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&esp32_counter_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&esp32_counter_resume_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&esp32_counter_irq_obj) },
#endif
    // class constants
    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(PCNT_COUNT_INC) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(PCNT_COUNT_DEC) },
    { MP_ROM_QSTR(MP_QSTR_RISING), MP_ROM_INT(EDGE_RISING) },
    { MP_ROM_QSTR(MP_QSTR_FALLING), MP_ROM_INT(EDGE_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_BOTH), MP_ROM_INT(EDGE_BOTH) },
    { MP_ROM_QSTR(MP_QSTR_ZERO), MP_ROM_INT(TRIG_ZERO) },
    { MP_ROM_QSTR(MP_QSTR_LIMIT), MP_ROM_INT(TRIG_LIMIT) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_counter_locals_dict,
    esp32_counter_locals_dict_table);

const mp_obj_type_t machine_counter_type = {
    { &mp_type_type },
    .name = MP_QSTR_COUNTER,
    .print = esp32_counter_print,
    .make_new = esp32_counter_make_new,
    .locals_dict = (mp_obj_dict_t*)&esp32_counter_locals_dict,
};
