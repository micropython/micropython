#define QiMingF103RC

#define MICROPY_HW_BOARD_NAME       "QiMingF103RC"
#define MICROPY_HW_MCU_NAME         "STM32F103RC"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_STM              (0)

#define MICROPY_VFS_FAT             (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_ENABLE_ADC       (0)

#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A1)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// HSE is 8MHz
#define MICROPY_HW_CLK_USE_HSE      (1)
#define MICROPY_HW_SYSCLK_SRC       (RCC_SYSCLKSOURCE_PLLCLK)
#define MICROPY_HW_HSE_PREDIV       (RCC_HSE_PREDIV_DIV1)
#define MICROPY_HW_CLK_PLLMUL       (RCC_PLL_MUL9)

// The board has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
//#define MICROPY_HW_RTC_USE_CALOUT   (0)

// -------------------- soft spi flash W25Q128(default W25Q16) --------------------
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024)

// SPI1's 4 pins
#define MICROPY_HW_SPIFLASH_CS      (pin_A4)
#define MICROPY_HW_SPIFLASH_SCK     (pin_A5)
#define MICROPY_HW_SPIFLASH_MOSI    (pin_A7)
#define MICROPY_HW_SPIFLASH_MISO    (pin_A6)

extern struct _spi_bdev_t spi_bdev;
extern const struct _mp_spiflash_config_t spiflash_config;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    ((op) == BDEV_IOCTL_NUM_BLOCKS) ? \
        (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
            ((op) == BDEV_IOCTL_INIT) ? \
                spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
                spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
    )

#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))

// --------------------------------------------------------------
// USART1
// #define MICROPY_HW_UART1_TX     (pin_A9)
// #define MICROPY_HW_UART1_RX     (pin_A10)

// USART2
// #define MICROPY_HW_UART2_TX     (pin_A2)
// #define MICROPY_HW_UART2_RX     (pin_A3)

// USART3
// #define MICROPY_HW_UART3_TX     (pin_B10)
// #define MICROPY_HW_UART3_RX     (pin_B11)

// UART4 for ESP8266
// #define MICROPY_HW_UART4_TX     (pin_C10)
// #define MICROPY_HW_UART4_RX     (pin_C11)

// UART5
// #define MICROPY_HW_UART5_TX     (pin_C12)
// #define MICROPY_HW_UART5_RX     (pin_D2)

// I2C busses
// #define MICROPY_HW_I2C1_SCL (pin_B6)
// #define MICROPY_HW_I2C1_SDA (pin_B7)

// #define MICROPY_HW_I2C2_SCL (pin_B10)
// #define MICROPY_HW_I2C2_SDA (pin_B11)

// SPI2
// #define MICROPY_HW_SPI2_NSS  (pin_B12)
// #define MICROPY_HW_SPI2_SCK  (pin_B13)
// #define MICROPY_HW_SPI2_MISO (pin_B14)
// #define MICROPY_HW_SPI2_MOSI (pin_B15)

// CAN (no chip)

// KEY0 has no pullup or pulldown, and pressing the switch makes the input go low
#if MICROPY_HW_HAS_SWITCH == 1
#define MICROPY_HW_USRSW_PIN        (pin_C12)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)
#endif

// LEDs
#define MICROPY_HW_LED1             (pin_A1)  // RED LED D1
#define MICROPY_HW_LED2             (pin_C13) // RED LED D2
// Green LED D3 is the power LED and always on

#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))
