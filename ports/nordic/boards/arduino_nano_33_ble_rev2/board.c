// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
