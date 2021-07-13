/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#ifndef NRF5_MPCONFIGPORT_H__
#define NRF5_MPCONFIGPORT_H__

#include "ble_drv.h"

#include "nrf_mbr.h"  // for MBR_SIZE
#include "nrf_sdm.h"  // for SD_FLASH_SIZE
#include "peripherals/nrf/nvm.h" // for FLASH_PAGE_SIZE

#define MICROPY_PY_FUNCTION_ATTRS                (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS       (1)
#define MICROPY_PY_SYS_STDIO_BUFFER              (1)

// 24kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            (24 * 1024)

#ifdef NRF52840
#define MICROPY_PY_SYS_PLATFORM "nRF52840"
#define FLASH_SIZE                  (1024 * 1024)  // 1MiB
#define RAM_SIZE                    (256 * 1024)   // 256 KiB
// Special RAM area for SPIM3 transmit buffer, to work around hardware bug.
// See common.template.ld.
#define SPIM3_BUFFER_RAM_SIZE       (8 * 1024)     // 8 KiB
#endif

#ifdef NRF52833
#define MICROPY_PY_SYS_PLATFORM "nRF52833"
#define FLASH_SIZE                  (512 * 1024)  // 512 KiB
#define RAM_SIZE                    (128 * 1024)  // 128 KiB
// SPIM3 buffer is not needed on nRF52833: the SPIM3 hw bug is not present.
#ifndef SPIM3_BUFFER_RAM_SIZE
#define SPIM3_BUFFER_RAM_SIZE       (0)
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

// Definitions that might be overriden by mpconfigboard.h

#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE (8 * 1024)
#endif

#ifndef BOARD_HAS_32KHZ_XTAL
// Assume crystal is present, which is the most common case.
#define BOARD_HAS_32KHZ_XTAL (1)
#endif

#if INTERNAL_FLASH_FILESYSTEM
#ifndef CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (256 * 1024)
#endif
#else
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (0)
#endif

// Flash layout, starting at 0x00000000
//
// - SoftDevice
// - ISR
// - firmware
// - BLE config (bonding info, etc.) (optional)
// - microcontroller.nvm (optional)
// - internal CIRCUITPY flash filesystem (optional)
//   The flash filesystem is adjacent to the bootloader, so that its location will not change even if
//   other regions change in size.
// - bootloader (note the MBR at 0x0 redirects to the bootloader here, in high flash)
// - bootloader settings

// Define these regions starting up from the bottom of flash:

#define MBR_START_ADDR  (0x0)
// MBR_SIZE is from nrf_mbr.h
#define SD_FLASH_START_ADDR   (MBR_START_ADDR + MBR_SIZE)

// SD_FLASH_SIZE is from nrf_sdm.h
#define ISR_START_ADDR  (SD_FLASH_START_ADDR + SD_FLASH_SIZE)
#define ISR_SIZE        (4 * 1024)   // 4kiB

// Smallest unit of flash that can be erased.
#define FLASH_ERASE_SIZE FLASH_PAGE_SIZE

#define CIRCUITPY_FIRMWARE_START_ADDR  (ISR_START_ADDR + ISR_SIZE)

// Define these regions starting down from the bootloader:

// Bootloader values from https://github.com/adafruit/Adafruit_nRF52_Bootloader/blob/master/src/linker/s140_v6.ld
#define BOOTLOADER_START_ADDR          (FLASH_SIZE - BOOTLOADER_SIZE - BOOTLOADER_SETTINGS_SIZE - BOOTLOADER_MBR_SIZE)
#define BOOTLOADER_MBR_SIZE            (4 * 1024)     // 4kib
#ifndef BOOTLOADER_SIZE
#define BOOTLOADER_SIZE                (40 * 1024)     // 40kiB
#endif
#define BOOTLOADER_SETTINGS_START_ADDR (FLASH_SIZE - BOOTLOADER_SETTINGS_SIZE)
#define BOOTLOADER_SETTINGS_SIZE       (4 * 1024)     // 4kiB

#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR (BOOTLOADER_START_ADDR - CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE)

#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE > 0 && CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR != (BOOTLOADER_START_ADDR - CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE)
#warning Internal flash filesystem location has moved!
#endif

#define CIRCUITPY_INTERNAL_NVM_START_ADDR (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR - CIRCUITPY_INTERNAL_NVM_SIZE)

// 32kiB for bonding, etc.
#ifndef CIRCUITPY_BLE_CONFIG_SIZE
#define CIRCUITPY_BLE_CONFIG_SIZE       (32 * 1024)
#endif
#define CIRCUITPY_BLE_CONFIG_START_ADDR (CIRCUITPY_INTERNAL_NVM_START_ADDR - CIRCUITPY_BLE_CONFIG_SIZE)

// The firmware space is the space left over between the fixed lower and upper regions.
#define CIRCUITPY_FIRMWARE_SIZE (CIRCUITPY_BLE_CONFIG_START_ADDR - CIRCUITPY_FIRMWARE_START_ADDR)

#if BOOTLOADER_START_ADDR % FLASH_ERASE_SIZE != 0
#error BOOTLOADER_START_ADDR must be on a flash erase boundary.
#endif

#if CIRCUITPY_INTERNAL_NVM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_START_ADDR must be on a flash erase boundary.
#endif
#if CIRCUITPY_INTERNAL_NVM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_NVM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_BLE_CONFIG_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_BLE_CONFIG_SIZE must be on a flash erase boundary.
#endif
#if CIRCUITPY_BLE_CONFIG_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_BLE_CONFIG_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be on a flash erase boundary.
#endif
#if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE % FLASH_ERASE_SIZE != 0
#error CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE must be a multiple of FLASH_ERASE_SIZE.
#endif

#if CIRCUITPY_FIRMWARE_SIZE < 0
#error No space left in flash for firmware after specifying other regions!
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// RAM space definitions

// Max RAM used by SoftDevice. Can be changed when SoftDevice parameters are changed.
// On nRF52840, the first 64kB of RAM is composed of 8 8kB RAM blocks. Above those is
// RAM block 8, which is 192kB.
// If SPIM3_BUFFER_RAM_SIZE is 8kB, as opposed to zero, it must be in the first 64kB of RAM.
// So the amount of RAM reserved for the SoftDevice must be no more than 56kB.
// SoftDevice 6.1.0 with 5 connections and various increases can be made to use < 56kB.
// To measure the minimum required amount of memory for given configuration, set this number
// high enough to work and then check the mutation of the value done by sd_ble_enable().
// See common.template.ld.
#ifndef SOFTDEVICE_RAM_SIZE
#define SOFTDEVICE_RAM_SIZE         (56 * 1024)
#endif


#define RAM_START_ADDR              (0x20000000)
#define SOFTDEVICE_RAM_START_ADDR   (RAM_START_ADDR)
#define SPIM3_BUFFER_RAM_START_ADDR (SOFTDEVICE_RAM_START_ADDR + SOFTDEVICE_RAM_SIZE)
#define APP_RAM_START_ADDR          (SPIM3_BUFFER_RAM_START_ADDR + SPIM3_BUFFER_RAM_SIZE)
#define APP_RAM_SIZE                (RAM_START_ADDR + RAM_SIZE - APP_RAM_START_ADDR)

#if SPIM3_BUFFER_RAM_SIZE > 0 && SOFTDEVICE_RAM_SIZE + SPIM3_BUFFER_RAM_SIZE > (64 * 1024)
#error SPIM3 buffer must be in the first 64kB of RAM.
#endif

#if SOFTDEVICE_RAM_SIZE + SPIM3_BUFFER_RAM_SIZE + APP_RAM_SIZE > RAM_SIZE
#error RAM size regions overflow RAM
#endif

#if SOFTDEVICE_RAM_SIZE + SPIM3_BUFFER_RAM_SIZE + APP_RAM_SIZE < RAM_SIZE
#error RAM size regions do not use all of RAM
#endif


#define MICROPY_PORT_ROOT_POINTERS                              \
    CIRCUITPY_COMMON_ROOT_POINTERS                              \
    uint16_t *pixels_pattern_heap;                              \
    ble_drv_evt_handler_entry_t *ble_drv_evt_handler_entries;   \


#endif  // NRF5_MPCONFIGPORT_H__
