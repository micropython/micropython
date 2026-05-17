#define MICROPY_HW_BOARD_NAME               "WEACTSTUDIO_MINI_STM32U585"
#define MICROPY_HW_MCU_NAME                 "STM32U585CI"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_ENABLE_RNG               (0)
#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_HAS_FLASH                (1)
#define MICROPY_HW_HAS_SWITCH               (1)

// The board has a 25MHz oscillator, the following gives 150MHz CPU speed
#define MICROPY_HW_CLK_PLLM                 (4)
#define MICROPY_HW_CLK_PLLN                 (48)
#define MICROPY_HW_CLK_PLLP                 (2)
#define MICROPY_HW_CLK_PLLQ                 (2)
#define MICROPY_HW_CLK_PLLR                 (2)
#define MICROPY_HW_CLK_PLLVCI               (RCC_PLLVCIRANGE_0)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// 5 wait states, according to Table 37, Reference Manual (RM0481 Rev 1)
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_4

// There is an external 32kHz oscillator
#define MICROPY_HW_RTC_USE_LSE              (1)
#define MICROPY_HW_RCC_RTC_CLKSOURCE        (RCC_RTCCLKSOURCE_LSE)

// HSI48
#define MICROPY_HW_RCC_HSI48_STATE          (RCC_HSI48_ON)

// UART config
#define MICROPY_HW_UART1_TX                 (pin_A9)
#define MICROPY_HW_UART1_RX                 (pin_A10)
#define MICROPY_HW_UART2_TX                 (pin_A2)
#define MICROPY_HW_UART2_RX                 (pin_A3)
#define MICROPY_HW_UART2_RTS                (pin_A1)
#define MICROPY_HW_UART2_CTS                (pin_A0)

// Connect REPL to UART1 which is provided on ST-Link USB interface
#define MICROPY_HW_UART_REPL                PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD           115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B6)
#define MICROPY_HW_I2C1_SDA                 (pin_B7)
#define MICROPY_HW_I2C2_SCL                 (pin_B10)
#define MICROPY_HW_I2C2_SDA                 (pin_B14)

// SPI buses
#define MICROPY_HW_SPI1_NSS                 (pin_A4)
#define MICROPY_HW_SPI1_SCK                 (pin_A5)
#define MICROPY_HW_SPI1_MISO                (pin_A6)
#define MICROPY_HW_SPI1_MOSI                (pin_A7)
#define MICROPY_HW_SPI2_NSS                 (pin_B12)
#define MICROPY_HW_SPI2_SCK                 (pin_B10)
#define MICROPY_HW_SPI2_MISO                (pin_B14)
#define MICROPY_HW_SPI2_MOSI                (pin_B15)
#define MICROPY_HW_SPI3_NSS                 (pin_A15)
#define MICROPY_HW_SPI3_SCK                 (pin_B3)
#define MICROPY_HW_SPI3_MISO                (pin_B4)
#define MICROPY_HW_SPI3_MOSI                (pin_B5)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN                (pin_A0)
#define MICROPY_HW_USRSW_PULL               (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE          (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED            (0)

// LEDs
#define MICROPY_HW_LED1                     (pin_C13) // Blue
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_high(pin))

// USB Configuration
#define MICROPY_HW_USB_FS                   (MICROPY_HW_ENABLE_USB)
