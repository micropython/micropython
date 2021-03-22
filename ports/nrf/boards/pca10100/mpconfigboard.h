/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "PCA10100 nRF52833 Dongle"
#define MICROPY_HW_MCU_NAME         "nRF52833"

#define MICROPY_HW_LED_STATUS          (&pin_P0_13)
#define MICROPY_HW_LED_TX              (&pin_P0_14)
#define MICROPY_HW_LED_RX              (&pin_P0_15)

#define CIRCUITPY_INTERNAL_NVM_SIZE 0
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (60 * 1024)

#define CIRCUITPY_BLE_CONFIG_SIZE       (12 * 1024)

// Reduce nRF SoftRadio memory usage
#define BLEIO_VS_UUID_COUNT 10
#define BLEIO_HVN_TX_QUEUE_SIZE 2
#define BLEIO_CENTRAL_ROLE_COUNT 2
#define BLEIO_PERIPH_ROLE_COUNT 2
#define BLEIO_TOTAL_CONNECTION_COUNT 2
#define BLEIO_ATTR_TAB_SIZE (BLE_GATTS_ATTR_TAB_SIZE_DEFAULT * 2)

#define SOFTDEVICE_RAM_SIZE (32 * 1024)
