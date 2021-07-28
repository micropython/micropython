#define MICROPY_HW_BOARD_NAME       "CubScout2 V2.2 16MB"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"
/**********************************************
Versions:
a = removed SWITCH
2 = 2/9/21 added block device to support ext 16M flash
************************************************/
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1) //bcs not using internal flash--removes os.Flash()?
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (0) //bcs

// bcs use external SPI flash for storage
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0) //bcs
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024) //16MB
#define MICROPY_HW_SPIFLASH_CS      (pin_C4)
#define MICROPY_HW_SPIFLASH_SCK     (pin_A5)
#define MICROPY_HW_SPIFLASH_MOSI    (pin_A7)
#define MICROPY_HW_SPIFLASH_MISO    (pin_A6)

// bcs block device config for SPI flash
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)
#define MICROPY_HW_CLK_LAST_FREQ (1)

// The pyboard has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)


#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
//end bcs
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
#define MICROPY_HW_UART2_RTS    (pin_A1)
#define MICROPY_HW_UART2_CTS    (pin_A0)

#define MICROPY_HW_UART3_TX     (pin_B10)
#define MICROPY_HW_UART3_RX     (pin_B11)
#define MICROPY_HW_UART3_RTS    (pin_B14)
#define MICROPY_HW_UART3_CTS    (pin_B13)

#define MICROPY_HW_UART4_TX     (pin_C10)
#define MICROPY_HW_UART4_RX     (pin_C11)

#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

#define MICROPY_HW_UART5_TX     (pin_D2)
#define MICROPY_HW_UART5_RX     (pin_C12)

// I2C busses

#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)

#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

#define MICROPY_HW_I2C3_SCL (pin_A8)
#define MICROPY_HW_I2C3_SDA (pin_C9)

#define MICROPY_HW_SPI2_NSS  (pin_B12) // Y5
#define MICROPY_HW_SPI2_SCK  (pin_B13) // Y6
#define MICROPY_HW_SPI2_MISO (pin_B14) // Y7
#define MICROPY_HW_SPI2_MOSI (pin_B15) // Y8

// USB config
#define MICROPY_HW_USB_FS              (1)

