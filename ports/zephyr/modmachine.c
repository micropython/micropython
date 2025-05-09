/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2016 Linaro Limited
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

#include <stdio.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/poweroff.h>

#include "modmachine.h"

#ifdef CONFIG_REBOOT
#define MICROPY_PY_MACHINE_RESET_ENTRY { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
#else
#define MICROPY_PY_MACHINE_RESET_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    MICROPY_PY_MACHINE_RESET_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) }, \

static mp_obj_t machine_reset(void) {
    sys_reboot(SYS_REBOOT_COLD);
    // Won't get here, Zephyr has infiniloop on its side
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

static mp_obj_t machine_reset_cause(void) {
    printf("Warning: %s is not implemented\n", __func__);
    return MP_OBJ_NEW_SMALL_INT(42);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

static void mp_machine_idle(void) {
    k_yield();
}

#ifdef CONFIG_PM_DEVICE
static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    // Check if no argument is provided
    if (n_args == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("value must be provided"));
    }
    mp_int_t milliseconds = mp_obj_get_int(args[0]);
    // Get the UART device
    const struct device *console = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    // Set UART device to low power state
    pm_device_action_run(console, PM_DEVICE_ACTION_SUSPEND);
    k_sleep(K_MSEC(milliseconds));
    // Set UART device back to active state
    pm_device_action_run(console, PM_DEVICE_ACTION_RESUME);
}
#else
static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    mp_raise_ValueError(MP_ERROR_TEXT("not implemented"));
}
#endif

#if defined(CONFIG_PM_DEVICE) && defined(CONFIG_SYS_POWER_OFF)
static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    // Check if argument is provided
    if (n_args > 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("timeout not supported"));
    }
    // Get the UART device
    const struct device *console = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    // Set UART device to low power state
    pm_device_action_run(console, PM_DEVICE_ACTION_SUSPEND);
    sys_poweroff();
}
#else
static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_raise_ValueError(MP_ERROR_TEXT("not implemented"));
}
#endif

static mp_obj_t mp_machine_get_freq(void) {
    mp_raise_ValueError(MP_ERROR_TEXT("not implemented"));
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_ValueError(MP_ERROR_TEXT("not implemented"));
}

static mp_obj_t mp_machine_unique_id(void) {
    mp_raise_ValueError(MP_ERROR_TEXT("not implemented"));
}
