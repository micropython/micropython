/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#define MICROPY_HW_BOARD_NAME       "NUCLEO STM32F767"
#define MICROPY_HW_MCU_NAME         "STM32F767"

#define FLASH_SIZE                  (0x200000)
#define FLASH_PAGE_SIZE             (0x4000)

// H7 and F7 MPU definitions
#define CPY_FLASH_REGION_SIZE   ARM_MPU_REGION_SIZE_2MB
#define CPY_ITCM_REGION_SIZE    ARM_MPU_REGION_SIZE_16KB
#define CPY_DTCM_REGION_SIZE    ARM_MPU_REGION_SIZE_128KB
#define CPY_SRAM_REGION_SIZE    ARM_MPU_REGION_SIZE_512KB
#define CPY_SRAM_SUBMASK        0xFC // Mask 512 to 384
#define CPY_SRAM_START_ADDR     0x20020000

#define HSE_VALUE ((uint32_t)8000000)
#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS) // ST boards use the STLink clock signal
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
