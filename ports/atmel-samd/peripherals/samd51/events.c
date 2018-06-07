/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "peripherals/events.h"

#include "peripherals/clocks.h"

#include "py/runtime.h"

#include "hri/hri_mclk_d51.h"

void turn_on_event_system(void) {
    hri_mclk_set_APBBMASK_EVSYS_bit(MCLK);
}

void reset_event_system(void) {
    EVSYS->CTRLA.bit.SWRST = true;
    hri_mclk_clear_APBBMASK_EVSYS_bit(MCLK);
}

bool event_channel_free(uint8_t channel) {
    uint8_t generator;
    generator = EVSYS->Channel[channel].CHANNEL.bit.EVGEN;
    return generator == 0;
}

void disable_event_channel(uint8_t channel_number) {
    EVSYS->Channel[channel_number].CHANNEL.reg = 0;
}

void disable_event_user(uint8_t user_number) {
    EVSYS->USER[user_number].reg = 0;
}

void connect_event_user_to_channel(uint8_t user, uint8_t channel) {
    EVSYS->USER[user].reg = channel + 1;
}

void init_async_event_channel(uint8_t channel, uint8_t generator) {
    EVSYS->Channel[channel].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(generator) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
}

void init_event_channel_interrupt(uint8_t channel, uint8_t gclk, uint8_t generator) {
    connect_gclk_to_peripheral(gclk, EVSYS_GCLK_ID_0 + channel);
    EVSYS->Channel[channel].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(generator) |
                                          EVSYS_CHANNEL_PATH_SYNCHRONOUS |
                                          EVSYS_CHANNEL_EDGSEL_RISING_EDGE;
    EVSYS->Channel[channel].CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD | EVSYS_CHINTFLAG_OVR;
    EVSYS->Channel[channel].CHINTENSET.reg = EVSYS_CHINTENSET_EVD | EVSYS_CHINTENSET_OVR;
}

bool event_interrupt_active(uint8_t channel) {
    bool active = false;
    active = EVSYS->Channel[channel].CHINTFLAG.bit.EVD;
    // Only clear if we know its active, otherwise there is the possibility it becomes after we
    // check but before we clear.
    if (active) {
        EVSYS->Channel[channel].CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD | EVSYS_CHINTFLAG_OVR;
    }
    return active;
}

bool event_interrupt_overflow(uint8_t channel) {
    return EVSYS->Channel[channel].CHINTFLAG.bit.OVR;
}
