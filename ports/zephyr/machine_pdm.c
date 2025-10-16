/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 NED KONZ <NED@METAMAGIX.TECH>
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_PDM_INCLUDEFILE.

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "py/ringbuf.h"

#if MICROPY_PY_MACHINE_PDM

#include <zephyr/audio/dmic.h>
#include <zephyr/kernel.h>
#include "zephyr_device.h"

// PDM operates in blocking mode:
// readinto() blocks until buffer is full
// Non-blocking and asyncio modes are planned for future implementation

#define PDM_BLOCK_SIZE (4096)
#define PDM_BLOCK_COUNT (4)

typedef enum {
    MONO = 0,
    STEREO = 1
} format_t;

typedef struct _machine_pdm_obj_t {
    mp_obj_base_t base;
    const struct device *dev;

    // Configuration
    uint32_t rate;
    uint8_t bits;
    format_t format;
    uint32_t ibuf;

    // Zephyr DMIC structures
    struct dmic_cfg dmic_cfg;
    struct pcm_stream_cfg pcm_cfg;
    struct k_mem_slab mem_slab;
    uint8_t *mem_slab_storage;

    // Ring buffer
    ringbuf_t ring_buffer;

    // State
    bool is_started;
    bool is_configured;
} machine_pdm_obj_t;

// Forward declarations
static void mp_machine_pdm_init_helper(machine_pdm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static void mp_machine_pdm_deinit(machine_pdm_obj_t *self);

// Constructor and init
static void mp_machine_pdm_init_helper(machine_pdm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_rate,
        ARG_bits,
        ARG_format,
        ARG_ibuf,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_rate,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 16000} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 16} },
        { MP_QSTR_format,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MONO} },
        { MP_QSTR_ibuf,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8192} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate parameters
    self->rate = args[ARG_rate].u_int;
    self->bits = args[ARG_bits].u_int;
    self->format = args[ARG_format].u_int;
    self->ibuf = args[ARG_ibuf].u_int;

    if (self->bits != 16 && self->bits != 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 16 or 32"));
    }

    if (self->format != MONO && self->format != STEREO) {
        mp_raise_ValueError(MP_ERROR_TEXT("format must be MONO or STEREO"));
    }

    // Allocate memory slab storage
    size_t slab_size = PDM_BLOCK_SIZE * PDM_BLOCK_COUNT;
    self->mem_slab_storage = m_new(uint8_t, slab_size);

    // Initialize memory slab
    int ret = k_mem_slab_init(&self->mem_slab, self->mem_slab_storage, PDM_BLOCK_SIZE, PDM_BLOCK_COUNT);
    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to init memory slab"));
    }

    // Configure PCM stream
    self->pcm_cfg.pcm_rate = self->rate;
    self->pcm_cfg.pcm_width = self->bits;
    self->pcm_cfg.block_size = PDM_BLOCK_SIZE;
    self->pcm_cfg.mem_slab = &self->mem_slab;

    // Configure PDM channels
    struct pdm_chan_cfg chan_cfg = {0};
    int num_channels = (self->format == STEREO) ? 2 : 1;

    // Set number of channels and streams
    chan_cfg.req_num_chan = num_channels;
    chan_cfg.req_num_streams = 1;

    // Build channel map: channel 0, PDM 0, LEFT for first channel
    chan_cfg.req_chan_map_lo = dmic_build_channel_map(0, 0, PDM_CHAN_LEFT);
    if (num_channels == 2) {
        // Add channel 1, PDM 0, RIGHT for stereo
        chan_cfg.req_chan_map_lo |= dmic_build_channel_map(1, 0, PDM_CHAN_RIGHT);
    }
    chan_cfg.req_chan_map_hi = 0;

    // Configure PDM I/O
    struct pdm_io_cfg io_cfg;
    io_cfg.min_pdm_clk_freq = 1000000;   // 1 MHz
    io_cfg.max_pdm_clk_freq = 3500000;   // 3.5 MHz
    io_cfg.min_pdm_clk_dc = 40;          // 40% duty cycle
    io_cfg.max_pdm_clk_dc = 60;          // 60% duty cycle

    // Build DMIC config
    self->dmic_cfg.io = io_cfg;
    self->dmic_cfg.streams = &self->pcm_cfg;
    self->dmic_cfg.channel = chan_cfg;

    // Configure device
    ret = dmic_configure(self->dev, &self->dmic_cfg);
    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to configure DMIC"));
    }

    // Allocate ring buffer using MicroPython's ringbuf
    ringbuf_alloc(&self->ring_buffer, self->ibuf);

    self->is_started = false;
    self->is_configured = true;
}

static mp_obj_t mp_machine_pdm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get device by name (e.g., "pdm0")
    const struct device *dev = zephyr_device_find(args[0]);

    if (!device_is_ready(dev)) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("PDM device not ready"));
    }

    machine_pdm_obj_t *self = mp_obj_malloc(machine_pdm_obj_t, type);
    self->dev = dev;
    self->is_configured = false;
    self->is_started = false;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_args);
    mp_machine_pdm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// PDM.deinit()
static void mp_machine_pdm_deinit(machine_pdm_obj_t *self) {
    if (self->is_started) {
        dmic_trigger(self->dev, DMIC_TRIGGER_STOP);
        self->is_started = false;
    }

    if (self->is_configured) {
        if (self->ring_buffer.buf) {
            m_del(uint8_t, self->ring_buffer.buf, self->ring_buffer.size);
            self->ring_buffer.buf = NULL;
        }

        if (self->mem_slab_storage) {
            m_del(uint8_t, self->mem_slab_storage, PDM_BLOCK_SIZE * PDM_BLOCK_COUNT);
            self->mem_slab_storage = NULL;
        }

        self->is_configured = false;
    }
}

// Stream read implementation (blocking mode)
static mp_uint_t mp_machine_pdm_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Start capture if not already running
    if (!self->is_started) {
        int ret = dmic_trigger(self->dev, DMIC_TRIGGER_START);
        if (ret < 0) {
            *errcode = MP_EIO;
            return MP_STREAM_ERROR;
        }
        self->is_started = true;
    }

    if (size == 0) {
        return 0;
    }

    // Clear ring buffer to ensure fresh data
    self->ring_buffer.iget = 0;
    self->ring_buffer.iput = 0;

    // Keep reading from DMIC and filling ring buffer until we have enough data
    while (ringbuf_avail(&self->ring_buffer) < size) {
        void *buffer;
        uint32_t buffer_size;

        // Read one block from DMIC (this blocks until data is available)
        int ret = dmic_read(self->dev, 0, &buffer, &buffer_size, 1000); // 1 second timeout

        if (ret < 0) {
            *errcode = MP_ETIMEDOUT;
            return MP_STREAM_ERROR;
        }

        if (buffer && buffer_size > 0) {
            // Copy to ring buffer using efficient memcpy
            // Only copy what we need and what fits
            size_t space_available = ringbuf_free(&self->ring_buffer);
            size_t bytes_to_copy = (buffer_size < space_available) ? buffer_size : space_available;

            if (bytes_to_copy > 0) {
                ringbuf_memcpy_put_internal(&self->ring_buffer, (uint8_t *)buffer, bytes_to_copy);
            }

            // Free the buffer back to the memory slab
            k_mem_slab_free(&self->mem_slab, buffer);

            // If ring buffer is full, we have enough data
            if (space_available == 0) {
                break;
            }
        }
    }

    // Copy from ring buffer to application buffer
    // This will block polling the ring buffer byte-by-byte until we have all the data
    uint8_t *app_p = (uint8_t *)buf_in;
    for (size_t i = 0; i < size; i++) {
        int byte;
        // Poll until byte available
        while ((byte = ringbuf_get(&self->ring_buffer)) == -1) {
            k_msleep(1);
        }
        app_p[i] = (uint8_t)byte;
    }

    return size;
}

// Print method
static void mp_machine_pdm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PDM(rate=%u, bits=%u, format=%s, ibuf=%u)",
              self->rate,
              self->bits,
              self->format == STEREO ? "STEREO" : "MONO",
              self->ibuf);
}

// Port-specific class constants
#define MICROPY_PY_MACHINE_PDM_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_MONO),        MP_ROM_INT(MONO) }, \
    { MP_ROM_QSTR(MP_QSTR_STEREO),      MP_ROM_INT(STEREO) },

#endif // MICROPY_PY_MACHINE_PDM
