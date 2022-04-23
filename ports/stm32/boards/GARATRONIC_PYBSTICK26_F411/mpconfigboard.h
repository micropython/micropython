#define MICROPY_HW_BOARD_NAME       "PYBSTICK26_STD"
#define MICROPY_HW_MCU_NAME         "STM32F411RE"
#define MICROPY_PY_THREAD           (1)

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_KXTJ3        (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)

// HSE is 16MHz
#define MICROPY_HW_CLK_PLLM (16)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_LAST_FREQ (1)

// Pybstick26 STD has an optional 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (0)

// UART config
#define MICROPY_HW_UART1_NAME   "XB"
#define MICROPY_HW_UART1_TX     (pin_A15)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_NAME   "XA"
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
// #define MICROPY_HW_UART2_RTS    (pin_A1)
// #define MICROPY_HW_UART2_CTS    (pin_A0)
#define MICROPY_HW_UART6_NAME   "YA"
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C buses
#define MICROPY_HW_I2C1_NAME "X"
#define MICROPY_HW_I2C1_SCL (pin_B8) // S5
#define MICROPY_HW_I2C1_SDA (pin_B9) // S3
#define MICROPY_HW_I2C2_NAME "Y"
#define MICROPY_HW_I2C2_SCL (pin_B10) // S13
#define MICROPY_HW_I2C2_SDA (pin_B3) // S11

// SPI buses
#define MICROPY_HW_SPI1_NAME "X"
#define MICROPY_HW_SPI1_NSS  (pin_A4) // S26
#define MICROPY_HW_SPI1_SCK  (pin_A5) // S23
#define MICROPY_HW_SPI1_MISO (pin_B4) // S21
#define MICROPY_HW_SPI1_MOSI (pin_A7) // S19
#define MICROPY_HW_SPI2_NAME "Y"
#define MICROPY_HW_SPI2_NSS  (pin_B12) // S15
#define MICROPY_HW_SPI2_SCK  (pin_B13) // S16
#define MICROPY_HW_SPI2_MISO (pin_B14) // S18
#define MICROPY_HW_SPI2_MOSI (pin_C3) // S13

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// The board has 4 LEDs
#define MICROPY_HW_LED1             (pin_A13) // red
#define MICROPY_HW_LED2             (pin_A14) // green
#define MICROPY_HW_LED3             (pin_B0) // yellow
#define MICROPY_HW_LED4             (pin_B1) // blue
#define MICROPY_HW_LED3_PWM         { TIM3, 3, TIM_CHANNEL_3, GPIO_AF2_TIM3 }
#define MICROPY_HW_LED4_PWM         { TIM3, 4, TIM_CHANNEL_4, GPIO_AF2_TIM3 }
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// SD card with custom SDIO pins
#define MICROPY_HW_SDCARD_D0 (pin_B7)
#define MICROPY_HW_SDCARD_D1 (pin_A8)
#define MICROPY_HW_SDCARD_D2 (pin_A9)
#define MICROPY_HW_SDCARD_D3 (pin_B5)
#define MICROPY_HW_SDCARD_CK (pin_B15)
#define MICROPY_HW_SDCARD_CMD (pin_A6)

// USB config
#define MICROPY_HW_USB_FS              (1)
