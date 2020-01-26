#define MICROPY_HW_BOARD_NAME       "WEACTV20_F411"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"
#define MICROPY_HW_FLASH_FS_LABEL   "WEACTV20_F411"

#define MICROPY_PY_THREAD           (1)

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (0)
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
#define MICROPY_HW_SPI1_NSS     (pin_A4)
#define MICROPY_HW_SPI1_SCK     (pin_A5)
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)

#define MICROPY_HW_SPI2_NSS     (pin_B12)
#define MICROPY_HW_SPI2_SCK     (pin_B13)
#define MICROPY_HW_SPI2_MISO    (pin_B14)
#define MICROPY_HW_SPI2_MOSI    (pin_B15)

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

// USB config
#define MICROPY_HW_USB_FS (1)