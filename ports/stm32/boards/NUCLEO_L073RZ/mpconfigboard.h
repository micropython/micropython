/* This file is part of the MicroPython project, http://micropython.org/
 * MIT License; Copyright (c) 2019 Damien P. George
 */

#define MICROPY_HW_BOARD_NAME       "NUCLEO-L073RZ"
#define MICROPY_HW_MCU_NAME         "STM32L073RZT6"

#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_FRAMEBUF         (0)
#define MICROPY_PY_USOCKET          (0)
#define MICROPY_PY_NETWORK          (0)
#define MICROPY_PY_STM              (0)
#define MICROPY_PY_PYB_LEGACY       (0)
#define MICROPY_VFS_FAT             (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_ADC       (0)
#define MICROPY_HW_HAS_SWITCH       (1)

// UART config
#define MICROPY_HW_UART1_TX     (pin_B6)
#define MICROPY_HW_UART1_RX     (pin_B7)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)

// USART2 is connected to the ST-LINK USB VCP
#define MICROPY_HW_UART_REPL PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD 115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)        // Arduino D15, pin 3 on CN10
#define MICROPY_HW_I2C1_SDA (pin_B9)        // Arduino D14, pin 5 on CN10

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A15)   //              pin 17 on CN7
#define MICROPY_HW_SPI1_SCK     (pin_A5)    // Arduino D13, pin 11 on CN10
#define MICROPY_HW_SPI1_MISO    (pin_A6)    // Arduino D12, pin 13 on CN10
#define MICROPY_HW_SPI1_MOSI    (pin_A7)    // Arduino D11, pin 15 on CN10

// USER B1 has a pull-up and is active low
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (0)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// NUCLEO-64 has one user LED
#define MICROPY_HW_LED1             (pin_A5) // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))
