/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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
#include "peripherals/dma.h"

#include <string.h>

#include "py/gc.h"
#include "py/mpstate.h"

#include "hal/utils/include/utils.h"

#include "shared-bindings/microcontroller/__init__.h"

uint8_t sercom_index(Sercom* sercom) {
    const Sercom* sercoms[SERCOM_INST_NUM] = SERCOM_INSTS;
    for (uint8_t i = 0; i < SERCOM_INST_NUM; i++) {
        if (sercoms[i] == sercom) {
            return i;
        }
    }
    return 0;
}

void dma_configure(uint8_t channel_number, uint8_t trigsrc, bool output_event) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    channel->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    channel->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    if (output_event) {
        channel->CHEVCTRL.reg = DMAC_CHEVCTRL_EVOE;
    }
    channel->CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(trigsrc) |
                           DMAC_CHCTRLA_TRIGACT_BURST |
                           DMAC_CHCTRLA_BURSTLEN_SINGLE;
}

void dma_enable_channel(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    channel->CHCTRLA.bit.ENABLE = true;
    // Clear any previous interrupts.
    channel->CHINTFLAG.reg = DMAC_CHINTFLAG_MASK;
}

void dma_disable_channel(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    channel->CHCTRLA.bit.ENABLE = false;
}

void dma_suspend_channel(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    channel->CHCTRLB.reg = DMAC_CHCTRLB_CMD_SUSPEND;
}

void dma_resume_channel(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    channel->CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;
}

bool dma_channel_enabled(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    return channel->CHCTRLA.bit.ENABLE;
}

uint8_t dma_transfer_status(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    return channel->CHINTFLAG.reg;
}

bool dma_channel_free(uint8_t channel_number) {
    DmacChannel* channel = &DMAC->Channel[channel_number];
    return channel->CHSTATUS.reg == 0;
}
