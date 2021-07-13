/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

// Definitions for which SAMD chip we're using.
#include "include/sam.h"

// Definitions that control circuitpy_mpconfig.h:

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SAMD21

// HMCRAMC0_SIZE is defined in the ASF4 include files for each SAMD21 chip.
#define RAM_SIZE                                    HMCRAMC0_SIZE
#define BOOTLOADER_SIZE                             (8 * 1024)
#define CIRCUITPY_MCU_FAMILY                        samd21
#define MICROPY_PY_SYS_PLATFORM                     "Atmel SAMD21"
#define SPI_FLASH_MAX_BAUDRATE 8000000
#define MICROPY_PY_BUILTINS_COMPLEX                 (0)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (0)
#define MICROPY_PY_FUNCTION_ATTRS                   (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (0)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT          (0)
#define MICROPY_PY_UERRNO_LIST \
    X(EPERM) \
    X(ENOENT) \
    X(EIO) \
    X(EAGAIN) \
    X(ENOMEM) \
    X(EACCES) \
    X(EEXIST) \
    X(ENODEV) \
    X(EISDIR) \
    X(EINVAL) \

#endif // SAMD21

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SAM_D5X_E5X

// HSRAM_SIZE is defined in the ASF4 include files for each SAM_D5X_E5X chip.
#define RAM_SIZE                                    HSRAM_SIZE
#define BOOTLOADER_SIZE                             (16 * 1024)
#define CIRCUITPY_MCU_FAMILY                        samd51
#ifdef SAMD51
#define MICROPY_PY_SYS_PLATFORM                     "MicroChip SAMD51"
#elif defined(SAME54)
#define MICROPY_PY_SYS_PLATFORM                     "MicroChip SAME54"
#endif
#define SPI_FLASH_MAX_BAUDRATE 24000000
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (1)
#define MICROPY_PY_FUNCTION_ATTRS                   (1)
//      MICROPY_PY_UERRNO_LIST - Use the default

#endif // SAM_D5X_E5X

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

// Definitions that can be overridden by mpconfigboard.h:

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SAMD21

#ifndef CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE
#if INTERNAL_FLASH_FILESYSTEM
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (64 * 1024)
#else
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (0)
#endif
#endif

#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE (256)
#endif

#ifndef CIRCUITPY_DEFAULT_STACK_SIZE
#define CIRCUITPY_DEFAULT_STACK_SIZE                3584
#endif

#ifndef SAMD21_BOD33_LEVEL
// Set brownout detection to ~2.7V. Default from factory is 1.7V,
// which is too low for proper operation of external SPI flash chips
// (they are 2.7-3.6V).
#define SAMD21_BOD33_LEVEL (39)
// 2.77V with hysteresis off. Table 37.20 in datasheet.
#endif

// Smallest unit of flash that can be erased.
#define FLASH_ERASE_SIZE NVMCTRL_ROW_SIZE

#endif // SAMD21

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SAM_D5X_E5X

#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE (8192)
#endif

#ifndef CIRCUITPY_DEFAULT_STACK_SIZE
#define CIRCUITPY_DEFAULT_STACK_SIZE                (24 * 1024)
#endif

#ifndef SAMD5x_E5x_BOD33_LEVEL
// Set brownout detection to ~2.7V. Default from factory is 1.7V,
// which is too low for proper operation of external SPI flash chips
// (they are 2.7-3.6V).
#define SAMD5x_E5x_BOD33_LEVEL (200)
// 2.7V: 1.5V + LEVEL * 6mV.
#endif

// Smallest unit of flash that can be erased.
#define FLASH_ERASE_SIZE NVMCTRL_BLOCK_SIZE

// If CIRCUITPY is internal, use half of flash for it.
#if INTERNAL_FLASH_FILESYSTEM
  #ifndef CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE
    #define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (FLASH_SIZE / 2)
  #endif
#else
  #define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (0)
#endif

#endif // SAM_D5X_E5X

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATE_CRYSTALLESS
#define CALIBRATE_CRYSTALLESS (0)
#endif

#ifndef BOARD_HAS_CRYSTAL
#define BOARD_HAS_CRYSTAL (0)
#endif

// if CALIBRATE_CRYSTALLESS is requested, make room for storing
// calibration data generated from external USB.
#ifndef CIRCUITPY_INTERNAL_CONFIG_SIZE
  #if CALIBRATE_CRYSTALLESS
    #define CIRCUITPY_INTERNAL_CONFIG_SIZE (NVMCTRL_ROW_SIZE) // 256
  #else
    #define CIRCUITPY_INTERNAL_CONFIG_SIZE (0)
  #endif
#endif

// Flash layout, starting at 0x00000000
//
// - bootloader (8 or 16kB)
// - firmware
// - internal CIRCUITPY flash filesystem (optional)
// - internal config, used to store crystalless clock calibration info (optional)
// - microntroller.nvm (optional)

// Define these regions starting up from the bottom of flash:

#define BOOTLOADER_START_ADDR          (0x00000000)

#define CIRCUITPY_FIRMWARE_START_ADDR  (BOOTLOADER_START_ADDR + BOOTLOADER_SIZE)

// Define these regions start down from the top of flash:

#define CIRCUITPY_INTERNAL_NVM_START_ADDR \
    (FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE)

#define CIRCUITPY_INTERNAL_CONFIG_START_ADDR \
    (CIRCUITPY_INTERNAL_NVM_START_ADDR - CIRCUITPY_INTERNAL_CONFIG_SIZE)

#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR \
    (CIRCUITPY_INTERNAL_CONFIG_START_ADDR - CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE)

// The firmware space is the space left over between the fixed lower and upper regions.
#define CIRCUITPY_FIRMWARE_SIZE \
    (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR - CIRCUITPY_FIRMWARE_START_ADDR)

#if BOOTLOADER_START_ADDR % FLASH_PAGE_SIZE != 0
#error BOOTLOADER_START_ADDR must be on a flash page boundary.
#endif

#if CIRCUITPY_INTERNAL_NVM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_START_ADDR must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_NVM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_INTERNAL_CONFIG_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_CONFIG_SIZE must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_CONFIG_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_CONFIG_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be on a flash erase (row or block) boundary.
#endif
#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_FIRMWARE_SIZE < 0
#error No space left in flash for firmware after specifying other regions!
#endif

// Turning off audioio, audiobusio, and touchio as necessary
// due to limitations of chips is handled in mpconfigboard.mk

#include "peripherals/samd/dma.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \
    mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT];

#endif  // __INCLUDED_MPCONFIGPORT_H
