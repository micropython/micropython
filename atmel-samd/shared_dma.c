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
#include "shared_dma.h"

#include "py/gc.h"
#include "py/mpstate.h"

#include "asf/sam0/drivers/events/events.h"
#include "asf/sam0/drivers/system/interrupt/system_interrupt.h"
#include "asf/sam0/drivers/tc/tc.h"

#undef ENABLE

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

static void dma_configure(uint8_t channel, uint8_t trigsrc, bool output_event) {
    system_interrupt_enter_critical_section();
    /** Select the DMA channel and clear software trigger */
    DMAC->CHID.reg = DMAC_CHID_ID(channel);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel));
    uint32_t event_output_enable = 0;
    if (output_event) {
        event_output_enable = DMAC_CHCTRLB_EVOE;
    }
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(DMA_PRIORITY_LEVEL_0) |
            DMAC_CHCTRLB_TRIGSRC(trigsrc) |
            DMAC_CHCTRLB_TRIGACT(DMA_TRIGGER_ACTION_BEAT) |
            event_output_enable;
    system_interrupt_leave_critical_section();
}

enum status_code shared_dma_write(Sercom* sercom, const uint8_t* buffer, uint32_t length) {
    if (general_dma_tx.job_status != STATUS_OK) {
        return general_dma_tx.job_status;
    }
    dma_configure(general_dma_tx.channel_id, sercom_index(sercom) * 2 + 2, false);

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
    sercom->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_ERROR;

    return general_dma_tx.job_status;
}

enum status_code shared_dma_read(Sercom* sercom, uint8_t* buffer, uint32_t length, uint8_t tx) {
    if (general_dma_tx.job_status != STATUS_OK) {
        return general_dma_tx.job_status;
    }

    dma_configure(general_dma_tx.channel_id, sercom_index(sercom) * 2 + 2, false);
    dma_configure(general_dma_rx.channel_id, sercom_index(sercom) * 2 + 1, false);

    // Set up RX first.
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
    descriptor_config.src_increment_enable = false;
    descriptor_config.dst_increment_enable = true;
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

bool allocate_block_counter() {
    // Find a timer to count DMA block completions.
    Tc *t = NULL;
    Tc *tcs[TC_INST_NUM] = TC_INSTS;
    for (uint8_t i = TC_INST_NUM; i > 0; i--) {
        if (tcs[i - 1]->COUNT16.CTRLA.bit.ENABLE == 0) {
            t = tcs[i - 1];
            break;
        }
    }
    if (t == NULL) {
        return false;
    }
    MP_STATE_VM(audiodma_block_counter) = gc_alloc(sizeof(struct tc_module), false);
    if (MP_STATE_VM(audiodma_block_counter) == NULL) {
        return false;
    }

    // Don't bother setting the period. We set it before you playback anything.
    struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);
    config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
    config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    if (tc_init(MP_STATE_VM(audiodma_block_counter), t, &config_tc) != STATUS_OK) {
        return false;
    };

    struct tc_events events_tc;
    events_tc.generate_event_on_overflow = false;
    events_tc.on_event_perform_action = true;
    events_tc.event_action = TC_EVENT_ACTION_INCREMENT_COUNTER;
    tc_enable_events(MP_STATE_VM(audiodma_block_counter), &events_tc);

    // Connect the timer overflow event, which happens at the target frequency,
    // to the DAC conversion trigger.
    MP_STATE_VM(audiodma_block_event) = gc_alloc(sizeof(struct events_resource), false);
    if (MP_STATE_VM(audiodma_block_event) == NULL) {
        return false;
    }
    struct events_config config;
    events_get_config_defaults(&config);

    uint8_t user = EVSYS_ID_USER_TC3_EVU;
    if (t == TC4) {
        user = EVSYS_ID_USER_TC4_EVU;
    } else if (t == TC5) {
        user = EVSYS_ID_USER_TC5_EVU;
#ifdef TC6
    } else if (t == TC6) {
        user = EVSYS_ID_USER_TC6_EVU;
#endif
#ifdef TC7
    } else if (t == TC7) {
        user = EVSYS_ID_USER_TC7_EVU;
#endif
    }

    config.generator    = EVSYS_ID_GEN_DMAC_CH_0;
    config.path         = EVENTS_PATH_ASYNCHRONOUS;
    if (events_allocate(MP_STATE_VM(audiodma_block_event), &config) != STATUS_OK ||
        events_attach_user(MP_STATE_VM(audiodma_block_event), user) != STATUS_OK) {
        return false;
    }

    tc_enable(MP_STATE_VM(audiodma_block_counter));
    tc_stop_counter(MP_STATE_VM(audiodma_block_counter));
    return true;
}

void switch_audiodma_trigger(uint8_t trigger_dmac_id) {
    dma_configure(audio_dma.channel_id, trigger_dmac_id, true);
}
