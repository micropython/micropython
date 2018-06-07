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

#include "peripherals/external_interrupts.h"

#include "hpl/gclk/hpl_gclk_base.h"
#include "peripherals/clocks.h"
#include "sam.h"

void turn_on_external_interrupt_controller(void) {
    PM->APBAMASK.bit.EIC_ = true;
    _gclk_enable_channel(EIC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
    eic_set_enable(true);
}

void turn_off_external_interrupt_controller(void) {
    eic_set_enable(false);
    PM->APBAMASK.bit.EIC_ = false;
    hri_gclk_write_CLKCTRL_reg(GCLK, GCLK_CLKCTRL_ID(EIC_GCLK_ID));
}

void turn_on_cpu_interrupt(uint8_t eic_channel) {
    // Ignore the channel since the CPU interrupt line is shared.
    (void) eic_channel;

    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
}

bool eic_get_enable(void) {
    return EIC->CTRL.bit.ENABLE;
}

void eic_set_enable(bool value) {
    EIC->CTRL.bit.ENABLE = value;
    while (EIC->STATUS.bit.SYNCBUSY != 0) {}
}

void eic_reset(void) {
    EIC->CTRL.bit.SWRST = true;
    while (EIC->STATUS.bit.SYNCBUSY != 0) {}
    for (int i = 0; i < EIC_EXTINT_NUM; i++) {
        set_eic_channel_data(i, NULL);
    }
    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
}

bool eic_channel_free(uint8_t eic_channel) {
    uint32_t mask = 1 << eic_channel;
    return get_eic_channel_data(eic_channel) == NULL &&
           (EIC->INTENSET.vec.EXTINT & mask) == 0 &&
           (EIC->EVCTRL.vec.EXTINTEO & mask) == 0;
}

void EIC_Handler(void) {
    for (uint8_t i = 0; i < 16; i++) {
        if ((EIC->INTFLAG.vec.EXTINT & (1 << i)) != 0) {
            external_interrupt_handler(i);
        }
    }
}
