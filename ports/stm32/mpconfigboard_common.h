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

// Whether to include the stm module
#ifndef MICROPY_PY_STM
#define MICROPY_PY_STM (1)
#endif

// Whether to include named register constants in the stm module
#ifndef MICROPY_PY_STM_CONST
#define MICROPY_PY_STM_CONST (MICROPY_PY_STM)
#endif

// Whether to include the pyb module
#ifndef MICROPY_PY_PYB
#define MICROPY_PY_PYB (1)
#endif

// Whether to include legacy functions and classes in the pyb module
#ifndef MICROPY_PY_PYB_LEGACY
#define MICROPY_PY_PYB_LEGACY (1)
#endif

// Whether to include legacy methods and constants in machine.Pin (which is also pyb.Pin).
#ifndef MICROPY_PY_MACHINE_PIN_LEGACY
#define MICROPY_PY_MACHINE_PIN_LEGACY (!MICROPY_PREVIEW_VERSION_2)
#endif

// Whether to include support for alternate function selection in machine.Pin (and pyb.Pin).
#ifndef MICROPY_PY_MACHINE_PIN_ALT_SUPPORT
#define MICROPY_PY_MACHINE_PIN_ALT_SUPPORT (1)
#endif

// Whether machine.bootloader() will enter the bootloader via reset, or direct jump.
#ifndef MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET
#if defined(STM32N6)
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET (0)
#else
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET (1)
#endif
#endif

// Whether to enable ROMFS on the internal flash.
#ifndef MICROPY_HW_ROMFS_ENABLE_INTERNAL_FLASH
#define MICROPY_HW_ROMFS_ENABLE_INTERNAL_FLASH (0)
#endif

// Whether to enable ROMFS on external QSPI flash.
#ifndef MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI (0)
#endif

// Whether to enable ROMFS on external XSPI flash.
#ifndef MICROPY_HW_ROMFS_ENABLE_EXTERNAL_XSPI
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_XSPI (0)
#endif

// Whether to enable ROMFS partition 0.
#ifndef MICROPY_HW_ROMFS_ENABLE_PART0
#define MICROPY_HW_ROMFS_ENABLE_PART0 (0)
#endif

// Whether to enable ROMFS partition 1.
#ifndef MICROPY_HW_ROMFS_ENABLE_PART1
#define MICROPY_HW_ROMFS_ENABLE_PART1 (0)
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
#ifndef MICROPY_HW_ENABLE_SDCARD
#define MICROPY_HW_ENABLE_SDCARD (0)
#endif

// Whether to enable the MMC interface, exposed as pyb.MMCard
#ifndef MICROPY_HW_ENABLE_MMCARD
#define MICROPY_HW_ENABLE_MMCARD (0)
#endif

// Which SDMMC peripheral to use for the SD/MMC card driver (1 or 2)
#ifndef MICROPY_HW_SDCARD_SDMMC
#define MICROPY_HW_SDCARD_SDMMC (1)
#endif

// SD/MMC card driver interface bus width (defaults to 4 bits)
#ifndef MICROPY_HW_SDCARD_BUS_WIDTH
#define MICROPY_HW_SDCARD_BUS_WIDTH (4)
#endif

// Whether to automatically mount (and boot from) the SD card if it's present
#ifndef MICROPY_HW_SDCARD_MOUNT_AT_BOOT
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT (MICROPY_HW_ENABLE_SDCARD)
#endif

// Which SDMMC peripheral to use for the SDIO driver (1 or 2)
#ifndef MICROPY_HW_SDIO_SDMMC
#define MICROPY_HW_SDIO_SDMMC (1)
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
#define MICROPY_HW_FLASH_FS_LABEL "pybflash"
#endif

// Function to determine if the given can_id is reserved for system use or not.
#ifndef MICROPY_HW_CAN_IS_RESERVED
#define MICROPY_HW_CAN_IS_RESERVED(can_id) (false)
#endif

// Function to determine if the given i2c_id is reserved for system use or not.
#ifndef MICROPY_HW_I2C_IS_RESERVED
#define MICROPY_HW_I2C_IS_RESERVED(i2c_id) (false)
#endif

// Function to determine if the given spi_id is reserved for system use or not.
#ifndef MICROPY_HW_SPI_IS_RESERVED
#define MICROPY_HW_SPI_IS_RESERVED(spi_id) (false)
#endif

// Function to determine if the given spi_id is static or not.
// Static SPI instances can be accessed by the user but are not deinit'd on soft reset.
#ifndef MICROPY_HW_SPI_IS_STATIC
#define MICROPY_HW_SPI_IS_STATIC(spi_id) (false)
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
// USB configuration

// The USBD_xxx macros have been renamed to MICROPY_HW_USB_xxx.
#if defined(USBD_VID) \
    || defined(USBD_LANGID_STRING) \
    || defined(USBD_MANUFACTURER_STRING) \
    || defined(USBD_PRODUCT_HS_STRING) \
    || defined(USBD_PRODUCT_FS_STRING) \
    || defined(USBD_CONFIGURATION_HS_STRING) \
    || defined(USBD_INTERFACE_HS_STRING) \
    || defined(USBD_CONFIGURATION_FS_STRING) \
    || defined(USBD_INTERFACE_FS_STRING) \
    || defined(USBD_CDC_RX_DATA_SIZE) \
    || defined(USBD_CDC_TX_DATA_SIZE)
#error "Old USBD_xxx configuration option used, renamed to MICROPY_HW_USB_xxx"
#endif

// Select whether TinyUSB or legacy STM stack is used to provide USB.
#ifndef MICROPY_HW_TINYUSB_STACK
#define MICROPY_HW_TINYUSB_STACK (0)
#endif

// Central definition for STM USB stack (when not using TinyUSB)
#define MICROPY_HW_STM_USB_STACK (MICROPY_HW_ENABLE_USB && !MICROPY_HW_TINYUSB_STACK)

#if MICROPY_HW_TINYUSB_STACK
#ifndef MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_ENABLE_USBDEV (1)
#endif

#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC (1)
#endif

#ifndef MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE    (1) // Support machine.USBDevice
#endif
#endif

// Configure maximum number of CDC VCP interfaces, and whether MSC/HID are supported
#ifndef MICROPY_HW_USB_CDC_NUM
#define MICROPY_HW_USB_CDC_NUM (1)
#endif
#ifndef MICROPY_HW_USB_MSC
#define MICROPY_HW_USB_MSC (MICROPY_HW_ENABLE_USB)
#endif
#ifndef MICROPY_HW_USB_HID
#define MICROPY_HW_USB_HID (MICROPY_HW_STM_USB_STACK)
#endif

// Default VID and PID values to use for the USB device.  If MICROPY_HW_USB_VID
// is defined by a board then all needed PID options must also be defined.  The
// VID and PID can also be set dynamically in pyb.usb_mode().
// Windows needs a different PID to distinguish different device configurations.
#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID              (0xf055)

// USB PID for TinyUSB Stack.
#define MICROPY_HW_USB_PID              (0x9802)

// USB PID table for STM USB stack.
#define MICROPY_HW_USB_PID_CDC_MSC      (0x9800)
#define MICROPY_HW_USB_PID_CDC_HID      (0x9801)
#define MICROPY_HW_USB_PID_CDC          (0x9802)
#define MICROPY_HW_USB_PID_MSC          (0x9803)
#define MICROPY_HW_USB_PID_CDC2_MSC     (0x9804)
#define MICROPY_HW_USB_PID_CDC2         (0x9805)
#define MICROPY_HW_USB_PID_CDC3         (0x9806)
#define MICROPY_HW_USB_PID_CDC3_MSC     (0x9807)
#define MICROPY_HW_USB_PID_CDC_MSC_HID  (0x9808)
#define MICROPY_HW_USB_PID_CDC2_MSC_HID (0x9809)
#define MICROPY_HW_USB_PID_CDC3_MSC_HID (0x980a)
#endif

#ifndef MICROPY_HW_USB_LANGID_STRING
#define MICROPY_HW_USB_LANGID_STRING            0x409
#endif

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING      "MicroPython"
#endif

#ifndef MICROPY_HW_USB_PRODUCT_HS_STRING
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "Pyboard Virtual Comm Port in HS Mode"
#endif

#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "Pyboard Virtual Comm Port in FS Mode"
#endif

#ifndef MICROPY_HW_USB_CONFIGURATION_HS_STRING
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "Pyboard Config"
#endif

#ifndef MICROPY_HW_USB_INTERFACE_HS_STRING
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "Pyboard Interface"
#endif

#ifndef MICROPY_HW_USB_CONFIGURATION_FS_STRING
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "Pyboard Config"
#endif

#ifndef MICROPY_HW_USB_INTERFACE_FS_STRING
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "Pyboard Interface"
#endif

// Must be 8 bytes.
#ifndef MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING "MicroPy "
#endif

// Must be 16 bytes.
#ifndef MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING "pyboard Flash   "
#endif

// Must be 4 bytes.
#ifndef MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING "1.00"
#endif

// Amount of incoming buffer space for each CDC instance.
// This must be 2 or greater, and a power of 2.
#ifndef MICROPY_HW_USB_CDC_RX_DATA_SIZE
#define MICROPY_HW_USB_CDC_RX_DATA_SIZE (1024)
#endif

// Amount of outgoing buffer space for each CDC instance.
// This must be a power of 2 and no greater than 16384.
#ifndef MICROPY_HW_USB_CDC_TX_DATA_SIZE
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE (1024)
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

// Configuration for STM32F0 series
#if defined(STM32F0)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ffff7ac)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_I2C (2)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (8)
#define MICROPY_HW_MAX_LPUART (0)

// Configuration for STM32F4 series
#elif defined(STM32F4)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7a10)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (14)
#if defined(UART10)
#define MICROPY_HW_MAX_UART (10)
#elif defined(UART9)
#define MICROPY_HW_MAX_UART (9)
#elif defined(UART8)
#define MICROPY_HW_MAX_UART (8)
#elif defined(UART7)
#define MICROPY_HW_MAX_UART (7)
#else
#define MICROPY_HW_MAX_UART (6)
#endif
#define MICROPY_HW_MAX_LPUART (0)

#define CFG_TUSB_MCU OPT_MCU_STM32F4

// Configuration for STM32F7 series
#elif defined(STM32F7)

#if defined(STM32F722xx) || defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff07a10)
#else
#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff0f420)
#endif

#define PYB_EXTI_NUM_VECTORS (24)
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (8)
#define MICROPY_HW_MAX_LPUART (0)

#define CFG_TUSB_MCU OPT_MCU_STM32F7

// Configuration for STM32G0 series
#elif defined(STM32G0)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (22) // previously 23
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (6)
#define MICROPY_HW_MAX_LPUART (2)

#define CFG_TUSB_MCU OPT_MCU_STM32G0

// Configuration for STM32G4 series
#elif defined(STM32G4)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (42) // up to 42 event/interrupt requests: 28 configurable lines, 14 direct lines
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (20) // TIM1-8, 20
#define MICROPY_HW_MAX_UART (5) // UART1-5 + LPUART1
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32G4

// Configuration for STM32H5 series
#elif defined(STM32H5)

#define MP_HAL_UNIQUE_ID_ADDRESS (mp_hal_unique_id_address)
#define PYB_EXTI_NUM_VECTORS (58)
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (12)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32H5

// Configuration for STM32H7A3/B3 series
#elif defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || \
    defined(STM32H7B3xx) || defined(STM32H7B3xxQ)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x08fff800)
#define PYB_EXTI_NUM_VECTORS (24)
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32H7

// Configuration for STM32H7 series
#elif defined(STM32H7)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1ff1e800)
#define PYB_EXTI_NUM_VECTORS (24)
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (8)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32H7

#if defined(MICROPY_HW_ANALOG_SWITCH_PA0) \
    || defined(MICROPY_HW_ANALOG_SWITCH_PA1) \
    || defined(MICROPY_HW_ANALOG_SWITCH_PC2) \
    || defined(MICROPY_HW_ANALOG_SWITCH_PC3)
#define MICROPY_HW_ENABLE_ANALOG_ONLY_PINS (1)
#else
#define MICROPY_HW_ENABLE_ANALOG_ONLY_PINS (0)
#endif

// Configuration for STM32L0 series
#elif defined(STM32L0)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1FF80050)
#define PYB_EXTI_NUM_VECTORS (30) // TODO (22 configurable, 7 direct)
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (22)
#define MICROPY_HW_MAX_UART (5)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32L0

// Configuration for STM32L1 series
#elif defined(STM32L1)
#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_I2C (2)
// TODO: L1 has 9 timers but tim0 and tim1 don't exist.
#define MICROPY_HW_MAX_TIMER (11)
#define MICROPY_HW_MAX_UART (5)
#define MICROPY_HW_MAX_LPUART (0)

#define CFG_TUSB_MCU OPT_MCU_STM32L1

// Configuration for STM32L4 series
#elif defined(STM32L4)

#define MP_HAL_UNIQUE_ID_ADDRESS (0x1fff7590)
#define PYB_EXTI_NUM_VECTORS (23)
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (5)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32L4

// Configuration for STM32N6 series
#elif defined(STM32N6)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (20) // only EXTI[15:0], RTC and TAMP currently supported
#define MICROPY_HW_MAX_I2C (4)
#define MICROPY_HW_MAX_TIMER (18)
#define MICROPY_HW_MAX_UART (10)
#define MICROPY_HW_MAX_LPUART (1)

// Configuration for STM32U5 series
#elif defined(STM32U5)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
// STM32U5 has 26 EXTI vectors but does not have line for RTC/USB.
// To treat these interrupts as same as exti, add virtual vectors for
// EXTI_RTC_TIMESTAMP (26), EXTI_RTC_WAKEUP (27),
// EXTI_RTC_ALARM (28), and EXTI_USB_OTG_FS_WAKEUP  (29)
#define PYB_EXTI_NUM_VECTORS (30)
#define MICROPY_HW_MAX_I2C (6)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (6)
#define MICROPY_HW_MAX_LPUART (1)

// Configuration for STM32WB series
#elif defined(STM32WB)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (20)
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (1)
#define MICROPY_HW_MAX_LPUART (1)

#define CFG_TUSB_MCU OPT_MCU_STM32WB

#ifndef MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
#define MICROPY_HW_STM32WB_FLASH_SYNCRONISATION (1)
#endif

// RF core BLE configuration (a board should define
// MICROPY_HW_RFCORE_BLE_NUM_GATT_ATTRIBUTES to override all values)
#ifndef MICROPY_HW_RFCORE_BLE_NUM_GATT_ATTRIBUTES
#define MICROPY_HW_RFCORE_BLE_NUM_GATT_ATTRIBUTES       (0)
#define MICROPY_HW_RFCORE_BLE_NUM_GATT_SERVICES         (0)
#define MICROPY_HW_RFCORE_BLE_ATT_VALUE_ARRAY_SIZE      (0)
#define MICROPY_HW_RFCORE_BLE_NUM_LINK                  (1)
#define MICROPY_HW_RFCORE_BLE_DATA_LENGTH_EXTENSION     (1)
#define MICROPY_HW_RFCORE_BLE_PREPARE_WRITE_LIST_SIZE   (0)
#define MICROPY_HW_RFCORE_BLE_MBLOCK_COUNT              (0x79)
#define MICROPY_HW_RFCORE_BLE_MAX_ATT_MTU               (0)
#define MICROPY_HW_RFCORE_BLE_SLAVE_SCA                 (0)
#define MICROPY_HW_RFCORE_BLE_MASTER_SCA                (0)
#define MICROPY_HW_RFCORE_BLE_LSE_SOURCE                (0) // use LSE to clock the rfcore (see errata 2.2.1)
#define MICROPY_HW_RFCORE_BLE_MAX_CONN_EVENT_LENGTH     (0xffffffff)
#define MICROPY_HW_RFCORE_BLE_HSE_STARTUP_TIME          (0x148)
#define MICROPY_HW_RFCORE_BLE_VITERBI_MODE              (1)
#define MICROPY_HW_RFCORE_BLE_LL_ONLY                   (1) // use LL only, we provide the rest of the BLE stack
#endif

// Configuration for STM32WL series
#elif defined(STM32WL)

#define MP_HAL_UNIQUE_ID_ADDRESS (UID_BASE)
#define PYB_EXTI_NUM_VECTORS (21) // up to RTC_WKUP
#define MICROPY_HW_MAX_I2C (3)
#define MICROPY_HW_MAX_TIMER (17)
#define MICROPY_HW_MAX_UART (2)
#define MICROPY_HW_MAX_LPUART (1)

#else
#error Unsupported MCU series
#endif

#if MICROPY_HW_CLK_USE_HSI
// Use HSI as clock source
#define MICROPY_HW_CLK_VALUE (HSI_VALUE)
#define MICROPY_HW_RCC_OSCILLATOR_TYPE (RCC_OSCILLATORTYPE_HSI)
#define MICROPY_HW_RCC_PLL_SRC (RCC_PLLSOURCE_HSI)
#define MICROPY_HW_RCC_CR_HSxON (RCC_CR_HSION)
#define MICROPY_HW_RCC_HSI_STATE (RCC_HSI_ON)
#define MICROPY_HW_RCC_FLAG_HSxRDY (RCC_FLAG_HSIRDY)
#define MICROPY_HW_RCC_HSE_STATE (RCC_HSE_OFF)
#else
// Use HSE as a clock source (bypass or oscillator)
#define MICROPY_HW_CLK_VALUE (HSE_VALUE)
#if defined(STM32G4)
// enable HSI48 to run RNG on this clock
#define MICROPY_HW_RCC_OSCILLATOR_TYPE (RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI48)
#else
#define MICROPY_HW_RCC_OSCILLATOR_TYPE (RCC_OSCILLATORTYPE_HSE)
#endif
#define MICROPY_HW_RCC_PLL_SRC (RCC_PLLSOURCE_HSE)
#define MICROPY_HW_RCC_CR_HSxON (RCC_CR_HSEON)
#define MICROPY_HW_RCC_HSI_STATE (RCC_HSI_OFF)
#define MICROPY_HW_RCC_FLAG_HSxRDY (RCC_FLAG_HSERDY)
#if MICROPY_HW_CLK_USE_BYPASS
#if !defined(STM32WL)
#define MICROPY_HW_RCC_HSE_STATE (RCC_HSE_BYPASS)
#else
#define MICROPY_HW_RCC_HSE_STATE (RCC_HSE_BYPASS_PWR)
#endif
#else
#define MICROPY_HW_RCC_HSE_STATE (RCC_HSE_ON)
#endif
#endif

// Configure the default bus clock divider values
#ifndef MICROPY_HW_CLK_AHB_DIV
#if defined(STM32H7)
#define MICROPY_HW_CLK_AHB_DIV (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB1_DIV (RCC_APB1_DIV2)
#define MICROPY_HW_CLK_APB2_DIV (RCC_APB2_DIV2)
#define MICROPY_HW_CLK_APB3_DIV (RCC_APB3_DIV2)
#define MICROPY_HW_CLK_APB4_DIV (RCC_APB4_DIV2)
#elif defined(STM32L4)
#define MICROPY_HW_CLK_AHB_DIV (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV (RCC_HCLK_DIV1)
#define MICROPY_HW_CLK_APB2_DIV (RCC_HCLK_DIV1)
#elif defined(STM32U5)
#define MICROPY_HW_CLK_AHB_DIV (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV (RCC_HCLK_DIV1)
#define MICROPY_HW_CLK_APB2_DIV (RCC_HCLK_DIV1)
#define MICROPY_HW_CLK_APB3_DIV (RCC_HCLK_DIV1)
#else
#define MICROPY_HW_CLK_AHB_DIV (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV (RCC_HCLK_DIV4)
#define MICROPY_HW_CLK_APB2_DIV (RCC_HCLK_DIV2)
#endif
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

#if defined(MICROPY_HW_BDEV_SPIFLASH)
// Provide block device macros using spi_bdev.
// The board must provide settings for:
//  - MICROPY_HW_BDEV_SPIFLASH - pointer to a spi_bdev_t
//  - MICROPY_HW_BDEV_SPIFLASH_CONFIG - pointer to an mp_spiflash_config_t
//  - MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES - size in bytes of the SPI flash
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(MICROPY_HW_BDEV_SPIFLASH, (op), (uint32_t)MICROPY_HW_BDEV_SPIFLASH_CONFIG) : \
    spi_bdev_ioctl(MICROPY_HW_BDEV_SPIFLASH, (op), (arg)) \
    )
#ifndef MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES
#define MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES (0)
#endif
#define MICROPY_HW_BDEV_SPIFLASH_OFFSET_BLOCKS (MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES / FLASH_BLOCK_SIZE)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(MICROPY_HW_BDEV_SPIFLASH, (dest), MICROPY_HW_BDEV_SPIFLASH_OFFSET_BLOCKS + (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(MICROPY_HW_BDEV_SPIFLASH, (src), MICROPY_HW_BDEV_SPIFLASH_OFFSET_BLOCKS + (bl), (n))
#endif

// Define the FATFS maximum sector size.
#ifndef MICROPY_FATFS_MAX_SS
#if defined(STM32N6)
#define MICROPY_FATFS_MAX_SS                    (4096)
#else
#define MICROPY_FATFS_MAX_SS                    (512)
#endif
#endif

// Whether to enable caching for external SPI flash, to allow block writes that are
// smaller than the native page-erase size of the SPI flash, eg when FAT FS is used.
// Enabling this enables spi_bdev_readblocks() and spi_bdev_writeblocks() functions,
// and requires a valid mp_spiflash_config_t.cache pointer.
#ifndef MICROPY_HW_SPIFLASH_ENABLE_CACHE
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (0)
#endif

// Enable the storage sub-system if a block device is defined
#if defined(MICROPY_HW_BDEV_IOCTL)
#define MICROPY_HW_ENABLE_STORAGE (1)
#else
#define MICROPY_HW_ENABLE_STORAGE (0)
#endif

// Enable hardware I2C if there are any peripherals defined
#if defined(MICROPY_HW_I2C1_SCL) || defined(MICROPY_HW_I2C2_SCL) \
    || defined(MICROPY_HW_I2C3_SCL) || defined(MICROPY_HW_I2C4_SCL)
#define MICROPY_HW_ENABLE_HW_I2C (1)
#ifndef MICROPY_HW_ENABLE_HW_I2C_TARGET
#if defined(STM32F4) || defined(STM32F7) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB)
#define MICROPY_HW_ENABLE_HW_I2C_TARGET (1)
#else
#define MICROPY_HW_ENABLE_HW_I2C_TARGET (0)
#endif
#endif
#else
#define MICROPY_HW_ENABLE_HW_I2C (0)
#define MICROPY_HW_ENABLE_HW_I2C_TARGET (0)
#endif

// Enable CAN if there are any peripherals defined
#if defined(MICROPY_HW_CAN1_TX) || defined(MICROPY_HW_CAN2_TX) || defined(MICROPY_HW_CAN3_TX)
#define MICROPY_HW_ENABLE_CAN (1)
#if defined(STM32G0) || defined(STM32G4) || defined(STM32H7)
#define MICROPY_HW_ENABLE_FDCAN (1) // define for MCUs with FDCAN
#endif
#else
#define MICROPY_HW_ENABLE_CAN (0)
#define MICROPY_HW_MAX_CAN (0)
#endif
#if defined(MICROPY_HW_CAN3_TX)
#define MICROPY_HW_MAX_CAN (3)
#elif defined(MICROPY_HW_CAN2_TX)
#define MICROPY_HW_MAX_CAN (2)
#elif defined(MICROPY_HW_CAN1_TX)
#define MICROPY_HW_MAX_CAN (1)
#endif

// Enable I2S if there are any peripherals defined
#if defined(MICROPY_HW_I2S1) || defined(MICROPY_HW_I2S2)
#define MICROPY_PY_MACHINE_I2S (1)
#define MICROPY_HW_MAX_I2S (2)
#else
#define MICROPY_PY_MACHINE_I2S (0)
#define MICROPY_HW_MAX_I2S (0)
#endif

// Define MICROPY_HW_SDMMCx_CK values if that peripheral is used, so that make-pins.py
// generates the relevant AF constants.
#if MICROPY_HW_SDCARD_SDMMC == 1 || MICROPY_HW_SDIO_SDMMC == 1
#define MICROPY_HW_SDMMC1_CK (1)
#endif
#if MICROPY_HW_SDCARD_SDMMC == 2 || MICROPY_HW_SDIO_SDMMC == 2
#define MICROPY_HW_SDMMC2_CK (1)
#endif

// Whether the USB peripheral is device-only, or multiple OTG
// For STM32G0 and STM32H5 the USB peripheral supports device and host mode,
// but otherwise acts like a non-multi-OTG peripheral.
#if defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32L0) || defined(STM32L1) || defined(STM32L432xx) || defined(STM32L452xx) || defined(STM32WB)
#define MICROPY_HW_USB_IS_MULTI_OTG (0)
#else
#define MICROPY_HW_USB_IS_MULTI_OTG (1)
#endif

// Pin definition header file
#define MICROPY_PIN_DEFS_PORT_H "pin_defs_stm32.h"

// D-cache clean/invalidate helpers
#if __DCACHE_PRESENT == 1
// Note: The SCB_Clean<...> functions automatically align their arguments to cover full cache lines.
// CLEANINVALIDATE will write back (flush) any dirty lines in this region to RAM, then
// invalidate (evict) the whole region from the cache.
#define MP_HAL_CLEANINVALIDATE_DCACHE(addr, size) SCB_CleanInvalidateDCache_by_Addr((volatile void *)(addr), (size))
// CLEAN will write back (flush) any dirty lines in this region to RAM.
#define MP_HAL_CLEAN_DCACHE(addr, size) SCB_CleanDCache_by_Addr((volatile void *)(addr), (size))
#else
#define MP_HAL_CLEANINVALIDATE_DCACHE(addr, size)
#define MP_HAL_CLEAN_DCACHE(addr, size)
#endif

#define MICROPY_HW_USES_BOOTLOADER (MICROPY_HW_VTOR != 0x08000000)

#ifndef MICROPY_HW_ETH_DMA_ATTRIBUTE
#define MICROPY_HW_ETH_DMA_ATTRIBUTE __attribute__((aligned(16384)));
#endif
