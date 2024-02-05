/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "timer.h"
#include "extmod/mpbthci.h"

#if !BUILDING_MBOOT

#define CC2564_PIN_CTS (pyb_pin_BT_CTS)
#define CC2564_PIN_RTS (pyb_pin_BT_RTS)
#define CC2564_PIN_BT_SLOWCLK (pyb_pin_BT_SLOWCLK)
#define CC2564_PIN_BT_ENABLE (pyb_pin_BT_ENABLE)

// slight difference between LEGO_HUB_NO6 and LEGO_HUB_NO7
#ifndef CC2564_TIMER_BT_SLOWCLOCK_TIM
#define CC2564_TIMER_BT_SLOWCLOCK_TIM 8
#define CC2564_TIMER_BT_SLOWCLOCK_TIM_CH 4
#endif

STATIC void cc2564_wait_cts_low(mp_hal_pin_obj_t cts, uint32_t timeout_ms) {
    for (int i = 0; i < timeout_ms; ++i) {
        if (mp_hal_pin_read(cts) == 0) {
            break;
        }
        mp_hal_delay_ms(1);
    }
}

int mp_bluetooth_hci_controller_init(void) {
    // Pull BTNSHUTD low to disable chip.
    mp_hal_pin_output(CC2564_PIN_BT_ENABLE);
    mp_hal_pin_low(CC2564_PIN_BT_ENABLE);

    // Output a 32768Hz signal on BTSLOWCLK.
    //  tim = pyb.Timer(TIM, freq=32768)
    //  tim_ch = tim.channel(TIM_CH, pyb.Timer.PWM, pin=btclk)
    //  tim_ch.pulse_width_percent(50)
    mp_obj_t args[6] = { MP_OBJ_NEW_SMALL_INT(CC2564_TIMER_BT_SLOWCLOCK_TIM), MP_OBJ_NEW_QSTR(MP_QSTR_freq), MP_OBJ_NEW_SMALL_INT(32768), MP_OBJ_NULL };
    mp_obj_t tim = MP_OBJ_TYPE_GET_SLOT(&pyb_timer_type, make_new)(&pyb_timer_type, 1, 1, args);
    mp_load_method(tim, MP_QSTR_channel, args);
    args[2] = MP_OBJ_NEW_SMALL_INT(CC2564_TIMER_BT_SLOWCLOCK_TIM_CH);
    args[3] = MP_OBJ_NEW_SMALL_INT(0); // CHANNEL_MODE_PWM_NORMAL
    args[4] = MP_OBJ_NEW_QSTR(MP_QSTR_pin);
    args[5] = (mp_obj_t)CC2564_PIN_BT_SLOWCLK;
    mp_obj_t tim_ch = mp_call_method_n_kw(2, 1, args);
    mp_load_method(tim_ch, MP_QSTR_pulse_width_percent, args);
    args[2] = MP_OBJ_NEW_SMALL_INT(50);
    mp_call_method_n_kw(1, 0, args);

    // Pull BTNSHUTD high to enable chip and wait for CTS to go low to indicate ready.
    mp_hal_pin_high(CC2564_PIN_BT_ENABLE);
    cc2564_wait_cts_low(CC2564_PIN_CTS, 500);

    return 0;
}

int mp_bluetooth_hci_controller_deinit(void) {
    // Pull BTNSHUTD low to disable chip.
    mp_hal_pin_low(CC2564_PIN_BT_ENABLE);

    return 0;
}

#endif
