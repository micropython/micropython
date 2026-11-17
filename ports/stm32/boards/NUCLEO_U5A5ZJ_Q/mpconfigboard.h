#define MICROPY_HW_BOARD_NAME               "NUCLEO_U5A5ZJ_Q"
#define MICROPY_HW_MCU_NAME                 "STM32U5A5ZJ"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_ENABLE_RNG               (1)
#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_HAS_FLASH                (1)
#define MICROPY_HW_HAS_SWITCH               (1)

// The board has a 16MHz oscillator, the following gives 160MHz CPU speed
#define MICROPY_HW_CLK_PLLM                 (1)
#define MICROPY_HW_CLK_PLLN                 (10)
#define MICROPY_HW_CLK_PLLP                 (10)
#define MICROPY_HW_CLK_PLLQ                 (2)
#define MICROPY_HW_CLK_PLLR                 (1)
#define MICROPY_HW_CLK_PLLVCI               (RCC_PLLVCIRANGE_1)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// 5 wait states, according to Table 37, Reference Manual (RM0481 Rev 1)
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_4

// There is an external 32kHz oscillator
#define MICROPY_HW_RTC_USE_LSE              (1)
#define MICROPY_HW_RCC_RTC_CLKSOURCE        (RCC_RTCCLKSOURCE_LSE)

// UART config
#define MICROPY_HW_UART1_TX                 (pin_A9)
#define MICROPY_HW_UART1_RX                 (pin_A10)
#define MICROPY_HW_UART2_TX                 (pin_D5)
#define MICROPY_HW_UART2_RX                 (pin_D6)
#define MICROPY_HW_UART2_RTS                (pin_D4)
#define MICROPY_HW_UART2_CTS                (pin_D3)

// Connect REPL to UART1 which is provided on ST-Link USB interface
#define MICROPY_HW_UART_REPL                PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD           115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B8) // Arduino Connector CN7-Pin2 (D15)
#define MICROPY_HW_I2C1_SDA                 (pin_B9) // Arduino Connector CN7-Pin4 (D14)
#define MICROPY_HW_I2C2_SCL                 (pin_F1) // Connector CN9-Pin19
#define MICROPY_HW_I2C2_SDA                 (pin_F0) // Connector CN9-Pin21

// SPI buses
#define MICROPY_HW_SPI1_NSS                 (pin_D14) // Arduino Connector CN7-Pin16 (D10)
#define MICROPY_HW_SPI1_SCK                 (pin_A5) // Arduino Connector CN7-Pin10 (D13)
#define MICROPY_HW_SPI1_MISO                (pin_A6) // Arduino Connector CN7-Pin12 (D12)
#define MICROPY_HW_SPI1_MOSI                (pin_A7) // Arduino Connector CN7-Pin14 (D11)
#define MICROPY_HW_SPI2_NSS                 (pin_D0)
#define MICROPY_HW_SPI2_SCK                 (pin_B10)
#define MICROPY_HW_SPI2_MISO                (pin_D3)
#define MICROPY_HW_SPI2_MOSI                (pin_C1)
#define MICROPY_HW_SPI3_NSS                 (pin_A4)
#define MICROPY_HW_SPI3_SCK                 (pin_B3)
#define MICROPY_HW_SPI3_MISO                (pin_B4)
#define MICROPY_HW_SPI3_MOSI                (pin_B5)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN                (pin_C13)
#define MICROPY_HW_USRSW_PULL               (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE          (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED            (1)

// LEDs
#define MICROPY_HW_LED1                     (pin_C7) // Green
#define MICROPY_HW_LED2                     (pin_B7) // Orange
#define MICROPY_HW_LED3                     (pin_G2) // Red
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_low(pin))

// USB Configuration
#define MICROPY_HW_USB_HS                   (1)
#define MICROPY_HW_USB_HS_IN_FS             (1)

#if 0 // FDCAN bus
#define MICROPY_HW_CAN1_NAME                "FDCAN1"
#define MICROPY_HW_CAN1_TX                  (pin_D1)
#define MICROPY_HW_CAN1_RX                  (pin_D0)
#endif
