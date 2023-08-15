/*
 * This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022 Arduino SA
 */

#define MICROPY_HW_BOARD_NAME           "Arduino Nano 33 BLE Sense"
#define MICROPY_HW_MCU_NAME             "NRF52840"

#define MICROPY_MBFS                    (1)

#define MICROPY_BOARD_EARLY_INIT        NANO33_board_early_init
#define MICROPY_BOARD_DEINIT            NANO33_board_deinit
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) NANO33_board_enter_bootloader()

#define MICROPY_PY_MACHINE_UART         (1)
#define MICROPY_PY_MACHINE_PWM          (1)
#define MICROPY_PY_MACHINE_HW_PWM       (1)
#define MICROPY_PY_MACHINE_HW_SPI       (1)
#define MICROPY_PY_MACHINE_RTCOUNTER    (1)
#define MICROPY_PY_MACHINE_I2C          (1)
#define MICROPY_PY_MACHINE_ADC          (1)
#define MICROPY_PY_MACHINE_TEMP         (1)

#define MICROPY_HW_USB_CDC              (1)
#define MICROPY_HW_HAS_LED              (1)
#define MICROPY_HW_HAS_SWITCH           (0)
#define MICROPY_HW_HAS_FLASH            (0)
#define MICROPY_HW_HAS_SDCARD           (0)
#define MICROPY_HW_HAS_MMA7660          (0)
#define MICROPY_HW_HAS_LIS3DSH          (0)
#define MICROPY_HW_HAS_LCD              (0)
#define MICROPY_HW_ENABLE_RNG           (1)
#define MICROPY_HW_ENABLE_RTC           (1)
#define MICROPY_HW_ENABLE_TIMER         (0)
#define MICROPY_HW_ENABLE_SERVO         (0)
#define MICROPY_HW_ENABLE_DAC           (0)
#define MICROPY_HW_ENABLE_CAN           (0)

// LEDs config
#define MICROPY_HW_LED_COUNT            (4)     // 3 RGB + 1 Yellow
#define MICROPY_HW_LED_PULLUP           (1)     // RGB LED is active low
#define MICROPY_HW_LED4_PULLUP          (0)     // Yellow is active high
#define MICROPY_HW_LED1                 (24)    // RED
#define MICROPY_HW_LED2                 (16)    // GREEN
#define MICROPY_HW_LED3                 (6)     // BLUE
#define MICROPY_HW_LED4                 (13)    // Yellow
#define HELP_TEXT_BOARD_LED             "1,2,3,4"

// UART config
#define MICROPY_HW_UART1_TX             (32 + 3)
#define MICROPY_HW_UART1_RX             (32 + 10)
// #define MICROPY_HW_UART1_CTS            (7)
// #define MICROPY_HW_UART1_RTS            (5)
// #define MICROPY_HW_UART1_HWFC           (1)

// SPI0 config
#define MICROPY_HW_SPI0_NAME            "SPI0"
#define MICROPY_HW_SPI0_SCK             (13)
#define MICROPY_HW_SPI0_MOSI            (32 + 1)
#define MICROPY_HW_SPI0_MISO            (32 + 8)

// PWM config
#define MICROPY_HW_PWM0_NAME            "PWM0"
#define MICROPY_HW_PWM1_NAME            "PWM1"
#define MICROPY_HW_PWM2_NAME            "PWM2"
#define MICROPY_HW_PWM3_NAME            "PWM3"

#define MICROPY_HW_USB_VID              (0x2341)
#define MICROPY_HW_USB_PID              (0x025A)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH (1)

void NANO33_board_early_init(void);
void NANO33_board_deinit(void);
void NANO33_board_enter_bootloader(void);
