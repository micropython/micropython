#define MICROPY_HW_BOARD_NAME       "MikroE Quail"
#define MICROPY_HW_MCU_NAME         "STM32F427VI"

// 1 = use STM32 internal flash (1 MByte)
// 0 = use onboard external SPI flash (8 MByte)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_FLASH        (1)

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM         (6)
#define MICROPY_HW_CLK_PLLN         (336)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV4)
#define MICROPY_HW_CLK_PLLQ         (14)
#define MICROPY_HW_CLK_LAST_FREQ    (1)

// The board has no crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (0)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (0)  // turn on/off PC13 512Hz output

// UART config
// mikroBUS slot 1
#define MICROPY_HW_UART3_NAME       "SLOT1"
#define MICROPY_HW_UART3_TX         (pin_D8)
#define MICROPY_HW_UART3_RX         (pin_D9)
// mikroBUS slot 2
#define MICROPY_HW_UART2_NAME       "SLOT2"
#define MICROPY_HW_UART2_TX         (pin_D5)
#define MICROPY_HW_UART2_RX         (pin_D6)
// mikroBUS slot 3
#define MICROPY_HW_UART6_NAME       "SLOT3"
#define MICROPY_HW_UART6_TX         (pin_C6)
#define MICROPY_HW_UART6_RX         (pin_C7)
// mikroBUS slot 4
#define MICROPY_HW_UART1_NAME       "SLOT4"
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)

// I2C buses
// mikroBUS slot 1, 2, 3, 4, and header
#define MICROPY_HW_I2C1_NAME        "SLOT1234H"
#define MICROPY_HW_I2C1_SCL         (pin_B6)
#define MICROPY_HW_I2C1_SDA         (pin_B7)

// SPI buses
// mikroBUS slot 1, 2, and header
#define MICROPY_HW_SPI1_NAME        "SLOT12H"
#define MICROPY_HW_SPI1_SCK         (pin_B3)
#define MICROPY_HW_SPI1_MISO        (pin_B4)
#define MICROPY_HW_SPI1_MOSI        (pin_B5)
// mikroBUS slot 3, 4, and FLASH
#define MICROPY_HW_SPI3_NAME        "SLOT34F"
#define MICROPY_HW_SPI3_SCK         (pin_C10)
#define MICROPY_HW_SPI3_MISO        (pin_C11)
#define MICROPY_HW_SPI3_MOSI        (pin_C12)

// LEDs
#define MICROPY_HW_LED1             (pin_E15) // orange
#define MICROPY_HW_LED2             (pin_E10) // green
#define MICROPY_HW_LED3             (pin_C3)  // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)

// External SPI Flash config (Cypress S25FL164K)
#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

#define MICROPY_HW_SPIFLASH_SIZE_BITS (64 * 1024 * 1024) // 64 Mbit (8 MByte)

#define MICROPY_HW_SPIFLASH_CS (pin_A13)
#define MICROPY_HW_SPIFLASH_SCK (MICROPY_HW_SPI3_SCK)
#define MICROPY_HW_SPIFLASH_MISO (MICROPY_HW_SPI3_MISO)
#define MICROPY_HW_SPIFLASH_MOSI (MICROPY_HW_SPI3_MOSI)

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

#endif // !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

// Bootloader configuration (only needed if Mboot is used)
#define MBOOT_I2C_PERIPH_ID         1
#define MBOOT_I2C_SCL               (pin_B6)
#define MBOOT_I2C_SDA               (pin_B7)
#define MBOOT_I2C_ALTFUNC           (4)
#define MBOOT_FSLOAD                (1)
#define MBOOT_VFS_FAT               (1)

#define MBOOT_SPIFLASH_ADDR         (0x80000000)
#define MBOOT_SPIFLASH_BYTE_SIZE    (8 * 1024 * 1024)
#define MBOOT_SPIFLASH_LAYOUT       "/0x80000000/512*8Kg"
#define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE \
    (8 / 4)                                 // 8k page, 4k erase block
#define MBOOT_SPIFLASH_CONFIG       (&spiflash_config)
#define MBOOT_SPIFLASH_SPIFLASH     (&spi_bdev.spiflash)
