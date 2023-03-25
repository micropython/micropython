/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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

#include <string.h>
#include "py/mpconfig.h"
#include "sam.h"
#include "dma_manager.h"
#include "samd_soc.h"

#if MICROPY_HW_DMA_MANAGER

// Set a number of dma channels managed here. samd21 has 21 dma channels, samd51
// has 32 channels, as defined by the lib macro DMAC_CH_NUM.
// At first, we use a smaller number here to save RAM. May be increased as needed.

#ifndef MICROPY_HW_DMA_CHANNELS
#if defined(MCU_SAMD21)
#define MICROPY_HW_DMA_CHANNELS       2
#elif defined(MCU_SAMD51)
#define MICROPY_HW_DMA_CHANNELS       4
#endif
#endif

#if MICROPY_HW_DMA_CHANNELS > DMAC_CH_NUM
#error Number of DMA channels too large
#endif

volatile DmacDescriptor dma_desc[MICROPY_HW_DMA_CHANNELS] __attribute__ ((aligned(16)));
static volatile DmacDescriptor dma_write_back[MICROPY_HW_DMA_CHANNELS] __attribute__ ((aligned(16)));

// List of channel flags: true: channel used, false: channel available
static bool channel_list[MICROPY_HW_DMA_CHANNELS];

static bool dma_initialized = false;

// allocate_channel(): retrieve an available channel. Return the number or -1
int allocate_dma_channel(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(channel_list); i++) {
        if (channel_list[i] == false) { // Channel available
            channel_list[i] = true;
            return i;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("no dma channel available"));
}

// free_channel(n): Declare channel as free
void free_dma_channel(int n) {
    if (n >= 0 && n < MP_ARRAY_SIZE(channel_list)) {
        channel_list[n] = false;
    }
}

void dma_init(void) {
    if (!dma_initialized) {

        // Enable the DMA clock
        #if defined(MCU_SAMD21)
        PM->AHBMASK.reg |= PM_AHBMASK_DMAC;
        PM->APBBMASK.reg |= PM_APBBMASK_DMAC;
        #elif defined(MCU_SAMD51)
        MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
        #endif
        // Setup the initial DMA configuration
        DMAC->CTRL.reg = DMAC_CTRL_SWRST;
        while (DMAC->CTRL.reg & DMAC_CTRL_SWRST) {
        }
        // Set the DMA descriptor pointers
        DMAC->BASEADDR.reg = (uint32_t)dma_desc;
        DMAC->WRBADDR.reg = (uint32_t)dma_write_back;
        // Enable the DMA
        DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);

        dma_initialized = true;
    }
}

void dma_deinit(void) {
    memset((uint8_t *)dma_desc, 0, sizeof(dma_desc));
    memset((uint8_t *)dma_write_back, 0, sizeof(dma_write_back));
    memset((uint8_t *)channel_list, 0, sizeof(channel_list));
    dma_initialized = false;
    // Disable DMA
    DMAC->CTRL.reg = 0;
    for (int ch = 0; ch < DMAC_CH_NUM; ch++) {
        dma_register_irq(ch, NULL);
    }
}

void dac_stop_dma(int dma_channel, bool wait) {
    #if defined(MCU_SAMD21)
    NVIC_DisableIRQ(DMAC_IRQn);
    DMAC->CHID.reg = dma_channel;
    DMAC->CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL | DMAC_CHINTENSET_TERR | DMAC_CHINTENSET_SUSP;
    DMAC->CHCTRLA.reg = 0;
    while (wait && DMAC->CHCTRLA.bit.ENABLE) {
    }
    #elif defined(MCU_SAMD51)
    if (0 <= dma_channel && dma_channel < 4) {
        NVIC_DisableIRQ(DMAC_0_IRQn + dma_channel);
    } else if (dma_channel >= 4) {
        NVIC_DisableIRQ(DMAC_4_IRQn);
    }
    DMAC->Channel[dma_channel].CHINTENCLR.reg =
        DMAC_CHINTENSET_TCMPL | DMAC_CHINTENSET_TERR | DMAC_CHINTENSET_SUSP;
    DMAC->Channel[dma_channel].CHCTRLA.reg = 0;
    while (wait && DMAC->Channel[dma_channel].CHCTRLA.bit.ENABLE) {
    }
    #endif
}

#endif
