/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/status_leds.h"

#if CIRCUITPY_DIGITALIO
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#endif

#ifdef MICROPY_HW_LED_RX
digitalio_digitalinout_obj_t rx_led;
#endif

#ifdef MICROPY_HW_LED_TX
digitalio_digitalinout_obj_t tx_led;
#endif

void init_status_leds(void) {
    #ifdef MICROPY_HW_LED_RX
    common_hal_digitalio_digitalinout_construct(&rx_led, MICROPY_HW_LED_RX);
    common_hal_digitalio_digitalinout_switch_to_output(&rx_led, true, DRIVE_MODE_PUSH_PULL);
    #endif
    #ifdef MICROPY_HW_LED_TX
    common_hal_digitalio_digitalinout_construct(&tx_led, MICROPY_HW_LED_TX);
    common_hal_digitalio_digitalinout_switch_to_output(&tx_led, true, DRIVE_MODE_PUSH_PULL);
    #endif
}

void toggle_rx_led(void) {
    #ifdef MICROPY_HW_LED_RX
    common_hal_digitalio_digitalinout_set_value(&rx_led, !common_hal_digitalio_digitalinout_get_value(&rx_led));
    #endif
}


void toggle_tx_led(void) {
    #ifdef MICROPY_HW_LED_TX
    common_hal_digitalio_digitalinout_set_value(&tx_led, !common_hal_digitalio_digitalinout_get_value(&tx_led));
    #endif
}
