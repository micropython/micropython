// based off the following two repositories:
// 	https://github.com/mcauser/WEACT_F411CEU6
//  https://github.com/YXZhu/micropython

#define MICROPY_HW_BOARD_NAME       "WEACTV20_F411"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"
#define MICROPY_HW_FLASH_FS_LABEL   "WEACTV20_F411"

// Set to 1 to use internal flash, set to 0 to use external SPI flash
// You also need to set the size of the flash later in this header
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// some users having issues with FLASH_LATENCY_2, so set to 3
// from https://forum.micropython.org/viewtopic.php?t=7154
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3

#define MICROPY_PY_THREAD           (1)

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_ENABLE_RNG       (0)

// HSE is 25MHz, CPU freq set to 96MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)

#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B9)
#define MICROPY_HW_I2C3_SCL (pin_A8)
#define MICROPY_HW_I2C3_SDA (pin_B8)

// SPI busses
// SPI 1 is used for the SPI flash module if present
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

// Switch
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_C13) 
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// SPI flash pins
#define MICROPY_HW_SPIFLASH_CS		(pyb_pin_FLASH_CS)
#define MICROPY_HW_SPIFLASH_SCK		(pyb_pin_FLASH_SCK)
#define MICROPY_HW_SPIFLASH_MISO	(pyb_pin_FLASH_MISO)
#define MICROPY_HW_SPIFLASH_MOSI	(pyb_pin_FLASH_MOSI)

// Set flash size to 128MBit by default
// note that this is in megabits, not megabytes
// to convert megabytes to megabits, multiply by 8
// 4MB => 32MBit
// 8MB => 64MBit
// 16MB => 128MBit
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024)

#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))

#endif

// USB config
#define MICROPY_HW_USB_FS (1)