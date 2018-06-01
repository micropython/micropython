/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Scott Shawcroft for Adafruit Industries
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
    return ((uint32_t) sercom - (uint32_t) SERCOM0) / 0x400;
}

void dma_configure(uint8_t channel_number, uint8_t trigsrc, bool output_event) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel_number));
    uint32_t event_output_enable = 0;
    if (output_event) {
        event_output_enable = DMAC_CHCTRLB_EVOE;
    }
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL_LVL0 |
            DMAC_CHCTRLB_TRIGSRC(trigsrc) |
            DMAC_CHCTRLB_TRIGACT_BEAT |
            event_output_enable;
    common_hal_mcu_enable_interrupts();
}

void dma_enable_channel(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    // Clear any previous interrupts.
    DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_MASK;
    DMAC->CHCTRLA.bit.ENABLE = true;
    common_hal_mcu_enable_interrupts();
}

void dma_disable_channel(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    DMAC->CHCTRLA.bit.ENABLE = false;
    common_hal_mcu_enable_interrupts();
}

void dma_suspend_channel(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    DMAC->CHCTRLB.bit.CMD = DMAC_CHCTRLB_CMD_SUSPEND_Val;
    common_hal_mcu_enable_interrupts();
}

void dma_resume_channel(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    DMAC->CHCTRLB.bit.CMD = DMAC_CHCTRLB_CMD_RESUME_Val;
    DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_SUSP;
    common_hal_mcu_enable_interrupts();
}

bool dma_channel_enabled(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    bool enabled = DMAC->CHCTRLA.bit.ENABLE;
    common_hal_mcu_enable_interrupts();
    return enabled;
}

uint8_t dma_transfer_status(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    uint8_t status = DMAC->CHINTFLAG.reg;
    common_hal_mcu_enable_interrupts();
    return status;
}

bool dma_channel_free(uint8_t channel_number) {
    common_hal_mcu_disable_interrupts();
    DMAC->CHID.reg = DMAC_CHID_ID(channel_number);
    bool channel_free = DMAC->CHSTATUS.reg == 0;
    common_hal_mcu_enable_interrupts();
    return channel_free;
}
