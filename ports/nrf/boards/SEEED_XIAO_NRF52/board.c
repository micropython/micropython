/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Arduino SA
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

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_rtc.h"

#define PIN_ENABLE_SENSORS_3V3        (40)
#define DFU_MAGIC_SERIAL_ONLY_RESET   0x4e
#define DFU_MAGIC_UF2_RESET           0x57
#define DFU_MAGIC_OTA_RESET           0xA8

void XIAO_board_early_init(void) {
    // Errata XIAOBLE - I2C pullup is on SWO line, need to disable TRACE
    // was being enabled by nrfx_clock_anomaly_132
    CoreDebug->DEMCR = 0;
    NRF_CLOCK->TRACECONFIG = 0;

    // Bootloader enables interrupt on COMPARE[0], which we don't handle
    // Disable it here to avoid getting stuck when OVERFLOW irq is triggered
    nrf_rtc_event_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);
    nrf_rtc_int_disable(NRF_RTC1, NRF_RTC_INT_COMPARE0_MASK);

    // Always enable IMU power and I2C Pull-up on startup
    nrf_gpio_cfg(PIN_ENABLE_SENSORS_3V3,
                 NRF_GPIO_PIN_DIR_OUTPUT,
                 NRF_GPIO_PIN_INPUT_DISCONNECT,
                 NRF_GPIO_PIN_NOPULL,
                 GPIO_PIN_CNF_DRIVE_S0H1,
                 NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_pin_set(PIN_ENABLE_SENSORS_3V3);
}

void XIAO_board_deinit(void) {
    nrf_gpio_cfg_output(PIN_ENABLE_SENSORS_3V3);

    nrf_gpio_pin_clear(PIN_ENABLE_SENSORS_3V3);
}

void XIAO_board_enter_bootloader(void) {
    __disable_irq();
    NRF_POWER->GPREGRET = DFU_MAGIC_UF2_RESET;
    NVIC_SystemReset();
}
