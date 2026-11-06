// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tyeth Gundry
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"

#include "py/obj.h"

#include "driver/gptimer.h"

// The LEDC carrier is APB_CLK (80 MHz) / 2**PWM_BITS. 10 bits puts the carrier
// at 78.125 kHz: comfortably above hearing, while leaving 10 bits of amplitude
// resolution. Going finer drags the carrier down into the audible band.
#define PWM_BITS (10)
#define SAMPLE_BITS_TO_DISCARD (16 - PWM_BITS)

// Everything the alarm ISR touches. CircuitPython builds the GPTimer driver
// with CONFIG_GPTIMER_ISR_CACHE_SAFE, so the ISR can run while the flash cache
// is disabled - during any flash write, including the user saving code.py.
// Anything it dereferences must therefore live in internal RAM, which the
// MicroPython heap is not: port_malloc() prefers PSRAM when a board has it.
typedef struct {
    uint16_t *buffers[2];            // interleaved duty values, internal RAM
    size_t buffer_frames[2];         // valid frames currently in each buffer
    volatile size_t playing_frame;   // read cursor within the active buffer
    volatile uint8_t playing_buffer; // which buffer the ISR is draining
    volatile bool buffer_ready[2];   // buffer holds data the ISR has not used
    volatile bool last_buffer[2];    // this buffer ends the sample
    volatile bool stop_queued;       // the ISR reached the end of the sample
    uint16_t quiescent_value;        // scaled to PWM_BITS
    uint8_t left_channel;            // LEDC channel numbers, copied in so the
    uint8_t right_channel;           // ISR never reaches into the pwmio objects
    uint8_t out_channels;            // 1 or 2; the stride of buffers[]
    bool stereo;
    bool single_buffer;
    bool loop;
} audiopwmio_internal_data_t;

typedef struct {
    mp_obj_base_t base;

    pwmio_pwmout_obj_t left_pwm;
    pwmio_pwmout_obj_t right_pwm;
    gptimer_handle_t timer;

    mp_obj_t sample;
    audiopwmio_internal_data_t *internal_data;

    size_t buffer_size[2];           // allocated bytes, for the free path

    uint8_t sample_channel_count;
    uint8_t bytes_per_sample;

    bool signed_to_unsigned;
    bool playing;
    bool paused;
} audiopwmio_pwmaudioout_obj_t;

// Called from port_background_task(): refills buffers the ISR has drained.
void audiopwmout_background(void);
