#define MPY_ST103

#define MICROPY_HW_BOARD_NAME       "MPY ST103"
#define MICROPY_HW_MCU_NAME         "STM32F103ZG"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_PY_STM              (1)

#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_ENABLE_ADC       (0)

// HSE is 8MHz
#define MICROPY_HW_CLK_USE_HSE (1)
#define MICROPY_HW_SYSCLK_SRC  (RCC_SYSCLKSOURCE_PLLCLK)
#define MICROPY_HW_HSE_PREDIV  (RCC_HSE_PREDIV_DIV1)
#define MICROPY_HW_CLK_PLLMUL  (RCC_PLL_MUL9)

// The board has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
// #define MICROPY_HW_RTC_USE_CALOUT   (1)  // turn on/off PC13 512Hz output
#if 1
#define MICROPY_HW_ENABLE_SDCARD            (1)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_D3)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (0)
#endif
// -------------------- soft spi flash W25Q128 --------------------
#if 1
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024)

// SPI1's 4 pins
#define MICROPY_HW_SPIFLASH_CS      (pin_G15)
#define MICROPY_HW_SPIFLASH_SCK     (pin_B3)
#define MICROPY_HW_SPIFLASH_MISO    (pin_B5)
#define MICROPY_HW_SPIFLASH_MOSI    (pin_B4)


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
#endif
// --------------------------------------------------------------
// USART1 use to auto download with hardware
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

// USART2
// #define MICROPY_HW_UART2_TX     (pin_A2)
// #define MICROPY_HW_UART2_RX     (pin_A3)

// USART3 for esp8266
// #define MICROPY_HW_UART3_TX     (pin_B10)  // PB10,PC10
// #define MICROPY_HW_UART3_RX     (pin_B11)  // PB11,PC11

// I2C busses, need remap
// #define MICROPY_HW_I2C1_SCL (pin_B8)
// #define MICROPY_HW_I2C1_SDA (pin_B9)


// 3 x SPI, SPI1 for W25Q128
// #define MICROPY_HW_SPI2_NSS  (pin_B12) // PB12,PB9
// #define MICROPY_HW_SPI2_SCK  (pin_B13) // PB13,PB10
// #define MICROPY_HW_SPI2_MISO (pin_B14) // PB14,PC2
// #define MICROPY_HW_SPI2_MOSI (pin_B15) // PB15,PC3

// #define MICROPY_HW_SPI3_NSS  (pin_A15) // PA15
// #define MICROPY_HW_SPI3_SCK  (pin_B3)  // PB3,PC10
// #define MICROPY_HW_SPI3_MISO (pin_B4)  // PB4,PC11
// #define MICROPY_HW_SPI3_MOSI (pin_B5)  // PB5,PC12

// 1 x CAN busses
// #define MICROPY_HW_CAN1_TX (pin_B9) // PB9,PD1,PA12
// #define MICROPY_HW_CAN1_RX (pin_B8) // PB8,PD0,PA11

// DAC
// DAC_OUT1 PA4
// DAC_OUT2 PA5

// KEY0 has pullup, and pressing the switch makes the input go low
// #define MICROPY_HW_USRSW_PIN        (pin_A0)
// #define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
// #define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
// #define MICROPY_HW_USRSW_PRESSED    (0)

// KEY1 - no support for multiple user buttons, yet
// pin_A0

// LEDs
#define MICROPY_HW_LED1             (pin_B0)  // Green LED D1
#define MICROPY_HW_LED2             (pin_B1)  // Blue  LED D2
#define MICROPY_HW_LED3             (pin_A6)  // Red   LED D3
#define MICROPY_HW_LED4             (pin_A7)  // Red   LED D3

#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))
