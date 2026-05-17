/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 - 2018 Glenn Ruben Bakke
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

#ifndef LED_H
#define LED_H

typedef enum {
#if MICROPY_HW_LED_TRICOLOR
    BOARD_LED_RED = 1,
    BOARD_LED_GREEN = 2,
    BOARD_LED_BLUE = 3
#elif (MICROPY_HW_LED_COUNT == 1)
    BOARD_LED1 = 1,
#elif (MICROPY_HW_LED_COUNT == 2)
    BOARD_LED1 = 1,
    BOARD_LED2 = 2,
#elif (MICROPY_HW_LED_COUNT == 3)
    BOARD_LED1 = 1,
    BOARD_LED2 = 2,
    BOARD_LED3 = 3,
#else
    BOARD_LED1 = 1,
    BOARD_LED2 = 2,
    BOARD_LED3 = 3,
    BOARD_LED4 = 4
#endif
} board_led_t;

void led_init(void);
void led_state(board_led_t, int);
void led_toggle(board_led_t);

extern const mp_obj_type_t board_led_type;

#endif // LED_H
