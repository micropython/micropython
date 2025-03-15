/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define MICROPY_HW_BOARD_NAME       "XIAO nRF52840 Sense"
#define MICROPY_HW_MCU_NAME         "NRF52840"
#define MICROPY_PY_SYS_PLATFORM     "nrf52"

#define MICROPY_BOARD_EARLY_INIT        XIAO_board_early_init
#define MICROPY_BOARD_DEINIT            XIAO_board_deinit
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) XIAO_board_enter_bootloader()

#define MICROPY_HW_ENABLE_USBDEV    (1)
#define MICROPY_HW_USB_CDC          (1)
#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_HW_PWM   (1)
#define MICROPY_PY_MACHINE_RTCOUNTER (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_ADC      (1)
#define MICROPY_PY_MACHINE_TEMP     (1)
#define MICROPY_HW_HAS_FLASH        (1)

#define MICROPY_HW_ENABLE_RNG       (1)

#define MICROPY_HW_HAS_LED          (1)
#define MICROPY_HW_LED_COUNT        (4)
#define MICROPY_HW_LED_PULLUP       (1)

#define MICROPY_HW_LED1             (17) // LED1
#define MICROPY_HW_LED2             (26) // LED2
#define MICROPY_HW_LED3             (30) // LED3
#define MICROPY_HW_LED4             (6)  // LED4

// UART config
#define MICROPY_HW_UART1_TX         (32 + 11)
#define MICROPY_HW_UART1_RX         (32 + 12)

// SPI0 config
#define MICROPY_HW_SPI0_NAME        "SPI0"

#define MICROPY_HW_SPI0_SCK         (32 + 13)
#define MICROPY_HW_SPI0_MISO        (32 + 14)
#define MICROPY_HW_SPI0_MOSI        (32 + 15)

#define MICROPY_HW_PWM0_NAME        "PWM0"
#define MICROPY_HW_PWM1_NAME        "PWM1"
#define MICROPY_HW_PWM2_NAME        "PWM2"

#define HELP_TEXT_BOARD_LED         "1,2,3,4"

// Enabling these VID/PID values require to include mpconfigboard.h into usb_descriptors.c.
// But there is an open discussion on whether specific VID/PID values are wanted.
// 1200BPS touch is not supported at the moment in the USB driver, but at least the flag is set.
#define MICROPY_HW_USB_VID          (0x2886)
#define MICROPY_HW_USB_PID          (0x0045)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH (1)

void XIAO_board_early_init(void);
void XIAO_board_deinit(void);
void XIAO_board_enter_bootloader(void);
