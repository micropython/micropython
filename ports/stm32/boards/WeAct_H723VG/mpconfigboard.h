#define MICROPY_HW_BOARD_NAME               "WeAct Studio Core"
#define MICROPY_HW_MCU_NAME                 "STM32H723VG"

#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_ENABLE_SERVO             (1)
#define MICROPY_HW_ENABLE_RNG               (0) // RNG needs proper configuration
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_ENABLE_SDCARD            (1)
#define MICROPY_HW_HAS_SWITCH               (1)
#define MICROPY_HW_HAS_FLASH                (1)

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
#define MICROPY_HW_UART7_TX                 (pin_F7)
#define MICROPY_HW_UART7_RX                 (pin_F6)
#define MICROPY_HW_UART8_TX                 (pin_E1)
#define MICROPY_HW_UART8_RX                 (pin_E0)

#define MICROPY_HW_UART_REPL                PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD           115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B8)
#define MICROPY_HW_I2C1_SDA                 (pin_B9)
#define MICROPY_HW_I2C2_SCL                 (pin_F1)
#define MICROPY_HW_I2C2_SDA                 (pin_F0)
#define MICROPY_HW_I2C4_SCL                 (pin_F14)
#define MICROPY_HW_I2C4_SDA                 (pin_F15)

// SPI buses
#define MICROPY_HW_SPI2_NSS                 (pin_A11)
#define MICROPY_HW_SPI2_SCK                 (pin_A9)
#define MICROPY_HW_SPI2_MISO                (pin_B14)
#define MICROPY_HW_SPI2_MOSI                (pin_C1)
#define MICROPY_HW_SPI3_NSS                 (pin_A4)
#define MICROPY_HW_SPI3_SCK                 (pin_B3)
#define MICROPY_HW_SPI3_MISO                (pin_B4)
#define MICROPY_HW_SPI3_MOSI                (pin_B5)
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

// USB config
#define MICROPY_HW_USB_HS                   (1)
#define MICROPY_HW_USB_HS_IN_FS             (1)

// FDCAN bus
#define MICROPY_HW_CAN1_NAME                "FDCAN1"
#define MICROPY_HW_CAN1_TX                  (pin_D1)
#define MICROPY_HW_CAN1_RX                  (pin_D0)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_D4)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_SET)

// SPI1 is not accessible because SPI flash module is used.
#define MICROPY_HW_SPI1_NSS                 (pin_D6)
#define MICROPY_HW_SPI1_SCK                 (pin_B3)
#define MICROPY_HW_SPI1_MISO                (pin_B4)
#define MICROPY_HW_SPI1_MOSI                (pin_D7)

// External SPI Flash configuration
#define MICROPY_HW_SPI_IS_RESERVED(id)      (id == 1)

// Disable internal filesystem to use spiflash.
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// 8MB SPI flash
#define MICROPY_HW_SPIFLASH_SIZE_BYTES      (8 * 1024 * 1024)

// SPI flash pins
#define MICROPY_HW_SPIFLASH_CS              (pyb_pin_FLASH_CS)
#define MICROPY_HW_SPIFLASH_SCK             (pyb_pin_FLASH_SCK)
#define MICROPY_HW_SPIFLASH_MOSI            (pyb_pin_FLASH_MOSI)
#define MICROPY_HW_SPIFLASH_MISO            (pyb_pin_FLASH_MISO)

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE    (1)
#define MICROPY_HW_BDEV_SPIFLASH            (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG     (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED   (&spi_bdev) // for extended block protocol
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (MICROPY_HW_SPIFLASH_SIZE_BYTES * 8)

void WeAct_Core_early_init(void);
