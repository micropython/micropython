// machine_wdt_port.c - machine.WDT for GD32F4xx
// Included by extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "gd32f4xx.h"

struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    uint32_t timeout_ms;
};

// FWDGT counts down at IRC32K divided by (4 << PSC), where PSC is a 3-bit field
// whose values 0..6 mean DIV4..DIV256 (FWDGT_PSC_DIV4 .. FWDGT_PSC_DIV256).
//
// The clock was documented here as 40 kHz; the datasheet (GD32F407xx Rev3.0,
// Table 4-20) names it IRC32K with a 32 kHz typical value and no min/max
// figures at all, so treat the timeout as approximate - this RC oscillator is
// characterised, not trimmed.  Measured on a real board the effective rate is
// closer to 28 kHz, i.e. roughly 10-15 % longer than asked for.
//
// This table used to hold shift amounts {4,5,6,7,8,9,10,11}.  Those are not the
// register encoding: the 3-bit field truncates 11 to 3, i.e. DIV32, so a
// requested 8000 ms watchdog actually expired in a few tens of milliseconds -
// the board rebooted almost immediately after `WDT(timeout=8000)` and looked
// dead.  Keep these as register values.
#define WDT_IRC32K_HZ       32000u   // datasheet typ; no min/max given
#define WDT_MAX_RELOAD      4095u
#define WDT_PSC_MAX         6u          // FWDGT_PSC_DIV256

static uint32_t wdt_find_config(uint32_t timeout_ms, uint8_t *prescaler_out) {
    // Walk from the finest divider upwards and take the first one that fits the
    // 12-bit reload register, so short timeouts keep good resolution.
    for (uint8_t psc = 0; psc <= WDT_PSC_MAX; psc++) {
        uint32_t tick_hz = WDT_IRC32K_HZ >> (2 + psc);      // 32000 / (4 << psc)
        uint32_t ticks = (uint64_t)timeout_ms * tick_hz / 1000u;
        if (ticks > 0 && ticks <= WDT_MAX_RELOAD) {
            *prescaler_out = psc;
            return ticks;
        }
    }
    // Longer than ~33 s: use the slowest clock and the largest reload.
    *prescaler_out = WDT_PSC_MAX;
    return WDT_MAX_RELOAD;
}

static machine_wdt_obj_t machine_wdt_obj = {
    .base = { &machine_wdt_type },
    .timeout_ms = 0,
};

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

