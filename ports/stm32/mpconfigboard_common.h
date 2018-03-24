/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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

// Common settings and defaults for board configuration.
// The defaults here should be overridden in mpconfigboard.h.

#include STM32_HAL_H

/*****************************************************************************/
// Feature settings with defaults

// Whether to enable storage on the internal flash of the MCU
#ifndef MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#endif

// Whether to enable the RTC, exposed as pyb.RTC
#ifndef MICROPY_HW_ENABLE_RTC
#define MICROPY_HW_ENABLE_RTC (0)
#endif

// Whether to enable the hardware RNG peripheral, exposed as pyb.rng()
#ifndef MICROPY_HW_ENABLE_RNG
#define MICROPY_HW_ENABLE_RNG (0)
#endif

// Whether to enable the ADC peripheral, exposed as pyb.ADC and pyb.ADCAll
#ifndef MICROPY_HW_ENABLE_ADC
#define MICROPY_HW_ENABLE_ADC (1)
#endif

// Whether to enable the DAC peripheral, exposed as pyb.DAC
#ifndef MICROPY_HW_ENABLE_DAC
#define MICROPY_HW_ENABLE_DAC (0)
#endif

// Whether to enable the CAN peripheral, exposed as pyb.CAN
#ifndef MICROPY_HW_ENABLE_CAN
#define MICROPY_HW_ENABLE_CAN (0)
#endif

// Whether to enable USB support
#ifndef MICROPY_HW_ENABLE_USB
#define MICROPY_HW_ENABLE_USB (0)
#endif

// Whether to enable the PA0-PA3 servo driver, exposed as pyb.Servo
#ifndef MICROPY_HW_ENABLE_SERVO
#define MICROPY_HW_ENABLE_SERVO (0)
#endif

// Whether to enable a USR switch, exposed as pyb.Switch
#ifndef MICROPY_HW_HAS_SWITCH
#define MICROPY_HW_HAS_SWITCH (0)
#endif

// Whether to expose internal flash storage as pyb.Flash
#ifndef MICROPY_HW_HAS_FLASH
#define MICROPY_HW_HAS_FLASH (0)
#endif

// Whether to enable the SD card interface, exposed as pyb.SDCard
#ifndef MICROPY_HW_HAS_SDCARD
#define MICROPY_HW_HAS_SDCARD (0)
#endif

// Whether to enable the MMA7660 driver, exposed as pyb.Accel
#ifndef MICROPY_HW_HAS_MMA7660
#define MICROPY_HW_HAS_MMA7660 (0)
#endif

// Whether to enable the LCD32MK driver, exposed as pyb.LCD
#ifndef MICROPY_HW_HAS_LCD
#define MICROPY_HW_HAS_LCD (0)
#endif

/*****************************************************************************/
// General configuration

// Configuration for STM32F4 series
#if defined(STM32F4)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7a10)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_TIMER (14)
#define MICROPY_HW_MAX_UART (6)

// Configuration for STM32F7 series
#elif defined(STM32F7)

#if defined(STM32F722xx) || defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff07a10)
#else
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff0f420)
#endif

#define PYB_EXTI_NUM_VECTORS (24)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (8)

// Configuration for STM32H7 series
#elif defined(STM32H7)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff1e800)
#define PYB_EXTI_NUM_VECTORS (24)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (8)

// Configuration for STM32L4 series
#elif defined(STM32L4)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7590)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (6)

#else
#error Unsupported MCU series
#endif

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
// Provide block device macros if internal flash storage is enabled
#define MICROPY_HW_BDEV_IOCTL flash_bdev_ioctl
#define MICROPY_HW_BDEV_READBLOCK flash_bdev_readblock
#define MICROPY_HW_BDEV_WRITEBLOCK flash_bdev_writeblock
#endif

// Enable hardware I2C if there are any peripherals defined
#if defined(MICROPY_HW_I2C1_SCL) || defined(MICROPY_HW_I2C2_SCL) \
    || defined(MICROPY_HW_I2C3_SCL) || defined(MICROPY_HW_I2C4_SCL)
#define MICROPY_HW_ENABLE_HW_I2C (1)
#else
#define MICROPY_HW_ENABLE_HW_I2C (0)
#endif

// Pin definition header file
#define MICROPY_PIN_DEFS_PORT_H "pin_defs_stm32.h"

// D-cache clean/invalidate helpers
#if __DCACHE_PRESENT == 1
#define MP_HAL_CLEANINVALIDATE_DCACHE(addr, size) \
    (SCB_CleanInvalidateDCache_by_Addr((uint32_t*)((uint32_t)addr & ~0x1f), \
        ((uint32_t)((uint8_t*)addr + size + 0x1f) & ~0x1f) - ((uint32_t)addr & ~0x1f)))
#define MP_HAL_CLEAN_DCACHE(addr, size) \
    (SCB_CleanDCache_by_Addr((uint32_t*)((uint32_t)addr & ~0x1f), \
        ((uint32_t)((uint8_t*)addr + size + 0x1f) & ~0x1f) - ((uint32_t)addr & ~0x1f)))
#else
#define MP_HAL_CLEANINVALIDATE_DCACHE(addr, size)
#define MP_HAL_CLEAN_DCACHE(addr, size)
#endif
