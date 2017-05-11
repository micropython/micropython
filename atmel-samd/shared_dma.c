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

#include "asf/sam0/drivers/system/interrupt/system_interrupt.h"

// We allocate two DMA resources for the entire lifecycle of the board (not the
// vm) because the general_dma resource will be shared between the REPL and SPI
// flash. Both uses must block each other in order to prevent conflict.
struct dma_resource audio_dma;
struct dma_resource general_dma_tx;
struct dma_resource general_dma_rx;

void init_shared_dma(void) {
    struct dma_resource_config config;
    dma_get_config_defaults(&config);

    // This allocates the lowest channel first so make sure the audio is first
    // so it gets the highest priority.
    config.peripheral_trigger = DAC_DMAC_ID_EMPTY;
    config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
    config.event_config.input_action = DMA_EVENT_INPUT_TRIG;
    config.event_config.event_output_enable = true;
    dma_allocate(&audio_dma, &config);
    // Turn on the transfer complete interrupt so that the job_status changes to done.
    g_chan_interrupt_flag[audio_dma.channel_id] |= (1UL << DMA_CALLBACK_TRANSFER_DONE);

    // Prioritize the RX channel over the TX channel because TX can cause an RX
    // overflow.
    dma_get_config_defaults(&config);
    config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
    config.event_config.input_action = DMA_EVENT_INPUT_TRIG;
    dma_allocate(&general_dma_rx, &config);
    g_chan_interrupt_flag[general_dma_rx.channel_id] |= (1UL << DMA_CALLBACK_TRANSFER_DONE);

    dma_get_config_defaults(&config);
    config.trigger_action = DMA_TRIGGER_ACTION_BEAT;
    config.event_config.input_action = DMA_EVENT_INPUT_TRIG;
    dma_allocate(&general_dma_tx, &config);
    g_chan_interrupt_flag[general_dma_tx.channel_id] |= (1UL << DMA_CALLBACK_TRANSFER_DONE);

    // Be sneaky and reuse the active descriptor memory.
    audio_dma.descriptor = &descriptor_section[audio_dma.channel_id];
    general_dma_rx.descriptor = &descriptor_section[general_dma_rx.channel_id];
    general_dma_tx.descriptor = &descriptor_section[general_dma_tx.channel_id];
}

static uint8_t sercom_index(Sercom* sercom) {
    return ((uint32_t) sercom - (uint32_t) SERCOM0) / 0x400;
}

static void dma_configure(uint8_t channel, uint8_t trigsrc) {
    system_interrupt_enter_critical_section();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel));
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(DMA_PRIORITY_LEVEL_0) | \
            DMAC_CHCTRLB_TRIGSRC(trigsrc) | \
            DMAC_CHCTRLB_TRIGACT(DMA_TRIGGER_ACTION_BEAT);
    system_interrupt_leave_critical_section();
}

enum status_code shared_dma_write(Sercom* sercom, const uint8_t* buffer, uint32_t length) {
    if (general_dma_tx.job_status != STATUS_OK) {
        return general_dma_tx.job_status;
    }
    dma_configure(general_dma_tx.channel_id, sercom_index(sercom) * 2 + 2);

    // Set up TX second.
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
    descriptor_config.dst_increment_enable = false;
    descriptor_config.block_transfer_count = length;
    descriptor_config.source_address = ((uint32_t)buffer + length);
    // DATA register is consistently addressed across all SERCOM modes.
    descriptor_config.destination_address = ((uint32_t)&sercom->SPI.DATA.reg);

    dma_descriptor_create(general_dma_tx.descriptor, &descriptor_config);
    enum status_code status = dma_start_transfer_job(&general_dma_tx);
    if (status != STATUS_OK) {
        return status;
    }

    // Wait for the dma transfer to finish.
    while (general_dma_tx.job_status == STATUS_BUSY) {}

    // Wait for the SPI transfer to complete.
    while (sercom->SPI.INTFLAG.bit.TXC == 0) {}

    // This transmit will cause the RX buffer overflow but we're OK with that.
    // So, read the garbage and clear the overflow flag.
    while (sercom->SPI.INTFLAG.bit.RXC == 1) {
        sercom->SPI.DATA.reg;
    }
    sercom->SPI.STATUS.bit.BUFOVF = 1;

    return general_dma_tx.job_status;
}

enum status_code shared_dma_read(Sercom* sercom, uint8_t* buffer, uint32_t length, uint8_t tx) {
    if (general_dma_tx.job_status != STATUS_OK) {
        return general_dma_tx.job_status;
    }

    dma_configure(general_dma_tx.channel_id, sercom_index(sercom) * 2 + 2);
    dma_configure(general_dma_rx.channel_id, sercom_index(sercom) * 2 + 1);

    // Set up RX first.
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
    descriptor_config.src_increment_enable = false;
    descriptor_config.block_transfer_count = length;
    // DATA register is consistently addressed across all SERCOM modes.
    descriptor_config.source_address = ((uint32_t)&sercom->SPI.DATA.reg);
    descriptor_config.destination_address = ((uint32_t)buffer + length);

    dma_descriptor_create(general_dma_rx.descriptor, &descriptor_config);

    // Set up TX to retransmit the same byte over and over.
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
    descriptor_config.src_increment_enable = false;
    descriptor_config.dst_increment_enable = false;
    descriptor_config.block_transfer_count = length;
    descriptor_config.source_address = ((uint32_t)&tx);
    // DATA register is consistently addressed across all SERCOM modes.
    descriptor_config.destination_address = ((uint32_t)&sercom->SPI.DATA.reg);

    dma_descriptor_create(general_dma_tx.descriptor, &descriptor_config);

    // Start the RX job first so we don't miss the first byte. The TX job clocks
    // the output.
    general_dma_rx.transfered_size = 0;
    dma_start_transfer_job(&general_dma_rx);
    general_dma_tx.transfered_size = 0;
    dma_start_transfer_job(&general_dma_tx);

    // Wait for the transfer to finish.
    while (general_dma_rx.job_status == STATUS_BUSY) {}

    while (sercom->SPI.INTFLAG.bit.RXC == 1) {}
    return general_dma_rx.job_status;
}
