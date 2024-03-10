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

#include <string.h>

#include "audio_dma.h"
#include "samd/clocks.h"
#include "samd/events.h"
#include "samd/dma.h"

#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/background_callback.h"

#include "py/mpstate.h"
#include "py/runtime.h"

#if CIRCUITPY_AUDIOIO || CIRCUITPY_AUDIOBUSIO

// Flag value for dma->buffer_to_load, indicating there is nothing to do.
// Otherwise dma->buffer_to_load is 0 or 1.
#define NO_BUFFER_TO_LOAD 0xff

static audio_dma_t *audio_dma_state[AUDIO_DMA_CHANNEL_COUNT];

// This cannot be in audio_dma_state because it's volatile.
static volatile bool audio_dma_pending[AUDIO_DMA_CHANNEL_COUNT];

static bool audio_dma_allocated[AUDIO_DMA_CHANNEL_COUNT];

uint8_t find_sync_event_channel_raise() {
    uint8_t event_channel = find_sync_event_channel();
    if (event_channel >= EVSYS_SYNCH_NUM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All sync event channels in use"));
    }
    return event_channel;
}

uint8_t dma_allocate_channel(void) {
    uint8_t channel;
    for (channel = 0; channel < AUDIO_DMA_CHANNEL_COUNT; channel++) {
        if (!audio_dma_allocated[channel]) {
            audio_dma_allocated[channel] = true;
            return channel;
        }
    }
    return channel; // i.e., return failure
}

void dma_free_channel(uint8_t channel) {
    assert(channel < AUDIO_DMA_CHANNEL_COUNT);
    assert(audio_dma_allocated[channel]);
    audio_dma_disable_channel(channel);
    audio_dma_allocated[channel] = false;
}

void audio_dma_disable_channel(uint8_t channel) {
    if (channel >= AUDIO_DMA_CHANNEL_COUNT) {
        return;
    }
    dma_disable_channel(channel);
}

void audio_dma_enable_channel(uint8_t channel) {
    if (channel >= AUDIO_DMA_CHANNEL_COUNT) {
        return;
    }
    dma_enable_channel(channel);
}

static void audio_dma_convert_samples(
    audio_dma_t *dma,
    uint8_t *input, uint32_t input_length,
    uint8_t *available_output_buffer, uint32_t available_output_buffer_length,
    uint8_t **output, uint32_t *output_length,
    uint8_t *output_spacing) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    if (dma->signed_to_unsigned || dma->unsigned_to_signed) {

        // Must convert.
        // Write the conversion into the passed-in output buffer
        *output = available_output_buffer;
        *output_length = input_length / dma->spacing;
        *output_spacing = 1;

        if (*output_length > available_output_buffer_length) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Internal audio buffer too small"));
        }

        uint32_t out_i = 0;
        if (dma->bytes_per_sample == 1) {
            for (uint32_t i = 0; i < input_length; i += dma->spacing) {
                if (dma->signed_to_unsigned) {
                    ((uint8_t *)*output)[out_i] = ((int8_t *)input)[i] + 0x80;
                } else {
                    ((int8_t *)*output)[out_i] = ((uint8_t *)input)[i] - 0x80;
                }
                out_i += 1;
            }
        } else if (dma->bytes_per_sample == 2) {
            for (uint32_t i = 0; i < input_length / 2; i += dma->spacing) {
                if (dma->signed_to_unsigned) {
                    ((uint16_t *)*output)[out_i] = ((int16_t *)input)[i] + 0x8000;
                } else {
                    ((int16_t *)*output)[out_i] = ((uint16_t *)input)[i] - 0x8000;
                }
                out_i += 1;
            }
        }
    } else {
        *output = input;
        *output_length = input_length;
        *output_spacing = dma->spacing;
    }
    #pragma GCC diagnostic pop
}

static void audio_dma_load_next_block(audio_dma_t *dma, size_t buffer_idx) {
    uint8_t *sample_buffer;
    uint32_t sample_buffer_length;
    audioio_get_buffer_result_t get_buffer_result =
        audiosample_get_buffer(dma->sample, dma->single_channel_output, dma->audio_channel,
            &sample_buffer, &sample_buffer_length);

    DmacDescriptor *descriptor = dma->descriptor[buffer_idx];

    if (get_buffer_result == GET_BUFFER_ERROR) {
        audio_dma_stop(dma);
        return;
    }

    // Use one of the allocated buffers for conversion. But if there's no conversion,
    // this will be set to buffer in audio_dma_convert_samples() to avoid any copying.
    uint8_t *output_buffer;
    uint32_t output_buffer_length;
    uint8_t output_spacing;

    audio_dma_convert_samples(dma, sample_buffer, sample_buffer_length,
        // Available output buffer: may be used or not.
        dma->buffer[buffer_idx], dma->buffer_length[buffer_idx],
        // Buffer where output was placed.
        &output_buffer, &output_buffer_length,
        &output_spacing);

    descriptor->BTCNT.reg = output_buffer_length / dma->beat_size / output_spacing;
    descriptor->SRCADDR.reg = ((uint32_t)output_buffer) + output_buffer_length;
    if (get_buffer_result == GET_BUFFER_DONE) {
        if (dma->loop) {
            audiosample_reset_buffer(dma->sample, dma->single_channel_output, dma->audio_channel);
        } else {
            if (output_buffer_length == 0) {
                // Nothing further to read and previous buffer is finished.
                audio_dma_stop(dma);
                return;
            } else {
                // Break descriptor chain.
                descriptor->DESCADDR.reg = 0;
            }
        }
    }
    descriptor->BTCTRL.bit.VALID = true;
}

static void setup_audio_descriptor(DmacDescriptor *descriptor, uint8_t beat_size,
    uint8_t spacing, uint32_t output_register_address) {
    uint32_t beat_size_reg = DMAC_BTCTRL_BEATSIZE_BYTE;
    if (beat_size == 2) {
        beat_size_reg = DMAC_BTCTRL_BEATSIZE_HWORD;
    } else if (beat_size == 4) {
        beat_size_reg = DMAC_BTCTRL_BEATSIZE_WORD;
    }
    descriptor->BTCTRL.reg = beat_size_reg |
        DMAC_BTCTRL_SRCINC |
        DMAC_BTCTRL_EVOSEL_BLOCK |
        DMAC_BTCTRL_STEPSIZE(spacing - 1) |
        DMAC_BTCTRL_STEPSEL_SRC;
    descriptor->DSTADDR.reg = output_register_address;
}

// Playback should be shutdown before calling this.
audio_dma_result audio_dma_setup_playback(audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint32_t output_register_address,
    uint8_t dma_trigger_source) {
    uint8_t dma_channel = dma_allocate_channel();
    if (dma_channel >= AUDIO_DMA_CHANNEL_COUNT) {
        return AUDIO_DMA_DMA_BUSY;
    }

    dma->sample = sample;
    dma->loop = loop;
    dma->single_channel_output = single_channel_output;
    dma->audio_channel = audio_channel;
    dma->dma_channel = dma_channel;
    dma->signed_to_unsigned = false;
    dma->unsigned_to_signed = false;
    dma->spacing = 1;
    audiosample_reset_buffer(sample, single_channel_output, audio_channel);
    dma->buffer_to_load = NO_BUFFER_TO_LOAD;
    dma->descriptor[0] = dma_descriptor(dma_channel);
    dma->descriptor[1] = &dma->second_descriptor;

    bool samples_signed;
    uint32_t max_buffer_length;
    audiosample_get_buffer_structure(sample, single_channel_output, &dma->single_buffer, &samples_signed,
        &max_buffer_length, &dma->spacing);
    uint8_t output_spacing = dma->spacing;
    if (output_signed != samples_signed) {
        output_spacing = 1;
        max_buffer_length /= dma->spacing;
    }


    dma->buffer[0] = (uint8_t *)m_realloc(dma->buffer[0], max_buffer_length);
    dma->buffer_length[0] = max_buffer_length;
    if (dma->buffer[0] == NULL) {
        return AUDIO_DMA_MEMORY_ERROR;
    }

    if (!dma->single_buffer) {
        dma->buffer[1] = (uint8_t *)m_realloc(dma->buffer[1], max_buffer_length);
        dma->buffer_length[1] = max_buffer_length;
        if (dma->buffer[1] == NULL) {
            return AUDIO_DMA_MEMORY_ERROR;
        }
    }

    dma->signed_to_unsigned = !output_signed && samples_signed;
    dma->unsigned_to_signed = output_signed && !samples_signed;

    dma->event_channel = 0xff;
    if (!dma->single_buffer) {
        // We're likely double buffering so set up the block interrupts.
        turn_on_event_system();
        dma->event_channel = find_sync_event_channel_raise();
        init_event_channel_interrupt(dma->event_channel, CORE_GCLK, EVSYS_ID_GEN_DMAC_CH_0 + dma_channel);

        // We keep the audio_dma_t for internal use and the sample as a root pointer because it
        // contains the audiodma structure.
        audio_dma_state[dma->dma_channel] = dma;
        MP_STATE_PORT(playing_audio)[dma->dma_channel] = dma->sample;
    }


    if (audiosample_bits_per_sample(sample) == 16) {
        dma->beat_size = 2;
        dma->bytes_per_sample = 2;
    } else {
        dma->beat_size = 1;
        dma->bytes_per_sample = 1;
        if (single_channel_output) {
            output_register_address += 1;
        }
    }
    // Transfer both channels at once.
    if (!single_channel_output && audiosample_channel_count(sample) == 2) {
        dma->beat_size *= 2;
    }

    #ifdef SAM_D5X_E5X
    int irq = dma->event_channel < 4 ? EVSYS_0_IRQn + dma->event_channel : EVSYS_4_IRQn;
    // Only disable and clear on SAMD51 because the SAMD21 shares EVSYS with ticks.
    NVIC_DisableIRQ(irq);
    NVIC_ClearPendingIRQ(irq);
    #else
    int irq = EVSYS_IRQn;
    #endif

    setup_audio_descriptor(dma->descriptor[0], dma->beat_size, output_spacing, output_register_address);
    if (dma->single_buffer) {
        dma->descriptor[0]->DESCADDR.reg = 0;
        if (dma->loop) {
            // The descriptor chains to itself.
            dma->descriptor[0]->DESCADDR.reg = (uint32_t)dma->descriptor[0];
        }
    } else {
        // Set up the two descriptors to chain to each other.
        dma->descriptor[0]->DESCADDR.reg = (uint32_t)dma->descriptor[1];
        setup_audio_descriptor(dma->descriptor[1], dma->beat_size, output_spacing, output_register_address);
        dma->descriptor[1]->DESCADDR.reg = (uint32_t)dma->descriptor[0];
    }

    // Load the first two blocks up front.
    audio_dma_load_next_block(dma, 0);
    if (!dma->single_buffer) {
        audio_dma_load_next_block(dma, 1);
    }

    dma->playing_in_progress = true;
    dma_configure(dma_channel, dma_trigger_source, true);
    audio_dma_enable_channel(dma_channel);

    NVIC_EnableIRQ(irq);

    return AUDIO_DMA_OK;
}

void audio_dma_stop(audio_dma_t *dma) {
    uint8_t channel = dma->dma_channel;
    if (channel < AUDIO_DMA_CHANNEL_COUNT) {
        audio_dma_disable_channel(channel);
        disable_event_channel(dma->event_channel);
        MP_STATE_PORT(playing_audio)[channel] = NULL;
        audio_dma_state[channel] = NULL;
        dma_free_channel(dma->dma_channel);
    }
    dma->dma_channel = AUDIO_DMA_CHANNEL_COUNT;
    dma->playing_in_progress = false;
}

void audio_dma_pause(audio_dma_t *dma) {
    dma_suspend_channel(dma->dma_channel);
}

void audio_dma_resume(audio_dma_t *dma) {
    dma_resume_channel(dma->dma_channel);
}

bool audio_dma_get_paused(audio_dma_t *dma) {
    if (dma->dma_channel >= AUDIO_DMA_CHANNEL_COUNT) {
        return false;
    }
    uint32_t status = dma_transfer_status(dma->dma_channel);

    return (status & DMAC_CHINTFLAG_SUSP) != 0;
}

void audio_dma_init(audio_dma_t *dma) {
    dma->dma_channel = AUDIO_DMA_CHANNEL_COUNT;
}

void audio_dma_reset(void) {
    for (uint8_t i = 0; i < AUDIO_DMA_CHANNEL_COUNT; i++) {
        audio_dma_state[i] = NULL;
        audio_dma_pending[i] = false;
        audio_dma_allocated[i] = false;
        audio_dma_disable_channel(i);
        dma_descriptor(i)->BTCTRL.bit.VALID = false;
        MP_STATE_PORT(playing_audio)[i] = NULL;
    }
}

bool audio_dma_get_playing(audio_dma_t *dma) {
    if (dma->dma_channel >= AUDIO_DMA_CHANNEL_COUNT) {
        return false;
    }
    return dma->playing_in_progress;
}

// WARN(tannewt): DO NOT print from here, or anything it calls. Printing calls
// background tasks such as this and causes a stack overflow.
STATIC void dma_callback_fun(void *arg) {
    audio_dma_t *dma = arg;
    if (dma == NULL) {
        return;
    }

    common_hal_mcu_disable_interrupts();
    uint8_t buffer_to_load = dma->buffer_to_load;
    dma->buffer_to_load = NO_BUFFER_TO_LOAD;
    common_hal_mcu_enable_interrupts();

    if (buffer_to_load == NO_BUFFER_TO_LOAD) {
        audio_dma_stop(dma);
    } else {
        audio_dma_load_next_block(dma, buffer_to_load);
    }
}

void audio_dma_evsys_handler(void) {
    for (uint8_t i = 0; i < AUDIO_DMA_CHANNEL_COUNT; i++) {
        audio_dma_t *dma = audio_dma_state[i];
        if (dma == NULL) {
            continue;
        }
        bool block_done = event_interrupt_active(dma->event_channel);
        if (!block_done) {
            continue;
        }

        // By the time we get here, the write-back descriptor has been set to the
        // current running descriptor. Fill the buffer that the next chained descriptor
        // will play.
        //
        // The state of the write-back descriptor was determined empirically,
        // The datasheet appears to imply that the descriptor that just finished would
        // be in the write-back descriptor. But the VALID bit is set in the write-back descriptor,
        // and reversing which buffer to fill produces crackly output. So the choice
        // of which buffer to fill here appears correct.
        DmacDescriptor *next_descriptor =
            (DmacDescriptor *)dma_write_back_descriptor(dma->dma_channel)->DESCADDR.reg;
        if (next_descriptor == dma->descriptor[0]) {
            dma->buffer_to_load = 0;
        } else if (next_descriptor == dma->descriptor[1]) {
            dma->buffer_to_load = 1;
        } else if (next_descriptor == NULL) {
            dma->buffer_to_load = NO_BUFFER_TO_LOAD;
        } else {
            continue;
        }

        background_callback_add(&dma->callback, dma_callback_fun, (void *)dma);
    }
}

MP_REGISTER_ROOT_POINTER(mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT]);

#endif
