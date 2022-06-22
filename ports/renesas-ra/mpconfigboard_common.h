/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include RA_HAL_H

/*****************************************************************************/
// Feature settings with defaults

// Whether to include the ra module, with peripheral register constants
#ifndef MICROPY_PY_RA
#define MICROPY_PY_RA (1)
#endif

// Whether to include the pyb module
#ifndef MICROPY_PY_PYB
#define MICROPY_PY_PYB (1)
#endif

// Whether to include legacy functions and classes in the pyb module
#ifndef MICROPY_PY_PYB_LEGACY
#define MICROPY_PY_PYB_LEGACY (1)
#endif

// Whether machine.bootloader() will enter the bootloader via reset, or direct jump.
#ifndef MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET (1)
#endif

// Whether to enable storage on the internal flash of the MCU
#ifndef MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#endif

// If internal flash storage is enabled, whether to use a second segment of flash.
#ifndef MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE_SEGMENT2 (0)
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

// Whether to enable the DCMI peripheral
#ifndef MICROPY_HW_ENABLE_DCMI
#define MICROPY_HW_ENABLE_DCMI (0)
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

// Whether to enable the MMA7660 driver, exposed as pyb.Accel
#ifndef MICROPY_HW_HAS_MMA7660
#define MICROPY_HW_HAS_MMA7660 (0)
#endif

// Whether to enable the LCD32MK driver, exposed as pyb.LCD
#ifndef MICROPY_HW_HAS_LCD
#define MICROPY_HW_HAS_LCD (0)
#endif

// Whether to automatically mount (and boot from) the flash filesystem
#ifndef MICROPY_HW_FLASH_MOUNT_AT_BOOT
#define MICROPY_HW_FLASH_MOUNT_AT_BOOT (MICROPY_HW_ENABLE_STORAGE)
#endif

// The volume label used when creating the flash filesystem
#ifndef MICROPY_HW_FLASH_FS_LABEL
#define MICROPY_HW_FLASH_FS_LABEL "raflash"
#endif

// Function to determine if the given i2c_id is reserved for system use or not.
#ifndef MICROPY_HW_I2C_IS_RESERVED
#define MICROPY_HW_I2C_IS_RESERVED(i2c_id) (false)
#endif

// Function to determine if the given spi_id is reserved for system use or not.
#ifndef MICROPY_HW_SPI_IS_RESERVED
#define MICROPY_HW_SPI_IS_RESERVED(spi_id) (false)
#endif

// Function to determine if the given tim_id is reserved for system use or not.
#ifndef MICROPY_HW_TIM_IS_RESERVED
#define MICROPY_HW_TIM_IS_RESERVED(tim_id) (false)
#endif

// Function to determine if the given uart_id is reserved for system use or not.
#ifndef MICROPY_HW_UART_IS_RESERVED
#define MICROPY_HW_UART_IS_RESERVED(uart_id) (false)
#endif

/*****************************************************************************/
// General configuration

// Heap start / end definitions
#ifndef MICROPY_HEAP_START
#define MICROPY_HEAP_START &_heap_start
#endif
#ifndef MICROPY_HEAP_END
#define MICROPY_HEAP_END &_heap_end
#endif

// Configuration for RA series
#if defined(RA4M1)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#elif defined(RA4M3)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)   /* To be fixed */
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#elif defined(RA4W1)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)   /* To be fixed */
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#elif defined(RA6M1)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x0100A150)   /* To be fixed */
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#elif defined(RA6M2)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)   /* To be fixed */
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#elif defined(RA6M3)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)   /* To be fixed */
// 16 IRQ + 1 EXTI_RTC_WAKEUP defined in exti.h
#define PYB_EXTI_NUM_VECTORS (17)
#define MICROPY_HW_MAX_TIMER (2)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (0)

#error Unsupported MCU series
#endif

// If disabled then try normal (non-bypass) LSE first, with fallback to LSI.
// If enabled first try LSE in bypass mode.  If that fails to start, try non-bypass mode, with fallback to LSI.
#ifndef MICROPY_HW_RTC_USE_BYPASS
#define MICROPY_HW_RTC_USE_BYPASS (0)
#endif

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
// Provide block device macros if internal flash storage is enabled
#define MICROPY_HW_BDEV_IOCTL flash_bdev_ioctl
#define MICROPY_HW_BDEV_READBLOCK flash_bdev_readblock
#define MICROPY_HW_BDEV_WRITEBLOCK flash_bdev_writeblock
#endif

// Enable the storage sub-system if a block device is defined
#if defined(MICROPY_HW_BDEV_IOCTL)
#define MICROPY_HW_ENABLE_STORAGE (1)
#else
#define MICROPY_HW_ENABLE_STORAGE (0)
#endif

// Enable hardware I2C if there are any peripherals defined
#if defined(MICROPY_HW_I2C0_SCL) || defined(MICROPY_HW_I2C1_SCL) \
    || defined(MICROPY_HW_I2C2_SCL)
#define MICROPY_HW_ENABLE_HW_I2C (1)
#else
#define MICROPY_HW_ENABLE_HW_I2C (0)
#endif

// Pin definition header file
#define MICROPY_PIN_DEFS_PORT_H "pin_defs_ra.h"
