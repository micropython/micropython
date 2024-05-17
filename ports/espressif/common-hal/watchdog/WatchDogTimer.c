// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "common-hal/watchdog/WatchDogTimer.h"
#include "bindings/espidf/__init__.h"

#include "esp_task_wdt.h"

extern void esp_task_wdt_isr_user_handler(void);

void esp_task_wdt_isr_user_handler(void) {
    // just delete, deiniting TWDT in isr context causes a crash
    if (esp_task_wdt_delete(NULL) == ESP_OK) {
        watchdog_watchdogtimer_obj_t *self = &common_hal_mcu_watchdogtimer_obj;
        self->mode = WATCHDOGMODE_NONE;
    }

    // schedule watchdog timeout exception
    mp_obj_exception_clear_traceback(MP_OBJ_FROM_PTR(&mp_watchdog_timeout_exception));
    MP_STATE_THREAD(mp_pending_exception) = &mp_watchdog_timeout_exception;

    #if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    esp_task_wdt_reset();
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_NONE) {
        return;
    }
    if (esp_task_wdt_delete(NULL) == ESP_OK && esp_task_wdt_deinit() == ESP_OK) {
        self->mode = WATCHDOGMODE_NONE;
    }
}

static void wdt_config(uint32_t timeout_ms, watchdog_watchdogmode_t mode) {
    // enable panic handler in WATCHDOGMODE_RESET mode
    // initialize Task Watchdog Timer (TWDT)
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = timeout_ms,
        .idle_core_mask = 0, // Don't monitor idle tasks just the ones we add.
        .trigger_panic = (mode == WATCHDOGMODE_RESET),
    };
    // Reconfigure if we are already running.
    if (esp_task_wdt_init(&twdt_config) != ESP_OK) {
        CHECK_ESP_RESULT(esp_task_wdt_reconfigure(&twdt_config));
    }
    CHECK_ESP_RESULT(esp_task_wdt_add(NULL));
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t new_timeout) {
    uint64_t new_timeout_ms = new_timeout * 1000;

    if (new_timeout_ms > UINT32_MAX) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be <= %u"), MP_QSTR_timeout, UINT32_MAX / 1000);
    }
    self->timeout = new_timeout;

    if (self->mode != WATCHDOGMODE_NONE) {
        wdt_config(new_timeout_ms, self->mode);
    }
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    if (self->mode == new_mode) {
        return;
    }

    switch (new_mode) {
        case WATCHDOGMODE_NONE:
            common_hal_watchdog_deinit(self);
            break;
        case WATCHDOGMODE_RAISE:
        case WATCHDOGMODE_RESET:
            wdt_config(self->timeout * 1000, new_mode);
            break;
        default:
            return;
    }

    self->mode = new_mode;
}
