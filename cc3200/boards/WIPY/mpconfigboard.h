/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#define WIPY

#define MICROPY_HW_BOARD_NAME                       "WiPy"
#define MICROPY_HW_MCU_NAME                         "CC3200"

#define MICROPY_HW_ANTENNA_DIVERSITY                (1)

#define MICROPY_STDIO_UART                          1
#define MICROPY_STDIO_UART_BAUD                     115200

#define MICROPY_SYS_LED_PRCM                        PRCM_GPIOA3
#define MICROPY_SAFE_BOOT_PRCM                      PRCM_GPIOA3
#define MICROPY_SYS_LED_PORT                        GPIOA3_BASE
#define MICROPY_SAFE_BOOT_PORT                      GPIOA3_BASE
#define MICROPY_SYS_LED_GPIO                        pin_GP25
#define MICROPY_SYS_LED_PIN_NUM                     PIN_21      // GP25   (SOP2)
#define MICROPY_SAFE_BOOT_PIN_NUM                   PIN_18      // GP28
#define MICROPY_SYS_LED_PORT_PIN                    GPIO_PIN_1
#define MICROPY_SAFE_BOOT_PORT_PIN                  GPIO_PIN_4

#define MICROPY_PORT_SFLASH_BLOCK_COUNT             96
