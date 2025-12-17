/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Mike Tolkachev <contact@miketolkachev.dev>
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

// This board is configured to communicate over USB port, not ST-Link

#define MICROPY_HW_BOARD_NAME       "F469DISC"
#define MICROPY_HW_MCU_NAME         "STM32F469"

// Use external QSPI flash for storage by default
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)

#define MICROPY_BOARD_EARLY_INIT    STM32F469DISC_board_early_init
void STM32F469DISC_board_early_init(void);

// QSPI flash storage configuration
#if !BUILDING_MBOOT
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE    (1)
#endif
#define MICROPY_HW_SPIFLASH_SOFT_RESET      (1)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (128 * 1024 * 1024)
#define MICROPY_HW_SPIFLASH_CHIP_PARAMS     (1) // enable extended parameters
#define MICROPY_HW_QSPI_PRESCALER           (3)
#define MICROPY_HW_QSPIFLASH_DUMMY_CYCLES   (4)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI_D0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI_D1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI_D2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI_D3)

// QSPI flash block device configuration
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6

// UART config
#define MICROPY_HW_UART3_NAME       "YB"
#define MICROPY_HW_UART3_TX         (pin_B10)
#define MICROPY_HW_UART3_RX         (pin_B11)
#define MICROPY_HW_UART6_NAME       "YA"
#define MICROPY_HW_UART6_TX         (pin_G14)
#define MICROPY_HW_UART6_RX         (pin_G9)
#define MICROPY_HW_UART2_NAME       "UART2"
#define MICROPY_HW_UART2_TX         (pin_A2) // Needed to enable AF
#define MICROPY_HW_UART2_RX         (pin_A3) // Dummy, not routed on PCB
#define MICROPY_HW_UART2_CK         (pin_A4)

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)

// SPI
#define MICROPY_HW_SPI2_NSS         (pin_B9)
#define MICROPY_HW_SPI2_SCK         (pin_D3)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// CAN buses
#define MICROPY_HW_CAN1_TX          (pin_B9)
#define MICROPY_HW_CAN1_RX          (pin_B8)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_G6) // green
#define MICROPY_HW_LED2             (pin_D4) // orange
#define MICROPY_HW_LED3             (pin_D5) // red
#define MICROPY_HW_LED4             (pin_K3) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD Card SDMMC
#define MICROPY_HW_SDCARD_CK               (pin_C12)
#define MICROPY_HW_SDCARD_CMD              (pin_D2)
#define MICROPY_HW_SDCARD_D0               (pin_C8)
#define MICROPY_HW_SDCARD_D1               (pin_C9)
#define MICROPY_HW_SDCARD_D2               (pin_C10)
#define MICROPY_HW_SDCARD_D3               (pin_C11)
#define MICROPY_HW_SDCARD_DETECT_PIN       (pin_G2)
#define MICROPY_HW_SDCARD_DETECT_PULL      (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT   (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
