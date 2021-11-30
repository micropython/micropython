/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/board.h"
#include "nrf.h"
#include "nrf_rtc.h"

void board_init(void) {
    // Initializations below from Arduino variant.cpp.

    // // turn power LED on
    // pinMode(LED_PWR, OUTPUT);
    // digitalWrite(LED_PWR, HIGH);

    // Errata Nano33BLE - I2C pullup is on SWO line, need to disable TRACE
    // was being enabled by nrfx_clock_anomaly_132
    CoreDebug->DEMCR = 0;
    NRF_CLOCK->TRACECONFIG = 0;

    // FIXME: bootloader enables interrupt on COMPARE[0], which we don't handle
    // Disable it here to avoid getting stuck when OVERFLOW irq is triggered
    nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
    nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);

    // // FIXME: always enable I2C pullup and power @startup
    // // Change for maximum powersave
    // pinMode(PIN_ENABLE_SENSORS_3V3, OUTPUT);
    // pinMode(PIN_ENABLE_I2C_PULLUP, OUTPUT);

    // digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH);
    // digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {

}

void board_deinit(void) {
}
