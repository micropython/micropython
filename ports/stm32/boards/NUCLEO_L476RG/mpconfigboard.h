#define MICROPY_HW_BOARD_NAME       "NUCLEO-L476RG"
#define MICROPY_HW_MCU_NAME         "STM32L476RG"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_DAC       (1)

// MSI is used and is 4MHz
#define MICROPY_HW_CLK_PLLM         (1)
#define MICROPY_HW_CLK_PLLN         (40)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV7)
#define MICROPY_HW_CLK_PLLQ         (RCC_PLLQ_DIV2)
#define MICROPY_HW_CLK_PLLR         (RCC_PLLR_DIV2)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// The board has an external 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)

// USART1 config
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
// USART2 config. Connected to ST-Link
#define MICROPY_HW_UART2_TX         (pin_A2)
#define MICROPY_HW_UART2_RX         (pin_A3)
#define MICROPY_HW_UART2_RTS        (pin_A1)
#define MICROPY_HW_UART2_CTS        (pin_A0)
// USART3 config
#define MICROPY_HW_UART3_TX         (pin_C4)
#define MICROPY_HW_UART3_RX         (pin_C5)
#define MICROPY_HW_UART3_RTS        (pin_B14)
#define MICROPY_HW_UART3_CTS        (pin_B13)
// UART4 config
#define MICROPY_HW_UART4_TX         (pin_A0)
#define MICROPY_HW_UART4_RX         (pin_A1)
// UART5 config
#define MICROPY_HW_UART5_TX         (pin_C12)
#define MICROPY_HW_UART5_RX         (pin_D2)
// LPUART config
#define MICROPY_HW_LPUART1_TX       (pin_C1)
#define MICROPY_HW_LPUART1_RX       (pin_C0)
// USART2 is connected to the virtual com port on the ST-Link
#define MICROPY_HW_UART_REPL        PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)
#define MICROPY_HW_I2C2_SCL         (pin_B10)
#define MICROPY_HW_I2C2_SDA         (pin_B11)
#define MICROPY_HW_I2C3_SCL         (pin_C0)
#define MICROPY_HW_I2C3_SDA         (pin_C1)

// SPI busses
#define MICROPY_HW_SPI1_NSS         (pin_A4)
#define MICROPY_HW_SPI1_SCK         (pin_B3)
#define MICROPY_HW_SPI1_MISO        (pin_B4)
#define MICROPY_HW_SPI1_MOSI        (pin_B5)
#define MICROPY_HW_SPI2_NSS         (pin_B12)
#define MICROPY_HW_SPI2_SCK         (pin_B13)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// CAN bus
#define MICROPY_HW_CAN1_TX          (pin_A12)
#define MICROPY_HW_CAN1_RX          (pin_A11)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_A5) // Green LED on Nucleo
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)
