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

#include "events.h"

#include "clocks.h"

#include "py/runtime.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

#ifdef SAMD51
#include "hri/hri_mclk_d51.h"
#endif

void turn_on_event_system(void) {
    #ifdef SAMD51
    hri_mclk_set_APBBMASK_EVSYS_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, EVSYS);
    #endif
}

void reset_event_system(void) {
    #ifdef SAMD51
    EVSYS->CTRLA.bit.SWRST = true;
    hri_mclk_clear_APBBMASK_EVSYS_bit(MCLK);
    #endif

    #ifdef SAMD21
    EVSYS->CTRL.bit.SWRST = true;
    _pm_disable_bus_clock(PM_BUS_APBC, EVSYS);
    #endif
}

static bool channel_free(int8_t channel) {
    uint8_t generator;
    #ifdef SAMD51
    generator = EVSYS->Channel[channel].CHANNEL.bit.EVGEN;
    #endif
    #ifdef SAMD21
    // Explicitly do a byte write so the peripheral knows we're just wanting to read the channel
    // rather than write to it.
    *((uint8_t*) &EVSYS->CHANNEL.reg) = channel;
    generator = (EVSYS->CHANNEL.reg & EVSYS_CHANNEL_EVGEN_Msk) >> EVSYS_CHANNEL_EVGEN_Pos;
    #endif
    return generator == 0;
}

uint8_t find_async_event_channel(void) {
    int8_t channel;
    for (channel = EVSYS_CHANNELS - 1; channel > 0; channel--) {
        if (channel_free(channel)) {
            break;
        }
    }
    if (channel < 0) {
        mp_raise_RuntimeError("All event channels in use");
    }
    return channel;
}

#ifdef SAMD21
#define EVSYS_SYNCH_NUM EVSYS_CHANNELS
#endif
uint8_t find_sync_event_channel(void) {
    int8_t channel;
    for (channel = 0; channel < EVSYS_SYNCH_NUM; channel++) {
        if (channel_free(channel)) {
            break;
        }
    }
    if (channel >= EVSYS_SYNCH_NUM) {
        mp_raise_RuntimeError("All sync event channels in use");
    }
    return channel;
}

void disable_event_channel(uint8_t channel_number) {
    #ifdef SAMD51
    EVSYS->Channel[channel_number].CHANNEL.reg = 0;
    #endif
    #ifdef SAMD21
    EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(channel_number);
    #endif
}

void disable_event_user(uint8_t user_number) {
    #ifdef SAMD51
    EVSYS->USER[user_number].reg = 0;
    #endif
    #ifdef SAMD21
    EVSYS->USER.reg = EVSYS_USER_USER(user_number);
    #endif
}

void connect_event_user_to_channel(uint8_t user, uint8_t channel) {
    #ifdef SAMD51
    EVSYS->USER[user].reg = channel + 1;
    #endif
    #ifdef SAMD21
    EVSYS->USER.reg = EVSYS_USER_USER(user) | EVSYS_USER_CHANNEL(channel + 1);
    #endif
}

void init_async_event_channel(uint8_t channel, uint8_t generator) {
    #ifdef SAMD51
    EVSYS->Channel[channel].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(generator) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
    #endif
    #ifdef SAMD21
    EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(channel) | EVSYS_CHANNEL_EVGEN(generator) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
    #endif
}

void init_event_channel_interrupt(uint8_t channel, uint8_t gclk, uint8_t generator) {
    connect_gclk_to_peripheral(gclk, EVSYS_GCLK_ID_0 + channel);
    #ifdef SAMD51
    EVSYS->Channel[channel].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(generator) |
                                          EVSYS_CHANNEL_PATH_SYNCHRONOUS |
                                          EVSYS_CHANNEL_EDGSEL_RISING_EDGE;
    #endif
    #ifdef SAMD21
    EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(channel) |
                         EVSYS_CHANNEL_EVGEN(generator) |
                         EVSYS_CHANNEL_PATH_RESYNCHRONIZED |
                         EVSYS_CHANNEL_EDGSEL_RISING_EDGE;
    if (channel > 7) {
        uint8_t value = 1 << (channel - 7);
        EVSYS->INTFLAG.reg = EVSYS_INTFLAG_EVDp8(value) | EVSYS_INTFLAG_OVRp8(value);
        EVSYS->INTENSET.reg = EVSYS_INTENSET_EVDp8(value) | EVSYS_INTENSET_OVRp8(value);
    } else {
        uint8_t value = 1 << channel;
        EVSYS->INTFLAG.reg = EVSYS_INTFLAG_EVD(value) | EVSYS_INTFLAG_OVR(value);
        EVSYS->INTENSET.reg = EVSYS_INTENSET_EVD(value) | EVSYS_INTENSET_OVR(value);
    }
    #endif
}

bool event_interrupt_active(uint8_t channel) {
    bool active = false;
    #ifdef SAMD21
    if (channel > 7) {
        uint8_t value = 1 << (channel - 7);
        active = (EVSYS->INTFLAG.reg & EVSYS_INTFLAG_EVDp8(value)) != 0;
        // Only clear if we know its active, otherwise there is the possibility it becomes active
        // after we check but before we clear.
        if (active) {
            EVSYS->INTFLAG.reg = EVSYS_INTFLAG_EVDp8(value) | EVSYS_INTFLAG_OVRp8(value);
        }
    } else {
        uint8_t value = 1 << channel;
        active = (EVSYS->INTFLAG.reg & EVSYS_INTFLAG_EVD(value)) != 0;
        if (active) {
            EVSYS->INTFLAG.reg = EVSYS_INTFLAG_EVD(value) | EVSYS_INTFLAG_OVR(value);
        }
    }
    #endif
    #ifdef SAMD51
    active = EVSYS->Channel[channel].CHINTFLAG.bit.EVD;
    // Only clear if we know its active, otherwise there is the possibility it becomes after we
    // check but before we clear.
    if (active) {
        EVSYS->Channel[channel].CHINTFLAG.reg = EVSYS_CHINTFLAG_EVD;
    }
    #endif
    return active;
}
