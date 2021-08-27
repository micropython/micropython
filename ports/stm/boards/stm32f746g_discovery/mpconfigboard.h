/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2020 Mark Olsson <mark@markolsson.se>
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

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ST STM32F746G Discovery"
#define MICROPY_HW_MCU_NAME         "STM32F746"
#define STM32F746G_DISCO

#define FLASH_SIZE                  (0x100000)
#define FLASH_PAGE_SIZE             (0x4000)

// H7 and F7 MPU definitions
#define CPY_FLASH_REGION_SIZE   ARM_MPU_REGION_SIZE_1MB
#define CPY_ITCM_REGION_SIZE    ARM_MPU_REGION_SIZE_16KB
#define CPY_DTCM_REGION_SIZE    ARM_MPU_REGION_SIZE_128KB
#define CPY_SRAM_REGION_SIZE    ARM_MPU_REGION_SIZE_256KB
#define CPY_SRAM_SUBMASK        0x00
#define CPY_SRAM_START_ADDR     0x20010000

// Lower frequency to allow external RAM use
#define HSE_VALUE ((uint32_t)25000000)
#define LSE_VALUE ((uint32_t)32768)
#define CPY_CLK_PLLN (400)
#define CPY_CLK_PLLQ (9)
#define CPY_CLK_FLASH_LATENCY (FLASH_LATENCY_6)
#define CPY_CLK_USB_USES_AUDIOPLL (1)

#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS) // ST boards use the STLink clock signal
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)

#define BOARD_NO_VBUS_SENSE 1
