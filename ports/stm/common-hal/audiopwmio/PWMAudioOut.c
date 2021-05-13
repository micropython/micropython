/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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
#include "common-hal/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"

#include "timers.h"

// TODO: support multiple concurrently active outputs.
STATIC TIM_HandleTypeDef tim_handle;
STATIC audiopwmio_pwmaudioout_obj_t *active_audio = NULL;

STATIC void set_pin(uint8_t channel, GPIO_PinState state) {
    HAL_GPIO_WritePin(pin_port(active_audio->pin[channel]->port),
        pin_mask(active_audio->pin[channel]->number), state);
}

STATIC void toggle_pin(uint8_t channel) {
    HAL_GPIO_TogglePin(pin_port(active_audio->pin[channel]->port),
        pin_mask(active_audio->pin[channel]->number));
}

STATIC void set_drive_mode(const mcu_pin_obj_t *pin, uint32_t mode) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);
}

STATIC void start_timer(audiopwmio_pwmaudioout_obj_t *self) {
    if (self->buffer_ptr[0] >= self->buffer_length[0]) { // no more pulses
        return;
    }

    self->period = self->buffer[0][self->buffer_ptr[0]];
    if (self->pin[1] && self->period > self->buffer[1][self->buffer_ptr[1]]) {
        self->period = self->buffer[1][self->buffer_ptr[1]];
    }

    // Set the new period
    tim_handle.Init.Period = self->period - 1;
    HAL_TIM_Base_Init(&tim_handle);

    // TIM7 has limited HAL support, set registers manually
    tim_handle.Instance->SR = 0; // Prevent the SR from triggering an interrupt
    tim_handle.Instance->CR1 |= TIM_CR1_CEN; // Resume timer
    tim_handle.Instance->CR1 |= TIM_CR1_URS; // Disable non-overflow interrupts
    __HAL_TIM_ENABLE_IT(&tim_handle, TIM_IT_UPDATE);
}

STATIC bool fill_buffers(audiopwmio_pwmaudioout_obj_t *self) {
    // Naive PCM-to-PWM conversion
    int16_t threshold = 0x666; // 0.05; TODO: make configurable
    uint8_t *buffer;
    uint32_t buffer_length;
    audioio_get_buffer_result_t get_buffer_result;

    bool average = (self->sample_channel_count > 1) && !self->pin[1];
    bool replicate = (self->sample_channel_count == 1) && self->pin[1];
    int8_t effective_channels = average ? 1 : self->sample_channel_count;

    do {
        get_buffer_result = audiosample_get_buffer(self->sample, false, 0, &buffer, &buffer_length);
        if (get_buffer_result == GET_BUFFER_ERROR) {
            return false;
        }

        uint32_t num_samples = buffer_length / self->bytes_per_sample / self->sample_channel_count;
        int16_t *buffer16 = (int16_t *)buffer;

        while (num_samples--) {
            for (int8_t channel = 0; channel < effective_channels; channel++) {
                int16_t val;
                if (self->bytes_per_sample == 1) {
                    val = *buffer++ << 8;
                } else {
                    val = *buffer16++;
                }
                val += self->sample_offset;

                if (average) {
                    int16_t next;
                    if (self->bytes_per_sample == 1) {
                        next = *buffer++ << 8;
                    } else {
                        next = *buffer16++;
                    }
                    next += self->sample_offset;
                    val += (next - val) / 2;
                }

                int8_t new_pos = (val > threshold) - (val < -threshold);
                if (new_pos == -self->pos[channel]) {
                    if (self->len[channel] > 1) {
                        self->buffer[channel][self->buffer_length[channel]++] = self->len[channel];
                        if (replicate) {
                            self->buffer[1 - channel][self->buffer_length[1 - channel]++] = self->len[channel];
                        }
                        self->len[channel] = 0;
                    }
                    self->pos[channel] = new_pos;
                }
                self->len[channel]++;
            }
        }
    } while (get_buffer_result == GET_BUFFER_MORE_DATA &&
        (!self->buffer_length[0] || (self->pin[1] && !self->buffer_length[1])));

    if (get_buffer_result == GET_BUFFER_DONE) {
        // It's the final countdown
        for (int8_t channel = 0; channel < effective_channels; channel++) {
            self->buffer[channel][self->buffer_length[channel]++] = self->len[channel];
            if (replicate) {
                self->buffer[1 - channel][self->buffer_length[1 - channel]++] = self->len[channel];
            }
        }

        if (self->loop) {
            audiosample_reset_buffer(self->sample, false, 0);
        } else {
            self->stopping = true;
        }
    }
    return true;
}

STATIC void move_to_beginning(uint16_t *buffer, uint16_t *buffer_length, uint16_t *buffer_ptr) {
    if (*buffer_ptr < *buffer_length) {
        memmove(buffer, buffer + *buffer_ptr, *buffer_length - *buffer_ptr);
        *buffer_length -= *buffer_ptr;
    } else {
        *buffer_length = 0;
    }
    *buffer_ptr = 0;
}

STATIC void pwmaudioout_event_handler(void) {
    // Detect TIM Update event
    if (__HAL_TIM_GET_FLAG(&tim_handle, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&tim_handle, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_IT(&tim_handle, TIM_IT_UPDATE);
            if (!active_audio || active_audio->paused) {
                __HAL_TIM_DISABLE_IT(&tim_handle, TIM_IT_UPDATE);
                return;
            }

            bool refill = false;

            active_audio->buffer[0][active_audio->buffer_ptr[0]] -= active_audio->period;
            if (!active_audio->buffer[0][active_audio->buffer_ptr[0]]) {
                toggle_pin(0);
                if (++(active_audio->buffer_ptr[0]) >= active_audio->buffer_length[0]) {
                    refill = true;
                }
            }
            if (active_audio->pin[1]) {
                active_audio->buffer[1][active_audio->buffer_ptr[1]] -= active_audio->period;
                if (!active_audio->buffer[1][active_audio->buffer_ptr[1]]) {
                    toggle_pin(1);
                    if (++(active_audio->buffer_ptr[1]) >= active_audio->buffer_length[1]) {
                        refill = true;
                    }
                }
            }

            if (refill) {
                __HAL_TIM_DISABLE_IT(&tim_handle, TIM_IT_UPDATE);

                move_to_beginning(active_audio->buffer[0], &active_audio->buffer_length[0], &active_audio->buffer_ptr[0]);
                if (active_audio->pin[1]) {
                    move_to_beginning(active_audio->buffer[1], &active_audio->buffer_length[1], &active_audio->buffer_ptr[1]);
                }

                if (active_audio->stopping || !fill_buffers(active_audio)) {
                    // No more audio. Turn off output and don't restart.
                    common_hal_audiopwmio_pwmaudioout_stop(active_audio);
                    return;
                }
            }

            // Count up to the next given value.
            start_timer(active_audio);
        }
    }
}

void audiopwmout_reset() {
    if (active_audio) {
        common_hal_audiopwmio_pwmaudioout_stop(active_audio);
    }
}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t quiescent_value) {

    // Set up the pin(s) for output
    self->pin[0] = left_channel;
    self->pin[1] = right_channel;
    set_drive_mode(left_channel, GPIO_MODE_OUTPUT_PP);
    if (right_channel) {
        set_drive_mode(right_channel, GPIO_MODE_OUTPUT_PP);
    }

    self->buffer[0] = NULL;
    self->buffer[1] = NULL;

    self->quiescent_value = quiescent_value;
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t *self) {
    return !self->pin[0];
}

STATIC void free_buffers(audiopwmio_pwmaudioout_obj_t *self) {
    m_free(self->buffer[0]);
    self->buffer[0] = NULL;
    m_free(self->buffer[1]);
    self->buffer[1] = NULL;
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        return;
    }
    common_hal_audiopwmio_pwmaudioout_stop(self);

    free_buffers(self);

    self->pin[0] = 0;
    self->pin[1] = 0;
}

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t *self, mp_obj_t sample, bool loop) {
    common_hal_audiopwmio_pwmaudioout_stop(self);
    if (active_audio) {
        mp_raise_RuntimeError(translate("Another PWMAudioOut is already active")); // TODO
    }
    self->sample = sample;
    self->loop = loop;

    uint32_t sample_rate = audiosample_sample_rate(sample);
    self->bytes_per_sample = audiosample_bits_per_sample(sample) / 8;

    uint32_t max_buffer_length;
    uint8_t spacing;
    bool single_buffer;
    bool samples_signed;
    audiosample_get_buffer_structure(sample, /* single channel */ false,
        &single_buffer, &samples_signed, &max_buffer_length, &spacing);
    self->sample_channel_count = audiosample_channel_count(sample);
    self->sample_offset = (samples_signed ? 0x8000 : 0) - self->quiescent_value;

    free_buffers(self);

    if (max_buffer_length > UINT16_MAX) {
        mp_raise_ValueError_varg(translate("Buffer length %d too big. It must be less than %d"), max_buffer_length, UINT16_MAX);
    }
    uint16_t buffer_length = (uint16_t)max_buffer_length / self->bytes_per_sample;
    self->buffer[0] = m_malloc(buffer_length * sizeof(uint16_t), false);
    self->buffer_ptr[0] = self->buffer_length[0] = 0;
    if (self->pin[1]) {
        self->buffer[1] = m_malloc(buffer_length * sizeof(uint16_t), false);
        self->buffer_ptr[1] = self->buffer_length[1] = 0;
    }

    self->pos[0] = self->pos[1] = 1; // initially on
    self->len[0] = self->len[1] = 0;

    audiosample_reset_buffer(self->sample, false, 0);
    self->stopping = false;
    self->paused = false;
    if (!fill_buffers(self)) {
        mp_raise_RuntimeError(translate("Failed to buffer the sample"));
    }

    // Calculate period (TODO: supersample to 1 MHz?)
    TIM_TypeDef *tim_instance = stm_peripherals_find_timer();
    uint32_t source = stm_peripherals_timer_get_source_freq(tim_instance);
    uint32_t prescaler = source / sample_rate;

    // Activate timer
    active_audio = self;
    stm_peripherals_timer_reserve(tim_instance);
    stm_peripherals_timer_preinit(tim_instance, 4, pwmaudioout_event_handler);

    tim_handle.Instance = tim_instance;
    tim_handle.Init.Period = 100; // immediately replaced.
    tim_handle.Init.Prescaler = prescaler - 1;
    tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&tim_handle);
    tim_handle.Instance->SR = 0;

    // Alternate on and off, starting with on.
    set_pin(0, GPIO_PIN_SET);
    if (self->pin[1]) {
        set_pin(1, GPIO_PIN_SET);
    }

    // Count up to the next given value.
    start_timer(self);
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t *self) {
    if (active_audio != self) {
        return;
    }

    // Turn off timer counter.
    tim_handle.Instance->CR1 &= ~TIM_CR1_CEN;
    stm_peripherals_timer_free(tim_handle.Instance);

    active_audio = NULL;
    self->stopping = false;
    self->paused = false;

    // Make sure pins are left low.
    set_pin(0, GPIO_PIN_RESET);
    if (self->pin[1]) {
        set_pin(1, GPIO_PIN_RESET);
    }

    // Cannot free buffers here because we may be called from
    // the interrupt handler, and the heap is not reentrant.
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t *self) {
    return active_audio == self;
}

void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t *self) {
    self->paused = true;
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t *self) {
    self->paused = false;
    if (active_audio == self) {
        start_timer(self);
    }
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t *self) {
    return self->paused;
}
