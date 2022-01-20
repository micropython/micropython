/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
 *
 * Audio Python module.
 */

// Example usage, record 5 seconds and save to storage:

/*
import audio, time

audio.init(channels=1, frequency=16000, gain_db=8, buffers=128, overflow=True)

# Start audio streaming
audio.start_streaming(None)

# do something else
print("recording audio...")
start = time.ticks_ms()

buf = bytearray()
while ((time.ticks_ms() - start) < 5000):
    buf.extend(audio.get_buffer(timeout=10))

# Stop streaming
audio.stop_streaming()
print("samples captured: {} overflow: {}".format(audio.samples(), audio.overflow()))

# Write recorded PCM
with open('test.pcm', 'w') as fout:
    fout.write(buf)
*/

#include <stdio.h>
#include <stdint.h>
#include "py/obj.h"
#include "py/objarray.h"
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/binary.h"
#include "py/runtime.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "pdm_filter.h"
#include "pdm.pio.h"

#define PDM_DEFAULT_GAIN    (8)
#define PDM_DEFAULT_FREQ    (16000)
#define PDM_DEFAULT_BUFFERS (128)   // Number of PCM samples buffers.
#define PDM_BUFFER_SIZE     (512)
#define PDM_TIME_CONV       (0)     // Enable to print average conversion time.

#ifndef MICROPY_PY_AUDIO_USE_SYNC_EVENTS
#define MICROPY_PY_AUDIO_USE_SYNC_EVENTS    (1)
#endif

#define RAISE_OS_EXCEPTION(msg) mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT(msg))

typedef struct _audio_data_t {
    volatile uint32_t head;
    volatile uint32_t tail;
    #if PDM_TIME_CONV
    uint32_t conv_total;
    uint32_t conv_times;
    #endif
    uint32_t n_samples;
    uint32_t n_buffers;
    uint32_t t_samples;
    uint8_t *pdm_buffer;
    int16_t *pcm_buffer;
    mp_obj_array_t *pcm_buffer_user;
    volatile bool streaming;
    volatile bool overflow;
    bool abort_on_overflow;
    int dma_channel;
    uint8_t dma_buf_idx;
    mp_obj_t user_callback;
    TPDMFilter_InitStruct pdm_filter; // OpenPDM filter used to convert PDM into PCM
    void (*pdm_filter_func)(uint8_t *, int16_t *, uint16_t, TPDMFilter_InitStruct *);
} audio_data_t;

static bool audio_initialized = false;

#define audio_data MP_STATE_PORT(audio_data)
#define NEXT_BUFFER(x) (((x) + 1) % (audio_data->n_buffers))

#if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
volatile bool audio_task_scheduled = false;
static const mp_obj_fun_builtin_fixed_t audio_task_obj;
static mp_obj_t audio_task(mp_obj_t none_in)
#else
#include "pendsv.h"
static void audio_pendsv_callback(void)
#endif
{
    if (audio_data->streaming == false) {
        return mp_const_none;
    }

    if (audio_data->head != audio_data->tail) {
        audio_data->pcm_buffer_user->items =
            &audio_data->pcm_buffer[audio_data->head * audio_data->n_samples];

        // Advance head to next buffer.
        audio_data->head = NEXT_BUFFER(audio_data->head);

        // Call user callback.
        mp_call_function_1(audio_data->user_callback, MP_OBJ_FROM_PTR(audio_data->pcm_buffer_user));
    } else if (audio_data->overflow == true && audio_data->abort_on_overflow) {
        #if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
        RAISE_OS_EXCEPTION("Audio buffer overflow.");
        #endif
        return mp_const_none;
    }

    #if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
    if (audio_data->head != audio_data->tail) {
        // Re-schedule function
        audio_task_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&audio_task_obj), mp_const_none);
    } else {
        audio_task_scheduled = false;
    }
    return mp_const_none;
    #endif
}
#if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audio_task_obj, audio_task);
#endif

static void dma_irq_handler() {
    if (dma_irqn_get_channel_status(MICROPY_HW_PDM_DMA, audio_data->dma_channel)) {
        // Clear the interrupt request.
        dma_irqn_acknowledge_channel(MICROPY_HW_PDM_DMA, audio_data->dma_channel);

        // Set the next PDM buffer and retrigger the DMA channel
        // immediatley while PDM samples are converted to PCM samples.
        dma_channel_set_write_addr(audio_data->dma_channel,
            &audio_data->pdm_buffer[(audio_data->dma_buf_idx ^ 1) * PDM_BUFFER_SIZE], true);

        #if PDM_TIME_CONV
        mp_uint_t start = mp_hal_ticks_us();
        #endif

        // Convert PDM to PCM samples.
        audio_data->pdm_filter_func(
            &audio_data->pdm_buffer[audio_data->dma_buf_idx * PDM_BUFFER_SIZE],
            &audio_data->pcm_buffer[audio_data->tail * audio_data->n_samples],
            1, &audio_data->pdm_filter);

        #if PDM_TIME_CONV
        audio_data->conv_total += (mp_hal_ticks_us() - start);
        audio_data->conv_times += 1;
        #endif

        audio_data->dma_buf_idx ^= 1;

        if (NEXT_BUFFER(audio_data->tail) != audio_data->head) {
            // Advance buffer.
            audio_data->tail = NEXT_BUFFER(audio_data->tail);
            audio_data->t_samples += audio_data->n_samples;
        } else {
            // Use current buffer and set overflow flag.
            audio_data->overflow = true;
        }

        if (mp_obj_is_callable(audio_data->user_callback)) {
            // Schedule audio callback.
            #if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
            if (audio_task_scheduled == false) {
                audio_task_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&audio_task_obj), mp_const_none);
            }
            #else
            pendsv_schedule_dispatch(PENDSV_DISPATCH_AUDIO, audio_pendsv_callback);
            #endif
        }
    }
}

static mp_obj_t modaudio_init(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_channels, ARG_frequency, ARG_gain_db, ARG_buffers, ARG_overflow, ARG_clkdiv };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_channels,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_frequency,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PDM_DEFAULT_FREQ} },
        { MP_QSTR_gain_db,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PDM_DEFAULT_GAIN} },
        { MP_QSTR_buffers,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PDM_DEFAULT_BUFFERS} },
        { MP_QSTR_overflow,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = true} },
        { MP_QSTR_clkdiv,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Sanity checks
    if (args[ARG_channels].u_int != 1) {
        RAISE_OS_EXCEPTION("Invalid number of channels. Expected 1.");
    }

    if (!(args[ARG_frequency].u_int == 16000
          || args[ARG_frequency].u_int == 32000
          || args[ARG_frequency].u_int == 48000)) {
        RAISE_OS_EXCEPTION("Invalid frequency. Expected 16KHz, 32KHz or 48KHz.");
    }

    #if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
    audio_task_scheduled = false;
    #endif

    // The decimation factor is set to keep the PDM datarate within min/max specs for the mic:
    // 16 KHz * 128 -> 2048 KHz
    // 32 KHz * 64  -> 2048 KHz
    // 48 KHz * 64  -> 3072 KHz
    uint32_t decimation = (args[ARG_frequency].u_int == 16000) ? 128 : 64;
    uint32_t n_samples = (PDM_BUFFER_SIZE * 8) / decimation;

    audio_data = m_new_obj(audio_data_t);
    audio_data->head = 0;
    audio_data->tail = 0;
    #if PDM_TIME_CONV
    audio_data->conv_total = 0;
    audio_data->conv_times = 0;
    #endif
    audio_data->t_samples = 0;
    audio_data->n_samples = n_samples;
    audio_data->n_buffers = args[ARG_buffers].u_int;
    audio_data->streaming = false;
    audio_data->overflow = false;
    audio_data->abort_on_overflow = args[ARG_overflow].u_int;
    audio_data->user_callback = mp_const_none;
    audio_data->dma_channel = -1;
    audio_data->dma_buf_idx = 0;
    // Allocate PDM/PCM buffers.
    // Using double buffers for PDM samples to keep the DMA busy.
    audio_data->pdm_buffer = m_new(uint8_t, PDM_BUFFER_SIZE * 2);
    audio_data->pcm_buffer = m_new(int16_t, audio_data->n_buffers * audio_data->n_samples);

    // Instead of returning a new heap object for every buffer, which will result in heap
    // fragmentation, reuse the same object and point to PCM buffers returned to the user.
    audio_data->pcm_buffer_user = mp_obj_new_bytearray_by_ref(
        audio_data->n_samples * sizeof(int16_t),
        &audio_data->pcm_buffer[audio_data->n_samples * 0]);

    // Initialize OpenPDM filter.
    audio_data->pdm_filter.Fs = args[ARG_frequency].u_int;
    audio_data->pdm_filter.MaxVolume = 1;
    audio_data->pdm_filter.nSamples = n_samples;
    audio_data->pdm_filter.LP_HZ = args[ARG_frequency].u_int / 2;
    audio_data->pdm_filter.HP_HZ = 10;
    audio_data->pdm_filter.In_MicChannels = args[ARG_channels].u_int;
    audio_data->pdm_filter.Out_MicChannels = args[ARG_channels].u_int;
    audio_data->pdm_filter.Decimation = decimation;
    audio_data->pdm_filter.filterGain = args[ARG_gain_db].u_int;
    audio_data->pdm_filter_func = (decimation == 64) ? Open_PDM_Filter_64 : Open_PDM_Filter_128;
    Open_PDM_Filter_Init(&audio_data->pdm_filter);

    // Configure PIO state machine
    float div;
    if (args[ARG_clkdiv].u_int != 0) {
        div = (float)args[ARG_clkdiv].u_int;
    } else {
        div = ((float)clock_get_hz(clk_sys)) / (args[ARG_frequency].u_int * decimation * 2);
    }

    uint offset = pio_add_program(MICROPY_HW_PDM_PIO, &pdm_pio_program);
    pdm_pio_program_init(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM, offset, MICROPY_HW_PDM_CLK_PIN, MICROPY_HW_PDM_DIN_PIN, div);

    // Wait for microphone to settle.
    mp_hal_delay_ms(100);

    // Find unused channel
    audio_data->dma_channel = dma_claim_unused_channel(false);
    if (audio_data->dma_channel < 0) {
        RAISE_OS_EXCEPTION("Failed to claim a DMA channel.");
    }

    // Configure DMA for transferring PIO rx buffer to raw buffers
    dma_channel_config c = dma_channel_get_default_config(audio_data->dma_channel);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);

    // Configure DMA channel without starting.
    dma_channel_configure(audio_data->dma_channel, &c,
        audio_data->pdm_buffer,     // Destinatinon pointer, first buffer.
        &MICROPY_HW_PDM_PIO->rxf[MICROPY_HW_PDM_SM],      // Source pointer.
        PDM_BUFFER_SIZE,            // Number of transfers.
        false                       // Don't start immediately.
        );

    // Setup DMA IRQ handler.
    // Disable IRQs.
    irq_set_enabled(MICROPY_HW_PDM_DMA_IRQ, false);

    // Clear DMA interrupts.
    dma_irqn_acknowledge_channel(MICROPY_HW_PDM_DMA, audio_data->dma_channel);

    static bool irq_handler_installed = false;
    if (!irq_handler_installed) {
        irq_handler_installed = true;
        // Install shared DMA IRQ handler.
        irq_add_shared_handler(MICROPY_HW_PDM_DMA_IRQ, dma_irq_handler, PICO_HIGHEST_IRQ_PRIORITY);
    }

    // Re-enable IRQs.
    irq_set_enabled(MICROPY_HW_PDM_DMA_IRQ, true);
    dma_irqn_set_channel_enabled(MICROPY_HW_PDM_DMA, audio_data->dma_channel, true);

    audio_initialized = true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(modaudio_init_obj, 0, modaudio_init);

static mp_obj_t modaudio_samples() {
    return mp_obj_new_int(audio_data->t_samples);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modaudio_samples_obj, modaudio_samples);

static mp_obj_t modaudio_overflow() {
    return mp_obj_new_bool(audio_data->overflow);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modaudio_overflow_obj, modaudio_overflow);

static mp_obj_t modaudio_start_streaming(mp_obj_t callback_obj) {
    audio_data->head = 0;
    audio_data->tail = 0;
    #if PDM_TIME_CONV
    audio_data->conv_total = 0;
    audio_data->conv_times = 0;
    #endif
    audio_data->t_samples = 0;
    audio_data->overflow = false;

    if (mp_obj_is_callable(callback_obj)) {
        audio_data->user_callback = callback_obj;
    } else {
        audio_data->user_callback = mp_const_none;
    }

    // Re-enable the state machine.
    pio_sm_clear_fifos(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM);
    pio_sm_set_enabled(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM, true);

    dma_channel_start(audio_data->dma_channel);

    audio_data->streaming = true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modaudio_start_streaming_obj, modaudio_start_streaming);

static mp_obj_t modaudio_stop_streaming() {

    if (audio_data->streaming) {
        // Disable PDM and IRQ
        dma_channel_abort(audio_data->dma_channel);
        dma_irqn_set_channel_enabled(MICROPY_HW_PDM_DMA, audio_data->dma_channel, false);

        // Disable state machine.
        pio_sm_set_enabled(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM, false);
        pio_sm_clear_fifos(MICROPY_HW_PDM_PIO, MICROPY_HW_PDM_SM);

        audio_data->streaming = false;
        #if MICROPY_PY_AUDIO_USE_SYNC_EVENTS
        for (mp_uint_t start = mp_hal_ticks_ms();
             audio_task_scheduled && (mp_hal_ticks_ms() - start) >= 1000;
             mp_hal_delay_ms(10)) {;
        }
        #endif

        #if PDM_TIME_CONV
        mp_printf(&mp_plat_print, "Average conversion time:%ld us\n",
            (audio_data->conv_total / audio_data->conv_times));
        #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modaudio_stop_streaming_obj, modaudio_stop_streaming);

static mp_obj_t modaudio_get_buffer(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (audio_data->streaming == false) {
        RAISE_OS_EXCEPTION("Audio streaming is not enabled.");
    }

    if (audio_data->overflow == true && audio_data->abort_on_overflow) {
        RAISE_OS_EXCEPTION("Audio buffer overflow.");
    }

    if (mp_obj_is_callable(audio_data->user_callback)) {
        RAISE_OS_EXCEPTION("Audio streaming with callback function is enabled.");
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); (audio_data->head == audio_data->tail);) {
        if (args[ARG_timeout].u_int && (mp_hal_ticks_ms() - start) >= args[ARG_timeout].u_int) {
            RAISE_OS_EXCEPTION("Timeout waiting for audio buffer.");
        }
    }

    audio_data->pcm_buffer_user->items
        = &audio_data->pcm_buffer[audio_data->head * audio_data->n_samples];

    // Advance head to next buffer.
    audio_data->head = NEXT_BUFFER(audio_data->head);

    // Return PCM buffer.
    return MP_OBJ_FROM_PTR(audio_data->pcm_buffer_user);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(modaudio_get_buffer_obj, 0, modaudio_get_buffer);

void modaudio_deinit() {
    if (audio_initialized) {
        modaudio_stop_streaming();

        audio_data->head = 0;
        audio_data->tail = 0;
        audio_data->t_samples = 0;
        audio_data->n_samples = 0;
        audio_data->n_buffers = 0;
        audio_data->pdm_buffer = NULL;
        audio_data->pcm_buffer = NULL;
        audio_data->streaming = false;
        audio_data->overflow = false;
        audio_data->abort_on_overflow = false;
        audio_data->user_callback = mp_const_none;

        if (audio_data->dma_channel >= 0) {
            dma_channel_unclaim(audio_data->dma_channel);
        }
    }
    audio_data = MP_OBJ_NULL;
    audio_initialized = false;
}

static const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_audio) },
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&modaudio_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_samples),         MP_ROM_PTR(&modaudio_samples_obj) },
    { MP_ROM_QSTR(MP_QSTR_overflow),        MP_ROM_PTR(&modaudio_overflow_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_streaming), MP_ROM_PTR(&modaudio_start_streaming_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_streaming),  MP_ROM_PTR(&modaudio_stop_streaming_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_buffer),      MP_ROM_PTR(&modaudio_get_buffer_obj) },
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t audio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t)&globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_audio, audio_module, MICROPY_PY_AUDIO);
