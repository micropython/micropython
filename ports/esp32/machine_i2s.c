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

// This file is never compiled standalone, it's included directly from
// extmod/machine_i2s.c via MICROPY_PY_MACHINE_I2S_INCLUDEFILE.

#include "py/mphal.h"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task.h"

// Notes on this port's specific implementation of I2S:
// - a FreeRTOS task is created to implement the asynchronous background operations
// - a FreeRTOS queue is used to transfer the supplied buffer to the background task
// - all sample data transfers use DMA

#define I2S_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define I2S_TASK_STACK_SIZE      (2048)

#define DMA_BUF_LEN_IN_I2S_FRAMES (256)

// The transform buffer is used with the readinto() method to bridge the opaque DMA memory on the ESP devices
// with the app buffer.  It facilitates audio sample transformations.  e.g.  32-bits samples to 16-bit samples.
// The size of 240 bytes is an engineering optimum that balances transfer performance with an acceptable use of heap space
#define SIZEOF_TRANSFORM_BUFFER_IN_BYTES (240)

typedef enum {
    I2S_TX_TRANSFER,
    I2S_RX_TRANSFER,
} direction_t;

typedef struct _non_blocking_descriptor_t {
    mp_buffer_info_t appbuf;
    mp_obj_t callback;
    direction_t direction;
} non_blocking_descriptor_t;

typedef enum {
    DMA_MEMORY_FULL,
    DMA_MEMORY_NOT_FULL,
    DMA_MEMORY_EMPTY,
    DMA_MEMORY_NOT_EMPTY,
} dma_buffer_status_t;

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    i2s_port_t i2s_id;
    i2s_chan_handle_t i2s_chan_handle;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t ws;
    mp_hal_pin_obj_t sd;
    i2s_dir_t mode;
    i2s_data_bit_width_t bits;
    format_t format;
    int32_t rate;
    int32_t ibuf;
    mp_obj_t callback_for_non_blocking;
    io_mode_t io_mode;
    bool is_deinit;
    uint8_t transform_buffer[SIZEOF_TRANSFORM_BUFFER_IN_BYTES];
    QueueHandle_t non_blocking_mode_queue;
    TaskHandle_t non_blocking_mode_task;
    dma_buffer_status_t dma_buffer_status;
} machine_i2s_obj_t;

static mp_obj_t machine_i2s_deinit(mp_obj_t self_in);

// The frame map is used with the readinto() method to transform the audio sample data coming
// from DMA memory (32-bit stereo, with the L and R channels reversed) to the format specified
// in the I2S constructor.  e.g.  16-bit mono
static const int8_t i2s_frame_map[NUM_I2S_USER_FORMATS][I2S_RX_FRAME_SIZE_IN_BYTES] = {
    { 2,  3, -1, -1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    { 2,  3,  6,  7, -1, -1, -1, -1 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

void machine_i2s_init0() {
    for (i2s_port_t p = 0; p < I2S_NUM_AUTO; p++) {
        MP_STATE_PORT(machine_i2s_obj)[p] = NULL;
    }
}

static int8_t get_frame_mapping_index(i2s_data_bit_width_t bits, format_t format) {
    if (format == MONO) {
        if (bits == I2S_DATA_BIT_WIDTH_16BIT) {
            return 0;
        } else { // 32 bits
            return 1;
        }
    } else { // STEREO
        if (bits == I2S_DATA_BIT_WIDTH_16BIT) {
            return 2;
        } else { // 32 bits
            return 3;
        }
    }
}

static i2s_data_bit_width_t get_dma_bits(uint8_t mode, i2s_data_bit_width_t bits) {
    if (mode == MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        return bits;
    } else { // Master Rx
        // read 32 bit samples for I2S hardware.  e.g. MEMS microphones
        return I2S_DATA_BIT_WIDTH_32BIT;
    }
}

static i2s_slot_mode_t get_dma_format(uint8_t mode, format_t format) {
    if (mode == MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        if (format == MONO) {
            return I2S_SLOT_MODE_MONO;
        } else {  // STEREO
            return I2S_SLOT_MODE_STEREO;
        }
    } else { // Master Rx
        // read stereo frames for all I2S hardware
        return I2S_SLOT_MODE_STEREO;
    }
}

static uint32_t get_dma_buf_count(uint8_t mode, i2s_data_bit_width_t bits, format_t format, int32_t ibuf) {
    // calculate how many DMA buffers need to be allocated
    uint32_t dma_frame_size_in_bytes =
        (get_dma_bits(mode, bits) / 8) * (get_dma_format(mode, format) == I2S_SLOT_MODE_STEREO ? 2: 1);

    uint32_t dma_buf_count = ibuf / (DMA_BUF_LEN_IN_I2S_FRAMES * dma_frame_size_in_bytes);

    return MAX(dma_buf_count, 2); // ESP-IDF requires at least two DMA buffers
}

static uint32_t fill_appbuf_from_dma(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {

    // copy audio samples from DMA memory to the app buffer
    // audio samples are read from DMA memory in chunks
    // loop, reading and copying chunks until the app buffer is filled
    // For asyncio mode, the loop will make an early exit if DMA memory becomes empty
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
        if (self->io_mode == ASYNCIO) {
            delay = 0; // stop i2s_channel_read() operation if DMA memory becomes empty
        } else {
            delay = portMAX_DELAY;  // block until supplied buffer is filled
        }

        esp_err_t ret = i2s_channel_read(
            self->i2s_chan_handle,
            self->transform_buffer,
            num_bytes_requested_from_dma,
            &num_bytes_received_from_dma,
            delay);

        // i2s_channel_read returns ESP_ERR_TIMEOUT when buffer cannot be filled by the timeout delay.
        if ((self->io_mode != ASYNCIO) ||
            ((self->io_mode == ASYNCIO) && (ret != ESP_ERR_TIMEOUT))) {
            check_esp_err(ret);
        }

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

        if ((self->io_mode == ASYNCIO) && (num_bytes_received_from_dma < num_bytes_requested_from_dma)) {
            // Unable to fill the entire app buffer from DMA memory.  This indicates all DMA RX buffers are empty.
            self->dma_buffer_status = DMA_MEMORY_EMPTY;
            break;
        }
    }

    return a_index;
}

static size_t copy_appbuf_to_dma(machine_i2s_obj_t *self, mp_buffer_info_t *appbuf) {
    size_t num_bytes_written = 0;

    TickType_t delay;
    if (self->io_mode == ASYNCIO) {
        delay = 0;  // stop i2s_channel_write() operation if DMA memory becomes full
    } else {
        delay = portMAX_DELAY;  // block until supplied buffer is emptied
    }

    esp_err_t ret = i2s_channel_write(self->i2s_chan_handle, appbuf->buf, appbuf->len, &num_bytes_written, delay);

    // i2s_channel_write returns ESP_ERR_TIMEOUT when buffer cannot be emptied by the timeout delay.
    if ((self->io_mode != ASYNCIO) ||
        ((self->io_mode == ASYNCIO) && (ret != ESP_ERR_TIMEOUT))) {
        check_esp_err(ret);
    }

    if ((self->io_mode == ASYNCIO) && (num_bytes_written < appbuf->len)) {
        // Unable to empty the entire app buffer into DMA memory.  This indicates all DMA TX buffers are full.
        self->dma_buffer_status = DMA_MEMORY_FULL;
    }

    return num_bytes_written;
}

// FreeRTOS task used for non-blocking mode
static void task_for_non_blocking_mode(void *self_in) {
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

// callback indicating that a DMA buffer was just filled with samples received from an I2S port
static IRAM_ATTR bool i2s_rx_recv_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *self_in) {
    machine_i2s_obj_t *self = (machine_i2s_obj_t *)self_in;
    self->dma_buffer_status = DMA_MEMORY_NOT_EMPTY;
    return false;
}

// callback indicating that samples in a DMA buffer were just transmitted to an I2S port
static IRAM_ATTR bool i2s_tx_sent_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *self_in) {
    machine_i2s_obj_t *self = (machine_i2s_obj_t *)self_in;
    self->dma_buffer_status = DMA_MEMORY_NOT_FULL;
    return false;
}

i2s_event_callbacks_t i2s_callbacks = {
    .on_recv = i2s_rx_recv_callback,
    .on_recv_q_ovf = NULL,
    .on_sent = i2s_tx_sent_callback,
    .on_send_q_ovf = NULL,
};

i2s_event_callbacks_t i2s_callbacks_null = {
    .on_recv = NULL,
    .on_recv_q_ovf = NULL,
    .on_sent = NULL,
    .on_send_q_ovf = NULL,
};

static void mp_machine_i2s_init_helper(machine_i2s_obj_t *self, mp_arg_val_t *args) {
    // are Pins valid?
    int8_t sck = args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_sck].u_obj);
    int8_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_ws].u_obj);
    int8_t sd = args[ARG_sd].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_sd].u_obj);

    // is Mode valid?
    int8_t mode = args[ARG_mode].u_int;
    if ((mode != (MICROPY_PY_MACHINE_I2S_CONSTANT_RX)) &&
        (mode != (MICROPY_PY_MACHINE_I2S_CONSTANT_TX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    // is Bits valid?
    i2s_data_bit_width_t bits = args[ARG_bits].u_int;
    if ((bits != I2S_DATA_BIT_WIDTH_16BIT) &&
        (bits != I2S_DATA_BIT_WIDTH_32BIT)) {
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
    self->non_blocking_mode_queue = NULL;
    self->non_blocking_mode_task = NULL;
    self->io_mode = BLOCKING;
    self->is_deinit = false;

    if (mode == MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        self->dma_buffer_status = DMA_MEMORY_NOT_FULL;
    } else { // rx
        self->dma_buffer_status = DMA_MEMORY_NOT_EMPTY;
    }

    i2s_chan_config_t chan_config = I2S_CHANNEL_DEFAULT_CONFIG(self->i2s_id, I2S_ROLE_MASTER);
    chan_config.dma_desc_num = get_dma_buf_count(mode, bits, format, self->ibuf);
    chan_config.dma_frame_num = DMA_BUF_LEN_IN_I2S_FRAMES;
    chan_config.auto_clear = true;

    if (mode == MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        check_esp_err(i2s_new_channel(&chan_config, &self->i2s_chan_handle, NULL));
    } else { // rx
        check_esp_err(i2s_new_channel(&chan_config, NULL, &self->i2s_chan_handle));
    }

    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(get_dma_bits(mode, bits), get_dma_format(mode, format));
    slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(self->rate),
        .slot_cfg = slot_cfg,
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = self->sck,
            .ws = self->ws,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    if (mode == MICROPY_PY_MACHINE_I2S_CONSTANT_TX) {
        std_cfg.gpio_cfg.dout = self->sd;
        std_cfg.gpio_cfg.din = I2S_GPIO_UNUSED;
    } else { // rx
        std_cfg.gpio_cfg.dout = I2S_GPIO_UNUSED;
        std_cfg.gpio_cfg.din = self->sd;
    }

    check_esp_err(i2s_channel_init_std_mode(self->i2s_chan_handle, &std_cfg));
    check_esp_err(i2s_channel_register_event_callback(self->i2s_chan_handle, &i2s_callbacks, self));
    check_esp_err(i2s_channel_enable(self->i2s_chan_handle));
}

static machine_i2s_obj_t *mp_machine_i2s_make_new_instance(mp_int_t i2s_id) {
    if (i2s_id < 0 || i2s_id >= I2S_NUM_AUTO) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid id"));
    }

    machine_i2s_obj_t *self;
    if (MP_STATE_PORT(machine_i2s_obj)[i2s_id] == NULL) {
        self = mp_obj_malloc_with_finaliser(machine_i2s_obj_t, &machine_i2s_type);
        MP_STATE_PORT(machine_i2s_obj)[i2s_id] = self;
        self->i2s_id = i2s_id;
    } else {
        self = MP_STATE_PORT(machine_i2s_obj)[i2s_id];
        machine_i2s_deinit(self);
    }

    return self;
}

static void mp_machine_i2s_deinit(machine_i2s_obj_t *self) {
    if (!self->is_deinit) {
        if (self->i2s_chan_handle) {
            i2s_channel_disable(self->i2s_chan_handle);
            i2s_channel_register_event_callback(self->i2s_chan_handle, &i2s_callbacks_null, self);
            i2s_del_channel(self->i2s_chan_handle);
            self->i2s_chan_handle = NULL;
        }

        if (self->non_blocking_mode_task != NULL) {
            vTaskDelete(self->non_blocking_mode_task);
            self->non_blocking_mode_task = NULL;
        }

        if (self->non_blocking_mode_queue != NULL) {
            vQueueDelete(self->non_blocking_mode_queue);
            self->non_blocking_mode_queue = NULL;
        }
        self->is_deinit = true;
    }
}

static void mp_machine_i2s_irq_update(machine_i2s_obj_t *self) {
    if (self->io_mode == NON_BLOCKING) {
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
    }
}

MP_REGISTER_ROOT_POINTER(struct _machine_i2s_obj_t *machine_i2s_obj[SOC_I2S_NUM]);
