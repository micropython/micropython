// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "py/gc.h"
#include "peripherals/touch.h"

static bool touch_inited = false;
static bool touch_never_reset = false;

void peripherals_touch_reset(void) {
    if (touch_inited && !touch_never_reset) {
        touch_pad_deinit();
        touch_inited = false;
    }
}

void peripherals_touch_never_reset(const bool enable) {
    touch_never_reset = enable;
}

void peripherals_touch_init(const touch_pad_t touchpad) {
    if (!touch_inited) {
        touch_pad_init();
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    }
    // touch_pad_config() must be done before touch_pad_fsm_start() the first time.
    // Otherwise the calibration is wrong and we get maximum raw values if there is
    // a trace of any significant length on the pin.
    #if defined(CONFIG_IDF_TARGET_ESP32)
    touch_pad_config(touchpad, 0);
    #else
    touch_pad_config(touchpad);
    touch_pad_fsm_start();
    #endif
    touch_inited = true;
}

uint16_t peripherals_touch_read(touch_pad_t touchpad) {
    #if defined(CONFIG_IDF_TARGET_ESP32)
    uint16_t touch_value;
    touch_pad_read(touchpad, &touch_value);

    // ESP32 touch_pad_read() returns a lower value when a pin is touched instead of a higher value.
    // Flip the values around to be consistent with TouchIn assumptions.
    return UINT16_MAX - touch_value;
    #else
    uint32_t touch_value;
    touch_pad_read_raw_data(touchpad, &touch_value);
    if (touch_value > UINT16_MAX) {
        return UINT16_MAX;
    }
    return (uint16_t)touch_value;
    #endif
}
