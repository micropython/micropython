/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2023 Arduino SA
 */

// MCU config
#define MICROPY_HW_BOARD_NAME       "Arduino Portenta C33"
#define MICROPY_HW_MCU_NAME         "RA6M5"
#define MICROPY_HW_MCU_SYSCLK       200000000
#define MICROPY_HW_MCU_PCLK         100000000
#define MICROPY_HW_FLASH_FS_LABEL   "Portenta C33"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-portenta-c33"

// module config
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_PY_BUILTINS_COMPLEX (1)
#define MICROPY_PY_GENERATOR_PEND_THROW (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UTIMEQ           (1)
#define MICROPY_PY_THREAD           (0) // disable ARM_THUMB_FP using vldr due to RA has single float only
#define MICROPY_PY_NETWORK          (1)

// peripheral config
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_RTC_SOURCE       (0)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_USBDEV    (1)
#define MICROPY_HW_ENABLE_UART_REPL (0)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

#define MICROPY_BOARD_STARTUP       PORTENTA_C33_board_startup
void PORTENTA_C33_board_startup(void);

#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) PORTENTA_C33_board_enter_bootloader()
void PORTENTA_C33_board_enter_bootloader(void);

// board config
// USB
#define MICROPY_HW_USB_VID          (0x2341)
#define MICROPY_HW_USB_PID          (0x0468)

// UART
#define MICROPY_HW_UART9_TX         (pin_P602)
#define MICROPY_HW_UART9_RX         (pin_P110)
#define MICROPY_HW_UART9_CTS        (pin_P604)
#define MICROPY_HW_UART9_RTS        (pin_P603)

#define MICROPY_HW_UART8_TX         (pin_PA00)
#define MICROPY_HW_UART8_RX         (pin_P607)
#define MICROPY_HW_UART8_CTS        (pin_P801)
#define MICROPY_HW_UART8_RTS        (pin_P606)

#if MICROPY_HW_ENABLE_UART_REPL
#define MICROPY_HW_UART_REPL        (HW_UART_9)
#define MICROPY_HW_UART_REPL_BAUD   (115200)
#endif

// I2C
#define MICROPY_HW_I2C0_SCL         (pin_P408)
#define MICROPY_HW_I2C0_SDA         (pin_P407)

// SPI
#define MICROPY_HW_SPI1_SSL         (pin_P104)
#define MICROPY_HW_SPI1_RSPCK       (pin_P102)
#define MICROPY_HW_SPI1_MISO        (pin_P100)
#define MICROPY_HW_SPI1_MOSI        (pin_P101)

// DAC
#define MICROPY_HW_DAC0             (pin_P014)
#define MICROPY_HW_DAC1             (pin_P015)

// PWM
#define MICROPY_HW_PWM_1A           (pin_P105)
#define MICROPY_HW_PWM_8B           (pin_P106)
#define MICROPY_HW_PWM_3A           (pin_P111)
#define MICROPY_HW_PWM_7B           (pin_P303)
#define MICROPY_HW_PWM_6B           (pin_P401)
// #define MICROPY_HW_PWM            (pin_P210) // AGT
#define MICROPY_HW_PWM_6A           (pin_P601)

// LEDs
#define MICROPY_HW_LED1             (pin_P107)
#define MICROPY_HW_LED2             (pin_P400)
#define MICROPY_HW_LED3             (pin_P800)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_low(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_high(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)

// Bluetooth config.
#define MICROPY_HW_BLE_UART_ID          (8)
#define MICROPY_HW_BLE_UART_RTS         (MICROPY_HW_UART8_RTS)
#define MICROPY_HW_BLE_UART_BAUDRATE    (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY (921600)

// WiFi config.
#define MICROPY_HW_WIFI_SPI_ID          (1)
#define MICROPY_HW_WIFI_SPI_CS          (pin_P104)
#define MICROPY_HW_WIFI_SPI_BAUDRATE    (25 * 1000 * 1000)
#define MICROPY_HW_WIFI_DATAREADY       (pin_P803)
#define MICROPY_HW_WIFI_HANDSHAKE       (pin_P806)
#define MICROPY_HW_WIFI_IRQ_PIN         (MICROPY_HW_WIFI_DATAREADY)

// ESP hosted control pins
#define MICROPY_HW_ESP_HOSTED_RESET     (pin_P804)
#define MICROPY_HW_ESP_HOSTED_GPIO0     (pin_P803)
