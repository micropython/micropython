/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// mpy includes
#include "py/runtime.h"
#include "py/mphal.h"


// MTB includes
#include "cybsp.h"
#include "cyhal.h"


#define TIMER_MODE_ONE_SHOT (0)
#define TIMER_MODE_PERIODIC (1)

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    cyhal_timer_t timer_obj;
    int timer_id;
    uint32_t mode;
    uint32_t period;
    uint32_t freq;
    mp_obj_t callback;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

machine_timer_obj_t *timer_obj[MAX_TIMER] = { NULL };

static void isr_timer(void *callback_arg, cyhal_timer_event_t event) {
    machine_timer_obj_t *self = callback_arg;
    mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
}

// Allocate timer
static inline machine_timer_obj_t *timer_obj_alloc() {
    for (uint8_t i = 0; i < MAX_TIMER; i++)
    {
        if (timer_obj[i] == NULL) {
            timer_obj[i] = mp_obj_malloc(machine_timer_obj_t, &machine_timer_type);
            return timer_obj[i];
        }
    }
    return NULL;
}

// Free timer
static inline void timer_obj_free(machine_timer_obj_t *timer_obj_ptr) {
    for (uint8_t i = 0; i < MAX_TIMER; i++)
    {
        if (timer_obj[i] == timer_obj_ptr) {
            timer_obj[i] = NULL;
        }
    }
}

// Write a function to check if a timer is already in use using id as input and compare to the timer_id of the timer_obj
static inline bool timer_in_use(int id) {
    for (uint8_t i = 0; i < MAX_TIMER; i++)
    {
        if (timer_obj[i] != NULL && timer_obj[i]->timer_id == id) {
            return true;
        }
    }
    return false;
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr mode = self->mode == TIMER_MODE_ONE_SHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    mp_printf(print, "Timer(id=%d, mode=%q, period=%u, tick_hz=%u)", self->timer_id, mode, self->period, self->freq);
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_mode, ARG_callback, ARG_period, ARG_freq};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_ONE_SHOT} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 9999u} },
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_rom_obj = MP_ROM_NONE} },
    };
    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float period;
    self->mode = args[ARG_mode].u_int;

    if (args[ARG_freq].u_obj != mp_const_none) {
        self->freq = args[ARG_freq].u_int;
        period = 1.0f / (float)(args[ARG_freq].u_int);   // Frequency to period conversion
    } else {
        self->period = args[ARG_period].u_int;
        period = (float)args[ARG_period].u_int / 1000.0f;   // ms to s conversion
    }

    if (args[ARG_callback].u_obj != mp_const_none) {
        self->callback = args[ARG_callback].u_obj;
    }


    uint32_t period_hal;        // Period/count input for the PSoC6 HAL timer configuration
    uint32_t fz_hal = 1000000;  // Frequency for the PSoC timer clock is fixed as 1 MHz
    period_hal = (uint32_t)(period * fz_hal) - 1; // Overflow Period = (Period + 1)/ frequency ;period = (overflow period * frequency)-1

    // Adjust the frequency & recalculate the period if period/count is  greater than the maximum overflow value for a 32 bit timer ie; 2^32
    while (period_hal > 4294967296) {
        fz_hal = fz_hal / 10;  // Reduce the fz_hal value by 10%
        period_hal = (uint32_t)(period * fz_hal) - 1;  // Recalculate Period input for the PSoC6 HAL timer configuration
    }

    // Timer initialisation of port
    cy_rslt_t rslt;

    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = period_hal,               /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = self->mode,        /* Run the timer */
        .value = 0                          /* Initial value of counter */
    };

    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */

    rslt = cyhal_timer_init(&self->timer_obj, NC, NULL);
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&self->timer_obj, &timer_cfg);

    /* Set the frequency of timer to Defined frequency */
    rslt = cyhal_timer_set_frequency(&self->timer_obj, fz_hal);

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&self->timer_obj, isr_timer, self);

    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&self->timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&self->timer_obj);

    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    return mp_const_none;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t id = 0;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    // Get timer id
    if (id == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Virtual timers are not supported yet!"));
    }

    if (id >= MAX_TIMER) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid timer id!"));
    }

    if (timer_in_use(id)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timer already in use!"));
    }

    machine_timer_obj_t *self = timer_obj_alloc();
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("All timers are already initialized. Deinit some timers to create new ones!"));
    }

    self->timer_id = id;

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError("Init is not supported in this port. Please use constructor to initialize the parameters!\n");
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyhal_timer_free(&self->timer_obj);
    timer_obj_free(self);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};

static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );


void mod_timer_deinit() {
    for (uint8_t i = 0; i < MAX_TIMER; i++) {
        if (timer_obj[i] != NULL) {
            machine_timer_deinit((mp_obj_base_t *)(timer_obj[i]));
        }
    }
}
