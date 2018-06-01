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

#include "peripherals/clocks.h"
#include "sam.h"

void turn_on_external_interrupt_controller(void) {
    MCLK->APBAMASK.bit.EIC_ = true;

    // We use the 48mhz clock to lightly filter the incoming pulse to reduce spurious interrupts.
    connect_gclk_to_peripheral(GCLK_PCHCTRL_GEN_GCLK1_Val, EIC_GCLK_ID);
    eic_set_enable(true);
}

void turn_off_external_interrupt_controller(void) {
    eic_set_enable(false);
    MCLK->APBAMASK.bit.EIC_ = false;
    disconnect_gclk_from_peripheral(GCLK_PCHCTRL_GEN_GCLK1_Val, EIC_GCLK_ID);
}

void turn_on_cpu_interrupt(uint8_t eic_channel) {
    // Ignore the channel since the CPU interrupt line is shared.
    (void) eic_channel;

    NVIC_DisableIRQ(EIC_0_IRQn + eic_channel);
    NVIC_ClearPendingIRQ(EIC_0_IRQn + eic_channel);
    NVIC_EnableIRQ(EIC_0_IRQn + eic_channel);
}

bool eic_get_enable(void) {
    return EIC->CTRLA.bit.ENABLE;
}

void eic_set_enable(bool value) {
    EIC->CTRLA.bit.ENABLE = value;
    while (EIC->SYNCBUSY.bit.ENABLE != 0) {}
    // This won't actually block long enough in Rev A of SAMD51 and will miss edges in the first
    // three cycles of the peripheral clock. See the errata for details. It shouldn't impact us.
}

void eic_reset(void) {
    EIC->CTRLA.bit.SWRST = true;
    while (EIC->SYNCBUSY.bit.SWRST != 0) {}
    // This won't actually block long enough in Rev A of SAMD51 and will miss edges in the first
    // three cycles of the peripheral clock. See the errata for details. It shouldn't impact us.
    for (int i = 0; i < EIC_EXTINT_NUM; i++) {
        set_eic_channel_data(i, NULL);
        NVIC_DisableIRQ(EIC_0_IRQn + i);
        NVIC_ClearPendingIRQ(EIC_0_IRQn + i);
    }
}

bool eic_channel_free(uint8_t eic_channel) {
    uint32_t mask = 1 << eic_channel;
    return get_eic_channel_data(eic_channel) == NULL &&
           (EIC->INTENSET.bit.EXTINT & mask) == 0 &&
           (EIC->EVCTRL.bit.EXTINTEO & mask) == 0;
}

void EIC_0_Handler(void) {
    external_interrupt_handler(0);
}
void EIC_1_Handler(void) {
    external_interrupt_handler(1);
}
void EIC_2_Handler(void) {
    external_interrupt_handler(2);
}
void EIC_3_Handler(void) {
    external_interrupt_handler(3);
}
void EIC_4_Handler(void) {
    external_interrupt_handler(4);
}
void EIC_5_Handler(void) {
    external_interrupt_handler(5);
}
void EIC_6_Handler(void) {
    external_interrupt_handler(6);
}
void EIC_7_Handler(void) {
    external_interrupt_handler(7);
}
void EIC_8_Handler(void) {
    external_interrupt_handler(8);
}
void EIC_9_Handler(void) {
    external_interrupt_handler(9);
}
void EIC_10_Handler(void) {
    external_interrupt_handler(10);
}
void EIC_11_Handler(void) {
    external_interrupt_handler(11);
}
void EIC_12_Handler(void) {
    external_interrupt_handler(12);
}
void EIC_13_Handler(void) {
    external_interrupt_handler(13);
}
void EIC_14_Handler(void) {
    external_interrupt_handler(14);
}
void EIC_15_Handler(void) {
    external_interrupt_handler(15);
}
