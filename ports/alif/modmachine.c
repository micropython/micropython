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

#include "lptimer_ext.h"
#include "modmachine.h"
#include "se_services.h"
#include "tusb.h"

// Use 4 bytes of backup SRAM to store the reset cause state.
// Note that this will be reset to a random value upon power on.
#define RESET_CAUSE_STATE_PTR ((uint32_t *)MP_BACKUP_STATE_BASE)
#define RESET_CAUSE_VALID_MASK (0xfffffff0)
#define RESET_CAUSE_VALID (0xa170)

extern void dcd_uninit(void);

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(MACHINE_RESET_PWRON) }, \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(MACHINE_RESET_HARD) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(MACHINE_RESET_WDT) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(MACHINE_RESET_DEEPSLEEP) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(MACHINE_RESET_SOFT) }, \

enum {
    MACHINE_RESET_PWRON = 1,
    MACHINE_RESET_HARD,
    MACHINE_RESET_WDT,
    MACHINE_RESET_DEEPSLEEP,
    MACHINE_RESET_SOFT,
};

static uint8_t reset_cause;

void machine_init(void) {
    // Get the current reset cause.
    if ((*RESET_CAUSE_STATE_PTR & RESET_CAUSE_VALID_MASK) == RESET_CAUSE_VALID) {
        reset_cause = *RESET_CAUSE_STATE_PTR & 0xf;
    } else {
        reset_cause = MACHINE_RESET_PWRON;
    }

    // Set the reset cause state in case there is a spontaneous hard reset.
    *RESET_CAUSE_STATE_PTR = RESET_CAUSE_VALID | MACHINE_RESET_HARD;
}

void machine_set_wdt_reset(void) {
    *RESET_CAUSE_STATE_PTR = RESET_CAUSE_VALID | MACHINE_RESET_WDT;
}

void machine_set_soft_reset(void) {
    reset_cause = MACHINE_RESET_SOFT;
}

static void mp_machine_idle(void) {
    mp_event_wait_indefinite();
}

static mp_obj_t mp_machine_unique_id(void) {
    uint8_t id[8] = {0};
    se_services_get_unique_id(id);
    return mp_obj_new_bytes(id, sizeof(id));
}

MP_NORETURN static void mp_machine_reset(void) {
    *RESET_CAUSE_STATE_PTR = RESET_CAUSE_VALID | MACHINE_RESET_HARD;
    se_services_reset_soc();
}

static mp_int_t mp_machine_reset_cause(void) {
    return reset_cause;
}

MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    __disable_irq();

    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);

    while (1) {
        ;
    }
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

static void mp_machine_config_wakeup(mp_int_t sleep_ms, bool enable) {
    if (sleep_ms >= 0) {
        // RTC has a resolution of 1 second so use LPTIMER for small sleep duration.
        if (sleep_ms < 10000) {
            if (enable) {
                lptimer_set_wakeup(sleep_ms * 1000);
            } else {
                lptimer_cancel_wakeup();
            }
        } else {
            if (enable) {
                machine_rtc_set_wakeup(sleep_ms / 1000);
            } else {
                machine_rtc_cancel_wakeup();
            }
        }
    }
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    mp_int_t sleep_ms = -1;
    if (n_args != 0) {
        sleep_ms = mp_obj_get_int(args[0]);
    }

    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(false);
    #endif

    #ifdef MICROPY_BOARD_ENTER_STANDBY
    MICROPY_BOARD_ENTER_STANDBY();
    #endif

    __disable_irq();

    mp_machine_config_wakeup(sleep_ms, true);

    // This enters the deepest possible CPU sleep state, without
    // losing CPU state. CPU and subsystem power will remain on.
    pm_core_enter_deep_sleep();

    mp_machine_config_wakeup(sleep_ms, false);

    __enable_irq();

    #ifdef MICROPY_BOARD_EXIT_STANDBY
    MICROPY_BOARD_EXIT_STANDBY();
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(true);
    #endif
}

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_int_t sleep_ms = -1;
    if (n_args != 0) {
        sleep_ms = mp_obj_get_int(args[0]);
    }

    #if MICROPY_HW_ENABLE_USBDEV
    mp_machine_enable_usb(false);
    #endif

    #ifdef MICROPY_BOARD_ENTER_STOP
    MICROPY_BOARD_ENTER_STOP();
    #endif

    __disable_irq();

    mp_machine_config_wakeup(sleep_ms, true);

    // Set the reset cause for when the device wakes up.
    *RESET_CAUSE_STATE_PTR = RESET_CAUSE_VALID | MACHINE_RESET_DEEPSLEEP;

    // If power is removed from the subsystem, the function does
    // not return, and the CPU will reboot when/if the subsystem
    // is next powered up.
    pm_core_enter_deep_sleep_request_subsys_off();
    mp_machine_reset();
}
