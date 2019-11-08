#define MICROPY_HW_BOARD_NAME       "NUCLEO-F413ZH"
#define MICROPY_HW_MCU_NAME         "STM32F413"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)

// HSE is 8MHz, CPU freq set to 96MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)

// For 2.7 to 3.6 V, 75 to 100 MHz: 3 wait states.
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3

// UART config
#define MICROPY_HW_UART1_TX     (pin_B6)
#define MICROPY_HW_UART1_RX     (pin_B3) // shared with CAN3
#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)
#define MICROPY_HW_UART4_TX     (pin_D1)
#define MICROPY_HW_UART4_RX     (pin_D0)
#define MICROPY_HW_UART5_TX     (pin_C12)
#define MICROPY_HW_UART5_RX     (pin_D2)
#define MICROPY_HW_UART6_TX     (pin_G14)
#define MICROPY_HW_UART6_RX     (pin_G9)
#define MICROPY_HW_UART7_TX     (pin_E8)
#define MICROPY_HW_UART7_RX     (pin_E7)
#define MICROPY_HW_UART8_TX     (pin_E1)
#define MICROPY_HW_UART8_RX     (pin_E0)
#define MICROPY_HW_UART9_TX     (pin_D15)
#define MICROPY_HW_UART9_RX     (pin_D14)
#define MICROPY_HW_UART10_TX    (pin_E3)
#define MICROPY_HW_UART10_RX    (pin_E2)
#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_F1)
#define MICROPY_HW_I2C2_SDA (pin_F0)
#define MICROPY_HW_I2C3_SCL (pin_A8)
#define MICROPY_HW_I2C3_SDA (pin_C9)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A4) // shared with DAC
#define MICROPY_HW_SPI1_SCK     (pin_A5) // shared with DAC
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)
#define MICROPY_HW_SPI2_NSS     (pin_B12)
#define MICROPY_HW_SPI2_SCK     (pin_C7)
#define MICROPY_HW_SPI2_MISO    (pin_C2)
#define MICROPY_HW_SPI2_MOSI    (pin_C3)
#define MICROPY_HW_SPI3_NSS     (pin_A15)
#define MICROPY_HW_SPI3_SCK     (pin_C10)
#define MICROPY_HW_SPI3_MISO    (pin_C11)
#define MICROPY_HW_SPI3_MOSI    (pin_B5)
#define MICROPY_HW_SPI4_NSS     (pin_E4)
#define MICROPY_HW_SPI4_SCK     (pin_B13)
#define MICROPY_HW_SPI4_MISO    (pin_E5)
#define MICROPY_HW_SPI4_MOSI    (pin_E6)
#define MICROPY_HW_SPI5_NSS     (pin_E11)
#define MICROPY_HW_SPI5_SCK     (pin_E12)
#define MICROPY_HW_SPI5_MISO    (pin_E13)
#define MICROPY_HW_SPI5_MOSI    (pin_E14)

// CAN busses
#define MICROPY_HW_CAN1_TX (pin_G1)
#define MICROPY_HW_CAN1_RX (pin_G0)
#define MICROPY_HW_CAN2_TX (pin_G12)
#define MICROPY_HW_CAN2_RX (pin_G11)
#define MICROPY_HW_CAN3_TX (pin_B4)
#define MICROPY_HW_CAN3_RX (pin_B3) // shared with UART1 or use pin_A8 shared with I2C3

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B14) // red
#define MICROPY_HW_LED2             (pin_B0) // green
#define MICROPY_HW_LED3             (pin_B7) // blue
#define MICROPY_HW_LED1_PWM         { TIM12, 12, TIM_CHANNEL_1, GPIO_AF9_TIM12 }
#define MICROPY_HW_LED2_PWM         { TIM3, 3, TIM_CHANNEL_3, GPIO_AF2_TIM3 }
#define MICROPY_HW_LED3_PWM         { TIM4, 4, TIM_CHANNEL_2, GPIO_AF2_TIM4 }
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config (CN13 - USB OTG FS)
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
