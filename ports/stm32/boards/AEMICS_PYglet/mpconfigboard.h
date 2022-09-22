#define MICROPY_HW_BOARD_NAME       "AEMICS PYglet"
#define MICROPY_HW_MCU_NAME         "STM32G473"
#define MICROPY_HW_FLASH_FS_LABEL   "PYglet"

#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)  // A4, A5
#define MICROPY_HW_ENABLE_USB       (1)  // A12 (dp), A11 (dm)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_HAS_FLASH        (1)  // QSPI extflash mounted
// #define MICROPY_HW_UART_REPL        (1)
// #define MICROPY_HW_UART_REPL_BAUD   (115200)

#define MICROPY_BOARD_EARLY_INIT    board_early_init
void board_early_init(void);

#if MICROPY_HW_USB_FS == 1
#define MICROPY_HW_USB_MANUFACTURER_STRING    "AEMICS"
#endif
#define MICROPY_HW_USB_PRODUCT_FS_STRING   MICROPY_HW_BOARD_NAME


// // ports/stm32/mpconfigport.h
// #define MICROPY_PY_LWIP      (0)  // Geen ETH
// #define MICROPY_PY_USSL      (0)
// #define MICROPY_SSL_MBEDTLS  (0)
// #define MICROPY_PY_UASYNCIO  (0)
// #define MICROPY_PY_UZLIB     (0)
// #define MICROPY_PY_UJSON     (1)
// #define MICROPY_PY_URE       (0)
// #define MICROPY_PY_FRAMEBUF  (0)
// #define MICROPY_PY_USOCKET   (0)
// #define MICROPY_PY_NETWORK   (0)
// #define MICROPY_PERSISTENT_CODE_LOAD (1)

// The board has an 16MHz HSI, the following gives 170MHz CPU speed and 48MHz for USB
#define MICROPY_HW_CLK_USE_HSE      (0)
#define MICROPY_HW_CLK_USE_HSI (1)
#define MICROPY_HW_CLK_PLLM (4)
#define MICROPY_HW_CLK_PLLN (85)
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (2)
#define MICROPY_HW_CLK_PLLR (2)

#define MICROPY_HW_CLK_USE_HSI48      (1)

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_8

// UART config
#define MICROPY_HW_UART4_TX         (pin_C10)
#define MICROPY_HW_UART4_RX         (pin_C11)

// xSPI
// SPI1
#define MICROPY_HW_SPI1_NSS         (pin_A3)
#define MICROPY_HW_SPI1_SCK         (pin_A5)
#define MICROPY_HW_SPI1_MISO        (pin_B4)
#define MICROPY_HW_SPI1_MOSI        (pin_B5)
// SPI2
#define MICROPY_HW_SPI2_NSS         (pin_B12)
#define MICROPY_HW_SPI2_SCK         (pin_B13)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)
// //SPI3
// #define MICROPY_HW_SPI3_NSS         (pin_A15)
// #define MICROPY_HW_SPI3_SCK         (pin_C10)
// #define MICROPY_HW_SPI3_MISO        (pin_C11)
// #define MICROPY_HW_SPI3_MOSI        (pin_C12)

// QSPI1
// 8MBit external QSPI flash, used for either the filesystem or XIP memory mapped
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (23)
#define MICROPY_HW_QSPIFLASH_CS     (pin_A2)
#define MICROPY_HW_QSPIFLASH_SCK    (pin_A3)
#define MICROPY_HW_QSPIFLASH_IO0    (pin_B1)
#define MICROPY_HW_QSPIFLASH_IO1    (pin_B0)
#define MICROPY_HW_QSPIFLASH_IO2    (pin_A7)
#define MICROPY_HW_QSPIFLASH_IO3    (pin_A6)
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#if !USE_QSPI_XIP
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? ((1 << MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2) / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol
#endif

// USRSW has pullup, pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_B9)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LED
#define MICROPY_HW_LED1             (pin_B7) // Red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))
