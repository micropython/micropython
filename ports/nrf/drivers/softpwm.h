/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 The MicroPython-on-micro:bit Developers:
 * Damien P. George (@dpgeorge)
 * Nicholas H. Tollervey (@ntoll)
 * Matthew Else (@matthewelse)
 * Alan M. Jackson (@alanmjackson)
 * Mark Shannon (@markshannon)
 * Larry Hastings (@larryhastings)
 * Mariia Koroliuk (@marichkakorolyuk)
 * Andrew Mulholland (@gbaman)
 * Joe Glancy (@JoeGlancy)
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

#ifndef __MICROPY_INCLUDED_LIB_PWM_H__
#define __MICROPY_INCLUDED_LIB_PWM_H__

void softpwm_init0(void);
void pwm_start(void);
void pwm_stop(void);

int pwm_set_period_us(int32_t us);
int32_t pwm_get_period_us(void);
void pwm_set_duty_cycle(int32_t pin, int32_t value);
void pwm_release(int32_t pin);

#endif // __MICROPY_INCLUDED_LIB_PWM_H__
