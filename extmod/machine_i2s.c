/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mike Teachman
 * Copyright (c) 2023 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_MACHINE_I2S

#include "extmod/modmachine.h"

#if MICROPY_PY_MACHINE_I2S_RING_BUF

typedef struct _ring_buf_t {
    uint8_t *buffer;
    size_t head;
    size_t tail;
    size_t size;
} ring_buf_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    uint32_t index;
    bool copy_in_progress;
} non_blocking_descriptor_t;

STATIC void ringbuf_init(ring_buf_t *rbuf, uint8_t *buffer, size_t size);
STATIC bool ringbuf_push(ring_buf_t *rbuf, uint8_t data);
STATIC bool ringbuf_pop(ring_buf_t *rbuf, uint8_t *data);
STATIC size_t ringbuf_available_data(ring_buf_t *rbuf);
STATIC size_t ringbuf_available_space(ring_buf_t *rbuf);
STATIC void fill_appbuf_from_ringbuf_non_blocking(machine_i2s_obj_t *self);
STATIC void copy_appbuf_to_ringbuf_non_blocking(machine_i2s_obj_t *self);

#endif // MICROPY_PY_MACHINE_I2S_RING_BUF

// The port must provide implementations of these low-level I2S functions.
STATIC void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
STATIC machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id);
STATIC void mp_machine_i2s_deinit(machine_i2s_obj_t *self);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_I2S_INCLUDEFILE

#if MICROPY_PY_MACHINE_I2S_RING_BUF

// Ring Buffer
// Thread safe when used with these constraints:
// - Single Producer, Single Consumer
// - Sequential atomic operations
// One byte of capacity is used to detect buffer empty/full

STATIC void ringbuf_init(ring_buf_t *rbuf, uint8_t *buffer, size_t size) {
    rbuf->buffer = buffer;
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

STATIC bool ringbuf_push(ring_buf_t *rbuf, uint8_t data) {
    size_t next_tail = (rbuf->tail + 1) % rbuf->size;

    if (next_tail != rbuf->head) {
        rbuf->buffer[rbuf->tail] = data;
        rbuf->tail = next_tail;
        return true;
    }

    // full
    return false;
}

STATIC bool ringbuf_pop(ring_buf_t *rbuf, uint8_t *data) {
    if (rbuf->head == rbuf->tail) {
        // empty
        return false;
    }

    *data = rbuf->buffer[rbuf->head];
    rbuf->head = (rbuf->head + 1) % rbuf->size;
    return true;
}

STATIC bool ringbuf_is_empty(ring_buf_t *rbuf) {
    return rbuf->head == rbuf->tail;
}

STATIC bool ringbuf_is_full(ring_buf_t *rbuf) {
    return ((rbuf->tail + 1) % rbuf->size) == rbuf->head;
}

STATIC size_t ringbuf_available_data(ring_buf_t *rbuf) {
    return (rbuf->tail - rbuf->head + rbuf->size) % rbuf->size;
}

STATIC size_t ringbuf_available_space(ring_buf_t *rbuf) {
    return rbuf->size - ringbuf_available_data(rbuf) - 1;
}

STATIC uint32_t fill_appbuf_from_ringbuf(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from the ring buffer to the app buffer
    // loop, copying samples until the app buffer is filled
    // For asyncio mode, the loop will make an early exit if the ring buffer becomes empty
    // Example:
    //   a MicroPython I2S object is configured for 16-bit mono (2 bytes per audio sample).
    //   For every frame coming from the ring buffer (8 bytes), 2 bytes are "cherry picked" and
    //   copied to the supplied app buffer.
    //   Thus, for every 1 byte copied to the app buffer, 4 bytes are read from the ring buffer.
    //   If a 8kB app buffer is supplied, 32kB of audio samples is read from the ring buffer.

    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = (uint8_t *)appbuf->buf;
    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_needed_from_ringbuf = appbuf->len * (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint8_t discard_byte;
    while (num_bytes_needed_from_ringbuf) {

        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

        for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++) {
            int8_t r_to_a_mapping = i2s_frame_map[f_index][i];
            if (r_to_a_mapping != -1) {
                if (self->io_mode == BLOCKING) {
                    // poll the ringbuf until a sample becomes available,  copy into appbuf using the mapping transform
                    while (ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping) == false) {
                        ;
                    }
                    num_bytes_copied_to_appbuf++;
                } else if (self->io_mode == ASYNCIO) {
                    if (ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping) == false) {
                        // ring buffer is empty, exit
                        goto exit;
                    } else {
                        num_bytes_copied_to_appbuf++;
                    }
                } else {
                    return 0;  // should never get here (non-blocking mode does not use this function)
                }
            } else { // r_a_mapping == -1
                // discard unused byte from ring buffer
                if (self->io_mode == BLOCKING) {
                    // poll the ringbuf until a sample becomes available
                    while (ringbuf_pop(&self->ring_buffer, &discard_byte) == false) {
                        ;
                    }
                } else if (self->io_mode == ASYNCIO) {
                    if (ringbuf_pop(&self->ring_buffer, &discard_byte) == false) {
                        // ring buffer is empty, exit
                        goto exit;
                    }
                } else {
                    return 0;  // should never get here (non-blocking mode does not use this function)
                }
            }
            num_bytes_needed_from_ringbuf--;
        }
        app_p += appbuf_sample_size_in_bytes;
    }
exit:
    return num_bytes_copied_to_appbuf;
}

// function is used in IRQ context
STATIC void fill_appbuf_from_ringbuf_non_blocking(machine_i2s_obj_t *self) {

    // attempt to copy a block of audio samples from the ring buffer to the supplied app buffer.
    // audio samples will be formatted as part of the copy operation

    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = &(((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index]);

    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 4) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_remaining_to_copy_to_appbuf = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t num_bytes_remaining_to_copy_from_ring_buffer = num_bytes_remaining_to_copy_to_appbuf *
        (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint32_t num_bytes_needed_from_ringbuf = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, num_bytes_remaining_to_copy_from_ring_buffer);
    uint8_t discard_byte;
    if (ringbuf_available_data(&self->ring_buffer) >= num_bytes_needed_from_ringbuf) {
        while (num_bytes_needed_from_ringbuf) {

            uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

            for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++) {
                int8_t r_to_a_mapping = i2s_frame_map[f_index][i];
                if (r_to_a_mapping != -1) {
                    ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping);
                    num_bytes_copied_to_appbuf++;
                } else { // r_a_mapping == -1
                    // discard unused byte from ring buffer
                    ringbuf_pop(&self->ring_buffer, &discard_byte);
                }
                num_bytes_needed_from_ringbuf--;
            }
            app_p += appbuf_sample_size_in_bytes;
        }
        self->non_blocking_descriptor.index += num_bytes_copied_to_appbuf;

        if (self->non_blocking_descriptor.index >= self->non_blocking_descriptor.appbuf.len) {
            self->non_blocking_descriptor.copy_in_progress = false;
            mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}

STATIC uint32_t copy_appbuf_to_ringbuf(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from the app buffer to the ring buffer
    // loop, reading samples until the app buffer is emptied
    // for asyncio mode, the loop will make an early exit if the ring buffer becomes full

    uint32_t a_index = 0;

    while (a_index < appbuf->len) {
        if (self->io_mode == BLOCKING) {
            // copy a byte to the ringbuf when space becomes available
            while (ringbuf_push(&self->ring_buffer, ((uint8_t *)appbuf->buf)[a_index]) == false) {
                ;
            }
            a_index++;
        } else if (self->io_mode == ASYNCIO) {
            if (ringbuf_push(&self->ring_buffer, ((uint8_t *)appbuf->buf)[a_index]) == false) {
                // ring buffer is full, exit
                break;
            } else {
                a_index++;
            }
        } else {
            return 0;  // should never get here (non-blocking mode does not use this function)
        }
    }

    return a_index;
}

// function is used in IRQ context
STATIC void copy_appbuf_to_ringbuf_non_blocking(machine_i2s_obj_t *self) {

    // copy audio samples from app buffer into ring buffer
    uint32_t num_bytes_remaining_to_copy = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t num_bytes_to_copy = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, num_bytes_remaining_to_copy);

    if (ringbuf_available_space(&self->ring_buffer) >= num_bytes_to_copy) {
        for (uint32_t i = 0; i < num_bytes_to_copy; i++) {
            ringbuf_push(&self->ring_buffer,
                ((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index + i]);
        }

        self->non_blocking_descriptor.index += num_bytes_to_copy;
        if (self->non_blocking_descriptor.index >= self->non_blocking_descriptor.appbuf.len) {
            self->non_blocking_descriptor.copy_in_progress = false;
            mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}

#endif // MICROPY_PY_MACHINE_I2S_RING_BUF

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t i2s_id = mp_obj_get_int(args[0]);

    machine_i2s_obj_t *self = mp_machine_i2s_make_new_instance(i2s_id);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    mp_machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// I2S.init(...)
STATIC mp_obj_t machine_i2s_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_machine_i2s_deinit(self);
    mp_machine_i2s_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_init);

// I2S.deinit()
STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i2s_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);

STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_i2s_irq_obj) },
    #if MICROPY_PY_MACHINE_I2S_FINALISER
    { MP_ROM_QSTR(MP_QSTR___del__),         MP_ROM_PTR(&machine_i2s_deinit_obj) },
    #endif

    // Static method
    { MP_ROM_QSTR(MP_QSTR_shift),           MP_ROM_PTR(&machine_i2s_shift_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_RX),              MP_ROM_INT(MICROPY_PY_MACHINE_I2S_CONSTANT_RX) },
    { MP_ROM_QSTR(MP_QSTR_TX),              MP_ROM_INT(MICROPY_PY_MACHINE_I2S_CONSTANT_TX) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);

STATIC mp_uint_t machine_i2s_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != MICROPY_PY_MACHINE_I2S_CONSTANT_RX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    uint8_t appbuf_sample_size_in_bytes = (self->bits / 8) * (self->format == STEREO ? 2: 1);
    if (size % appbuf_sample_size_in_bytes != 0) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == NON_BLOCKING) {
        #if MICROPY_PY_MACHINE_I2S_RING_BUF
        self->non_blocking_descriptor.appbuf.buf = (void *)buf_in;
        self->non_blocking_descriptor.appbuf.len = size;
        self->non_blocking_descriptor.index = 0;
        self->non_blocking_descriptor.copy_in_progress = true;
        #else
        non_blocking_descriptor_t descriptor;
        descriptor.appbuf.buf = (void *)buf_in;
        descriptor.appbuf.len = size;
        descriptor.callback = self->callback_for_non_blocking;
        descriptor.direction = I2S_RX_TRANSFER;
        // send the descriptor to the task that handles non-blocking mode
        xQueueSend(self->non_blocking_mode_queue, &descriptor, 0);
        #endif

        return size;
    } else { // blocking or asyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        #if MICROPY_PY_MACHINE_I2S_RING_BUF
        uint32_t num_bytes_read = fill_appbuf_from_ringbuf(self, &appbuf);
        #else
        uint32_t num_bytes_read = fill_appbuf_from_dma(self, &appbuf);
        #endif
        return num_bytes_read;
    }
}

STATIC mp_uint_t machine_i2s_stream_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == NON_BLOCKING) {
        #if MICROPY_PY_MACHINE_I2S_RING_BUF
        self->non_blocking_descriptor.appbuf.buf = (void *)buf_in;
        self->non_blocking_descriptor.appbuf.len = size;
        self->non_blocking_descriptor.index = 0;
        self->non_blocking_descriptor.copy_in_progress = true;
        #else
        non_blocking_descriptor_t descriptor;
        descriptor.appbuf.buf = (void *)buf_in;
        descriptor.appbuf.len = size;
        descriptor.callback = self->callback_for_non_blocking;
        descriptor.direction = I2S_TX_TRANSFER;
        // send the descriptor to the task that handles non-blocking mode
        xQueueSend(self->non_blocking_mode_queue, &descriptor, 0);
        #endif

        return size;
    } else { // blocking or asyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        #if MICROPY_PY_MACHINE_I2S_RING_BUF
        uint32_t num_bytes_written = copy_appbuf_to_ringbuf(self, &appbuf);
        #else
        uint32_t num_bytes_written = copy_appbuf_to_dma(self, &appbuf);
        #endif
        return num_bytes_written;
    }
}

STATIC mp_uint_t machine_i2s_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    uintptr_t flags = arg;
    self->io_mode = ASYNCIO; // a call to ioctl() is an indication that asyncio is being used

    if (request == MP_STREAM_POLL) {
        ret = 0;

        if (flags & MP_STREAM_POLL_RD) {
            if (self->mode != MICROPY_PY_MACHINE_I2S_CONSTANT_RX) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            #if MICROPY_PY_MACHINE_I2S_RING_BUF
            if (!ringbuf_is_empty(&self->ring_buffer)) {
                ret |= MP_STREAM_POLL_RD;
            }
            #else
            // check event queue to determine if a DMA buffer has been filled
            // (which is an indication that at least one DMA buffer is available to be read)
            // note:  timeout = 0 so the call is non-blocking
            i2s_event_t i2s_event;
            if (xQueueReceive(self->i2s_event_queue, &i2s_event, 0)) {
                if (i2s_event.type == I2S_EVENT_RX_DONE) {
                    // getting here means that at least one DMA buffer is now full
                    // indicating that audio samples can be read from the I2S object
                    ret |= MP_STREAM_POLL_RD;
                }
            }
            #endif
        }

        if (flags & MP_STREAM_POLL_WR) {
            if (self->mode != MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            #if MICROPY_PY_MACHINE_I2S_RING_BUF
            if (!ringbuf_is_full(&self->ring_buffer)) {
                ret |= MP_STREAM_POLL_WR;
            }
            #else
            // check event queue to determine if a DMA buffer has been emptied
            // (which is an indication that at least one DMA buffer is available to be written)
            // note:  timeout = 0 so the call is non-blocking
            i2s_event_t i2s_event;
            if (xQueueReceive(self->i2s_event_queue, &i2s_event, 0)) {
                if (i2s_event.type == I2S_EVENT_TX_DONE) {
                    // getting here means that at least one DMA buffer is now empty
                    // indicating that audio samples can be written to the I2S object
                    ret |= MP_STREAM_POLL_WR;
                }
            }
            #endif
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }

    return ret;
}

STATIC const mp_stream_p_t i2s_stream_p = {
    .read = machine_i2s_stream_read,
    .write = machine_i2s_stream_write,
    .ioctl = machine_i2s_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2s_type,
    MP_QSTR_I2S,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_i2s_make_new,
    print, machine_i2s_print,
    protocol, &i2s_stream_p,
    locals_dict, &machine_i2s_locals_dict
    );

#endif // MICROPY_PY_MACHINE_I2S
