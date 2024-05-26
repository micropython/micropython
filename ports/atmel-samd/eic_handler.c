// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/max3421e/Max3421E.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/ps2io/Ps2.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "common-hal/countio/Counter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/alarm/pin/PinAlarm.h"
// #include "samd/external_interrupts.h"
#include "eic_handler.h"

// Which handler should be called for a particular channel?
static uint8_t eic_channel_handler[EIC_EXTINT_NUM];

void set_eic_handler(uint8_t channel, uint8_t eic_handler) {
    eic_channel_handler[channel] = eic_handler;
}

void shared_eic_handler(uint8_t channel) {
    uint8_t handler = eic_channel_handler[channel];
    switch (handler) {
        #if CIRCUITPY_PULSEIO
        case EIC_HANDLER_PULSEIN:
            pulsein_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_PS2IO
        case EIC_HANDLER_PS2:
            ps2_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_ROTARYIO
        case EIC_HANDLER_INCREMENTAL_ENCODER:
            incrementalencoder_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_COUNTIO
        case EIC_HANDLER_COUNTER:
            counter_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_ALARM
        case EIC_HANDLER_ALARM:
            pin_alarm_callback(channel);
            break;
        #endif

        #if CIRCUITPY_MAX3421E
        case EIC_HANDLER_MAX3421E:
            samd_max3421e_interrupt_handler(channel);
            break;
        #endif

        default:
            break;
    }
}
