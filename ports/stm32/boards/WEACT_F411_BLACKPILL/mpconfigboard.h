// based off the following two repositories:
// * https://github.com/mcauser/WEACT_F411CEU6
// * https://github.com/YXZhu/micropython

#define MICROPY_HW_BOARD_NAME       "WEACT_F411_BLACKPILL"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"
#define MICROPY_HW_FLASH_FS_LABEL   "WEACT_F411_BLACKPILL"

// some users having issues with FLASH_LATENCY_2, so set to 3
// from https://forum.micropython.org/viewtopic.php?t=7154
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3

#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_USB_FS           (1)

// Switch
#if WEACT_F411_V13
#define MICROPY_HW_HAS_SWITCH       (0)
#else
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pyb_pin_SW)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)
#endif

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_BLUE)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// Set PLLM same as HSE in MHZ.
#define MICROPY_HW_CLK_PLLM (MICROPY_HW_HSE_SPEED_MHZ)
// Configure PLL for final CPU freq of 96MHz
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)

#define MICROPY_HW_UART_REPL        (PYB_UART_1)
#define MICROPY_HW_UART_REPL_BAUD   (115200)

// I2C bus
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B9)
#define MICROPY_HW_I2C3_SCL (pin_A8)
#define MICROPY_HW_I2C3_SDA (pin_B8)

// SPI bus
// SPI 1 is generally used for the SPI flash if enabled below.
#define MICROPY_HW_SPI1_NSS     (pin_A4)
#define MICROPY_HW_SPI1_SCK     (pin_A5)
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)

#define MICROPY_HW_SPI2_NSS     (pin_B12)
#define MICROPY_HW_SPI2_SCK     (pin_B13)
#define MICROPY_HW_SPI2_MISO    (pin_B14)
#define MICROPY_HW_SPI2_MOSI    (pin_B15)

// SPI 3 is not accessible if SPI flash module is used on V2.0 (PB4 conflict)
#define MICROPY_HW_SPI3_NSS     (pin_A15)
#define MICROPY_HW_SPI3_SCK     (pin_B3)
#define MICROPY_HW_SPI3_MISO    (pin_B4)
#define MICROPY_HW_SPI3_MOSI    (pin_B5)

// External SPI Flash configuration

#if !MICROPY_HW_SPIFLASH_SIZE_BYTES
// Use internal filesystem if spiflash not enabled.
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

#else
// Reserve SPI flash bus.
#define MICROPY_HW_SPI_IS_RESERVED(id)  (id == 1)

// Disable internal filesystem to use spiflash.
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// SPI flash pins
#define MICROPY_HW_SPIFLASH_CS          (pyb_pin_FLASH_CS)
#define MICROPY_HW_SPIFLASH_SCK         (pyb_pin_FLASH_SCK)
#define MICROPY_HW_SPIFLASH_MOSI        (pyb_pin_FLASH_MOSI)
#if WEACT_F411_V13
#define MICROPY_HW_SPIFLASH_MISO        (pyb_pin_FLASH_MISO_V13)
#elif WEACT_F411_V20
#define MICROPY_HW_SPIFLASH_MISO        (pyb_pin_FLASH_MISO_V20)
#else
#define MICROPY_HW_SPIFLASH_MISO        (pyb_pin_FLASH_MISO_V31)
#endif

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol
#define MICROPY_HW_SPIFLASH_SIZE_BITS (MICROPY_HW_SPIFLASH_SIZE_BYTES * 8)
#endif
