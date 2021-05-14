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

//Boar data
#define MICROPY_HW_BOARD_NAME       "EVK-NINA-B3"	//Board name
#define MICROPY_HW_MCU_NAME         "NRF52840"	//Microcontroller name
#define MICROPY_PY_SYS_PLATFORM     "nrf52"	//Returns when you call sys.ṕlatform

//Peripherals Config
#define MICROPY_PY_MACHINE_UART     (1)	// Enabels UART
#define MICROPY_PY_MACHINE_HW_PWM   (1)	// Enables Hardware PWM
#define MICROPY_PY_MACHINE_HW_SPI   (1)	// Enables Hardware SPI
#define MICROPY_PY_MACHINE_TIMER    (1)	// Enables Timer
#define MICROPY_PY_MACHINE_RTCOUNTER (1)	// Enables RTC
#define MICROPY_PY_MACHINE_I2C      (1)	// Enables I2C
#define MICROPY_PY_MACHINE_ADC      (1)	// Enables ADC
#define MICROPY_PY_MACHINE_TEMP     (1)	// Enables Temperature Sensor
#define MICROPY_HW_ENABLE_RNG       (1)	// Enables Random number generator

// Pin numbering scheme for nrf52840-based boards
//
// Software Pins 0-31 correspond to physical pins
// 0.x and software Pins 32-47 correspond to physical pins 1.x.
//
// Example: Pin(47) would be 1.15 on the PCA10059

//Configure LEDS
#define MICROPY_HW_HAS_LED          (1)	// If the board has LEDs
#define MICROPY_HW_LED_COUNT        (3)	// How many LEDs
#define MICROPY_HW_LED_PULLUP       (1)	// Tf the LEDs need pull-up

#define MICROPY_HW_LED1             (13) 	// LED1 RED
#define MICROPY_HW_LED2             (25) 	// LED2 GREEN
#define MICROPY_HW_LED3             (32) 	// LED3 BLUE

// UART config
#define MICROPY_HW_UART1_RX         (29)	// RX Pin
#define MICROPY_HW_UART1_TX         (45)	// TX Pin
#define MICROPY_HW_UART1_CTS        (44)	// CTS Pin (IF HWFC Enabled)
#define MICROPY_HW_UART1_RTS        (31)	// RTS Pin (IF HWFC Enabled)
#define MICROPY_HW_UART1_HWFC       (1)	//ENABLE HARDWARE FLOW CONTROL

// SPIs config
// configs the default SPI0 Pins
// I Chose these arbitralirly
#define MICROPY_HW_SPI0_NAME        "SPI0"	// SPI0 Name
#define MICROPY_HW_SPI0_SCK         (12)	// SCK Pin
#define MICROPY_HW_SPI0_MOSI        (14)	// MOSI Pin
#define MICROPY_HW_SPI0_MISO        (15)	// MISO Pin

// PWM Names
#define MICROPY_HW_PWM0_NAME        "PWM0"	// PWM0 Name
#define MICROPY_HW_PWM1_NAME        "PWM1"	// PWM1 Name
#define MICROPY_HW_PWM2_NAME        "PWM2"	// PWM2 Name

// How Many LED indexes appear in the help() message
#define HELP_TEXT_BOARD_LED         "1,2,3"

// Enable @viper and @native
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)

// Enable optional modules
#define MICROPY_PY_UERRNO           (1)     // Enable uerrno for working better with Pymakr Extension
#define MICROPY_PY_UHASHLIB         (1)     // Enable uhashlib for working better with Pymakr Extension