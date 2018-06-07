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

#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "peripherals/external_interrupts.h"

#include "sam.h"

// This structure is used to share per-channel storage amongst all users of external interrupts.
// Without this there would be multiple arrays even though they are disjoint because each channel
// has one user.
static void *channel_data[EIC_EXTINT_NUM];
static uint8_t channel_handler[EIC_EXTINT_NUM];

void external_interrupt_handler(uint8_t channel) {
    uint8_t handler = channel_handler[channel];
    if (handler == EIC_HANDLER_PULSEIN) {
        pulsein_interrupt_handler(channel);
    } else if (handler == EIC_HANDLER_INCREMENTAL_ENCODER) {
        incrementalencoder_interrupt_handler(channel);
    }
    EIC->INTFLAG.reg = (1 << channel) << EIC_INTFLAG_EXTINT_Pos;
}

void configure_eic_channel(uint8_t eic_channel, uint32_t sense_setting) {
    uint8_t config_index = eic_channel / 8;
    uint8_t position = (eic_channel % 8) * 4;
    #ifdef SAMD51
    eic_set_enable(false);
    #endif
    common_hal_mcu_disable_interrupts();
    uint32_t masked_value = EIC->CONFIG[config_index].reg & ~(0xf << position);
    EIC->CONFIG[config_index].reg = masked_value | (sense_setting << position);
    common_hal_mcu_enable_interrupts();
    #ifdef SAMD51
    eic_set_enable(true);
    #endif
}

void turn_on_eic_channel(uint8_t eic_channel, uint32_t sense_setting,
                         uint8_t channel_interrupt_handler) {
    // We do very light filtering using majority voting.
    sense_setting |= EIC_CONFIG_FILTEN0;
    configure_eic_channel(eic_channel, sense_setting);
    uint32_t mask = 1 << eic_channel;
    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;
    if (channel_interrupt_handler != EIC_HANDLER_NO_INTERRUPT) {
        channel_handler[eic_channel] = channel_interrupt_handler;
        turn_on_cpu_interrupt(eic_channel);
    }
}

void turn_off_eic_channel(uint8_t eic_channel) {
    uint32_t mask = 1 << eic_channel;
    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
    #ifdef SAMD51
    NVIC_DisableIRQ(EIC_0_IRQn + eic_channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + eic_channel);
    #endif
    channel_data[eic_channel] = NULL;

    #ifdef SAMD21
    if (EIC->INTENSET.reg == 0) {
        NVIC_DisableIRQ(EIC_IRQn);
        NVIC_ClearPendingIRQ(EIC_IRQn);
    }
    #endif
    // Test if all channels are null and deinit everything if they are.
    if (EIC->EVCTRL.reg == 0 && EIC->INTENSET.reg == 0) {
        turn_off_external_interrupt_controller();
    }
}

void* get_eic_channel_data(uint8_t eic_channel) {
    return channel_data[eic_channel];
}

void set_eic_channel_data(uint8_t eic_channel, void* data) {
    channel_data[eic_channel] = data;
}
