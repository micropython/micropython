/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#define MICROPY_HW_BOARD_NAME       "PYBD-SF2W"
#define MICROPY_HW_MCU_NAME         "STM32F722IEK"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "PYBD"

#define MICROPY_PY_PYB_LEGACY       (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_MMCARD    (1)
#define MICROPY_HW_ENABLE_RF_SWITCH (1)

#define MICROPY_BOARD_EARLY_INIT    board_early_init
#define MICROPY_BOARD_ENTER_STOP    board_sleep(1);
#define MICROPY_BOARD_LEAVE_STOP    board_sleep(0);
#define MICROPY_BOARD_ENTER_STANDBY board_sleep(1);
#define MICROPY_BOARD_SDCARD_POWER  mp_hal_pin_high(pyb_pin_EN_3V3);
void board_early_init(void);
void board_sleep(int value);

// HSE is 25MHz, run SYS at 120MHz
#define MICROPY_HW_CLK_PLLM         (20)
#define MICROPY_HW_CLK_PLLN         (192)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ         (5)
#define MICROPY_HW_FLASH_LATENCY    (FLASH_LATENCY_3)

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV           (3)
#define RTC_SYNCH_PREDIV            (0x1fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// SPI flash #1, for R/W storage
#define MICROPY_HW_SOFTQSPI_SCK_LOW(self) (GPIOE->BSRR = (0x10000 << 11))
#define MICROPY_HW_SOFTQSPI_SCK_HIGH(self) (GPIOE->BSRR = (1 << 11))
#define MICROPY_HW_SOFTQSPI_NIBBLE_READ(self) ((GPIOE->IDR >> 7) & 0xf)
#define MICROPY_HW_SPIFLASH_SIZE_BITS (16 * 1024 * 1024)
#define MICROPY_HW_SPIFLASH_CS      (pyb_pin_QSPI1_CS)
#define MICROPY_HW_SPIFLASH_SCK     (pyb_pin_QSPI1_CLK)
#define MICROPY_HW_SPIFLASH_IO0     (pyb_pin_QSPI1_D0)
#define MICROPY_HW_SPIFLASH_IO1     (pyb_pin_QSPI1_D1)
#define MICROPY_HW_SPIFLASH_IO2     (pyb_pin_QSPI1_D2)
#define MICROPY_HW_SPIFLASH_IO3     (pyb_pin_QSPI1_D3)

// SPI flash #1, block device config
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#if !BUILDING_MBOOT
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#endif
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

// SPI flash #2, to be memory mapped
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (24)
#define MICROPY_HW_QSPIFLASH_CS     (pyb_pin_QSPI2_CS)
#define MICROPY_HW_QSPIFLASH_SCK    (pyb_pin_QSPI2_CLK)
#define MICROPY_HW_QSPIFLASH_IO0    (pyb_pin_QSPI2_D0)
#define MICROPY_HW_QSPIFLASH_IO1    (pyb_pin_QSPI2_D1)
#define MICROPY_HW_QSPIFLASH_IO2    (pyb_pin_QSPI2_D2)
#define MICROPY_HW_QSPIFLASH_IO3    (pyb_pin_QSPI2_D3)

// SPI flash #2, block device config
extern const struct _mp_spiflash_config_t spiflash2_config;
extern struct _spi_bdev_t spi_bdev2;

// UART config
#define MICROPY_HW_UART1_NAME       "YA"
#define MICROPY_HW_UART1_TX         (pyb_pin_Y1)
#define MICROPY_HW_UART1_RX         (pyb_pin_Y2)
#define MICROPY_HW_UART2_TX         (pyb_pin_X3)
#define MICROPY_HW_UART2_RX         (pyb_pin_X4)
#define MICROPY_HW_UART2_RTS        (pyb_pin_X2)
#define MICROPY_HW_UART2_CTS        (pyb_pin_X1)
#define MICROPY_HW_UART3_NAME       "YB"
#define MICROPY_HW_UART3_TX         (pyb_pin_Y9)
#define MICROPY_HW_UART3_RX         (pyb_pin_Y10)
#define MICROPY_HW_UART4_NAME       "XA"
#define MICROPY_HW_UART4_TX         (pyb_pin_X1)
#define MICROPY_HW_UART4_RX         (pyb_pin_X2)
#define MICROPY_HW_UART6_TX         (pyb_pin_BT_TXD)
#define MICROPY_HW_UART6_RX         (pyb_pin_BT_RXD)
#define MICROPY_HW_UART6_RTS        (pyb_pin_BT_RTS)
#define MICROPY_HW_UART6_CTS        (pyb_pin_BT_CTS)

// I2C buses
#define MICROPY_HW_I2C1_NAME        "X"
#define MICROPY_HW_I2C1_SCL         (pyb_pin_X9)
#define MICROPY_HW_I2C1_SDA         (pyb_pin_X10)
#define MICROPY_HW_I2C2_NAME        "Y"
#define MICROPY_HW_I2C2_SCL         (pyb_pin_Y9)
#define MICROPY_HW_I2C2_SDA         (pyb_pin_Y10)

// SPI buses
#define MICROPY_HW_SPI1_NAME        "X"
#define MICROPY_HW_SPI1_NSS         (pyb_pin_X5)
#define MICROPY_HW_SPI1_SCK         (pyb_pin_X6)
#define MICROPY_HW_SPI1_MISO        (pyb_pin_X7)
#define MICROPY_HW_SPI1_MOSI        (pyb_pin_X8)
#define MICROPY_HW_SPI2_NAME        "Y"
#define MICROPY_HW_SPI2_NSS         (pyb_pin_Y5)
#define MICROPY_HW_SPI2_SCK         (pyb_pin_Y6)
#define MICROPY_HW_SPI2_MISO        (pyb_pin_Y7)
#define MICROPY_HW_SPI2_MOSI        (pyb_pin_Y8)
#define MICROPY_HW_SPI3_NSS         (pyb_pin_W16)
#define MICROPY_HW_SPI3_SCK         (pyb_pin_W29)
#define MICROPY_HW_SPI3_MISO        (pyb_pin_W50)
#define MICROPY_HW_SPI3_MOSI        (pyb_pin_W46)

// I2S buses
#define MICROPY_HW_I2S1             (1)
#define MICROPY_HW_I2S2             (1)

// CAN buses
#define MICROPY_HW_CAN1_NAME        "X"
#define MICROPY_HW_CAN1_TX          (pyb_pin_X10)
#define MICROPY_HW_CAN1_RX          (pyb_pin_X9)

// USRSW is not pulled, and pressing the button makes the input go low.
#define MICROPY_HW_USRSW_PIN        (pyb_pin_USR)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED_INVERTED     (1) // LEDs are on when pin is driven low
#define MICROPY_HW_LED1             (pyb_pin_LED_RED)
#define MICROPY_HW_LED2             (pyb_pin_LED_GREEN)
#define MICROPY_HW_LED3             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD card
#define MICROPY_HW_SDCARD_SDMMC             (2)
#define MICROPY_HW_SDCARD_CK                (pyb_pin_SD_CK)
#define MICROPY_HW_SDCARD_CMD               (pyb_pin_SD_CMD)
#define MICROPY_HW_SDCARD_D0                (pyb_pin_SD_D0)
#define MICROPY_HW_SDCARD_D1                (pyb_pin_SD_D1)
#define MICROPY_HW_SDCARD_D2                (pyb_pin_SD_D2)
#define MICROPY_HW_SDCARD_D3                (pyb_pin_SD_D3)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pyb_pin_SD_SW)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT     (0)

// MM card: the size is hard-coded to support the WBUS-EMMC add-on
#define MICROPY_HW_MMCARD_LOG_BLOCK_NBR     (7469056 + 2048)

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_IN_FS     (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID       (PYB_UART_6)
#define MICROPY_HW_BLE_UART_BAUDRATE (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY (3000000)
#define MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE (3000000)

/******************************************************************************/
// Bootloader configuration

#define MBOOT_USB_AUTODETECT_PORT   (1)
#define MBOOT_FSLOAD                (1)
#define MBOOT_VFS_FAT               (1)

#define MBOOT_I2C_PERIPH_ID         1
#define MBOOT_I2C_SCL               (pin_B8)
#define MBOOT_I2C_SDA               (pin_B9)
#define MBOOT_I2C_ALTFUNC           (4)

#define MBOOT_SPIFLASH_ADDR         (0x80000000)
#define MBOOT_SPIFLASH_BYTE_SIZE    (64 * 32 * 1024)
#define MBOOT_SPIFLASH_LAYOUT       "/0x80000000/64*32Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE (32 / 4)
#define MBOOT_SPIFLASH_SPIFLASH     (&spi_bdev.spiflash)
#define MBOOT_SPIFLASH_CONFIG       (&spiflash_config)

#define MBOOT_SPIFLASH2_ADDR        (0x90000000)
#define MBOOT_SPIFLASH2_BYTE_SIZE   (64 * 32 * 1024)
#define MBOOT_SPIFLASH2_LAYOUT      "/0x90000000/64*32Kg"
#define MBOOT_SPIFLASH2_ERASE_BLOCKS_PER_PAGE (32 / 4)
#define MBOOT_SPIFLASH2_SPIFLASH    (&spi_bdev2.spiflash)
#define MBOOT_SPIFLASH2_CONFIG      (&spiflash2_config)

#define MBOOT_BOARD_EARLY_INIT(initial_r0) mboot_board_early_init()
void mboot_board_early_init(void);
