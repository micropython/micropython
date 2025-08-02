#define MICROPY_HW_BOARD_NAME               "WEACT_STM32H723"
#define MICROPY_HW_MCU_NAME                 "STM32H723VGT6"
#define MICROPY_HW_FLASH_FS_LABEL           "WEACT_STM32H723"

#define MICROPY_FATFS_EXFAT                 (1)
#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_ENABLE_RNG               (1)
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_HAS_SWITCH               (1)
#define MICROPY_HW_HAS_FLASH                (1)
#define MICROPY_HW_ENABLE_SERVO             (1)
#define MICROPY_HW_ENABLE_TIMER             (1)
#define MICROPY_HW_ENABLE_SDCARD            (1)
#define MICROPY_HW_ENABLE_MMCARD            (0)

// ROMFS config
#define MICROPY_VFS_ROM                     (1)
#define MICROPY_VFS_ROM_IOCTL               (1)
#define MICROPY_HW_ROMFS_ENABLE_EXTERNAL_OSPI   (1)
#define MICROPY_HW_ROMFS_OSPI_SPIFLASH_OBJ      (&spi_bdev2.spiflash)
#define MICROPY_HW_ROMFS_ENABLE_PART0           (1)

// Flash storage config
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE    (1)
// Disable internal filesystem to use spiflash.
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

#if (MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE == 0)
// W25Q64 for storage
#define MICROPY_HW_SPIFLASH_SIZE_BYTES      (8 * 1024 * 1024)

// SPI flash #1, for R/W storage
#define MICROPY_HW_SPIFLASH_CS              (pin_D6)
#define MICROPY_HW_SPIFLASH_SCK             (pin_B3)
#define MICROPY_HW_SPIFLASH_MOSI            (pin_D7)
#define MICROPY_HW_SPIFLASH_MISO            (pin_B4)

// External SPI Flash configuration
#define MICROPY_HW_SPI_IS_RESERVED(id)      (id == 1)

// SPI flash #1, block device config
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;

#define MICROPY_HW_BDEV_SPIFLASH            (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG     (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED   (&spi_bdev) // for extended block protocol
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (MICROPY_HW_SPIFLASH_SIZE_BYTES * 8)

// SPI flash #2, to be memory mapped
#define MICROPY_HW_OSPI_PRESCALER           (3) // 120 MHz
#define MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2 (23) // 64Mbit
#define MICROPY_HW_OSPIFLASH_CS             (pin_B6)
#define MICROPY_HW_OSPIFLASH_SCK            (pin_B2)
#define MICROPY_HW_OSPIFLASH_IO0            (pin_D11)
#define MICROPY_HW_OSPIFLASH_IO1            (pin_D12)
#define MICROPY_HW_OSPIFLASH_IO2            (pin_E2)
#define MICROPY_HW_OSPIFLASH_IO3            (pin_D13)

// SPI flash #2, block device config
extern const struct _mp_spiflash_config_t spiflash2_config;
extern struct _spi_bdev_t spi_bdev2;
#endif

#define MICROPY_BOARD_EARLY_INIT            WeAct_Core_early_init

// This board has 25MHz HSE.
// The following gives a 550MHz CPU speed.
#define MICROPY_HW_CLK_USE_HSE              (1)
#define MICROPY_HW_CLK_PLLM                 (5)
#define MICROPY_HW_CLK_PLLN                 (110)
#define MICROPY_HW_CLK_PLLP                 (1)
#define MICROPY_HW_CLK_PLLQ                 (5)
#define MICROPY_HW_CLK_PLLR                 (2)
#define MICROPY_HW_CLK_PLLVCI               (RCC_PLL1VCIRANGE_2)
#define MICROPY_HW_CLK_PLLVCO               (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// The USB clock is set using PLL3
#define MICROPY_HW_CLK_PLL3M                (5)
#define MICROPY_HW_CLK_PLL3N                (96)
#define MICROPY_HW_CLK_PLL3P                (10)
#define MICROPY_HW_CLK_PLL3Q                (10)
#define MICROPY_HW_CLK_PLL3R                (2)
#define MICROPY_HW_CLK_PLL3VCI              (RCC_PLL3VCIRANGE_2)
#define MICROPY_HW_CLK_PLL3VCO              (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC             (0)

#define MICROPY_HW_RCC_OSPI_CLKSOURCE       (RCC_OSPICLKSOURCE_D1HCLK)

// 3 wait states
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_3

// The board has an external 32kHz crystal attached
#define MICROPY_HW_RTC_USE_LSE              (1)

// UART config
#define MICROPY_HW_UART1_TX                 (pin_B14)
#define MICROPY_HW_UART1_RX                 (pin_B15)
#define MICROPY_HW_UART2_TX                 (pin_D5)
#define MICROPY_HW_UART2_RX                 (pin_D6)
#define MICROPY_HW_UART2_RTS                (pin_D4)
#define MICROPY_HW_UART2_CTS                (pin_D3)
#define MICROPY_HW_UART3_TX                 (pin_D8)
#define MICROPY_HW_UART3_RX                 (pin_D9)
#define MICROPY_HW_UART5_TX                 (pin_B6)
#define MICROPY_HW_UART5_RX                 (pin_B12)
#define MICROPY_HW_UART6_TX                 (pin_C6)
#define MICROPY_HW_UART6_RX                 (pin_C7)
#define MICROPY_HW_UART7_TX                 (pin_E8)
#define MICROPY_HW_UART7_RX                 (pin_E7)
#define MICROPY_HW_UART8_TX                 (pin_E1)
#define MICROPY_HW_UART8_RX                 (pin_E0)

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B8)
#define MICROPY_HW_I2C1_SDA                 (pin_B9)
#define MICROPY_HW_I2C2_SCL                 (pin_B10)
#define MICROPY_HW_I2C2_SDA                 (pin_B11)
#define MICROPY_HW_I2C4_SCL                 (pin_D12)
#define MICROPY_HW_I2C4_SDA                 (pin_D13)

// SPI buses
// NOTE: SPI1 is used for the SPI flash.
#define MICROPY_HW_SPI1_NSS                 (pin_D6)
#define MICROPY_HW_SPI1_SCK                 (pin_B3)
#define MICROPY_HW_SPI1_MISO                (pin_B4)
#define MICROPY_HW_SPI1_MOSI                (pin_D7)

#define MICROPY_HW_SPI2_NSS                 (pin_B12)
#define MICROPY_HW_SPI2_SCK                 (pin_B13)
#define MICROPY_HW_SPI2_MISO                (pin_B14)
#define MICROPY_HW_SPI2_MOSI                (pin_B15)
// NOTE: SPI3 is used for the QSPI flash.
#define MICROPY_HW_SPI3_NSS                 (pin_A4)
#define MICROPY_HW_SPI3_SCK                 (pin_B3)
#define MICROPY_HW_SPI3_MISO                (pin_B4)
#define MICROPY_HW_SPI3_MOSI                (pin_B5)
// NOTE: SPI4 is used for the ST7735 LCD.
#define MICROPY_HW_SPI4_NSS                 (pin_E11)
#define MICROPY_HW_SPI4_SCK                 (pin_E12)
#define MICROPY_HW_SPI4_MISO                (pin_E13)
#define MICROPY_HW_SPI4_MOSI                (pin_E14)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN                (pin_C13)
#define MICROPY_HW_USRSW_PULL               (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE          (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED            (1)

// LEDs
#define MICROPY_HW_LED1                     (pin_E3)    // blue
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_low(pin))

// FDCAN bus
#define MICROPY_HW_CAN1_NAME                "FDCAN1"
#define MICROPY_HW_CAN1_TX                  (pin_D1)
#define MICROPY_HW_CAN1_RX                  (pin_D0)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_D4)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_SET)

// USB config
#define MICROPY_HW_USB_HS                   (1)
#define MICROPY_HW_USB_HS_IN_FS             (1)

void WeAct_Core_early_init(void);
