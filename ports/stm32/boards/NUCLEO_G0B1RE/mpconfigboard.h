#define MICROPY_HW_BOARD_NAME       "NUCLEO-G0B1RE"
#define MICROPY_HW_MCU_NAME         "STM32G0B1xE"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (0) // can be enabled if USB cable connected to PA11/PA12
#define MICROPY_PY_PYB_LEGACY       (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// HSE is 8MHz, HSI is 16MHz CPU freq set to 64MHz
// Default source for the clock is HSI.
#define MICROPY_HW_CLK_USE_HSI (1)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

#if MICROPY_HW_CLK_USE_HSI
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (8)
#else
// HSE comes from ST-LINK 8MHz, not crystal.
#define MICROPY_HW_CLK_USE_BYPASS (1)
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (16)
#endif
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (2)
#define MICROPY_HW_CLK_PLLR (2)

// USART1 config
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
// UART2 config
#define MICROPY_HW_UART2_TX         (pin_A2)
#define MICROPY_HW_UART2_RX         (pin_A3)
// USART3 config
#define MICROPY_HW_UART3_TX         (pin_C4)
#define MICROPY_HW_UART3_RX         (pin_C5)
#define MICROPY_HW_UART3_RTS        (pin_B14)
#define MICROPY_HW_UART3_CTS        (pin_B13)
// USART4 config
#define MICROPY_HW_UART4_TX         (pin_A0)
#define MICROPY_HW_UART4_RX         (pin_A1)
// USART5 config
#define MICROPY_HW_UART5_TX         (pin_B0)
#define MICROPY_HW_UART5_RX         (pin_B1)
// USART6 config
#define MICROPY_HW_UART6_TX         (pin_C0)
#define MICROPY_HW_UART6_RX         (pin_C1)
// LPUART1 config
#define MICROPY_HW_LPUART1_TX       (pin_C1)
#define MICROPY_HW_LPUART1_RX       (pin_C0)
// LPUART2 config
#define MICROPY_HW_LPUART2_TX       (pin_C6)
#define MICROPY_HW_LPUART2_RX       (pin_C7)
// USART2 is connected to the virtual com port on the ST-Link
#define MICROPY_HW_UART_REPL        PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)
#define MICROPY_HW_I2C2_SCL         (pin_B10)
#define MICROPY_HW_I2C2_SDA         (pin_B11)

// SPI buses
#define MICROPY_HW_SPI1_NSS         (pin_A4)
#define MICROPY_HW_SPI1_SCK         (pin_B3)
#define MICROPY_HW_SPI1_MISO        (pin_B4)
#define MICROPY_HW_SPI1_MOSI        (pin_B5)
#define MICROPY_HW_SPI2_NSS         (pin_B12)
#define MICROPY_HW_SPI2_SCK         (pin_B13)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// CAN buses
// #define MICROPY_HW_CAN1_TX          (pin_A12)
// #define MICROPY_HW_CAN1_RX          (pin_A11)
// #define MICROPY_HW_CAN2_TX          (pin_C2)
// #define MICROPY_HW_CAN2_RX          (pin_C3)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_A5) // Green LD2 LED on Nucleo
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_FS_ID)
#define MICROPY_HW_USB_MSC          (0)
#define MICROPY_HW_USB_HID          (0)
