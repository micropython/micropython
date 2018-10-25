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

#define MAKERDIARYNRF52840MDK

#define MICROPY_HW_BOARD_NAME       "MakerDiary nRF52840 MDK"
#define MICROPY_HW_MCU_NAME         "nRF52840"
#define MICROPY_PY_SYS_PLATFORM     "MakerDiary52840MDK"

#define MICROPY_QSPI_DATA0          (&pin_P1_05)
#define MICROPY_QSPI_DATA1          (&pin_P1_04)
#define MICROPY_QSPI_DATA2          (&pin_P1_02)
#define MICROPY_QSPI_DATA3          (&pin_P1_01)
#define MICROPY_QSPI_SCK            (&pin_P1_03)
#define MICROPY_QSPI_CS             (&pin_P1_06)

#define CIRCUITPY_AUTORELOAD_DELAY_MS 500

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define PORT_HEAP_SIZE              (128 * 1024)
// TODO #define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

// TODO #include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 1
// Datasheet for when this is implemented:
// http://www.mxic.com.tw/Lists/Datasheet/Attachments/7428/MX25R6435F,%20Wide%20Range,%2064Mb,%20v1.4.pdf
#define EXTERNAL_FLASH_DEVICES MX25R6435F

#define EXTERNAL_FLASH_QSPI_DUAL

// TODO include "external_flash/external_flash.h"

#define BOARD_HAS_CRYSTAL 0

#define DEFAULT_UART_BUS_RX         (&pin_P0_19)
#define DEFAULT_UART_BUS_TX         (&pin_P0_20)
