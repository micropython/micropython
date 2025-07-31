/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "se_services.h"
#include "tusb.h"

extern void dcd_uninit(void);

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \

static void mp_machine_idle(void) {
    mp_event_wait_indefinite();
}

static mp_obj_t mp_machine_unique_id(void) {
    uint8_t id[8] = {0};
    se_services_get_unique_id(id);
    return mp_obj_new_bytes(id, sizeof(id));
}

NORETURN static void mp_machine_reset(void) {
    se_services_reset_soc();
}

NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    __disable_irq();

    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);

    while (1) {
        ;
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    // TODO
    return 0;
}

static mp_obj_t mp_machine_get_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(SystemCoreClock);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(NULL);
}

#if MICROPY_HW_ENABLE_USBDEV
static void mp_machine_enable_usb(bool enable) {
    if (enable) {
        // Initialize TinyUSB and DCD.
        tusb_init();
    } else {
        // Disconnect USB device.
        tud_disconnect();
        // Deinitialize TinyUSB.
        tud_deinit(TUD_OPT_RHPORT);
        // Deinitialize DCD (disables IRQs).
        dcd_uninit();
    }
}
#endif

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(false);
    #endif

    #ifdef MICROPY_BOARD_ENTER_STANDBY
    MICROPY_BOARD_ENTER_STANDBY();
    #endif

    // This enters the deepest possible CPU sleep state, without
    // losing CPU state. CPU and subsystem power will remain on.
    pm_core_enter_deep_sleep();

    #ifdef MICROPY_BOARD_EXIT_STANDBY
    MICROPY_BOARD_EXIT_STANDBY();
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(true);
    #endif
}

NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(false);
    #endif

    #ifdef MICROPY_BOARD_ENTER_STOP
    MICROPY_BOARD_ENTER_STOP();
    #endif

    // If power is removed from the subsystem, the function does
    // not return, and the CPU will reboot when/if the subsystem
    // is next powered up.
    pm_core_enter_deep_sleep_request_subsys_off();
    mp_machine_reset();
}
