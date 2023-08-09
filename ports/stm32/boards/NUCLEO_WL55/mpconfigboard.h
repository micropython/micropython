/* This file is part of the MicroPython project, https://micropython.org/
 * MIT License; Copyright (c) 2021 Damien P. George
 */

#define MICROPY_HW_BOARD_NAME                   "NUCLEO-WL55"
#define MICROPY_HW_MCU_NAME                     "STM32WL55JCI7"

#define MICROPY_EMIT_THUMB                      (0)
#define MICROPY_EMIT_INLINE_THUMB               (0)
#define MICROPY_PY_BUILTINS_COMPLEX             (0)
#define MICROPY_PY_GENERATOR_PEND_THROW         (0)
#define MICROPY_PY_MATH                         (0)
#define MICROPY_PY_FRAMEBUF                     (0)
#define MICROPY_PY_SOCKET                       (0)
#define MICROPY_PY_NETWORK                      (0)
#define MICROPY_PY_ONEWIRE                      (0)
#define MICROPY_PY_STM                          (1) // for subghz radio functions
#define MICROPY_PY_STM_CONST                    (0) // saves size, no named registers
#define MICROPY_PY_PYB_LEGACY                   (0)
#define MICROPY_PY_HEAPQ                        (0)

#define MICROPY_HW_HAS_FLASH                    (1)
#define MICROPY_HW_ENABLE_RTC                   (1)
#define MICROPY_HW_ENABLE_RNG                   (1)
#define MICROPY_HW_ENABLE_ADC                   (0) // use machine.ADC instead
#define MICROPY_HW_HAS_SWITCH                   (1)

// There is an external 32kHz oscillator
#define MICROPY_HW_RTC_USE_LSE                  (1)
#define MICROPY_HW_RTC_USE_US                   (1)

// Use external 32MHz TCXO + PLL as system clock source
// (If unset, board will use the internal MSI oscillator instead.)
#define MICROPY_HW_CLK_USE_HSE                  (1)

// HSE bypass for STM32WL5x means TCXO is powered from PB0_VDDTCXO pin
#define MICROPY_HW_CLK_USE_BYPASS               (1)

// UART buses
#define MICROPY_HW_UART1_TX                     (pin_B6)    // Arduino D1, pin 7 on CN9
#define MICROPY_HW_UART1_RX                     (pin_B7)    // Arduino D0, pin 8 on CN9
#define MICROPY_HW_LPUART1_TX                   (pin_A2)    // ST-link
#define MICROPY_HW_LPUART1_RX                   (pin_A3)    // ST-link
#define MICROPY_HW_UART_REPL                    PYB_LPUART_1
#define MICROPY_HW_UART_REPL_BAUD               115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                     (pin_A12)   // Arduino D15, pin 10 on CN5
#define MICROPY_HW_I2C1_SDA                     (pin_A11)   // Arduino D14, pin 9 on CN5
#define MICROPY_HW_I2C3_SCL                     (pin_B13)   // Arduino A5, pin 6 on CN8
#define MICROPY_HW_I2C3_SDA                     (pin_B14)   // Arduino A4, pin 5 on CN8

// SPI buses
#define MICROPY_HW_SPI1_NSS                     (pin_A4)    // Arduino D10  pin 3 on CN5
#define MICROPY_HW_SPI1_SCK                     (pin_A5)    // Arduino D13, pin 6 on CN5
#define MICROPY_HW_SPI1_MISO                    (pin_A6)    // Arduino D12, pin 5 on CN5
#define MICROPY_HW_SPI1_MOSI                    (pin_A7)    // Arduino D11, pin 4 on CN5

// SUBGHZSPI Internal radio SPI BUS
#define MICROPY_HW_SUBGHZSPI_NAME               "SUBGHZ"
#define MICROPY_HW_SUBGHZSPI_ID                 3

// User switch; pressing the button makes the input go low
#define MICROPY_HW_USRSW_PIN                    (pin_A0)
#define MICROPY_HW_USRSW_PULL                   (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE              (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED                (0)

// LEDs
#define MICROPY_HW_LED1                         (pin_B15) // blue
#define MICROPY_HW_LED2                         (pin_B9) // green
#define MICROPY_HW_LED3                         (pin_B11) // red
#define MICROPY_HW_LED_ON(pin)                  (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)                 (mp_hal_pin_low(pin))
