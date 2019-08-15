/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

static inline void mp_hal_set_interrupt_char(char c) {}



// C-leave pin 
#include "pin.h"
#define mp_hal_pin_obj_t const pin_obj_t*

/* 
 Use nxp fsl_gpio driver realize pin read and write 
 */

#define IS_GPIO_DIRECTION(d) (((d) == kGPIO_DigitalInput) || ((d) == kGPIO_DigitalOutput))
#define mp_hal_pin_read(p) GPIO_PinRead(p->gpio, p->pin) 
#define mp_hal_pin_write(p, v) GPIO_PinWrite(p->gpio, p->pin, v)

#define mp_hal_pin_high(p) mp_hal_pin_write(p,1)
#define mp_hal_pin_low(p) mp_hal_pin_write(p,0)

#define mp_hal_pin_toggle(p) GPIO_PortToggle(p->gpio, (1<<p->pin))


//void mp_hal_delay_ms(mp_int_t delay_ms);
//void mp_hal_delay_us(mp_int_t delay_ms);
