/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

// Hardware timer using TCPWM0 counters.
// IDs 0..7 are Group 0 (32-bit) counters.
// IDs 8..31 are Group 1 (16-bit) counters.

#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "shared/runtime/mpirq.h"
#include "sys_int.h"

#include "cy_tcpwm_counter.h"
#include "cy_tcpwm.h"
#include "cy_sysclk.h"
#include "cycfg_peripheral_clocks.h"
#include "cycfg_peripherals.h"
#include "mtb_hal.h"
#include "tcpwm.h"
#include "genhdr/pins_af.h"

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

#define MACHINE_TIMER_NUM_INSTANCES  (32)

// Shared general-purpose timer divider target frequency used by this module.
#define TIMER_CLK_HZ  (1000000UL)

// Input-pin disabled sentinel (upper 2 bits are ignored; mask to 2-bit field)
#define TIMER_INPUT_DISABLED  (0x7U)

// Timer mode values — also exposed as Python class constants
#define TIMER_MODE_ONE_SHOT  (1)
#define TIMER_MODE_PERIODIC  (2)

// ---------------------------------------------------------------------------
// Object type
// ---------------------------------------------------------------------------

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    uint8_t id;                  // 0..31
    uint32_t counter_num;        // TCPWM0 counter index mapped from timer_hw[]
    en_clk_dst_t pclk_dst;
    uint16_t mode;               // TIMER_MODE_ONE_SHOT or TIMER_MODE_PERIODIC
    mp_obj_t callback;
    bool ishard;
    bool active;
    sys_int_cfg_t irq_cfg;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;  // forward declaration

// Maximum period register value for each counter group.
#define TIMER_GROUP0_PERIOD_MAX  (UINT32_MAX)  // 32-bit counters
#define TIMER_GROUP1_PERIOD_MAX  (0xFFFFU)     // 16-bit counters

// ---------------------------------------------------------------------------
// Per-ID hardware mapping — generated from the board AF configuration.
// Macro is defined in pins_af.h as MICROPY_PY_MACHINE_TIMER_HW_MAP(X).
// Each row: X(timer_id, tcpwm_counter_num, irq)
// ---------------------------------------------------------------------------

#define TIMER_HW_ENTRY(id, counter, irq, pclk) counter,
static const uint32_t timer_hw[MACHINE_TIMER_NUM_INSTANCES] = {
    MICROPY_PY_MACHINE_TCPWM_HW_MAP(TIMER_HW_ENTRY)
};
#undef TIMER_HW_ENTRY

#define TIMER_IRQ_CASE(id, counter, irq, pclk) case counter: \
        return irq;
static IRQn_Type machine_timer_counter_irq(uint32_t counter_num) {
    switch (counter_num) {
        MICROPY_PY_MACHINE_TCPWM_HW_MAP(TIMER_IRQ_CASE)
        default:
            mp_raise_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("Timer counter %lu does not have an IRQ mapping"),
                (unsigned long)counter_num);
    }
}
#undef TIMER_IRQ_CASE

// ---------------------------------------------------------------------------
// Static instance pool — one entry per hardware timer
// ---------------------------------------------------------------------------

static machine_timer_obj_t *timer_obj[MACHINE_TIMER_NUM_INSTANCES] = { NULL };

static bool machine_timer_clock_configured = false;

static void machine_timer_configure_clock(void) {
    if (machine_timer_clock_configured) {
        return;
    }

    // Keep timer periods stable even when other modules stop configuring this divider.
    cy_rslt_t rslt = mtb_hal_clock_set_peri_clock_freq(&CYBSP_GENERAL_PURPOSE_TIMER_clock_ref,
        TIMER_CLK_HZ, 1000);
    if (rslt != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Timer clock setup failed (0x%lx)"),
            (unsigned long)rslt);
    }

    machine_timer_clock_configured = true;
}

// ---------------------------------------------------------------------------
// IRQ handling helpers
// ---------------------------------------------------------------------------

static void machine_timer_isr(machine_timer_obj_t *self) {
    if (Cy_TCPWM_GetInterruptStatus(TCPWM0, self->counter_num) & CY_TCPWM_INT_ON_TC) {
        Cy_TCPWM_ClearInterrupt(TCPWM0, self->counter_num, CY_TCPWM_INT_ON_TC);
        if (self->mode == TIMER_MODE_ONE_SHOT) {
            Cy_TCPWM_Counter_Disable(TCPWM0, self->counter_num);
            self->active = false;
        }
        if (self->callback != mp_const_none
            && mp_irq_dispatch(self->callback, MP_OBJ_FROM_PTR(self), self->ishard) < 0) {
            // Uncaught exception: stop timer and disable callback to avoid repeated IRQ spam.
            Cy_TCPWM_Counter_Disable(TCPWM0, self->counter_num);
            self->active = false;
            self->callback = mp_const_none;
        }
    }
}

// Generate one IRQ handler per timer id for direct dispatch without scanning.
#define TIMER_IRQ_HANDLER_DECL(id, counter, irq, pclk) \
    static void machine_timer_irq_handler_##id(void) { \
        machine_timer_obj_t *self = timer_obj[id]; \
        if (self != NULL) { \
            machine_timer_isr(self); \
        } \
    }
MICROPY_PY_MACHINE_TCPWM_HW_MAP(TIMER_IRQ_HANDLER_DECL)
#undef TIMER_IRQ_HANDLER_DECL

#define TIMER_IRQ_HANDLER_ENTRY(id, counter, irq, pclk) machine_timer_irq_handler_##id,
static const cy_israddress machine_timer_irq_handlers[MACHINE_TIMER_NUM_INSTANCES] = {
    MICROPY_PY_MACHINE_TCPWM_HW_MAP(TIMER_IRQ_HANDLER_ENTRY)
};
#undef TIMER_IRQ_HANDLER_ENTRY

// ---------------------------------------------------------------------------
// Start (or restart) a timer with the given period in clock ticks
// ---------------------------------------------------------------------------

static void machine_timer_start(machine_timer_obj_t *self, uint32_t period_ticks) {
    machine_timer_configure_clock();

    // Stop counter and unregister any existing IRQ before reconfiguring.
    Cy_TCPWM_Counter_Disable(TCPWM0, self->counter_num);
    if (self->active) {
        sys_int_deinit(&self->irq_cfg);
        self->active = false;
    }

    // Connect the shared 1 MHz PCLK divider to this counter's clock input.
    Cy_SysClk_PeriPclkAssignDivider(self->pclk_dst,
        CY_SYSCLK_DIV_16_BIT, CYBSP_GENERAL_PURPOSE_TIMER_CLK_DIV_NUM);

    // The counter counts 0 → period_reg (inclusive) then fires TC.
    // So period_reg = period_ticks - 1.
    cy_stc_tcpwm_counter_config_t cfg = {
        .period = period_ticks - 1U,
        .clockPrescaler = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,
        .runMode = (self->mode == TIMER_MODE_ONE_SHOT)
                              ? CY_TCPWM_COUNTER_ONESHOT
                              : CY_TCPWM_COUNTER_CONTINUOUS,
        .countDirection = CY_TCPWM_COUNTER_COUNT_UP,
        .compareOrCapture = CY_TCPWM_COUNTER_MODE_COMPARE,
        .compare0 = 0U,
        .compare1 = 0U,
        .enableCompareSwap = false,
        .interruptSources = CY_TCPWM_INT_ON_TC,
        // All trigger inputs disabled
        .captureInputMode = TIMER_INPUT_DISABLED & 0x3U,
        .captureInput = CY_TCPWM_INPUT_0,
        .reloadInputMode = TIMER_INPUT_DISABLED & 0x3U,
        .reloadInput = CY_TCPWM_INPUT_0,
        .startInputMode = TIMER_INPUT_DISABLED & 0x3U,
        .startInput = CY_TCPWM_INPUT_0,
        .stopInputMode = TIMER_INPUT_DISABLED & 0x3U,
        .stopInput = CY_TCPWM_INPUT_0,
        .countInputMode = TIMER_INPUT_DISABLED & 0x3U,
        .countInput = CY_TCPWM_INPUT_1,
        .capture1InputMode = TIMER_INPUT_DISABLED & 0x3U,
        .capture1Input = CY_TCPWM_INPUT_0,
        .enableCompare1Swap = false,
        .compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT,
        .compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT,
        .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
        .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    };

    cy_en_tcpwm_status_t result = Cy_TCPWM_Counter_Init(TCPWM0, self->counter_num, &cfg);
    if (result != CY_TCPWM_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Timer(%u) init failed (PDL error 0x%lx)"),
            (unsigned)self->id, (unsigned long)result);
    }

    // Register IRQ before enabling so no TC edge is missed.
    self->irq_cfg.priority = SYS_INT_IRQ_LOWEST_PRIORITY;
    self->irq_cfg.handler = machine_timer_irq_handlers[self->id];
    sys_int_init(&self->irq_cfg);

    self->active = true;
    Cy_TCPWM_Counter_Enable(TCPWM0, self->counter_num);
    Cy_TCPWM_TriggerReloadOrIndex_Single(TCPWM0, self->counter_num);
}

// ---------------------------------------------------------------------------
// Python init helper (shared by make_new and .init())
// ---------------------------------------------------------------------------

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_mode, ARG_callback, ARG_period, ARG_freq, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_hard,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_FALSE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate mode
    mp_int_t mode = args[ARG_mode].u_int;
    if (mode != TIMER_MODE_ONE_SHOT && mode != TIMER_MODE_PERIODIC) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }
    self->mode = (uint16_t)mode;

    // Validate callback
    mp_obj_t callback = args[ARG_callback].u_obj;
    if (callback != mp_const_none && !mp_obj_is_callable(callback)) {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be callable"));
    }
    self->callback = callback;

    mp_obj_t hard = args[ARG_hard].u_obj;
    if (!mp_obj_is_bool(hard)) {
        mp_raise_ValueError(MP_ERROR_TEXT("hard must be bool"));
    }
    self->ishard = (hard == mp_const_true);

    // Hard IRQ callbacks not supported on FreeRTOS: scheduler cannot be locked from ISR context.
    // TODO: Re-enable hard irq and remove this patch
    #if defined(CY_RTOS_AWARE)
    if (self->ishard) {
        self->ishard = false;  // silently convert to soft callback
    }
    #endif

    // Compute period in timer clock ticks in 64-bit, then range-check.
    uint64_t period_ticks_64;
    uint32_t period_ticks;
    if (args[ARG_freq].u_obj != mp_const_none) {
        mp_int_t freq = mp_obj_get_int(args[ARG_freq].u_obj);
        if (freq <= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq must be > 0"));
        }
        period_ticks_64 = (uint64_t)TIMER_CLK_HZ / (uint64_t)(uint32_t)freq;
    } else if (args[ARG_period].u_int > 0) {
        // period is in milliseconds; convert to timer clock ticks (1 MHz base).
        period_ticks_64 = (uint64_t)TIMER_CLK_HZ * (uint64_t)(uint32_t)args[ARG_period].u_int
            / 1000UL;
    } else if (mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_period), MP_MAP_LOOKUP) != NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("period must be > 0"));
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("either freq or period must be specified"));
    }

    if (period_ticks_64 == 0U || period_ticks_64 > UINT32_MAX) {
        mp_raise_ValueError(MP_ERROR_TEXT("period out of range for 32-bit counter"));
    }

    // Group 1 counters (num >= 256) are 16-bit; reject values that would overflow their period register.
    if (self->counter_num >= 256U && period_ticks_64 > TIMER_GROUP1_PERIOD_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("16-bit Timer(%u) period exceeds %lu ticks; use a shorter period/higher freq, or use a 32-bit Timer (id 0-7)"),
            (unsigned)self->id,
            (unsigned long)TIMER_GROUP1_PERIOD_MAX);
    }

    period_ticks = (uint32_t)period_ticks_64;

    machine_timer_start(self, period_ticks);
    return mp_const_none;
}

// ---------------------------------------------------------------------------
// machine.Timer(id [, mode=, freq=, period=, callback=, hard=])
// ---------------------------------------------------------------------------

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t id = mp_obj_get_int(args[0]);
    if (id < 0 || id >= MACHINE_TIMER_NUM_INSTANCES) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Timer id must be in range 0-%d"),
            MACHINE_TIMER_NUM_INSTANCES - 1);
    }

    if (timer_obj[id] != NULL) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Timer(%u) already created."), (unsigned)id);
    }

    machine_timer_obj_t *self = mp_obj_malloc(machine_timer_obj_t, &machine_timer_type);
    timer_obj[id] = self;

    self->id = (uint8_t)id;
    self->counter_num = timer_hw[id];
    self->pclk_dst = machine_tcpwm_counter_pclk(self->counter_num);
    self->callback = mp_const_none;
    self->ishard = false;
    self->active = false;
    self->irq_cfg.irq_num = machine_timer_counter_irq(self->counter_num);

    nlr_buf_t nl;
    if (nlr_push(&nl) == 0) {
        machine_tcpwm_counter_alloc(self->counter_num, MP_OBJ_FROM_PTR(self));
        if (n_args > 1 || n_kw > 0) {
            mp_map_t kw_map;
            mp_map_init_fixed_table(&kw_map, n_kw, args + n_args);
            machine_timer_init_helper(self, n_args - 1, args + 1, &kw_map);
        }
        nlr_pop();
    } else {
        machine_tcpwm_counter_free(self->counter_num, MP_OBJ_FROM_PTR(self));
        timer_obj[id] = NULL;
        nlr_jump(nl.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

// ---------------------------------------------------------------------------
// timer.init(mode=, freq=, period=, callback=, hard=)
// ---------------------------------------------------------------------------

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args,
    mp_map_t *kw_args) {
    return machine_timer_init_helper(MP_OBJ_TO_PTR(args[0]),
        n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

// ---------------------------------------------------------------------------
// timer.deinit()
// ---------------------------------------------------------------------------

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    Cy_TCPWM_Counter_Disable(TCPWM0, self->counter_num);
    if (self->active) {
        sys_int_deinit(&self->irq_cfg);
    }
    self->callback = mp_const_none;
    self->ishard = false;
    self->active = false;
    machine_tcpwm_counter_free(self->counter_num, MP_OBJ_FROM_PTR(self));
    timer_obj[self->id] = NULL;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

// ---------------------------------------------------------------------------
// __repr__ / print
// ---------------------------------------------------------------------------

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in,
    mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t period_ticks = Cy_TCPWM_Counter_GetPeriod(TCPWM0, self->counter_num) + 1U;
    uint32_t freq = period_ticks != 0U ? (TIMER_CLK_HZ / period_ticks) : 0U;
    mp_printf(print, "Timer(id=%u, mode=Timer.%s, freq=%u)",
        (unsigned)self->id,
        self->mode == TIMER_MODE_ONE_SHOT ? "OneShot" : "Periodic",
        (unsigned)freq);
}

// ---------------------------------------------------------------------------
// Module-level lifecycle called from main.c
// ---------------------------------------------------------------------------

void machine_timer_deinit_all(void) {
    for (uint8_t i = 0; i < MACHINE_TIMER_NUM_INSTANCES; i++) {
        machine_timer_obj_t *self = timer_obj[i];
        if (self != NULL) {
            machine_timer_deinit(MP_OBJ_FROM_PTR(self));
        }
    }
}

// ---------------------------------------------------------------------------
// Type definition
// ---------------------------------------------------------------------------

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),     MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),   MP_ROM_PTR(&machine_timer_deinit_obj) },
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
