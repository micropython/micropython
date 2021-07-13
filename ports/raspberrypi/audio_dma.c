/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "audio_dma.h"

#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "supervisor/background_callback.h"

#include "py/mpstate.h"
#include "py/runtime.h"

#include "src/rp2_common/hardware_irq/include/hardware/irq.h"

#if CIRCUITPY_AUDIOPWMIO || CIRCUITPY_AUDIOBUSIO

#define AUDIO_DMA_CHANNEL_COUNT NUM_DMA_CHANNELS

void audio_dma_reset(void) {
    for (size_t channel = 0; channel < AUDIO_DMA_CHANNEL_COUNT; channel++) {
        if (MP_STATE_PORT(playing_audio)[channel] == NULL) {
            continue;
        }

        audio_dma_stop(MP_STATE_PORT(playing_audio)[channel]);
    }
}

void audio_dma_convert_signed(audio_dma_t *dma, uint8_t *buffer, uint32_t buffer_length,
    uint8_t **output_buffer, uint32_t *output_buffer_length) {
    if (dma->first_buffer_free) {
        *output_buffer = dma->first_buffer;
    } else {
        *output_buffer = dma->second_buffer;
    }
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    if (dma->signed_to_unsigned ||
        dma->unsigned_to_signed ||
        dma->sample_spacing > 1 ||
        (dma->sample_resolution != dma->output_resolution)) {
        *output_buffer_length = buffer_length / dma->sample_spacing;
        uint32_t out_i = 0;
        if (dma->sample_resolution <= 8 && dma->output_resolution > 8) {
            size_t shift = dma->output_resolution - dma->sample_resolution;

            for (uint32_t i = 0; i < buffer_length; i += dma->sample_spacing) {
                if (dma->signed_to_unsigned) {
                    ((uint16_t *)*output_buffer)[out_i] = ((uint16_t)((int8_t *)buffer)[i] + 0x80) << shift;
                } else if (dma->unsigned_to_signed) {
                    ((int16_t *)*output_buffer)[out_i] = ((int16_t)((uint8_t *)buffer)[i] - 0x80) << shift;
                } else {
                    ((uint16_t *)*output_buffer)[out_i] = ((uint16_t)((uint8_t *)buffer)[i]) << shift;
                }
                out_i += 1;
            }
        } else if (dma->sample_resolution <= 8 && dma->output_resolution <= 8) {
            for (uint32_t i = 0; i < buffer_length; i += dma->sample_spacing) {
                if (dma->signed_to_unsigned) {
                    ((uint8_t *)*output_buffer)[out_i] = ((int8_t *)buffer)[i] + 0x80;
                } else if (dma->unsigned_to_signed) {
                    ((int8_t *)*output_buffer)[out_i] = ((uint8_t *)buffer)[i] - 0x80;
                } else {
                    ((uint8_t *)*output_buffer)[out_i] = ((uint8_t *)buffer)[i];
                }
                out_i += 1;
            }
        } else if (dma->sample_resolution > 8 && dma->output_resolution > 8) {
            size_t shift = 16 - dma->output_resolution;
            for (uint32_t i = 0; i < buffer_length / 2; i += dma->sample_spacing) {
                if (dma->signed_to_unsigned) {
                    ((uint16_t *)*output_buffer)[out_i] = ((int16_t *)buffer)[i] + 0x8000;
                } else if (dma->unsigned_to_signed) {
                    ((int16_t *)*output_buffer)[out_i] = ((uint16_t *)buffer)[i] - 0x8000;
                } else {
                    ((uint16_t *)*output_buffer)[out_i] = ((uint16_t *)buffer)[i];
                }
                if (dma->output_resolution < 16) {
                    if (dma->output_signed) {
                        ((int16_t *)*output_buffer)[out_i] = ((int16_t *)*output_buffer)[out_i] >> shift;
                    } else {
                        ((uint16_t *)*output_buffer)[out_i] = ((uint16_t *)*output_buffer)[out_i] >> shift;
                    }
                }
                out_i += 1;
            }
        }
    } else {
        *output_buffer = buffer;
        *output_buffer_length = buffer_length;
    }
    #pragma GCC diagnostic pop
    dma->first_buffer_free = !dma->first_buffer_free;
}

void audio_dma_load_next_block(audio_dma_t *dma) {
    uint8_t dma_channel = dma->channel[1];
    if (dma->first_channel_free) {
        dma_channel = dma->channel[0];
    }
    dma->first_channel_free = !dma->first_channel_free;

    uint8_t *output_buffer;
    uint32_t output_buffer_length;
    audioio_get_buffer_result_t get_buffer_result;
    uint8_t *buffer;
    uint32_t buffer_length;
    get_buffer_result = audiosample_get_buffer(dma->sample,
        dma->single_channel_output, dma->audio_channel, &buffer, &buffer_length);

    if (get_buffer_result == GET_BUFFER_ERROR) {
        audio_dma_stop(dma);
        return;
    }

    audio_dma_convert_signed(dma, buffer, buffer_length, &output_buffer, &output_buffer_length);

    // If we don't have an output buffer, save the pointer to first_buffer for use in the single
    // buffer special case.
    if (dma->first_buffer == NULL) {
        dma->first_buffer = output_buffer;
    }

    dma_channel_set_trans_count(dma_channel, output_buffer_length / dma->output_size, false /* trigger */);
    dma_channel_set_read_addr(dma_channel, output_buffer, false /* trigger */);
    if (get_buffer_result == GET_BUFFER_DONE) {
        if (dma->loop) {
            audiosample_reset_buffer(dma->sample, dma->single_channel_output, dma->audio_channel);
        } else {
            // Set channel trigger to ourselves so we don't keep going.
            dma_channel_hw_t *c = &dma_hw->ch[dma_channel];
            c->al1_ctrl = (c->al1_ctrl & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (dma_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
        }
    }
}

// Playback should be shutdown before calling this.
audio_dma_result audio_dma_setup_playback(audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint8_t output_resolution,
    uint32_t output_register_address,
    uint8_t dma_trigger_source) {
    // Use two DMA channels to play because the DMA can't wrap to itself without the
    // buffer being power of two aligned.
    int dma_channel_0_maybe = dma_claim_unused_channel(false);
    if (dma_channel_0_maybe < 0) {
        return AUDIO_DMA_DMA_BUSY;
    }

    int dma_channel_1_maybe = dma_claim_unused_channel(false);
    if (dma_channel_1_maybe < 0) {
        dma_channel_unclaim((uint)dma_channel_0_maybe);
        return AUDIO_DMA_DMA_BUSY;
    }

    dma->channel[0] = (uint8_t)dma_channel_0_maybe;
    dma->channel[1] = (uint8_t)dma_channel_1_maybe;

    dma->sample = sample;
    dma->loop = loop;
    dma->single_channel_output = single_channel_output;
    dma->audio_channel = audio_channel;
    dma->signed_to_unsigned = false;
    dma->unsigned_to_signed = false;
    dma->output_signed = output_signed;
    dma->sample_spacing = 1;
    dma->first_channel_free = true;
    dma->output_resolution = output_resolution;
    dma->sample_resolution = audiosample_bits_per_sample(sample);
    audiosample_reset_buffer(sample, single_channel_output, audio_channel);

    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    audiosample_get_buffer_structure(sample, single_channel_output, &single_buffer, &samples_signed,
        &max_buffer_length, &dma->sample_spacing);

    // Check to see if we have to scale the resolution up.
    if (dma->sample_resolution <= 8 && dma->output_resolution > 8) {
        max_buffer_length *= 2;
    }
    if (output_signed != samples_signed ||
        dma->sample_spacing > 1 ||
        (dma->sample_resolution != dma->output_resolution)) {
        max_buffer_length /= dma->sample_spacing;
        dma->first_buffer = (uint8_t *)m_realloc(dma->first_buffer, max_buffer_length);
        if (dma->first_buffer == NULL) {
            return AUDIO_DMA_MEMORY_ERROR;
        }

        dma->first_buffer_free = true;
        if (!single_buffer) {
            dma->second_buffer = (uint8_t *)m_realloc(dma->second_buffer, max_buffer_length);
            if (dma->second_buffer == NULL) {
                return AUDIO_DMA_MEMORY_ERROR;
            }
        }
        dma->signed_to_unsigned = !output_signed && samples_signed;
        dma->unsigned_to_signed = output_signed && !samples_signed;
    }

    if (output_resolution > 8) {
        dma->output_size = 2;
    } else {
        dma->output_size = 1;
    }
    // Transfer both channels at once.
    if (!single_channel_output && audiosample_channel_count(sample) == 2) {
        dma->output_size *= 2;
    }
    enum dma_channel_transfer_size dma_size = DMA_SIZE_8;
    if (dma->output_size == 2) {
        dma_size = DMA_SIZE_16;
    } else if (dma->output_size == 4) {
        dma_size = DMA_SIZE_32;
    }

    for (size_t i = 0; i < 2; i++) {
        dma_channel_config c = dma_channel_get_default_config(dma->channel[i]);
        channel_config_set_transfer_data_size(&c, dma_size);
        channel_config_set_dreq(&c, dma_trigger_source);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        // Chain to the other channel by default.
        channel_config_set_chain_to(&c, dma->channel[(i + 1) % 2]);
        dma_channel_set_config(dma->channel[i], &c, false /* trigger */);
        dma_channel_set_write_addr(dma->channel[i], (void *)output_register_address, false /* trigger */);
    }

    // We keep the audio_dma_t for internal use and the sample as a root pointer because it
    // contains the audiodma structure.
    MP_STATE_PORT(playing_audio)[dma->channel[0]] = dma;
    MP_STATE_PORT(playing_audio)[dma->channel[1]] = dma;

    // Load the first two blocks up front.
    audio_dma_load_next_block(dma);
    if (!single_buffer) {
        audio_dma_load_next_block(dma);
    }

    // Special case the DMA for a single buffer. It's commonly used for a single wave length of sound
    // and may be short. Therefore, we use DMA chaining to loop quickly without involving interrupts.
    // On the RP2040 we chain by having a second DMA writing to the config registers of the first.
    // Read and write addresses change with DMA so we need to reset the read address back to the
    // start of the sample.
    if (single_buffer) {
        dma_channel_config c = dma_channel_get_default_config(dma->channel[1]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_dreq(&c, 0x3f); // dma as fast as possible
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, false);
        channel_config_set_chain_to(&c, dma->channel[1]); // Chain to ourselves so we stop.
        dma_channel_configure(dma->channel[1], &c,
            &dma_hw->ch[dma->channel[0]].al3_read_addr_trig, // write address
            &dma->first_buffer, // read address
            1, // transaction count
            false); // trigger
    } else {
        // Enable our DMA channels on DMA0 to the CPU. This will wake us up when
        // we're WFI.
        dma_hw->inte0 |= (1 << dma->channel[0]) | (1 << dma->channel[1]);
        irq_set_mask_enabled(1 << DMA_IRQ_0, true);
    }

    dma_channel_start(dma->channel[0]);

    return AUDIO_DMA_OK;
}

void audio_dma_stop(audio_dma_t *dma) {
    // Disable our interrupts.
    dma_hw->inte0 &= ~((1 << dma->channel[0]) | (1 << dma->channel[1]));
    irq_set_mask_enabled(1 << DMA_IRQ_0, false);

    // Run any remaining audio tasks because we remove ourselves from
    // playing_audio.
    RUN_BACKGROUND_TASKS;

    for (size_t i = 0; i < 2; i++) {
        size_t channel = dma->channel[i];

        dma_channel_config c = dma_channel_get_default_config(dma->channel[i]);
        channel_config_set_enable(&c, false);
        dma_channel_set_config(channel, &c, false /* trigger */);

        if (dma_channel_is_busy(channel)) {
            dma_channel_abort(channel);
        }
        dma_channel_set_read_addr(channel, NULL, false /* trigger */);
        dma_channel_set_write_addr(channel, NULL, false /* trigger */);
        dma_channel_set_trans_count(channel, 0, false /* trigger */);
        dma_channel_unclaim(channel);
        MP_STATE_PORT(playing_audio)[channel] = NULL;
        dma->channel[i] = NUM_DMA_CHANNELS;
    }

    // Hold onto our buffers.
}

// To pause we simply stop the DMA. It is the responsibility of the output peripheral
// to hold the previous value.
void audio_dma_pause(audio_dma_t *dma) {
    dma_hw->ch[dma->channel[0]].al1_ctrl &= ~DMA_CH0_CTRL_TRIG_EN_BITS;
    dma_hw->ch[dma->channel[1]].al1_ctrl &= ~DMA_CH1_CTRL_TRIG_EN_BITS;
}

void audio_dma_resume(audio_dma_t *dma) {
    // Always re-enable the non-busy channel first so it's ready to continue when the busy channel
    // finishes and chains to it. (An interrupt could make the time between enables long.)
    if (dma_channel_is_busy(dma->channel[0])) {
        dma_hw->ch[dma->channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
    } else {
        dma_hw->ch[dma->channel[0]].al1_ctrl |= DMA_CH0_CTRL_TRIG_EN_BITS;
        dma_hw->ch[dma->channel[1]].al1_ctrl |= DMA_CH1_CTRL_TRIG_EN_BITS;
    }
}

bool audio_dma_get_paused(audio_dma_t *dma) {
    if (dma->channel[0] >= AUDIO_DMA_CHANNEL_COUNT) {
        return false;
    }
    uint32_t control = dma_hw->ch[dma->channel[0]].ctrl_trig;

    return (control & DMA_CH0_CTRL_TRIG_EN_BITS) == 0;
}

void audio_dma_init(audio_dma_t *dma) {
    dma->first_buffer = NULL;
    dma->second_buffer = NULL;
    dma->channel[0] = NUM_DMA_CHANNELS;
    dma->channel[1] = NUM_DMA_CHANNELS;
}

void audio_dma_deinit(audio_dma_t *dma) {
    m_free(dma->first_buffer);
    dma->first_buffer = NULL;

    m_free(dma->second_buffer);
    dma->second_buffer = NULL;
}

bool audio_dma_get_playing(audio_dma_t *dma) {
    if (dma->channel[0] == NUM_DMA_CHANNELS) {
        return false;
    }
    if (!dma_channel_is_busy(dma->channel[0]) &&
        !dma_channel_is_busy(dma->channel[1])) {
        return false;
    }

    return true;
}

// WARN(tannewt): DO NOT print from here, or anything it calls. Printing calls
// background tasks such as this and causes a stack overflow.
// NOTE(dhalbert): I successfully printed from here while debugging.
// So it's possible, but be careful.
STATIC void dma_callback_fun(void *arg) {
    audio_dma_t *dma = arg;
    if (dma == NULL) {
        return;
    }

    audio_dma_load_next_block(dma);
}

void isr_dma_0(void) {
    for (size_t i = 0; i < NUM_DMA_CHANNELS; i++) {
        uint32_t mask = 1 << i;
        if ((dma_hw->intr & mask) != 0 && MP_STATE_PORT(playing_audio)[i] != NULL) {
            audio_dma_t *dma = MP_STATE_PORT(playing_audio)[i];
            background_callback_add(&dma->callback, dma_callback_fun, (void *)dma);
            dma_hw->ints0 = mask;
        }
    }
}

#endif
