/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "modmachine.h"
#include "py/runtime.h"

#if MICROPY_PY_MACHINE_PDM_PCM

// --------------------------------------------------------------------------------
// ----------------- extmod/machine_pdm_pcm.c ------------------------------------
// This file could be promoted in future to extmod/machine_pdm_pcm.c
// as it replicates the extmod/machine_i2s.c structure for separation of
// concerns between port specific code and higher level MPY functions.
// Eventually both could be refactored to share common code for
// serial audio interfaces.

#include "py/ringbuf.h"
#include "mphalport.h"

typedef enum
{
    SAMPLE_RATE_8000 = 8000,
    SAMPLE_RATE_16000 = 16000,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_44100 = 44100,
    SAMPLE_RATE_48000 = 48000,
} sample_rate_t;

typedef enum {
    BITS_16 = 16,
    BITS_32 = 32
} pdm_pcm_word_length_t;

typedef enum {
    MONO,
    STEREO
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
} io_mode_t;

// Arguments for PDM_PCM() constructor and PDM_PCM.init().
enum {
    ARG_sck,
    ARG_data,
    ARG_sample_rate,
    ARG_bits,
    ARG_format,
    ARG_gain,
    ARG_ibuf
};

#if MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

static void fill_appbuf_from_ringbuf_non_blocking(machine_pdm_pcm_obj_t *self);

#endif // MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

// The port must provide implementations of these low-level PDM PCM functions.
static void mp_machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, mp_arg_val_t *args);
static machine_pdm_pcm_obj_t *mp_machine_pdm_pcm_make_new_instance(mp_int_t pdm_pcm_id);
static void mp_machine_pdm_pcm_deinit(machine_pdm_pcm_obj_t *self);
static void mp_machine_pdm_pcm_irq_update(machine_pdm_pcm_obj_t *self);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_PDM_PCM_INCLUDEFILE

#if MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

static uint32_t fill_appbuf_from_ringbuf(machine_pdm_pcm_obj_t *self, mp_buffer_info_t *appbuf) {
    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = (uint8_t *)appbuf->buf;
    uint8_t appbuf_sample_size_in_bytes = (self->bits == BITS_16 ? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_needed_from_ringbuf = appbuf->len * (PDM_PCM_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    int data;

    uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

    while (num_bytes_needed_from_ringbuf) {
        for (uint8_t i = 0; i < PDM_PCM_RX_FRAME_SIZE_IN_BYTES; i++) {
            int8_t r_to_a_mapping = pdm_pcm_frame_map[f_index][i];
            if (r_to_a_mapping != -1) {
                if (self->io_mode == BLOCKING) {
                    /**
                     * Data Memory Barrier __DMB();
                     * Ensure ring buffer writes are visible to the main thread.
                     * Avoid non-volatile declaration read/write reordering when
                     * -0s optimization is enabled, which make the main thread
                     * miss the update and cause a deadlock.
                     */
                    while (__DMB(), (data = ringbuf_get(&self->ring_buffer)) == -1) {
                        ;
                    }
                    app_p[r_to_a_mapping] = (uint8_t)data;
                    num_bytes_copied_to_appbuf++;
                }
            } else {
                // Discard unused byte from ring buffer
                if (self->io_mode == BLOCKING) {
                    while (__DMB(), (data = ringbuf_get(&self->ring_buffer)) == -1) {
                        ;
                    }
                }
            }
            num_bytes_needed_from_ringbuf--;
        }
        app_p += appbuf_sample_size_in_bytes;
    }
    return num_bytes_copied_to_appbuf;
}

static void fill_appbuf_from_ringbuf_non_blocking(machine_pdm_pcm_obj_t *self) {
    /** TODO: Implement non-blocking version */
}

#endif // MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

MP_NOINLINE static void machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck,             MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_data,            MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sample_rate,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 16000} },
        { MP_QSTR_bits,            MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 16} },
        { MP_QSTR_format,          MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = MONO} },
        { MP_QSTR_gain,            MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = mp_const_none} },
        { MP_QSTR_ibuf,            MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 20000} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_machine_pdm_pcm_init_helper(self, args);
}

static void machine_pdm_pcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PDM_PCM(sck="MP_HAL_PIN_FMT ",\n"
        "data="MP_HAL_PIN_FMT ",\n"
        "sample_rate=%ld,\n"
        "bits=%u,\n"
        "format=%u,\n"
        "gain=%f,\n"
        "ibuf=%d)",
        mp_hal_pin_name(self->sck),
        mp_hal_pin_name(self->data),
        self->sample_rate,
        self->bits,
        self->format,
        self->gain_db,
        self->ring_buffer.size
        );
}

static mp_obj_t machine_pdm_pcm_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 0, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pdm_pcm_obj_t *self = mp_machine_pdm_pcm_make_new_instance(0);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_pdm_pcm_init_helper(self, n_pos_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pdm_pcm_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    machine_pdm_pcm_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pdm_pcm_init_obj, 1, machine_pdm_pcm_init);

static mp_obj_t machine_pdm_pcm_deinit(mp_obj_t self_in) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_pdm_pcm_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pdm_pcm_deinit_obj, machine_pdm_pcm_deinit);

static const mp_rom_map_elem_t machine_pdm_pcm_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_pdm_pcm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_pdm_pcm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },

    // Constants
    // Word lengths
    { MP_ROM_QSTR(MP_QSTR_BITS_16),         MP_ROM_INT(BITS_16) },
    { MP_ROM_QSTR(MP_QSTR_BITS_32),         MP_ROM_INT(BITS_32) },

    // Modes
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },

    #ifdef MICROPY_PY_MACHINE_PDM_PDM_CLASS_EXTRAS
    MICROPY_PY_MACHINE_PDM_PDM_CLASS_EXTRAS
    #endif
};

static mp_uint_t machine_pdm_pcm_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint8_t appbuf_sample_size_in_bytes = (self->bits / 8) * (self->format == STEREO ? 2: 1);

    if (size % appbuf_sample_size_in_bytes != 0) { // size should be multiple of sample size
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == BLOCKING) {
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        #if MICROPY_PY_MACHINE_PDM_PCM_RING_BUF
        uint32_t num_bytes_read = fill_appbuf_from_ringbuf(self, &appbuf);
        #else
        uint32_t num_bytes_read = fill_appbuf_from_dma(self, &appbuf);
        #endif
        return num_bytes_read;
    }
    /** TODO: NON-BLOCKING implementation */

    return 0;
}

static const mp_stream_p_t pdm_pcm_stream_p = {
    .read = machine_pdm_pcm_stream_read,
    .is_text = false,
};


MP_DEFINE_CONST_DICT(machine_pdm_pcm_locals_dict, machine_pdm_pcm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pdm_pcm_type,
    MP_QSTR_PDM_PCM,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_pdm_pcm_make_new,
    print, machine_pdm_pcm_print,
    protocol, &pdm_pcm_stream_p,
    locals_dict, &machine_pdm_pcm_locals_dict
    );

#endif // MICROPY_PY_MACHINE_PDM_PCM_TDD
