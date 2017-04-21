/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared_dma.h"

// We allocate two DMA resources for the entire lifecycle of the board (not the
// vm) because the general_dma resource will be shared between the REPL and SPI
// flash. Both uses must block each other in order to prevent conflict.
struct dma_resource audio_dma;
struct dma_resource general_dma;

void init_shared_dma(void) {
    struct dma_resource_config config;
    dma_get_config_defaults(&config);

    // This allocates the lowest channel first so make sure the audio is first
    // so it gets the highest priority.
    config.peripheral_trigger = DAC_DMAC_ID_EMPTY;
    config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
    config.event_config.input_action = DMA_EVENT_INPUT_TRIG;
    dma_allocate(&audio_dma, &config);
    // Turn on the transfer complete interrupt so that the job_status changes to done.
    g_chan_interrupt_flag[audio_dma.channel_id] |= (1UL << DMA_CALLBACK_TRANSFER_DONE);

    dma_get_config_defaults(&config);
    dma_allocate(&general_dma, &config);

    // Be sneaky and reuse the active descriptor memory.
    audio_dma.descriptor = &descriptor_section[audio_dma.channel_id];
    general_dma.descriptor = &descriptor_section[general_dma.channel_id];
}
