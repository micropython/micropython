#define MICROPY_HW_BOARD_NAME       "NUCLEO-F429ZI"
#define MICROPY_HW_MCU_NAME         "STM32F429"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// From the reference manual, for 2.7V to 3.6V
// 151-180 MHz => 5 wait states
// 181-210 MHz => 6 wait states
// 211-216 MHz => 7 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)

#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)

#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_C11)

#define MICROPY_HW_UART5_TX     (pin_C12)
#define MICROPY_HW_UART5_RX     (pin_D2)

#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C3_SCL (pin_A8)
#define MICROPY_HW_I2C3_SDA (pin_C9)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A4)
#define MICROPY_HW_SPI1_SCK     (pin_B3)
#define MICROPY_HW_SPI1_MISO    (pin_B4)
#define MICROPY_HW_SPI1_MOSI    (pin_B5)

#define MICROPY_HW_SPI4_NSS     (pin_E4)
#define MICROPY_HW_SPI4_SCK     (pin_E2)
#define MICROPY_HW_SPI4_MISO    (pin_E5)
#define MICROPY_HW_SPI4_MOSI    (pin_E6)

#define MICROPY_HW_SPI5_NSS     (pin_F6)
#define MICROPY_HW_SPI5_SCK     (pin_F7)
#define MICROPY_HW_SPI5_MISO    (pin_F8)
#define MICROPY_HW_SPI5_MOSI    (pin_F9)

// CAN busses
#define MICROPY_HW_CAN1_TX (pin_B9)
#define MICROPY_HW_CAN1_RX (pin_B8)
#define MICROPY_HW_CAN2_TX (pin_B13)
#define MICROPY_HW_CAN2_RX (pin_B12)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B0) // green
#define MICROPY_HW_LED2             (pin_B7) // blue
#define MICROPY_HW_LED3             (pin_B14) // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config (CN13 - USB OTG FS)
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
