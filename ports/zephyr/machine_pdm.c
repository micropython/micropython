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

// PDM operates in 3 modes:
// Mode 1: Blocking - readinto() blocks until buffer is full
// Mode 2: Non-Blocking - readinto() returns immediately, callback fires when ready
// Mode 3: Asyncio - implements stream protocol for asyncio integration

#define PDM_BLOCK_SIZE (2048)
#define PDM_BLOCK_COUNT (4)
#define SIZEOF_NON_BLOCKING_COPY_IN_BYTES (1024)

typedef enum {
    MONO = 0,
    STEREO = 1
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
    ASYNCIO
} io_mode_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    uint32_t index;
    bool copy_in_progress;
} non_blocking_descriptor_t;

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

    // Mode and non-blocking support
    io_mode_t io_mode;
    non_blocking_descriptor_t non_blocking_descriptor;
    mp_obj_t callback_for_non_blocking;

    // Background work queue for filling ring buffer
    struct k_work work;
    struct k_work_delayable delayed_work;
    bool work_running;

    // State
    bool is_started;
    bool is_configured;
} machine_pdm_obj_t;

// Forward declarations
static void mp_machine_pdm_init_helper(machine_pdm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static void mp_machine_pdm_deinit(machine_pdm_obj_t *self);
static void pdm_background_work_handler(struct k_work *work);
static void fill_appbuf_from_ringbuf_non_blocking(machine_pdm_obj_t *self);

// Background work handler - runs in work queue thread
// Continuously polls DMIC and fills ring buffer
static void pdm_background_work_handler(struct k_work *work) {
    machine_pdm_obj_t *self = CONTAINER_OF(work, machine_pdm_obj_t, work);

    if (!self->is_started || !self->work_running) {
        return;
    }

    static int work_call_count = 0;
    int buffers_read = 0;

    // Read up to 3 DMIC buffers per iteration to prevent overflow
    // but avoid spinning too much in the work handler
    for (int i = 0; i < 3; i++) {
        void *buffer;
        uint32_t buffer_size;

        // Check if ring buffer has enough space for another block
        size_t space_available = ringbuf_free(&self->ring_buffer);
        // Even if ring buffer is full, continue to call dmic_read() to drain DMIC queue
        // and keep the driver active. We'll discard data if necessary.

        // Use longer timeout to ensure DMIC has time to fill buffer
        // Note: dmic_read() timeout is in milliseconds, not k_timeout_t
        // At 16kHz 16-bit, 2048 bytes = 64ms, so use 200ms to be safe
        int timeout_ms = 200;
        int ret = dmic_read(self->dev, 0, &buffer, &buffer_size, timeout_ms);

        if (ret < 0) {
            // -EAGAIN means no data ready, keep trying
            if (ret == -11) {
                continue; // Try next iteration
            }
            break; // Other errors are fatal
        }

        if (!buffer || buffer_size == 0) {
            // No data available
            break;
        }

        // Copy to ring buffer if there's space
        size_t bytes_to_copy = (buffer_size < space_available) ? buffer_size : space_available;

        if (bytes_to_copy > 0) {
            ringbuf_memcpy_put_internal(&self->ring_buffer, (uint8_t *)buffer, bytes_to_copy);
            buffers_read++;
        }
        // If ring buffer is full (bytes_to_copy == 0), we still read and free the buffer
        // to keep the DMIC driver active

        // Always free the buffer back to the memory slab
        k_mem_slab_free(&self->mem_slab, buffer);

        // If in non-blocking mode, check if we can fill the app buffer
        if (self->io_mode == NON_BLOCKING && self->non_blocking_descriptor.copy_in_progress) {
            fill_appbuf_from_ringbuf_non_blocking(self);
        }
    }

    // Resubmit work if still running
    // For asyncio mode, always resubmit to keep background worker active
    // For non-blocking mode, only resubmit if copy is still in progress
    if (self->work_running) {
        bool should_resubmit = (self->io_mode == ASYNCIO ||
            (self->io_mode == NON_BLOCKING && self->non_blocking_descriptor.copy_in_progress));

        if (should_resubmit) {
            k_work_submit(&self->work);
        }
    }
}

// Copy data from ring buffer to app buffer (non-blocking mode)
// Called from work queue context
static void fill_appbuf_from_ringbuf_non_blocking(machine_pdm_obj_t *self) {
    uint8_t *app_p = &(((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index]);
    uint32_t bytes_remaining = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t bytes_to_copy = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, bytes_remaining);

    if (ringbuf_avail(&self->ring_buffer) >= bytes_to_copy) {
        // Copy bytes from ring buffer to app buffer
        for (uint32_t i = 0; i < bytes_to_copy; i++) {
            int byte = ringbuf_get(&self->ring_buffer);
            if (byte != -1) {
                app_p[i] = (uint8_t)byte;
            }
        }

        self->non_blocking_descriptor.index += bytes_to_copy;

        // Check if buffer is full
        if (self->non_blocking_descriptor.index >= self->non_blocking_descriptor.appbuf.len) {
            self->non_blocking_descriptor.copy_in_progress = false;
            // Schedule callback to run in main thread
            mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}

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
    ringbuf_alloc(&self->ring_buffer, self->ibuf + 1);

    // Initialize work queue
    k_work_init(&self->work, pdm_background_work_handler);

    // Initialize mode and state
    self->io_mode = BLOCKING;
    self->callback_for_non_blocking = mp_const_none;
    self->work_running = false;
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
    // Stop background work first
    if (self->work_running) {
        self->work_running = false;
        k_work_cancel(&self->work);
        // Give work a chance to exit
        k_msleep(10);
    }

    if (self->is_started) {
        dmic_trigger(self->dev, DMIC_TRIGGER_STOP);
        self->is_started = false;

        // Drain any remaining DMIC buffers to free them back to the memory slab
        // This ensures the memory slab is in a clean state
        void *buffer;
        uint32_t buffer_size;
        int drain_count = 0;
        while (drain_count < 10) {  // Max 10 attempts to avoid infinite loop
            int ret = dmic_read(self->dev, 0, &buffer, &buffer_size, 10);  // 10ms timeout
            if (ret < 0 || !buffer) {
                break;  // No more buffers to drain
            }
            k_mem_slab_free(&self->mem_slab, buffer);
            drain_count++;
        }
    }

    if (self->is_configured) {
        if (self->ring_buffer.buf) {
            size_t ring_size = self->ring_buffer.size;
            m_del(uint8_t, self->ring_buffer.buf, ring_size);
            self->ring_buffer.buf = NULL;
            self->ring_buffer.size = 0;
        }

        if (self->mem_slab_storage) {
            m_del(uint8_t, self->mem_slab_storage, PDM_BLOCK_SIZE * PDM_BLOCK_COUNT);
            self->mem_slab_storage = NULL;
        }

        self->is_configured = false;
    }

    // Reset descriptor state
    self->non_blocking_descriptor.copy_in_progress = false;
    self->non_blocking_descriptor.index = 0;
}

// IRQ update - called when callback is set/cleared
static void mp_machine_pdm_irq_update(machine_pdm_obj_t *self) {
    if (self->callback_for_non_blocking != mp_const_none) {
        // Enable non-blocking mode
        self->io_mode = NON_BLOCKING;

        // Start background work if not already running
        if (!self->work_running && self->is_started) {
            self->work_running = true;
            k_work_submit(&self->work);
        }
    } else {
        // Return to blocking mode
        self->io_mode = BLOCKING;

        // Stop background work
        if (self->work_running) {
            self->work_running = false;
            k_work_cancel(&self->work);
        }
    }
}

// Stream read implementation - supports all 3 modes
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

        // Start background work for non-blocking/asyncio modes
        if (self->io_mode != BLOCKING) {
            self->work_running = true;
            k_work_submit(&self->work);
        }
    }

    if (size == 0) {
        return 0;
    }

    // Handle non-blocking mode
    if (self->io_mode == NON_BLOCKING) {
        self->non_blocking_descriptor.appbuf.buf = (void *)buf_in;
        self->non_blocking_descriptor.appbuf.len = size;
        self->non_blocking_descriptor.index = 0;
        self->non_blocking_descriptor.copy_in_progress = true;
        return size;  // Return immediately
    }

    // Handle asyncio mode
    if (self->io_mode == ASYNCIO) {
        // Return available data without blocking
        size_t available = ringbuf_avail(&self->ring_buffer);
        size_t to_read = (size < available) ? size : available;
        uint8_t *app_p = (uint8_t *)buf_in;
        size_t bytes_read = 0;

        for (bytes_read = 0; bytes_read < to_read; bytes_read++) {
            int byte = ringbuf_get(&self->ring_buffer);
            if (byte == -1) {
                break; // Should not happen if `available` is correct
            }
            app_p[bytes_read] = (uint8_t)byte;
        }

        // Always resubmit work in asyncio mode to keep the background worker active.
        // Even if we read 0 bytes (ring buffer empty), we need the work handler
        // to keep trying to fill it from the DMIC.
        k_work_submit(&self->work);

        return bytes_read;
    }

    // Blocking mode - clear ring buffer for fresh data
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

// Stream ioctl for asyncio mode
static mp_uint_t mp_machine_pdm_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;

    // Calling ioctl indicates asyncio is being used
    if (self->io_mode == BLOCKING) {
        self->io_mode = ASYNCIO;
        // Start the device and background work immediately for asyncio.
        // The stream reader will poll first, and data needs to be flowing
        // for it to proceed to the read call.
        if (!self->is_started) {
            if (dmic_trigger(self->dev, DMIC_TRIGGER_START) == 0) {
                self->is_started = true;
                self->work_running = true;
                k_work_submit(&self->work);
            }
        }
    }

    if (request == MP_STREAM_POLL) {
        size_t available = ringbuf_avail(&self->ring_buffer);
        ret = 0;
        if ((arg & MP_STREAM_POLL_RD) && available > 0) {
            ret |= MP_STREAM_POLL_RD;  // Data available for reading
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }

    return ret;
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
