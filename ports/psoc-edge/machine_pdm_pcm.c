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

#include <stdlib.h>
#include <math.h>
#include "cy_pdl.h"
#include "cybsp.h"

#include "sys_int.h"

#include "extmod/vfs.h"
#include "py/stream.h"
#include "py/runtime.h"


#define pdm_pcm_assert_raise_val(msg, ret)   if (ret != CY_PDM_PCM_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/* -------------------------------- Rx FIFO Ping-Pong Buffer -------------------------------- */

/**
 * Number of FIFO frames that will be stored
 * in each half of the ping-pong buffer.
 */
#define NUM_OF_RX_HW_FIFO_FRAMES_IN_RX_BUFFER         16
#define NUM_OF_RX_HW_FIFO_FRAMES_IN_HALF_RX_BUFFER    (NUM_OF_RX_HW_FIFO_FRAMES_IN_RX_BUFFER / 2)

/**
 * The PDM PCM peripheral has hardware FIFO capable
 * of up to 64 samples.
 * The IRQ trigger level is set to half of the FIFO size.
 * That way we can process the samples when the FIFO is
 * half full, ensuring that we read the samples before the
 * FIFO is full and we lose data.
 * Each half size FIFO read will compose a frame that will
 * be added to the ping-pong buffer.
 * As each sample is considered to be 8 bytes,
 * a frame match the actual size of the hardware FIFO for
 * a mono channel, or 2 halves FIFO for stereo.
 */
#define MAX_SAMPLES_RX_HW_FIFO                 64
#define NUM_OF_SAMPLES_IN_RX_HW_FIFO_FRAME     32
#define NUM_OF_WORDS_IN_RX_HW_FIFO_FRAME       (NUM_OF_SAMPLES_IN_RX_HW_FIFO_FRAME * 2)
#define RX_HW_FIFO_IRQ_TRIGGER_LEVEL           (NUM_OF_SAMPLES_IN_RX_HW_FIFO_FRAME)

/**
 * A ping-pong buffer implementation will store the samples read
 * from the PDM PCM hardware FIFO.
 *
 * Each sample will contain up to 8 bytes of data, being able
 * to store up to 32-bit stereo samples.
 * As the buffer is a uint32_t array, each sample will take
 * 2 positions in the buffer.
 * In case of mono audio, the odd positions will be empty.
 *
 * The buffer is split into two halves.
 * The active half will be the one used in the IRQ handler to
 * store the incoming samples from the FIFO.
 * The processing half will be used to copy the samples
 * into the ring buffer.
 *
 * The ring buffer will be used to transfer the data stream
 * into the application provided buffer.
 */
#define SIZEOF_RX_BUFFER_IN_SAMPLES            (NUM_OF_RX_HW_FIFO_FRAMES_IN_RX_BUFFER * NUM_OF_SAMPLES_IN_RX_HW_FIFO_FRAME)
#define SIZEOF_RX_BUFFER_IN_WORDS              (SIZEOF_RX_BUFFER_IN_SAMPLES * 2)
#define SIZEOF_HALF_RX_BUFFER_IN_SAMPLES       (SIZEOF_RX_BUFFER_IN_SAMPLES / 2)
#define SIZEOF_HALF_RX_BUFFER_IN_WORDS         (SIZEOF_HALF_RX_BUFFER_IN_SAMPLES * 2)

#define SIZEOF_PDM_PCM_SAMPLE_IN_BYTES          8

typedef struct _pcm_pdm_rx_fifo_ping_pong_buffer_t {
    uint32_t rx_fifo_buffer[SIZEOF_RX_BUFFER_IN_WORDS];
    uint32_t *active_half_rx_fifo_buf_ptr;
    uint32_t *processing_half_rx_fifo_buf_ptr;
    volatile uint8_t rx_hw_fifo_frame_count;
} pcm_pdm_rx_fifo_ping_pong_buffer_t;

static void pdm_pcm_rx_buffer_init(pcm_pdm_rx_fifo_ping_pong_buffer_t *self) {
    memset(self->rx_fifo_buffer, 0, sizeof(self->rx_fifo_buffer));
    self->active_half_rx_fifo_buf_ptr = self->rx_fifo_buffer;
    self->processing_half_rx_fifo_buf_ptr = &self->rx_fifo_buffer[SIZEOF_HALF_RX_BUFFER_IN_WORDS];
    self->rx_hw_fifo_frame_count = 0;
}

static inline bool pdm_pcm_rx_buffer_is_half_full(pcm_pdm_rx_fifo_ping_pong_buffer_t *self) {
    return self->rx_hw_fifo_frame_count == 0;
}

static void _pdm_pcm_rx_buffer_update_hw_fifo_frame_counter(pcm_pdm_rx_fifo_ping_pong_buffer_t *self) {
    self->rx_hw_fifo_frame_count++;
    if (self->rx_hw_fifo_frame_count == NUM_OF_RX_HW_FIFO_FRAMES_IN_HALF_RX_BUFFER) {
        self->rx_hw_fifo_frame_count = 0;
    }
}

static void _pdm_pcm_rx_buffer_swap_halves(pcm_pdm_rx_fifo_ping_pong_buffer_t *self) {
    if (pdm_pcm_rx_buffer_is_half_full(self)) {
        uint32_t *temp = self->active_half_rx_fifo_buf_ptr;
        self->active_half_rx_fifo_buf_ptr = self->processing_half_rx_fifo_buf_ptr;
        self->processing_half_rx_fifo_buf_ptr = temp;
    }
}

static inline uint32_t *pdm_pcm_rx_buffer_get_next_frame_to_fill(pcm_pdm_rx_fifo_ping_pong_buffer_t *self) {
    uint32_t *next_writable_frame_segment = &self->active_half_rx_fifo_buf_ptr[NUM_OF_WORDS_IN_RX_HW_FIFO_FRAME * self->rx_hw_fifo_frame_count];
    _pdm_pcm_rx_buffer_update_hw_fifo_frame_counter(self);
    _pdm_pcm_rx_buffer_swap_halves(self);
    return next_writable_frame_segment;
}

/* -------------------------------- PDM Block -------------------------------- */

typedef struct _pdm_pcm_block_obj_t {
    uint8_t id;
    PDM_Type *periph;
    bool inited;
} pdm_pcm_block_obj_t;

/**
 * For the current PSE8x family there is only one PDM block.
 */
static pdm_pcm_block_obj_t pdm_pcm_block_obj[1] = { { 0, PDM0, false } };

static void pdm_pcm_block_init(pdm_pcm_block_obj_t *block) {
    cy_stc_pdm_pcm_config_v2_t pdm_pcm_block_conf = {
        .clkDiv = 7,
        .clksel = CY_PDM_PCM_SEL_SRSS_CLOCK,
        .halverate = CY_PDM_PCM_RATE_FULL,
        .route = 4,
        .fir0_coeff_user_value = false,
        .fir1_coeff_user_value = false,
    };

    cy_en_pdm_pcm_status_t ret = Cy_PDM_PCM_Init(block->periph, &pdm_pcm_block_conf);
    pdm_pcm_assert_raise_val("PDM PCM initialization failed with code: %d \r\n", ret);
    block->inited = true;
}

static inline bool pdm_pcm_block_is_inited(pdm_pcm_block_obj_t *block) {
    return block->inited;
}

static inline void pdm_pcm_block_deinit(pdm_pcm_block_obj_t *block) {
    Cy_PDM_PCM_DeInit(block->periph);
    block->inited = false;
}

/* -------------------------------- PDM Channel -------------------------------- */

#define MICROPY_PY_MACHINE_FOR_ALL_PDM_PCM(DO) \
    DO(0) \
    DO(1) \
    DO(2) \
    DO(3) \
    DO(4) \
    DO(5)

#define MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES 6

typedef struct _pdm_pcm_channel_obj_t {
    pdm_pcm_block_obj_t *block;
    uint8_t id;
    sys_int_cfg_t irq;
    mp_obj_t parent;
} pdm_pcm_channel_obj_t;

/* Forward declaration */
static void pdm_pcm_channel_irq_handler(uint8_t pdm_pcm_channel);

#define DEFINE_PDM_PCM_CHANNEL_IRQ_HANDLER(pdm_pcm_channel) \
    void PDM_PCM##pdm_pcm_channel##_Channel_IRQ_Handler(void) { \
        pdm_pcm_channel_irq_handler(pdm_pcm_channel); \
    }

MICROPY_PY_MACHINE_FOR_ALL_PDM_PCM(DEFINE_PDM_PCM_CHANNEL_IRQ_HANDLER)

#define MAP_PDM_PCM_IRQ_CONFIG(pdm_pcm_channel) \
    [pdm_pcm_channel] = { \
        &pdm_pcm_block_obj[0], \
        pdm_pcm_channel, \
        { \
            pdm_0_interrupts_##pdm_pcm_channel##_IRQn, \
            SYS_INT_IRQ_LOWEST_PRIORITY, \
            PDM_PCM##pdm_pcm_channel##_Channel_IRQ_Handler \
        }, \
        NULL, \
    },

static pdm_pcm_channel_obj_t pdm_pcm_channel_obj[MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES] = {
    MICROPY_PY_MACHINE_FOR_ALL_PDM_PCM(MAP_PDM_PCM_IRQ_CONFIG)
};

static pdm_pcm_channel_obj_t *pdm_pcm_channel_alloc(uint8_t channel_id, mp_obj_t parent) {
    if (pdm_pcm_channel_obj[channel_id].parent != NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PDM0 channel %u is already in use by another instance."), channel_id);
    }
    pdm_pcm_channel_obj[channel_id].parent = parent;
    return &pdm_pcm_channel_obj[channel_id];
}

static void pdm_pcm_channel_free(pdm_pcm_channel_obj_t *chan_obj) {
    if (chan_obj->id >= MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid channel id"));
    }
    chan_obj->parent = NULL;
}

static void pdm_pcm_channel_init(pdm_pcm_channel_obj_t *chan_obj, sample_rate_t sample_rate, pdm_pcm_word_length_t word_length, bool second_channel) {
    if (!pdm_pcm_block_is_inited(chan_obj->block)) {
        pdm_pcm_block_init(chan_obj->block);
    }

    Cy_PDM_PCM_Channel_Enable(chan_obj->block->periph, chan_obj->id);

    /**
     * Configuration notes:
     * - For stereo channels, the second channel should
     *   set a sample delay. See more in the
     *   PSOC™ Edge E8x2, E8x3, E8x5, E8x6 architecture reference manual Rev. *A
     *   Section 29.3.3, page 773.
     * - TODO: Filter configuration base on sample_rate and PDM0 clock config.
     */
    cy_stc_pdm_pcm_channel_config_t channel_config = {
        .sampledelay = (second_channel) ? 5: 1,
        .wordSize = (word_length == BITS_16) ? CY_PDM_PCM_WSIZE_16_BIT : CY_PDM_PCM_WSIZE_32_BIT,
        .signExtension = true,
        .rxFifoTriggerLevel = RX_HW_FIFO_IRQ_TRIGGER_LEVEL - 1,
        .fir0_enable = false,
        .cic_decim_code = CY_PDM_PCM_CHAN_CIC_DECIM_32,
        .fir0_decim_code = CY_PDM_PCM_CHAN_FIR0_DECIM_1,
        .fir0_scale = 0,
        .fir1_decim_code = CY_PDM_PCM_CHAN_FIR1_DECIM_3,
        .fir1_scale = 10,
        .dc_block_disable = false,
        .dc_block_code = CY_PDM_PCM_CHAN_DCBLOCK_CODE_16,
    };

    cy_en_pdm_pcm_status_t ret = Cy_PDM_PCM_Channel_Init(chan_obj->block->periph, &channel_config, chan_obj->id);
    pdm_pcm_assert_raise_val("PDM PCM channel initialization failed with code: %d \r\n", ret);
}

static void pdm_pcm_channel_deinit(pdm_pcm_channel_obj_t *chan_obj) {
    Cy_PDM_PCM_Channel_DeInit(chan_obj->block->periph, chan_obj->id);
    Cy_PDM_PCM_Channel_Disable(chan_obj->block->periph, chan_obj->id);
}

static void pdm_pcm_channel_irq_init(pdm_pcm_channel_obj_t *chan_obj) {
    Cy_PDM_PCM_Channel_ClearInterrupt(chan_obj->block->periph, chan_obj->id, CY_PDM_PCM_INTR_MASK);
    Cy_PDM_PCM_Channel_SetInterruptMask(chan_obj->block->periph, chan_obj->id, CY_PDM_PCM_INTR_MASK);

    sys_int_init(&(chan_obj->irq));
}

static void pdm_pcm_channel_irq_deinit(pdm_pcm_channel_obj_t *chan_obj) {
    sys_int_deinit(&(chan_obj->irq));
    Cy_PDM_PCM_Channel_ClearInterrupt(chan_obj->block->periph, chan_obj->id, CY_PDM_PCM_INTR_MASK);
}

/* -------------------------------- PDM PCM Object -------------------------------- */

typedef struct _machine_pdm_pcm_obj_t {
    mp_obj_base_t base;
    uint8_t id;     // Private variable in this port. ID not associated to any port pin pdm-pcm group.
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t data;
    io_mode_t io_mode;
    format_t format;
    uint8_t bits;
    uint32_t sample_rate;
    float gain_db;
    pdm_pcm_channel_obj_t *channels[2];
    pdm_pcm_channel_obj_t *irq_channel;
    pcm_pdm_rx_fifo_ping_pong_buffer_t rx_buffer;
    ringbuf_t ring_buffer;
    mp_obj_t callback_for_non_blocking;
    non_blocking_descriptor_t non_blocking_descriptor;
} machine_pdm_pcm_obj_t;

static void pdm_pcm_read_half_rx_fifo_into_rx_buffer(machine_pdm_pcm_obj_t *self, uint32_t *rx_buf_next_frame_start_ptr) {
    for (uint32_t index = 0; index < RX_HW_FIFO_IRQ_TRIGGER_LEVEL; index++)
    {
        uint32_t data = (uint32_t)Cy_PDM_PCM_Channel_ReadFifo(self->channels[0]->block->periph, self->channels[0]->id);
        /**
         * The mono/stereo first channel uses the even indexes.
         */
        rx_buf_next_frame_start_ptr[index * 2] = data;
        if (self->format == STEREO) {
            data = Cy_PDM_PCM_Channel_ReadFifo(self->channels[1]->block->periph, self->channels[1]->id);
            /**
             * The stereo second channel uses the odd indexes.
             */
            rx_buf_next_frame_start_ptr[index * 2 + 1] = data;
        }
    }
}

static void pdm_pcm_read_into_rx_buffer(machine_pdm_pcm_obj_t *self) {
    uint32_t *rx_buf_next_frame_start_ptr = pdm_pcm_rx_buffer_get_next_frame_to_fill(&self->rx_buffer);
    pdm_pcm_read_half_rx_fifo_into_rx_buffer(self, rx_buf_next_frame_start_ptr);
}

/* -------------------------------- Ring Buffer Transfer IRQ -------------------------------- */

#define PDM_PCM_RX_FRAME_SIZE_IN_BYTES     (8)
#define PDM_PCM_SIZEOF_NON_BLOCKING_COPY_IN_BYTES    (SIZEOF_HALF_RX_BUFFER_IN_SAMPLES * PDM_PCM_RX_FRAME_SIZE_IN_BYTES)

static const int8_t pdm_pcm_frame_map[4][PDM_PCM_RX_FRAME_SIZE_IN_BYTES] = {
    { 0,  1, -1, -1, -1, -1, -1, -1 },  // Mono, 16-bits
    { 0,  1,  2,  3, -1, -1, -1, -1 },  // Mono, 32-bits
    { 0,  1, -1, -1,  2,  3, -1, -1 },  // Stereo, 16-bits
    { 0,  1,  2,  3,  4,  5,  6,  7 },  // Stereo, 32-bits
};

int8_t get_frame_mapping_index(int8_t bits, format_t format) {
    if (format == MONO) {
        if (bits == 16) {
            return 0;
        } else if (bits == 32) {
            return 1;
        }
    } else { // STEREO
        if (bits == 16) {
            return 2;
        } else if (bits == 32) {
            return 3;
        }
    }
    return -1;
}

static void pdm_pcm_copy_rx_buffer_to_ringbuf(machine_pdm_pcm_obj_t *self) {
    uint8_t *rx_buf_sample_ptr = (uint8_t *)self->rx_buffer.processing_half_rx_fifo_buf_ptr;
    uint32_t num_bytes_needed_from_ringbuf = SIZEOF_HALF_RX_BUFFER_IN_SAMPLES * SIZEOF_PDM_PCM_SAMPLE_IN_BYTES;

    // when space exists, copy samples into ring buffer
    if (ringbuf_free(&self->ring_buffer) >= num_bytes_needed_from_ringbuf) {
        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);
        uint32_t i = 0;
        while (i < num_bytes_needed_from_ringbuf) {
            for (uint8_t j = 0; j < SIZEOF_PDM_PCM_SAMPLE_IN_BYTES; j++) {
                int8_t r_to_a_mapping = pdm_pcm_frame_map[f_index][j];
                if (r_to_a_mapping != -1) {
                    ringbuf_put(&self->ring_buffer, rx_buf_sample_ptr[i]);
                } else { // r_a_mapping == -1
                    ringbuf_put(&self->ring_buffer, -1);
                }
                i++;
            }
        }
    }
}

static void pdm_pcm_channel_irq_handler(uint8_t pdm_pcm_channel) {
    machine_pdm_pcm_obj_t *pdm_pcm_obj = (machine_pdm_pcm_obj_t *)pdm_pcm_channel_obj[pdm_pcm_channel].parent;
    volatile uint32_t intr_status = Cy_PDM_PCM_Channel_GetInterruptStatusMasked(pdm_pcm_obj->irq_channel->block->periph, pdm_pcm_obj->irq_channel->id);
    if (CY_PDM_PCM_INTR_RX_TRIGGER & intr_status) {
        pdm_pcm_read_into_rx_buffer(pdm_pcm_obj);
        Cy_PDM_PCM_Channel_ClearInterrupt(pdm_pcm_obj->irq_channel->block->periph, pdm_pcm_obj->irq_channel->id, CY_PDM_PCM_INTR_RX_TRIGGER);
        if (pdm_pcm_rx_buffer_is_half_full(&pdm_pcm_obj->rx_buffer)) {
            pdm_pcm_copy_rx_buffer_to_ringbuf(pdm_pcm_obj);
            if ((pdm_pcm_obj->io_mode == NON_BLOCKING) && (pdm_pcm_obj->non_blocking_descriptor.copy_in_progress)) {
                fill_appbuf_from_ringbuf_non_blocking(pdm_pcm_obj);
            }
        }
        if ((CY_PDM_PCM_INTR_RX_FIR_OVERFLOW | CY_PDM_PCM_INTR_RX_OVERFLOW | CY_PDM_PCM_INTR_RX_IF_OVERFLOW |
             CY_PDM_PCM_INTR_RX_UNDERFLOW) & intr_status) {
            Cy_PDM_PCM_Channel_ClearInterrupt(pdm_pcm_obj->irq_channel->block->periph, pdm_pcm_obj->irq_channel->id, CY_PDM_PCM_INTR_MASK);
        }
    }
}

/* -------------------------------- Private Functions -------------------------------- */

static void pdm_pcm_init(machine_pdm_pcm_obj_t *self) {
    self->irq_channel = self->channels[0];
    pdm_pcm_channel_init(self->channels[0], self->sample_rate, self->bits, false);
    if (self->format == STEREO) {
        self->irq_channel = self->channels[1];
        pdm_pcm_channel_init(self->channels[1], self->sample_rate, self->bits, true);
    }
    pdm_pcm_channel_irq_init(self->irq_channel);
}

static inline void pdm_pcm_alloc_channels(machine_pdm_pcm_obj_t *self, format_t format, const mp_hal_pin_af_config_t *af_config) {
    /**
     * The allowed pairs are form or consecutive odd + even channel starting
     * from 0.
     * Therefore, to find the secondary channel  id, XORing the primary
     * channel id with 0x01 will give the id of the other channel in the pair.
     */
    self->channels[0] = pdm_pcm_channel_alloc(af_config->af->unit, self);
    if (format == STEREO) {
        self->channels[1] = pdm_pcm_channel_alloc(af_config->af->unit ^ 0x01, self);
    }
}

static void pdm_pcm_start(machine_pdm_pcm_obj_t *self) {
    Cy_PDM_PCM_Activate_Channel(self->channels[0]->block->periph, self->channels[0]->id);
    if (self->format == STEREO) {
        Cy_PDM_PCM_Activate_Channel(self->channels[1]->block->periph, self->channels[1]->id);
    }
};

static void pdm_pcm_stop(machine_pdm_pcm_obj_t *self) {
    Cy_PDM_PCM_DeActivate_Channel(self->channels[0]->block->periph, self->channels[0]->id);
    if (self->format == STEREO) {
        Cy_PDM_PCM_DeActivate_Channel(self->channels[1]->block->periph, self->channels[1]->id);
    }
}

#define PDM_PCM_DEFAULT_GAIN_DB     (23.0)
#define PDM_PCM_MIN_GAIN_DB         (-103.0)
#define PDM_PCM_MAX_GAIN_DB         (83.0)

static float pdm_pcm_get_valid_gain_db(float gain_db) {
    if (gain_db < PDM_PCM_MIN_GAIN_DB) {
        return PDM_PCM_MIN_GAIN_DB;
    } else if (gain_db > PDM_PCM_MAX_GAIN_DB) {
        return PDM_PCM_MAX_GAIN_DB;
    } else {
        return gain_db;
    }
}

static cy_en_pdm_pcm_gain_sel_t pdm_pcm_gain_db_to_scale(float gain_db) {
    /**
    * The gain scale goes from PDM_PCM_MIN_GAIN_DB to PDM_PCM_MAX_GAIN_DB in
    * steps of 6 dB. The db gains map to an enum in the range from 0 to
    * (PDM_PCM_MAX_GAIN_DB - PDM_PCM_MIN_GAIN_DB) / 6 dB.
    */
    return (cy_en_pdm_pcm_gain_sel_t)roundf((PDM_PCM_MAX_GAIN_DB - gain_db) / 6.0);
}

static void pdm_pcm_set_gain(machine_pdm_pcm_obj_t *self) {
    cy_en_pdm_pcm_gain_sel_t gain_scale = pdm_pcm_gain_db_to_scale(self->gain_db);
    Cy_PDM_PCM_SetGain(self->channels[0]->block->periph, self->channels[0]->id, gain_scale);
    if (self->format == STEREO) {
        Cy_PDM_PCM_SetGain(self->channels[1]->block->periph, self->channels[1]->id, gain_scale);
    }
}

/* -------------------------------- Extmod API -------------------------------- */


static void mp_machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, mp_arg_val_t *args) {
    /**
     * Get the pin objects passed by the constructor.
     * Validate that they are valid PDM block and channel,
     * and configure them with the adequate drive mode and
     * alternate function.
     * {
     */
    self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    self->data = mp_hal_get_pin_obj(args[ARG_data].u_obj);

    const mp_hal_pin_af_config_t pdm_pins_config[] = {
        MP_HAL_PIN_AF_CONF(self->sck, CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_PDM_CLK),
        MP_HAL_PIN_AF_CONF(self->data, CY_GPIO_DM_HIGHZ, 1, MACHINE_PIN_AF_SIGNAL_PDM_DATA),
    };

    mp_hal_periph_pins_af_config(pdm_pins_config, 2);
    /** } */

    /**
     * Depending on the format, one or two channels
     * are then allocated from the available channels.
     * {
     */
    format_t pdm_pcm_format = args[ARG_format].u_int;
    if ((pdm_pcm_format != MONO) &&
        (pdm_pcm_format != STEREO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }
    self->format = pdm_pcm_format;

    pdm_pcm_alloc_channels(self, self->format, &pdm_pins_config[0]);
    /** } */

    uint8_t pdm_pcm_word_length = args[ARG_bits].u_int;
    if (pdm_pcm_word_length != BITS_16 &&
        pdm_pcm_word_length != BITS_32) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid word length"));
    }
    self->bits = args[ARG_bits].u_int;

    self->gain_db = PDM_PCM_DEFAULT_GAIN_DB;
    if (args[ARG_gain].u_obj != mp_const_none) {
        self->gain_db = mp_obj_get_float(args[ARG_gain].u_obj);
    }

    self->sample_rate = args[ARG_sample_rate].u_int;
    if (self->sample_rate == 16000) {
        /**
         * TODO: Enable different sample rates
         */
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("rate not supported"));
    }

    int32_t ring_buffer_len = args[ARG_ibuf].u_int;
    if (ring_buffer_len < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }

    self->io_mode = BLOCKING;
    self->callback_for_non_blocking = MP_OBJ_NULL;

    ringbuf_alloc(&self->ring_buffer, ring_buffer_len);
    pdm_pcm_rx_buffer_init(&self->rx_buffer);
    pdm_pcm_init(self);
    pdm_pcm_set_gain(self);
    pdm_pcm_start(self);
}

static machine_pdm_pcm_obj_t *mp_machine_pdm_pcm_make_new_instance(mp_int_t pdm_pcm_id) {
    (void)pdm_pcm_id;
    machine_pdm_pcm_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_pdm_pcm_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_pdm_pcm_obj_t, &machine_pdm_pcm_type);
            MP_STATE_PORT(machine_pdm_pcm_obj[i]) = self;
            self->id = i;
            break;
        }
    }

    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("all available PDM_PCM instances are allocated"));
    }

    return self;
}

static void mp_machine_pdm_pcm_irq_update(machine_pdm_pcm_obj_t *self) {
    /** TODO: Implementation of non-blocking  */
}

static mp_obj_t machine_pdm_pcm_gain(size_t n_args, const mp_obj_t *args) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        return mp_obj_new_float(self->gain_db);
    } else {
        self->gain_db = pdm_pcm_get_valid_gain_db(mp_obj_get_float(args[1]));
        pdm_pcm_set_gain(self);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pdm_pcm_gain_obj, 1, 2, machine_pdm_pcm_gain);


#define MICROPY_PY_MACHINE_PDM_PDM_CLASS_EXTRAS \
    { MP_ROM_QSTR(MP_QSTR_gain),            MP_ROM_PTR(&machine_pdm_pcm_gain_obj) }, \


static void mp_machine_pdm_pcm_deinit(machine_pdm_pcm_obj_t *self) {
    pdm_pcm_stop(self);
    pdm_pcm_channel_irq_deinit(self->irq_channel);
    pdm_pcm_channel_deinit(self->channels[0]);
    pdm_pcm_channel_free(self->channels[0]);
    if (self->format == STEREO) {
        pdm_pcm_channel_deinit(self->channels[1]);
        pdm_pcm_channel_free(self->channels[1]);
    }
    MP_STATE_PORT(machine_pdm_pcm_obj[self->id]) = NULL;

    /* Deinit the block if no other instances are using it */
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_pdm_pcm_obj[i]) != NULL) {
            return;
        }
    }
    pdm_pcm_block_deinit(self->channels[0]->block);
}

void machine_pdm_pcm_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES; i++) {
        machine_pdm_pcm_obj_t *pdm_pcm_obj = MP_STATE_PORT(machine_pdm_pcm_obj[i]);
        if (pdm_pcm_obj != NULL) {
            mp_machine_pdm_pcm_deinit(pdm_pcm_obj);
        }
    }
}

MP_REGISTER_ROOT_POINTER(struct _machine_pdm_pcm_obj_t *machine_pdm_pcm_obj[MICROPY_PY_MACHINE_PDM_PCM_NUM_ENTRIES]);
