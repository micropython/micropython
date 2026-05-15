/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Garatronic SAS, thanks to  Damien P. George
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

#define MICROPY_HW_BOARD_NAME       "PYMATEIO CORE V0.1.3"
#define MICROPY_HW_MCU_NAME         "STM32F412RE"
#define MICROPY_PY_THREAD           (1)

#define MICROPY_PY_PYB_LEGACY       (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_ENABLE_MMCARD    (0)

// board HSE is 16MHz, run SYS at 96 MHz
// board is 3.3V powered, 75 to 100 MHz: 3 wait states
#define MICROPY_HW_CLK_PLLM         (16)
#define MICROPY_HW_CLK_PLLN         (192)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ         (4)
#define MICROPY_HW_CLK_LAST_FREQ    (1)
#define MICROPY_HW_CLK_AHB_DIV      (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV     (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB2_DIV     (RCC_HCLK_DIV1)
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3

// board haves 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (0)


// 8MBit external QSPI flash
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (23)
#define MICROPY_HW_QSPIFLASH_CS             (pin_B6)
#define MICROPY_HW_QSPIFLASH_SCK            (pin_B2)
#define MICROPY_HW_QSPIFLASH_IO0            (pin_C9)
#define MICROPY_HW_QSPIFLASH_IO1            (pin_C10)
#define MICROPY_HW_QSPIFLASH_IO2            (pin_C8)
#define MICROPY_HW_QSPIFLASH_IO3            (pin_A1)

// ROMFS config
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI (1)
#define MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ (&spi_bdev.spiflash)
#define MICROPY_HW_ROMFS_ENABLE_PART0 (1)

// SPI flash, block device config (when used as the filesystem)
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;

#if !USE_QSPI_XIP
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES ((1 << MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2) / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol
#endif

// UART config
#define MICROPY_HW_UART1_NAME       "X"
#define MICROPY_HW_UART1_TX         (pin_A15)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART2_NAME       "M"
#define MICROPY_HW_UART2_TX         (pin_A2)
#define MICROPY_HW_UART2_RX         (pin_A3)


// I2C buses
// putain de fmpi2c controleur, ne marche pas encore :-(
//#define MICROPY_HW_I2C4_NAME        "X"
//#define MICROPY_HW_I2C4_SCL         (pin_B15)
//#define MICROPY_HW_I2C4_SDA         (pin_B14)
//#define MICROPY_HW_I2C4_SMB         (pin_B13)

// CAN buse
#define MICROPY_HW_CAN1_NAME        "X"
#define MICROPY_HW_CAN1_TX          (pin_B9)
#define MICROPY_HW_CAN1_RX          (pin_B8)

// SPI buses
#define MICROPY_HW_SPI1_NAME        "X"
#define MICROPY_HW_SPI1_NSS         (pin_A4)
#define MICROPY_HW_SPI1_SCK         (pin_B3)
#define MICROPY_HW_SPI1_MISO        (pin_B4)
#define MICROPY_HW_SPI1_MOSI        (pin_B5)

// USRSW has external pullup, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_B12)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// The board has 2 LEDs, LED1 & LED2 as fusionned
#define MICROPY_HW_LED1             (pin_A13) // RED legacy
#define MICROPY_HW_LED2             (pin_A13) // GREEN legacy
#define MICROPY_HW_LED3             (pin_A14) // YELLOW lecacy
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_FLASH_FS_LABEL   "pymate_io"

/******************************************************************************/
// Mboot bootloader configuration : only for QSPI version
// else STM32 rom based bootloader with BOOT0 trigger at reset need to be used

//#define MBOOT_SPIFLASH_ADDR         (0x90000000)
//#define MBOOT_SPIFLASH_BYTE_SIZE    (1024 * 1024)
//#define MBOOT_SPIFLASH_LAYOUT       "/0x90000000/256*4Kg"
//#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE (1)
//#define MBOOT_SPIFLASH_SPIFLASH     (&spi_bdev.spiflash)
//#define MBOOT_SPIFLASH_CONFIG       (&spiflash_config)

//#define MBOOT_USB_AUTODETECT_PORT   (1)
//#define MBOOT_FSLOAD                (1)
//#define MBOOT_VFS_LFS2              (1)

//#define MBOOT_BOOTPIN_PIN (pin_B12)
//#define MBOOT_BOOTPIN_PULL (MP_HAL_PIN_PULL_UP)
//#define MBOOT_BOOTPIN_ACTIVE (0)

// #define MBOOT_I2C_PERIPH_ID         1
// #define MBOOT_I2C_SCL               (pin_B8)
// #define MBOOT_I2C_SDA               (pin_B9)
// #define MBOOT_I2C_ALTFUNC           (4)

//#define MBOOT_BOARD_EARLY_INIT(initial_r0) mboot_board_early_init()
//void mboot_board_early_init(void);
