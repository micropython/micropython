#define STM32E407

#define MICROPY_HW_BOARD_NAME       "OLIMEX STM32-E407"
#define MICROPY_HW_MCU_NAME         "STM32F407"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_CAN       (1)

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#define MICROPY_HW_UART1_PORT (GPIOB)
#define MICROPY_HW_UART1_PINS (GPIO_PIN_6 | GPIO_PIN_7)

#define MICROPY_HW_UART2_PORT (GPIOA)
#define MICROPY_HW_UART2_PINS (GPIO_PIN_2 | GPIO_PIN_3)
#define MICROPY_HW_UART2_RTS  (GPIO_PIN_1)
#define MICROPY_HW_UART2_CTS  (GPIO_PIN_0)

#define MICROPY_HW_UART3_PORT (GPIOD)
#define MICROPY_HW_UART3_PINS (GPIO_PIN_8 | GPIO_PIN_9)
#define MICROPY_HW_UART3_RTS  (GPIO_PIN_12)
#define MICROPY_HW_UART3_CTS  (GPIO_PIN_11)

#if MICROPY_HW_HAS_SWITCH == 0
// NOTE: A0 also connects to the user switch. To use UART4 you should 
//       set MICROPY_HW_HAS_SWITCH to 0, and also remove SB20 (on the back
//       of the board near the USER switch).
#define MICROPY_HW_UART4_PORT (GPIOA)
#define MICROPY_HW_UART4_PINS (GPIO_PIN_0 | GPIO_PIN_1)
#endif

#define MICROPY_HW_UART6_PORT (GPIOC)
#define MICROPY_HW_UART6_PINS (GPIO_PIN_6 | GPIO_PIN_7)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// SPI busses
#define MICROPY_HW_SPI1_NSS  (pin_A4)
#define MICROPY_HW_SPI1_SCK  (pin_A5)
#define MICROPY_HW_SPI1_MISO (pin_A6)
#define MICROPY_HW_SPI1_MOSI (pin_A7)
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_C13)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C11)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
