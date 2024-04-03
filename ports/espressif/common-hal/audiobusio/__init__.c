/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "py/runtime.h"

#include "common-hal/audiobusio/__init__.h"
#include "bindings/espidf/__init__.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "shared-module/audiocore/__init__.h"

#define CIRCUITPY_BUFFER_COUNT 3
#define CIRCUITPY_BUFFER_SIZE 1023
#define CIRCUITPY_OUTPUT_SLOTS 2

static void i2s_fill_buffer(i2s_t *self) {
    if (self->next_buffer_size == 0) {
        // Error, no new buffer queued.
        return;
    }
    int16_t *output_buffer = (int16_t *)self->next_buffer;
    size_t output_buffer_size = self->next_buffer_size;
    const size_t bytes_per_output_frame = 4;
    size_t bytes_per_input_frame = self->channel_count * self->bytes_per_sample;
    if (!self->playing || self->paused || !self->sample || self->stopping) {
        memset(output_buffer, 0, self->next_buffer_size);
        self->next_buffer = NULL;
        self->next_buffer_size = 0;
        return;
    }
    while (!self->stopping && output_buffer_size > 0) {
        if (self->sample_data == self->sample_end) {
            uint32_t sample_buffer_length;
            audioio_get_buffer_result_t get_buffer_result =
                audiosample_get_buffer(self->sample, false, 0,
                    &self->sample_data, &sample_buffer_length);
            self->sample_end = self->sample_data + sample_buffer_length;
            if (get_buffer_result == GET_BUFFER_DONE) {
                if (self->loop) {
                    audiosample_reset_buffer(self->sample, false, 0);
                } else {
                    self->stopping = true;
                    break;
                }
            }
            if (get_buffer_result == GET_BUFFER_ERROR || sample_buffer_length == 0) {
                self->stopping = true;
                break;
            }
        }
        size_t sample_bytecount = self->sample_end - self->sample_data;
        // The framecount is the minimum of space left in the output buffer or left in the incoming sample.
        size_t framecount = MIN(output_buffer_size / bytes_per_output_frame, sample_bytecount / bytes_per_input_frame);
        if (self->samples_signed && self->channel_count == 2) {
            if (self->bytes_per_sample == 2) {
                memcpy(output_buffer, self->sample_data, framecount * bytes_per_output_frame);
            } else {
                audiosample_convert_s8s_s16s(output_buffer, ((int8_t *)self->sample_data), framecount);
            }
        } else {
            if (self->samples_signed) {
                assert(self->channel_count == 1);
                if (self->bytes_per_sample == 1) {
                    audiosample_convert_s8m_s16s(output_buffer, (int8_t *)(void *)self->sample_data, framecount);
                } else {
                    audiosample_convert_s16m_s16s(output_buffer, (int16_t *)(void *)self->sample_data, framecount);
                }
            } else {
                if (self->channel_count == 1) {
                    if (self->bytes_per_sample == 1) {
                        audiosample_convert_u8m_s16s(output_buffer, (uint8_t *)(void *)self->sample_data, framecount);
                    } else {
                        audiosample_convert_u16m_s16s(output_buffer, (uint16_t *)(void *)self->sample_data, framecount);
                    }
                } else {
                    if (self->bytes_per_sample == 1) {
                        audiosample_convert_u8s_s16s(output_buffer, (uint8_t *)(void *)self->sample_data, framecount);
                    } else {
                        audiosample_convert_u16s_s16s(output_buffer, (uint16_t *)(void *)self->sample_data, framecount);
                    }
                }
            }
        }
        self->sample_data += framecount * bytes_per_input_frame;
        output_buffer += framecount * CIRCUITPY_OUTPUT_SLOTS;
        output_buffer_size -= framecount * bytes_per_output_frame;
    }
    self->next_buffer = NULL;
    self->next_buffer_size = 0;
}

static void i2s_callback_fun(void *self_in) {
    i2s_t *self = self_in;
    i2s_fill_buffer(self);
}

static bool i2s_event_interrupt(i2s_chan_handle_t handle, i2s_event_data_t *event, void *self_in) {
    i2s_t *self = self_in;
    self->underrun = self->underrun || self->next_buffer != NULL;
    self->next_buffer = *(int16_t **)event->data;
    self->next_buffer_size = event->size;
    background_callback_add(&self->callback, i2s_callback_fun, self_in);
    return false;
}

void port_i2s_allocate_init(i2s_t *self, bool left_justified) {
    i2s_chan_config_t chan_config = {
        .id = I2S_NUM_AUTO,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = CIRCUITPY_BUFFER_COUNT,
        .dma_frame_num = CIRCUITPY_BUFFER_SIZE, // in _frames_, so 1023 is 4092 bytes per dma buf which is the maximum
    };
    esp_err_t err = i2s_new_channel(&chan_config, &self->handle, NULL);
    if (err == ESP_ERR_NOT_FOUND) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Peripheral in use"));
    }
    self->playing = false;
    self->paused = false;
    self->stopping = false;

    i2s_event_callbacks_t callbacks = {
        .on_recv = NULL,
        .on_recv_q_ovf = NULL,
        .on_sent = i2s_event_interrupt,
        .on_send_q_ovf = NULL,
    };
    i2s_channel_register_event_callback(self->handle, &callbacks, self);
}

void port_i2s_deinit(i2s_t *self) {
    port_i2s_stop(self);
    i2s_del_channel(self->handle);
    self->handle = NULL;
}

void port_i2s_play(i2s_t *self, mp_obj_t sample, bool loop) {
    // Pause to disable the I2S channel so we can adjust the clock.
    port_i2s_pause(self);
    self->sample = sample;
    self->loop = loop;
    self->bytes_per_sample = audiosample_bits_per_sample(sample) / 8;
    self->channel_count = audiosample_channel_count(sample);
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed,
        &max_buffer_length, &spacing);
    self->samples_signed = samples_signed;
    self->sample_data = self->sample_end = NULL;
    // We always output stereo so output twice as many bits.
    // uint16_t bits_per_sample_output = bits_per_sample * 2;

    audiosample_reset_buffer(self->sample, false, 0);

    uint32_t sample_rate = audiosample_sample_rate(sample);
    i2s_std_clk_config_t clk_config = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate);
    CHECK_ESP_RESULT(i2s_channel_reconfig_std_clock(self->handle, &clk_config));

    // preload the data
    self->playing = true;
    self->paused = false;
    self->stopping = false;
    // This will be slow but we can't rewind the underlying sample. So, we will
    // preload one frame at a time and drop the last sample that can't fit.
    // We cap ourselves at the max DMA set to prevent a sample drop if starting
    // fresh.
    uint32_t starting_frame;
    size_t bytes_loaded = 4;
    size_t preloaded = 0;
    while (bytes_loaded > 0 && preloaded < CIRCUITPY_BUFFER_SIZE * CIRCUITPY_BUFFER_COUNT) {
        self->next_buffer = &starting_frame;
        self->next_buffer_size = sizeof(starting_frame);
        i2s_fill_buffer(self);
        i2s_channel_preload_data(self->handle, &starting_frame, sizeof(uint32_t), &bytes_loaded);
        preloaded += 1;
    }

    // enable the channel
    i2s_channel_enable(self->handle);

    // The IDF will call us back when there is a free DMA buffer.
}

bool port_i2s_playing(i2s_t *self) {
    // TODO: Reason about stopping. This check leads to cases where the DMA is
    // "playing" but the common-hal thinks it isn't and skips pausing. Probably
    // best to move this functionality into I2SOut directly.
    return self->playing && !self->stopping;
}

bool port_i2s_paused(i2s_t *self) {
    return self->paused;
}

void port_i2s_stop(i2s_t *self) {
    port_i2s_pause(self);
    self->sample = NULL;
    self->playing = false;
    self->stopping = false;
}

void port_i2s_pause(i2s_t *self) {
    if (self->playing && !self->paused) {
        self->paused = true;
        CHECK_ESP_RESULT(i2s_channel_disable(self->handle));
    }
}

void port_i2s_resume(i2s_t *self) {
    if (self->playing && self->paused) {
        self->paused = false;
        CHECK_ESP_RESULT(i2s_channel_enable(self->handle));
    }
}
