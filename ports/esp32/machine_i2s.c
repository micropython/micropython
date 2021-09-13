/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mike Teachman
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "py/stream.h"
#include "py/objstr.h"
#include "modmachine.h"
#include "mphalport.h"

#if MICROPY_PY_MACHINE_I2S

#include "driver/i2s.h"
#include "soc/i2s_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task.h"

// The I2S module has 3 modes of operation:
//
// Mode1:  Blocking
// - readinto() and write() methods block until the supplied buffer is filled (read) or emptied (write)
// - this is the default mode of operation
//
// Mode2:  Non-Blocking
// - readinto() and write() methods return immediately.
// - buffer filling and emptying happens asynchronously to the main MicroPython task
// - a callback function is called when the supplied buffer has been filled (read) or emptied (write)
// - non-blocking mode is enabled when a callback is set with the irq() method
// - a FreeRTOS task is created to implement the asynchronous background operations
// - a FreeRTOS queue is used to transfer the supplied buffer to the background task
//
// Mode3: Uasyncio
// - implements the stream protocol
// - uasyncio mode is enabled when the ioctl() function is called
// - the I2S event queue is used to detect that I2S samples can be read or written from/to DMA memory
//
// The samples contained in the app buffer supplied for the readinto() and write() methods have the following convention:
//   Mono:  little endian format
//   Stereo:  little endian format, left channel first
//
// I2S terms:
//   "frame":  consists of two audio samples (Left audio sample + Right audio sample)
//
// Misc:
// - for Mono configuration:
//   - readinto method: samples are gathered from the L channel only
//   - write method: every sample is output to both the L and R channels
// - for readinto method the I2S hardware is read using 8-byte frames
//   (this is standard for almost all I2S hardware, such as MEMS microphones)
// - all sample data transfers use DMA

#define I2S_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define I2S_TASK_STACK_SIZE      (2048)

#define DMA_BUF_LEN_IN_I2S_FRAMES (256)

// The transform buffer is used with the readinto() method to bridge the opaque DMA memory on the ESP devices
// with the app buffer.  It facilitates audio sample transformations.  e.g.  32-bits samples to 16-bit samples.
// The size of 240 bytes is an engineering optimum that balances transfer performance with an acceptable use of heap space
#define SIZEOF_TRANSFORM_BUFFER_IN_BYTES (240)

#define NUM_I2S_USER_FORMATS (4)
#define I2S_RX_FRAME_SIZE_IN_BYTES (8)

typedef enum {
    MONO,
    STEREO
} format_t;

typedef enum {
    BLOCKING,
    NON_BLOCKING,
    UASYNCIO
} io_mode_t;

typedef enum {
    I2S_TX_TRANSFER,
    I2S_RX_TRANSFER,
} direction_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    mp_obj_t callback;
    direction_t direction;
} non_blocking_descriptor_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    i2s_port_t port;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    int8_t mode;
    i2s_bits_per_sample_t bits;
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    io_mode_t io_mode;
    uint8_t transform_buffer[SIZEOF_TRANSFORM_BUFFER_IN_BYTES];
    QueueHandle_t i2s_event_queue;
    QueueHandle_t non_blocking_mode_queue;
    TaskHandle_t non_blocking_mode_task;
} machine_i2s_obj_t;

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo, with the L and R channels reversed) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
STATIC const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    { 6,  7, -1, -1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 4,  5,  6,  7, -1, -1, -1, -1 },  // Mono, 32-bits
    { 6,  7,  2,  3, -1, -1, -1, -1 },  // Stereo, 16-bits
    { 4,  5,  6,  7,  0,  1,  2,  3 },  // Stereo, 32-bits
};

STATIC machine_i2s_obj_t *machine_i2s_obj[I2S_NUM_MAX];

void machine_i2s_init0() {
    for (i2s_port_t p = 0; p < I2S_NUM_MAX; p++) {
        machine_i2s_obj[p] = NULL;
    }
}

//  The following function takes a sample buffer and swaps L/R channels
//
//  Background:  For 32-bit stereo, the ESP-IDF API has a L/R channel orientation that breaks
//               convention with other ESP32 channel formats
//
//   appbuf[] = [L_0-7, L_8-15, L_16-23, L_24-31, R_0-7, R_8-15, R_16-23, R_24-31] = [Left channel, Right channel]
//   dma[] =    [R_0-7, R_8-15, R_16-23, R_24-31, L_0-7, L_8-15, L_16-23, L_24-31] = [Right channel, Left channel]
//
//   where:
//     L_0-7 is the least significant byte of the 32 bit sample in the Left channel
//     L_24-31 is the most significant byte of the 32 bit sample in the Left channel
//
//  Example:
//
//   appbuf[] = [0x99, 0xBB, 0x11, 0x22, 0x44, 0x55, 0xAB, 0x77] = [Left channel, Right channel]
//   dma[] =    [0x44, 0x55, 0xAB, 0x77, 0x99, 0xBB, 0x11, 0x22] = [Right channel,  Left channel]
//   where:
//      LEFT Channel =  0x99, 0xBB, 0x11, 0x22
//      RIGHT Channel = 0x44, 0x55, 0xAB, 0x77
//
//    samples in appbuf are in little endian format:
//       0x77 is the most significant byte of the 32-bit sample
//       0x44 is the least significant byte of the 32-bit sample
STATIC void swap_32_bit_stereo_channels(mp_buffer_info_t *bufinfo) {
    int32_t swap_sample;
    int32_t *sample = bufinfo->buf;
    uint32_t num_samples = bufinfo->len / 4;
    for (uint32_t i = 0; i < num_samples; i += 2) {
        swap_sample = sample[i + 1];
        sample[i + 1] = sample[i];
        sample[i] = swap_sample;
    }
}

STATIC int8_t get_frame_mapping_index(i2s_bits_per_sample_t bits, format_t format) {
    if (format == MONO) {
        if (bits == I2S_BITS_PER_SAMPLE_16BIT) {
            return 0;
        } else { // 32 bits
            return 1;
        }
    } else { // STEREO
        if (bits == I2S_BITS_PER_SAMPLE_16BIT) {
            return 2;
        } else { // 32 bits
            return 3;
        }
    }
}

STATIC i2s_bits_per_sample_t get_dma_bits(uint8_t mode, i2s_bits_per_sample_t bits) {
    if (mode == (I2S_MODE_MASTER | I2S_MODE_TX)) {
        return bits;
    } else { // Master Rx
        // read 32 bit samples for I2S hardware.  e.g. MEMS microphones
        return I2S_BITS_PER_SAMPLE_32BIT;
    }
}

STATIC i2s_channel_fmt_t get_dma_format(uint8_t mode, format_t format) {
    if (mode == (I2S_MODE_MASTER | I2S_MODE_TX)) {
        if (format == MONO) {
            return I2S_CHANNEL_FMT_ONLY_LEFT;
        } else {  // STEREO
            return I2S_CHANNEL_FMT_RIGHT_LEFT;
        }
    } else { // Master Rx
        // read stereo frames for all I2S hardware
        return I2S_CHANNEL_FMT_RIGHT_LEFT;
    }
}

STATIC uint32_t get_dma_buf_count(uint8_t mode, i2s_bits_per_sample_t bits, format_t format, int32_t ibuf) {
    // calculate how many DMA buffers need to be allocated
    uint32_t dma_frame_size_in_bytes =
        (get_dma_bits(mode, bits) / 8) * (get_dma_format(mode, format) == I2S_CHANNEL_FMT_RIGHT_LEFT ? 2: 1);

    uint32_t dma_buf_count = ibuf / (DMA_BUF_LEN_IN_I2S_FRAMES * dma_frame_size_in_bytes);

    return dma_buf_count;
}

STATIC uint32_t fill_appbuf_from_dma(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from DMA memory to the app buffer
    // audio samples are read from DMA memory in chunks
    // loop, reading and copying chunks until the app buffer is filled
    // For uasyncio mode, the loop will make an early exit if DMA memory becomes empty
    // Example:
    //   a MicroPython I2S object is configured for 16-bit mono (2 bytes per audio sample).
    //   For every frame coming from DMA (8 bytes), 2 bytes are "cherry picked" and
    //   copied to the supplied app buffer.
    //   Thus, for every 1 byte copied to the app buffer, 4 bytes are read from DMA memory.
    //   If a 8kB app buffer is supplied, 32kB of audio samples is read from DMA memory.

    uint32_t a_index = 0;
    uint8_t *app_p = appbuf->buf;
    uint8_t appbuf_sample_size_in_bytes = (self->bits / 8) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_needed_from_dma = appbuf->len * (I2S_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    while (num_bytes_needed_from_dma) {
        size_t num_bytes_requested_from_dma = MIN(sizeof(self->transform_buffer), num_bytes_needed_from_dma);
        size_t num_bytes_received_from_dma = 0;

        TickType_t delay;
        if (self->io_mode == UASYNCIO) {
            delay = 0; // stop i2s_read() operation if DMA memory becomes empty
        } else {
            delay = portMAX_DELAY;  // block until supplied buffer is filled
        }

        // read a chunk of audio samples from DMA memory
        check_esp_err(i2s_read(
            self->port,
            self->transform_buffer,
            num_bytes_requested_from_dma,
            &num_bytes_received_from_dma,
            delay));

        // process the transform buffer one frame at a time.
        // copy selected bytes from the transform buffer into the user supplied appbuf.
        // Example:
        //   a MicroPython I2S object is configured for 16-bit mono.  This configuration associates to
        //   a frame map index of 0 = { 6,  7, -1, -1, -1, -1, -1, -1 } in the i2s_frame_map array
        //   This mapping indicates:
        //      appbuf[x+0] = frame[6]
        //      appbuf[x+1] = frame[7]
        //      frame bytes 0-5 are not used

        uint32_t t_index = 0;
        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);
        while (t_index < num_bytes_received_from_dma) {
            uint8_t *transform_p = self->transform_buffer + t_index;

            for (uint8_t i = 0; i < I2S_RX_FRAME_SIZE_IN_BYTES; i++) {
                int8_t t_to_a_mapping = i2s_frame_map[f_index][i];
                if (t_to_a_mapping != -1) {
                    *app_p++ = transform_p[t_to_a_mapping];
                    a_index++;
                }
                t_index++;
            }
        }

        num_bytes_needed_from_dma -= num_bytes_received_from_dma;

        if ((self->io_mode == UASYNCIO) && (num_bytes_received_from_dma < num_bytes_requested_from_dma)) {
            // Unable to fill the entire app buffer from DMA memory.  This indicates all DMA RX buffers are empty.
            // Clear the I2S event queue so ioctl() indicates that the I2S object cannot currently
            // supply more audio samples
            xQueueReset(self->i2s_event_queue);
            break;
        }
    }

    return a_index;
}

STATIC size_t copy_appbuf_to_dma(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {
    if ((self->bits == I2S_BITS_PER_SAMPLE_32BIT) && (self->format == STEREO)) {
        swap_32_bit_stereo_channels(appbuf);
    }

    size_t num_bytes_written = 0;

    TickType_t delay;
    if (self->io_mode == UASYNCIO) {
        delay = 0;  // stop i2s_write() operation if DMA memory becomes full
    } else {
        delay = portMAX_DELAY;  // block until supplied buffer is emptied
    }

    check_esp_err(i2s_write(self->port, appbuf->buf, appbuf->len, &num_bytes_written, delay));

    if ((self->io_mode == UASYNCIO) && (num_bytes_written < appbuf->len)) {
        // Unable to empty the entire app buffer into DMA memory.  This indicates all DMA TX buffers are full.
        // Clear the I2S event queue so ioctl() indicates that the I2S object cannot currently
        // accept more audio samples
        xQueueReset(self->i2s_event_queue);

        // Undo the swap transformation as the buffer has not been completely emptied.
        // The uasyncio stream writer will use the same buffer in a future write call.
        if ((self->bits == I2S_BITS_PER_SAMPLE_32BIT) && (self->format == STEREO)) {
            swap_32_bit_stereo_channels(appbuf);
        }
    }
    return num_bytes_written;
}

// FreeRTOS task used for non-blocking mode
STATIC void task_for_non_blocking_mode(void *self_in) {
    machine_i2s_obj_t *self = (machine_i2s_obj_t *)self_in;

    non_blocking_descriptor_t descriptor;

    for (;;) {
        if (xQueueReceive(self->non_blocking_mode_queue, &descriptor, portMAX_DELAY)) {
            if (descriptor.direction == I2S_TX_TRANSFER) {
                copy_appbuf_to_dma(self, &descriptor.appbuf);
            } else { // RX
                fill_appbuf_from_dma(self, &descriptor.appbuf);
            }
            mp_sched_schedule(descriptor.callback, MP_OBJ_FROM_PTR(self));
        }
    }
}

STATIC void machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum {
        ARG_sck,
        ARG_ws,
        ARG_sd,
        ARG_mode,
        ARG_bits,
        ARG_format,
        ARG_rate,
        ARG_ibuf,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_ws,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sd,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_format,   MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_rate,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_ibuf,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //
    // ---- Check validity of arguments ----
    //

    // are Pins valid?
    int8_t sck = args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    int8_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_ws].u_obj);
    int8_t sd = args[ARG_sd].u_obj == MP_OBJ_NULL ? -1 : mp_hal_get_pin_obj(args[ARG_sd].u_obj);

    // is Mode valid?
    i2s_mode_t mode = args[ARG_mode].u_int;
    if ((mode != (I2S_MODE_MASTER | I2S_MODE_RX)) &&
        (mode != (I2S_MODE_MASTER | I2S_MODE_TX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // is Bits valid?
    i2s_bits_per_sample_t bits = args[ARG_bits].u_int;
    if ((bits != I2S_BITS_PER_SAMPLE_16BIT) &&
        (bits != I2S_BITS_PER_SAMPLE_32BIT)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }

    // is Format valid?
    format_t format = args[ARG_format].u_int;
    if ((format != STEREO) &&
        (format != MONO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    // is Rate valid?
    // Not checked:  ESP-IDF I2S API does not indicate a valid range for sample rate

    // is Ibuf valid?
    // Not checked: ESP-IDF I2S API will return error if requested buffer size exceeds available memory

    self->sck = sck;
    self->ws = ws;
    self->sd = sd;
    self->mode = mode;
    self->bits = bits;
    self->format = format;
    self->rate = args[ARG_rate].u_int;
    self->ibuf = args[ARG_ibuf].u_int;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->i2s_event_queue = NULL;
    self->non_blocking_mode_queue = NULL;
    self->non_blocking_mode_task = NULL;
    self->io_mode = BLOCKING;

    i2s_config_t i2s_config;
    i2s_config.communication_format = I2S_COMM_FORMAT_I2S;
    i2s_config.mode = mode;
    i2s_config.bits_per_sample = get_dma_bits(mode, bits);
    i2s_config.channel_format = get_dma_format(mode, format);
    i2s_config.sample_rate = self->rate;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LOWMED;
    i2s_config.dma_buf_count = get_dma_buf_count(mode, bits, format, self->ibuf);
    i2s_config.dma_buf_len = DMA_BUF_LEN_IN_I2S_FRAMES;
    i2s_config.use_apll = false;

    // I2S queue size equals the number of DMA buffers
    check_esp_err(i2s_driver_install(self->port, &i2s_config, i2s_config.dma_buf_count, &self->i2s_event_queue));

    // apply low-level workaround for bug in some ESP-IDF versions that swap
    // the left and right channels
    // https://github.com/espressif/esp-idf/issues/6625
    REG_SET_BIT(I2S_CONF_REG(self->port), I2S_TX_MSB_RIGHT);
    REG_SET_BIT(I2S_CONF_REG(self->port), I2S_RX_MSB_RIGHT);

    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = self->sck;
    pin_config.ws_io_num = self->ws;

    if (mode == (I2S_MODE_MASTER | I2S_MODE_RX)) {
        pin_config.data_in_num = self->sd;
        pin_config.data_out_num = -1;
    } else { // TX
        pin_config.data_in_num = -1;
        pin_config.data_out_num = self->sd;
    }

    check_esp_err(i2s_set_pin(self->port, &pin_config));
}

STATIC void machine_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2S(id=%u,\n"
        "sck="MP_HAL_PIN_FMT ",\n"
        "ws="MP_HAL_PIN_FMT ",\n"
        "sd="MP_HAL_PIN_FMT ",\n"
        "mode=%u,\n"
        "bits=%u, format=%u,\n"
        "rate=%d, ibuf=%d)",
        self->port,
        mp_hal_pin_name(self->sck),
        mp_hal_pin_name(self->ws),
        mp_hal_pin_name(self->sd),
        self->mode,
        self->bits, self->format,
        self->rate, self->ibuf
        );
}

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    i2s_port_t port = mp_obj_get_int(args[0]);
    if (port < 0 || port >= I2S_NUM_MAX) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self;
    if (machine_i2s_obj[port] == NULL) {
        self = m_new_obj(machine_i2s_obj_t);
        machine_i2s_obj[port] = self;
        self->base.type = &machine_i2s_type;
        self->port = port;
    } else {
        self = machine_i2s_obj[port];
        machine_i2s_deinit(self);
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2s_obj_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2s_obj_t *self = pos_args[0];
    machine_i2s_deinit(self);
    machine_i2s_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_obj_init);

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2s_driver_uninstall(self->port);

    if (self->non_blocking_mode_task != NULL) {
        vTaskDelete(self->non_blocking_mode_task);
        self->non_blocking_mode_task = NULL;
    }

    if (self->non_blocking_mode_queue != NULL) {
        vQueueDelete(self->non_blocking_mode_queue);
        self->non_blocking_mode_queue = NULL;
    }

    self->i2s_event_queue = NULL;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);

STATIC mp_obj_t machine_i2s_irq(mp_obj_t self_in, mp_obj_t handler) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }

    if (handler != mp_const_none) {
        self->io_mode = NON_BLOCKING;

        // create a queue linking the MicroPython task to a FreeRTOS task
        // that manages the non blocking mode of operation
        self->non_blocking_mode_queue = xQueueCreate(1, sizeof(non_blocking_descriptor_t));

        // non-blocking mode requires a background FreeRTOS task
        if (xTaskCreatePinnedToCore(
            task_for_non_blocking_mode,
            "i2s_non_blocking",
            I2S_TASK_STACK_SIZE,
            self,
            I2S_TASK_PRIORITY,
            (TaskHandle_t *)&self->non_blocking_mode_task,
            MP_TASK_COREID) != pdPASS) {

            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to create I2S task"));
        }
    } else {
        if (self->non_blocking_mode_task != NULL) {
            vTaskDelete(self->non_blocking_mode_task);
            self->non_blocking_mode_task = NULL;
        }

        if (self->non_blocking_mode_queue != NULL) {
            vQueueDelete(self->non_blocking_mode_queue);
            self->non_blocking_mode_queue = NULL;
        }

        self->io_mode = BLOCKING;
    }

    self->callback_for_non_blocking = handler;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_i2s_irq_obj, machine_i2s_irq);

// Shift() is typically used as a volume control.
// shift=1 increases volume by 6dB, shift=-1 decreases volume by 6dB
STATIC mp_obj_t machine_i2s_shift(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_bits, ARG_shift};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bits,   MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_shift,  MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_RW);

    int16_t *buf_16 = bufinfo.buf;
    int32_t *buf_32 = bufinfo.buf;

    uint8_t bits = args[ARG_bits].u_int;
    int8_t shift = args[ARG_shift].u_int;

    uint32_t num_audio_samples;
    switch (bits) {
        case 16:
            num_audio_samples = bufinfo.len / 2;
            break;

        case 32:
            num_audio_samples = bufinfo.len / 4;
            break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
            break;
    }

    for (uint32_t i = 0; i < num_audio_samples; i++) {
        switch (bits) {
            case 16:
                if (shift >= 0) {
                    buf_16[i] = buf_16[i] << shift;
                } else {
                    buf_16[i] = buf_16[i] >> abs(shift);
                }
                break;
            case 32:
                if (shift >= 0) {
                    buf_32[i] = buf_32[i] << shift;
                } else {
                    buf_32[i] = buf_32[i] >> abs(shift);
                }
                break;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_shift_fun_obj, 0, machine_i2s_shift);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(machine_i2s_shift_obj, MP_ROM_PTR(&machine_i2s_shift_fun_obj));

STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_i2s_irq_obj) },

    // Static method
    { MP_ROM_QSTR(MP_QSTR_shift),           MP_ROM_PTR(&machine_i2s_shift_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_RX),              MP_ROM_INT(I2S_MODE_MASTER | I2S_MODE_RX) },
    { MP_ROM_QSTR(MP_QSTR_TX),              MP_ROM_INT(I2S_MODE_MASTER | I2S_MODE_TX) },
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO),            MP_ROM_INT(MONO) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);

STATIC mp_uint_t machine_i2s_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != (I2S_MODE_MASTER | I2S_MODE_RX)) {
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
        non_blocking_descriptor_t descriptor;
        descriptor.appbuf.buf = (void *)buf_in;
        descriptor.appbuf.len = size;
        descriptor.callback = self->callback_for_non_blocking;
        descriptor.direction = I2S_RX_TRANSFER;
        // send the descriptor to the task that handles non-blocking mode
        xQueueSend(self->non_blocking_mode_queue, &descriptor, 0);
        return size;
    } else { // blocking or uasyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        uint32_t num_bytes_read = fill_appbuf_from_dma(self, &appbuf);
        return num_bytes_read;
    }
}

STATIC mp_uint_t machine_i2s_stream_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->mode != (I2S_MODE_MASTER | I2S_MODE_TX)) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    if (size == 0) {
        return 0;
    }

    if (self->io_mode == NON_BLOCKING) {
        non_blocking_descriptor_t descriptor;
        descriptor.appbuf.buf = (void *)buf_in;
        descriptor.appbuf.len = size;
        descriptor.callback = self->callback_for_non_blocking;
        descriptor.direction = I2S_TX_TRANSFER;
        // send the descriptor to the task that handles non-blocking mode
        xQueueSend(self->non_blocking_mode_queue, &descriptor, 0);
        return size;
    } else { // blocking or uasyncio mode
        mp_buffer_info_t appbuf;
        appbuf.buf = (void *)buf_in;
        appbuf.len = size;
        size_t num_bytes_written = copy_appbuf_to_dma(self, &appbuf);
        return num_bytes_written;
    }
}

STATIC mp_uint_t machine_i2s_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    mp_uint_t flags = arg;
    self->io_mode = UASYNCIO; // a call to ioctl() is an indication that uasyncio is being used

    if (request == MP_STREAM_POLL) {
        ret = 0;

        if (flags & MP_STREAM_POLL_RD) {
            if (self->mode != (I2S_MODE_MASTER | I2S_MODE_RX)) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            i2s_event_t i2s_event;

            // check event queue to determine if a DMA buffer has been filled
            // (which is an indication that at least one DMA buffer is available to be read)
            // note:  timeout = 0 so the call is non-blocking
            if (xQueueReceive(self->i2s_event_queue, &i2s_event, 0)) {
                if (i2s_event.type == I2S_EVENT_RX_DONE) {
                    // getting here means that at least one DMA buffer is now full
                    // indicating that audio samples can be read from the I2S object
                    ret |= MP_STREAM_POLL_RD;
                }
            }
        }

        if (flags & MP_STREAM_POLL_WR) {
            if (self->mode != (I2S_MODE_MASTER | I2S_MODE_TX)) {
                *errcode = MP_EPERM;
                return MP_STREAM_ERROR;
            }

            i2s_event_t i2s_event;

            // check event queue to determine if a DMA buffer has been emptied
            // (which is an indication that at least one DMA buffer is available to be written)
            // note:  timeout = 0 so the call is non-blocking
            if (xQueueReceive(self->i2s_event_queue, &i2s_event, 0)) {
                if (i2s_event.type == I2S_EVENT_TX_DONE) {
                    // getting here means that at least one DMA buffer is now empty
                    // indicating that audio samples can be written to the I2S object
                    ret |= MP_STREAM_POLL_WR;
                }
            }
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

const mp_obj_type_t machine_i2s_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2S,
    .print = machine_i2s_print,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &i2s_stream_p,
    .make_new = machine_i2s_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_i2s_locals_dict,
};

#endif // MICROPY_PY_MACHINE_I2S
