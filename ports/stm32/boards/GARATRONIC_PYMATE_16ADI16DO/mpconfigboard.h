/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Garatronic SAS, thanks to  Damien P. George
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

#define MICROPY_HW_BOARD_NAME       "PYMATE 16ADI16DO V0.1.1"
#define MICROPY_HW_MCU_NAME         "STM32F412RE"
#define MICROPY_PY_THREAD           (1)

#define MICROPY_PY_PYB_LEGACY       (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_SDCARD    (0)

// HSE is 16MHz
#define MICROPY_HW_CLK_PLLM (16)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_LAST_FREQ (1)
#define MICROPY_HW_CLK_AHB_DIV      (RCC_SYSCLK_DIV1)
#define MICROPY_HW_CLK_APB1_DIV     (RCC_HCLK_DIV2)
#define MICROPY_HW_CLK_APB2_DIV     (RCC_HCLK_DIV1)
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3

// Pybstick26 STD has an optional 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (0)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (0)

// UART config
#define MICROPY_HW_UART1_NAME   "X"
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

// USART1 support REPL
#define MICROPY_HW_UART_REPL PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD 115200

// CAN buse
#define MICROPY_HW_CAN1_NAME        "X"
#define MICROPY_HW_CAN1_TX          (pin_B9)
#define MICROPY_HW_CAN1_RX          (pin_B8)

// I2C buses
// idem carte core, fmpi2c :-(
//#define MICROPY_HW_I2C1_NAME        "X"
//#define MICROPY_HW_I2C1_SCL         (pin_B15)
//#define MICROPY_HW_I2C1_SDA         (pin_B14)
//#define MICROPY_HW_I2C1_SMB         (pin_B13)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_B12)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// The board has 2 LEDs, LED1 & LED2 as fusionned
#define MICROPY_HW_LED1             (pin_C14) // red
#define MICROPY_HW_LED2             (pin_C14) // green
#define MICROPY_HW_LED3             (pin_C15) // yellow
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_ENABLE_USB       (0)
#define MICROPY_HW_USB_FS           (0)
#define MICROPY_HW_FLASH_FS_LABEL   "16adi16do"
