/* This file is part of the Thanh20VN, https://github.com/thanh20VN
 * MIT License; Copyright (c) 2026 Thanh20VN
 */

#define MICROPY_HW_BOARD_NAME       "WEACT-STM32WB55CGU6"
#define MICROPY_HW_MCU_NAME         "STM32WB55CGU6"

#define MICROPY_PY_PYB_LEGACY       (0)

#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_USB       (1)

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV           (0)
#define RTC_SYNCH_PREDIV            (0x7fff)
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (1)

// UART buses
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)

// SPI buses
#define MICROPY_HW_SPI1_SCK         (pin_A5) // pin 8 on CN1
#define MICROPY_HW_SPI1_MISO        (pin_A6) // pin 9 on CN1
#define MICROPY_HW_SPI1_MOSI        (pin_A7) // pin 10 on CN1

// LEDs
#define MICROPY_HW_LED1             (pin_E4)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_CDC_RX_DATA_SIZE (512)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE (512)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID       (1)
#define MICROPY_HW_BLE_UART_BAUDRATE (115200)