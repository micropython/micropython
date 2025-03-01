/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2021 Damien P. George
 */

#include <stdint.h>

#define MICROPY_HW_BOARD_NAME                    "LEGO Technic Hub No.7"
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
#define MICROPY_HW_FLASH_FS_LABEL                "HUB_NO7"

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
#define MICROPY_HW_UART2_CTS                     (pyb_pin_BT_CTS)
#define MICROPY_HW_UART2_RTS                     (pyb_pin_BT_RTS)
#define MICROPY_HW_UART2_TX                      (pyb_pin_BT_TX)
#define MICROPY_HW_UART2_RX                      (pyb_pin_BT_RX)
// Port B
#define MICROPY_HW_UART3_TX                      (pyb_pin_PORTB_TX)
#define MICROPY_HW_UART3_RX                      (pyb_pin_PORTB_RX)
// Port A
#define MICROPY_HW_UART5_TX                      (pyb_pin_PORTA_TX)
#define MICROPY_HW_UART5_RX                      (pyb_pin_PORTA_RX)

// SPI buses
#define MICROPY_HW_SPI2_NSS                      (pyb_pin_FLASH_NSS)
#define MICROPY_HW_SPI2_SCK                      (pyb_pin_FLASH_SCK)
#define MICROPY_HW_SPI2_MISO                     (pyb_pin_FLASH_MISO)
#define MICROPY_HW_SPI2_MOSI                     (pyb_pin_FLASH_MOSI)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN           (pyb_pin_USB_VBUS)
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
#define MICROPY_HW_SPIFLASH_SIZE_BITS            (32 * 1024 * 1024 - MICROPY_HW_SPIFLASH_OFFSET_BYTES * 8)
#define MICROPY_HW_SPIFLASH_CS                   (MICROPY_HW_SPI2_NSS)
#define MICROPY_HW_SPIFLASH_SCK                  (MICROPY_HW_SPI2_SCK)
#define MICROPY_HW_SPIFLASH_MISO                 (MICROPY_HW_SPI2_MISO)
#define MICROPY_HW_SPIFLASH_MOSI                 (MICROPY_HW_SPI2_MOSI)

// SPI flash, block device config
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )

// Configuration for standard block protocol (block size FLASH_BLOCK_SIZE).
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

// Configure CPU frequency to 96MHz, to make updates from SPI flash faster
#define MBOOT_CLK_PLLM                           (MICROPY_HW_CLK_VALUE / 1000000)
#define MBOOT_CLK_PLLN                           (192)
#define MBOOT_CLK_PLLP                           (RCC_PLLP_DIV2)
#define MBOOT_CLK_PLLQ                           (4)
#define MBOOT_CLK_AHB_DIV                        (RCC_SYSCLK_DIV1)
#define MBOOT_CLK_APB1_DIV                       (RCC_HCLK_DIV4)
#define MBOOT_CLK_APB2_DIV                       (RCC_HCLK_DIV2)
#define MBOOT_FLASH_LATENCY                      FLASH_LATENCY_3

#define MBOOT_FSLOAD                             (1)
#define MBOOT_VFS_FAT                            (1)
#define MBOOT_LEAVE_BOOTLOADER_VIA_RESET         (0)

#define MBOOT_SPIFLASH_ADDR                      (0x80000000)
#define MBOOT_SPIFLASH_BYTE_SIZE                 (4 * 1024 * 1024)
#define MBOOT_SPIFLASH_LAYOUT                    "/0x80000000/1024*4Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE     (1)
#define MBOOT_SPIFLASH_SPIFLASH                  (&board_mboot_spiflash)
#define MBOOT_SPIFLASH_CONFIG                    (&board_mboot_spiflash_config)

#define MBOOT_LED1                               0
#define MBOOT_LED2                               1
#define MBOOT_LED3                               2
#define MBOOT_BOARD_LED_INIT                     board_mboot_led_init
#define MBOOT_BOARD_LED_STATE                    board_mboot_led_state

#define MBOOT_BOARD_EARLY_INIT(initial_r0)       board_init()
#define MBOOT_BOARD_CLEANUP                      board_mboot_cleanup
#define MBOOT_BOARD_GET_RESET_MODE               board_mboot_get_reset_mode
#define MBOOT_BOARD_STATE_CHANGE                 board_mboot_state_change

/******************************************************************************/
// Function declarations

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
extern const struct _mp_spiflash_config_t board_mboot_spiflash_config;
extern struct _mp_spiflash_t board_mboot_spiflash;

void board_init(void);
void board_mboot_cleanup(int reset_mode);
void board_mboot_led_init(void);
void board_mboot_led_state(int led, int state);
int board_mboot_get_reset_mode(uint32_t *initial_r0);
void board_mboot_state_change(int state, uint32_t arg);
void *btstack_chipset_cc256x_instance(void);
