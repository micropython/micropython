/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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
#include "py/mperrno.h"
#include "py/mphal.h"
#include "fsl_pit.h"
#include "modmachine.h"

#define TIMER_MODE_ONE_SHOT (0)
#define TIMER_MODE_PERIODIC (1)
#define TIMER_MIN_PERIOD 1

#define alarm_callback PIT_IRQHandler
#define PIT_SOURCE_CLOCK CLOCK_GetIpgFreq()
#define PIT_IRQ_ID PIT_IRQn

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    int8_t id;
    int8_t channel;
    uint32_t mode;
    uint32_t tick_hz;
    uint32_t delta_us; // for periodic mode
    mp_obj_t callback;
} machine_timer_obj_t;

static const int8_t channel_no[MICROPY_HW_PIT_NUM_CHANNELS] = {0, 2, 3}; // no channel 1
static pit_config_t pit_config;

// This is the interrupt handler
// To tell which channel fired one has to poll the flags
void alarm_callback(void) {
    for (uint8_t index = 0; index < MICROPY_HW_PIT_NUM_CHANNELS; index++) {
        uint32_t flag;
        machine_timer_obj_t *self = MP_STATE_PORT(timer_table)[index];
        if (self != NULL) {
            flag = PIT_GetStatusFlags(PIT, self->channel);
            if (flag & kPIT_TimerFlag) { // channel fired
                PIT_ClearStatusFlags(PIT, self->channel, kPIT_TimerFlag);
                __DSB();

                mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));

                if (self->mode == TIMER_MODE_ONE_SHOT) {
                    PIT_StopTimer(PIT, self->channel);
                }
            }
        }
    }
}

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr mode = self->mode == TIMER_MODE_ONE_SHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    mp_printf(print, "Timer(channel=%d, mode=%q, period=%d, tick_hz=%d)",
        self->id, mode, self->delta_us / self->tick_hz, self->tick_hz);
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_period, ARG_tick_hz, ARG_freq, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback,     MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->mode = args[ARG_mode].u_int;
    if (args[ARG_freq].u_obj != mp_const_none) {
        // Frequency specified in Hz
        #if MICROPY_PY_BUILTINS_FLOAT
        self->delta_us = (uint32_t)(MICROPY_FLOAT_CONST(1000000.0) / mp_obj_get_float(args[ARG_freq].u_obj));
        #else
        self->delta_us = 1000000 / mp_obj_get_int(args[ARG_freq].u_obj);
        #endif
    } else {
        // Period specified
        self->tick_hz = args[ARG_tick_hz].u_int;
        self->delta_us = (uint64_t)args[ARG_period].u_int * 1000000 / self->tick_hz;
    }
    if (self->delta_us < TIMER_MIN_PERIOD) {
        self->delta_us = TIMER_MIN_PERIOD;
    }

    self->callback = args[ARG_callback].u_obj;

    // Set timer period for channel id
    PIT_SetTimerPeriod(PIT, self->channel, USEC_TO_COUNT(self->delta_us, PIT_SOURCE_CLOCK));

    // Enable timer interrupts for the channel
    PIT_EnableInterrupts(PIT, self->channel, kPIT_TimerInterruptEnable);

    // Enable at the NVIC
    EnableIRQ(PIT_IRQ_ID);

    // Start channel 0
    PIT_StartTimer(PIT, self->channel);

    return mp_const_none;
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_timer_obj_t *self;

    // Get timer id in the range of 0..2
    mp_int_t id = 0;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    if (id < 0 || id >= MICROPY_HW_PIT_NUM_CHANNELS) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timer does not exist"));
    }

    // check, if a timer exists at that channel and stop it first
    if (MP_STATE_PORT(timer_table)[id] != NULL) {
        PIT_StopTimer(PIT, channel_no[id]);
        self = MP_STATE_PORT(timer_table)[id];
    } else {
        self = m_new_obj_with_finaliser(machine_timer_obj_t);
        self->base.type = &machine_timer_type;
        MP_STATE_PORT(timer_table)[id] = self;
    }

    // Set initial values
    self->id = id;
    self->channel = channel_no[id];

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_timer___del__(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    PIT_StopTimer(PIT, self->channel);
    MP_STATE_PORT(timer_table)[self->id] = NULL;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer___del___obj, machine_timer___del__);

STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    PIT_StopTimer(PIT, self->channel);
    return machine_timer_init_helper(self, n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    PIT_StopTimer(PIT, self->channel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

// Initialize clock an first config
void machine_timer_init_PIT(void) {
    // PIT timer
    // Enable clock gate for GPIO1
    CLOCK_EnableClock(kCLOCK_Gpio1); // ?
    // Set PERCLK_CLK divider to 1
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0U);

    PIT_GetDefaultConfig(&pit_config);
    PIT_Init(PIT, &pit_config);
}

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_timer_locals_dict,
};
