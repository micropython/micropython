/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2021 Damien P. George
 */

#include <stdint.h>

#define MICROPY_HW_BOARD_NAME                    "LEGO Technic Hub No.6"
#define MICROPY_HW_MCU_NAME                      "STM32F413"

#define MICROPY_HW_HAS_SWITCH                    (0)
#define MICROPY_HW_HAS_FLASH                     (1)
#define MICROPY_PY_PYB_LEGACY                    (0)
#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET    (0)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_ENABLE_RTC                    (1)
#define MICROPY_HW_ENABLE_RNG                    (1)
#define MICROPY_HW_ENABLE_DAC                    (1)
#define MICROPY_HW_ENABLE_USB                    (1)
#define MICROPY_HW_FLASH_FS_LABEL                "HUB_NO6"

// HSE is 16MHz, CPU freq set to 100MHz, buses at maximum freq
#define MICROPY_HW_CLK_PLLM                      (16)
#define MICROPY_HW_CLK_PLLN                      (200)
#define MICROPY_HW_CLK_PLLP                      (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ                      (4)
#define MICROPY_HW_CLK_AHB_DIV                   (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV                  (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB2_DIV                  (RCC_HCLK_DIV1)

// For 2.7 to 3.6 V, 75 to 100 MHz: 3 wait states.
#define MICROPY_HW_FLASH_LATENCY                 FLASH_LATENCY_3

// UART buses
// Bluetooth HCI
#define MICROPY_HW_UART2_CTS                     (pin_D3)
#define MICROPY_HW_UART2_RTS                     (pin_D4)
#define MICROPY_HW_UART2_TX                      (pin_D5)
#define MICROPY_HW_UART2_RX                      (pin_D6)
// Port B
#define MICROPY_HW_UART4_TX                      (pin_D1)
#define MICROPY_HW_UART4_RX                      (pin_D0)
// Port D
#define MICROPY_HW_UART5_TX                      (pin_C12)
#define MICROPY_HW_UART5_RX                      (pin_D2)
// Port A
#define MICROPY_HW_UART7_TX                      (pin_E8)
#define MICROPY_HW_UART7_RX                      (pin_E7)
// Port C
#define MICROPY_HW_UART8_TX                      (pin_E1)
#define MICROPY_HW_UART8_RX                      (pin_E0)
// Port F
#define MICROPY_HW_UART9_TX                      (pin_D15)
#define MICROPY_HW_UART9_RX                      (pin_D14)
// Port E
#define MICROPY_HW_UART10_TX                     (pin_E3)
#define MICROPY_HW_UART10_RX                     (pin_E2)

// SPI buses
#define MICROPY_HW_SPI1_NSS                      (pin_A4) // shared with DAC
#define MICROPY_HW_SPI1_SCK                      (pin_A5) // shared with DAC
#define MICROPY_HW_SPI1_MISO                     (pin_A6)
#define MICROPY_HW_SPI1_MOSI                     (pin_A7)
#define MICROPY_HW_SPI2_NSS                      (pin_B12)
#define MICROPY_HW_SPI2_SCK                      (pin_B13)
#define MICROPY_HW_SPI2_MISO                     (pin_C2)
#define MICROPY_HW_SPI2_MOSI                     (pin_C3)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN           (pin_A9)
#define MICROPY_HW_USB_FS                        (1)
#define MICROPY_HW_USB_MSC                       (1)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID                   (PYB_UART_2)
#define MICROPY_HW_BLE_UART_BAUDRATE             (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY   (921600)
#define MICROPY_HW_BLE_BTSTACK_CHIPSET_INSTANCE  btstack_chipset_cc256x_instance()

// SPI flash, for R/W storage
// The first 1MiB is skipped because it's used by the built-in bootloader
// Note: MICROPY_HW_SPIFLASH_OFFSET_BYTES must be a multiple of MP_SPIFLASH_ERASE_BLOCK_SIZE
#define MICROPY_HW_SPIFLASH_OFFSET_BYTES         (1024 * 1024)
#define MICROPY_HW_SPIFLASH_BLOCKMAP(bl)         ((bl) + MICROPY_HW_SPIFLASH_OFFSET_BYTES / FLASH_BLOCK_SIZE)
#define MICROPY_HW_SPIFLASH_BLOCKMAP_EXT(bl)     ((bl) + MICROPY_HW_SPIFLASH_OFFSET_BYTES / MP_SPIFLASH_ERASE_BLOCK_SIZE)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE         (1)
#define MICROPY_HW_SPIFLASH_SIZE_BITS            (256 * 1024 * 1024 - MICROPY_HW_SPIFLASH_OFFSET_BYTES * 8)
#define MICROPY_HW_SPIFLASH_CS                   (MICROPY_HW_SPI2_NSS)
#define MICROPY_HW_SPIFLASH_SCK                  (MICROPY_HW_SPI2_SCK)
#define MICROPY_HW_SPIFLASH_MISO                 (MICROPY_HW_SPI2_MISO)
#define MICROPY_HW_SPIFLASH_MOSI                 (MICROPY_HW_SPI2_MOSI)

// SPI flash, block device config
extern int32_t board_bdev_ioctl(void);
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? board_bdev_ioctl() : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )

// Configuration for stardard block protocol (block size FLASH_BLOCK_SIZE).
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) \
    spi_bdev_readblocks(&spi_bdev, (dest), MICROPY_HW_SPIFLASH_BLOCKMAP(bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) \
    spi_bdev_writeblocks(&spi_bdev, (src), MICROPY_HW_SPIFLASH_BLOCKMAP(bl), (n))

// Configuration for extended block protocol (block size MP_SPIFLASH_ERASE_BLOCK_SIZE).
#define MICROPY_HW_BDEV_BLOCKSIZE_EXT (MP_SPIFLASH_ERASE_BLOCK_SIZE)
#define MICROPY_HW_BDEV_READBLOCKS_EXT(dest, bl, off, len) \
    (spi_bdev_readblocks_raw(&spi_bdev, (dest), MICROPY_HW_SPIFLASH_BLOCKMAP_EXT(bl), (off), (len)))
#define MICROPY_HW_BDEV_WRITEBLOCKS_EXT(src, bl, off, len) \
    (spi_bdev_writeblocks_raw(&spi_bdev, (src), MICROPY_HW_SPIFLASH_BLOCKMAP_EXT(bl), (off), (len)))
#define MICROPY_HW_BDEV_ERASEBLOCKS_EXT(bl, len) \
    (spi_bdev_eraseblocks_raw(&spi_bdev, MICROPY_HW_SPIFLASH_BLOCKMAP_EXT(bl), (len)))

// Board control config
#define MICROPY_BOARD_STARTUP                    board_init

/******************************************************************************/
// Bootloader configuration

#define MBOOT_LEAVE_BOOTLOADER_VIA_RESET         (0)

#define MBOOT_LED1                               0
#define MBOOT_BOARD_LED_INIT                     board_mboot_led_init
#define MBOOT_BOARD_LED_STATE                    board_mboot_led_state

#define MBOOT_BOARD_EARLY_INIT                   board_init
#define MBOOT_BOARD_CLEANUP                      board_mboot_cleanup
#define MBOOT_BOARD_GET_RESET_MODE               board_mboot_get_reset_mode

/******************************************************************************/
// Function declarations

void board_init(void);
void board_mboot_cleanup(int reset_mode);
void board_mboot_led_init(void);
void board_mboot_led_state(int led, int state);
int board_mboot_get_reset_mode(void);
void *btstack_chipset_cc256x_instance(void);
