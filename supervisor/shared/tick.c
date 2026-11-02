// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/tick.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/background_callback.h"
#include "supervisor/port.h"
#include "supervisor/shared/stack.h"

#if CIRCUITPY_BLEIO_HCI
#include "common-hal/_bleio/__init__.h"
#endif

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_KEYPAD
#include "shared-module/keypad/__init__.h"
#endif

#if CIRCUITPY_AUDIOFILEWRITER
#include "shared-module/audiofilewriter/AudioFileWriter.h"
#endif

#if CIRCUITPY_AIRLIFT && CIRCUITPY_SOCKETPOOL
#include "shared-module/_airlift/socketpool.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/display_stream.h"
#endif

#include "shared-bindings/microcontroller/__init__.h"

#if CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/__init__.h"
#define WATCHDOG_EXCEPTION_CHECK() (MP_STATE_VM(mp_pending_exception) == &mp_watchdog_timeout_exception)
#else
#define WATCHDOG_EXCEPTION_CHECK() 0
#endif

static volatile uint64_t PLACE_IN_DTCM_BSS(background_ticks);

static background_callback_t tick_callback;

static volatile uint64_t last_finished_tick = 0;

static volatile size_t tick_enable_count = 0;

static void supervisor_background_tick(void *unused) {
    port_start_background_tick();

    assert_heap_ok();

    #if CIRCUITPY_BLEIO_HCI
    bleio_hci_background();
    #endif

    #if CIRCUITPY_DISPLAYIO
    displayio_background();
    #endif

    filesystem_background();

    #if CIRCUITPY_AUDIOFILEWRITER
    audiofilewriter_background();
    #endif

    port_background_tick();

    assert_heap_ok();

    last_finished_tick = port_get_raw_ticks(NULL);

    port_finish_background_tick();
}

bool supervisor_background_ticks_ok(void) {
    return port_get_raw_ticks(NULL) - last_finished_tick < 1024;
}

void supervisor_tick(void) {
    #if CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS > 0
    filesystem_tick();
    #endif


    #if CIRCUITPY_KEYPAD
    keypad_tick();
    #endif

    #if CIRCUITPY_AIRLIFT && CIRCUITPY_SOCKETPOOL
    airlift_socketpool_tick();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    display_stream_tick();
    #endif

    background_callback_add(&tick_callback, supervisor_background_tick, NULL);
}

static uint64_t _get_raw_subticks(void) {
    uint64_t ticks;
    uint8_t subticks;
    ticks = port_get_raw_ticks(&subticks);
    return (ticks << 5) | subticks;
}

uint64_t supervisor_ticks_ms64(void) {
    uint64_t result;
    result = port_get_raw_ticks(NULL);
    result = result * 1000 / 1024;
    return result;
}

uint32_t supervisor_ticks_ms32(void) {
    return supervisor_ticks_ms64();
}

void mp_hal_delay_ms(mp_uint_t delay_ms) {
    uint64_t start_subtick = _get_raw_subticks();
    // Convert delay from ms to subticks
    uint64_t delay_subticks = (delay_ms * (uint64_t)32768) / 1000;
    uint64_t end_subtick = start_subtick + delay_subticks;
    int64_t remaining = delay_subticks;

    // Loop until we've waited long enough or we've been CTRL-Ced by autoreload
    // or the user.
    while (remaining > 0 && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Exit if interrupted while running background tasks
        if (mp_hal_is_interrupted()) {
            break;
        }
        // Recalculate remaining delay after running background tasks
        remaining = end_subtick - _get_raw_subticks();
        // If remaining delay is less than 1 tick, idle loop until end of delay
        int64_t remaining_ticks = remaining / 32;
        if (remaining_ticks > 0) {
            port_interrupt_after_ticks(remaining_ticks);
            // Idle until an interrupt happens.
            port_idle_until_interrupt();
        }
        remaining = end_subtick - _get_raw_subticks();
    }
}

void supervisor_enable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count == 0) {
        port_enable_tick();
    }
    tick_enable_count++;
    common_hal_mcu_enable_interrupts();
}

void supervisor_disable_tick(void) {
    common_hal_mcu_disable_interrupts();
    if (tick_enable_count > 0) {
        tick_enable_count--;
    }
    if (tick_enable_count == 0) {
        port_disable_tick();
    }
    common_hal_mcu_enable_interrupts();
}
