// machine_wdt_port.c - machine.WDT for GD32F4xx
// Included by extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "gd32f4xx.h"

struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    uint32_t timeout_ms;
};

// FWDGT runs on IRC40K (40 kHz), divided by prescaler.
// clock = 40000 / 2^prescaler ; counter counts down from reload value.
static const uint8_t wdt_prescaler_table[] = {4, 5, 6, 7, 8, 9, 10, 11};

static machine_wdt_obj_t machine_wdt_obj = {
    .base = { &machine_wdt_type },
    .timeout_ms = 0,
};

static uint32_t wdt_find_config(uint32_t timeout_ms, uint8_t *prescaler_out) {
    // Find the largest counter value that fits in 12 bits (max 4095)
    // while still achieving the requested timeout.
    for (int i = MP_ARRAY_SIZE(wdt_prescaler_table) - 1; i >= 0; i--) {
        uint8_t ps = wdt_prescaler_table[i];
        uint32_t tick_hz = 40000u >> ps;
        // ticks for timeout_ms
        uint32_t ticks = (uint64_t)timeout_ms * tick_hz / 1000u;
        if (ticks > 0 && ticks <= 4095) {
            *prescaler_out = ps;
            return ticks;
        }
    }
    // Fall back to the largest possible timeout
    *prescaler_out = 11;
    return 4095;
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_obj_t id, mp_int_t timeout_ms) {
    uint8_t prescaler;
    uint32_t reload = wdt_find_config((uint32_t)timeout_ms, &prescaler);

    if (reload == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid timeout"));
    }

    // Unlock FWDGT config registers (needs the magic keys)
    fwdgt_write_enable();
    fwdgt_config((uint16_t)reload, prescaler);
    fwdgt_counter_reload();
    fwdgt_enable();

    machine_wdt_obj.timeout_ms = (uint32_t)timeout_ms;
    return &machine_wdt_obj;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    fwdgt_counter_reload();
}

static void mp_machine_wdt_timeout_ms_set(machine_wdt_obj_t *self_in, mp_int_t timeout_ms) {
    uint8_t prescaler;
    uint32_t reload = wdt_find_config((uint32_t)timeout_ms, &prescaler);

    if (reload == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid timeout"));
    }

    fwdgt_write_enable();
    fwdgt_config((uint16_t)reload, prescaler);
    fwdgt_counter_reload();

    self_in->timeout_ms = (uint32_t)timeout_ms;
}

