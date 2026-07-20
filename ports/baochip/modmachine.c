/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include "py/mphal.h"

#include "bao/platform.h"
#include "bao_reset.h"
#include "hardware/gpio.h"
#include "machine_pin.h"

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) }, \

static void mp_machine_idle(void) {
    __asm__ volatile ("wfi");
}

MP_NORETURN static void mp_machine_reset(void) {
    bao_software_reset();
    for (;;) {
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    // TODO: read the SCG (SoC Control / reset cause) register and map
    // to the standard mp_machine_reset_cause values (PWRON / HARD /
    // SOFT / WDT / DEEPSLEEP).
    return 0;
}

// Drive PC13 (PROG strap) LOW then reset so boot1 enters its CDC REPL.
// gpio_init tears down any prior alt-function so gpio_set_dir actually
// takes effect.  PC13 is dual-use: it is BOTH the boot strap AND the
// 0.85 V regulator feedback path on Dabao, so driving it must be brief
// (~1 ms) and only at the moment of reset.  Setting the data latch
// before flipping the direction prevents a high-Z glitch.
MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    gpio_init(pin_PC13->port, pin_PC13->pin);
    gpio_put(pin_PC13->port, pin_PC13->pin, false);
    gpio_set_dir(pin_PC13->port, pin_PC13->pin, true);
    mp_hal_delay_us(1000);
    bao_software_reset();
    for (;;) {
    }
}

static mp_obj_t mp_machine_unique_id(void) {
    // TODO: expose the per-chip serial via IFR or boot1 metadata.
    mp_raise_NotImplementedError(NULL);
}

static mp_obj_t mp_machine_get_freq(void) {
    return mp_obj_new_int_from_uint(ACLK_HZ);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    // TODO: implement clock tree reconfiguration via the SDK clk helpers.
    mp_raise_NotImplementedError(NULL);
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        // TODO: implement timeout via TickTimer wakeup.
        (void)args;
        mp_raise_NotImplementedError(MP_ERROR_TEXT("timeout"));
    }
    __asm__ volatile ("wfi");
}

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    // TODO: enter the SoC's deep-sleep state via the SDK pmu helpers.
    for (;;) {
        __asm__ volatile ("wfi");
    }
}
