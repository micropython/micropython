/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Infineon Technologies AG
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
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "mplogger.h"

#if MICROPY_PY_MACHINE_PDM_PCM
#include "machine_pdm_pcm.h"
#include "cy_sysclk.h"

#if MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

#define PDM_PCM_ISR_PRIORITY                    (2u)

/* PDM PCM interrupt configuration parameters */
const cy_stc_sysint_t PDM_IRQ_cfg =
{
    .intrSrc = (IRQn_Type)CYBSP_PDM_CHANNEL_3_IRQ,
    .intrPriority = PDM_PCM_ISR_PRIORITY
};

static cy_stc_pdm_pcm_channel_config_t channel_config = {
    .sampledelay = 1,
    .wordSize = CY_PDM_PCM_WSIZE_16_BIT,
    .signExtension = true,
    .rxFifoTriggerLevel = 31,
    .fir0_enable = false,
    .cic_decim_code = CY_PDM_PCM_CHAN_CIC_DECIM_32,
    .fir0_decim_code = CY_PDM_PCM_CHAN_FIR0_DECIM_1,
    .fir0_scale = 0,
    .fir1_decim_code = CY_PDM_PCM_CHAN_FIR1_DECIM_3,
    .fir1_scale = 10,
    .dc_block_disable = false,
    .dc_block_code = CY_PDM_PCM_CHAN_DCBLOCK_CODE_16,
};

void ringbuf_init(ring_buf_t *rbuf, size_t size) {
    mplogger_print("ringbuf_init \r\n");
    rbuf->buffer = m_new(uint8_t, size);
    memset(rbuf->buffer, 0, size);
    rbuf->size = size;
    rbuf->head = 0;
    rbuf->tail = 0;
}

bool ringbuf_push(ring_buf_t *rbuf, uint8_t data) {
    size_t next_tail = (rbuf->tail + 1) % rbuf->size;
    if (next_tail != rbuf->head) {
        rbuf->buffer[rbuf->tail] = data;
        rbuf->tail = next_tail;
        return true;
    }
    // full
    return false;
}

bool ringbuf_pop(ring_buf_t *rbuf, uint8_t *data) {
    if (rbuf->head == rbuf->tail) {
        // empty
        return false;
    }

    *data = rbuf->buffer[rbuf->head];
    rbuf->head = (rbuf->head + 1) % rbuf->size;
    return true;
}

size_t ringbuf_available_data(ring_buf_t *rbuf) {
    return (rbuf->tail - rbuf->head + rbuf->size) % rbuf->size;
}

size_t ringbuf_available_space(ring_buf_t *rbuf) {
    return rbuf->size - ringbuf_available_data(rbuf) - 1;
}

static uint32_t fill_appbuf_from_ringbuf(machine_pdm_pcm_obj_t *self, mp_buffer_info_t *appbuf) {
    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = (uint8_t *)appbuf->buf;
    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 3) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_needed_from_ringbuf = appbuf->len * (PDM_PCM_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint8_t discard_byte;
    while (num_bytes_needed_from_ringbuf) {

        uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

        for (uint8_t i = 0; i < PDM_PCM_RX_FRAME_SIZE_IN_BYTES; i++) {
            int8_t r_to_a_mapping = pdm_pcm_frame_map[f_index][i];
            if (r_to_a_mapping != -1) {
                if (self->io_mode == BLOCKING) {
                    while (ringbuf_pop(&self->ring_buffer, app_p + r_to_a_mapping) == false) {
                        ;
                    }
                    num_bytes_copied_to_appbuf++;
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
                } else {
                    return 0;  // should never get here (non-blocking mode does not use this function)
                }
            }
            num_bytes_needed_from_ringbuf--;
        }
        app_p += appbuf_sample_size_in_bytes;
    }
    return num_bytes_copied_to_appbuf;
}

// Copy from ringbuf to appbuf as soon as ASYNC_TRANSFER_COMPLETE is triggered
static void fill_appbuf_from_ringbuf_non_blocking(machine_pdm_pcm_obj_t *self) {
    uint32_t num_bytes_copied_to_appbuf = 0;
    uint8_t *app_p = &(((uint8_t *)self->non_blocking_descriptor.appbuf.buf)[self->non_blocking_descriptor.index]);

    uint8_t appbuf_sample_size_in_bytes = (self->bits == 16? 2 : 3) * (self->format == STEREO ? 2: 1);
    uint32_t num_bytes_remaining_to_copy_to_appbuf = self->non_blocking_descriptor.appbuf.len - self->non_blocking_descriptor.index;
    uint32_t num_bytes_remaining_to_copy_from_ring_buffer = num_bytes_remaining_to_copy_to_appbuf *
        (PDM_PCM_RX_FRAME_SIZE_IN_BYTES / appbuf_sample_size_in_bytes);
    uint32_t num_bytes_needed_from_ringbuf = MIN(SIZEOF_NON_BLOCKING_COPY_IN_BYTES, num_bytes_remaining_to_copy_from_ring_buffer);
    uint8_t discard_byte;
    if (ringbuf_available_data(&self->ring_buffer) >= num_bytes_needed_from_ringbuf) {
        while (num_bytes_needed_from_ringbuf) {

            uint8_t f_index = get_frame_mapping_index(self->bits, self->format);

            for (uint8_t i = 0; i < PDM_PCM_RX_FRAME_SIZE_IN_BYTES; i++) {
                int8_t r_to_a_mapping = pdm_pcm_frame_map[f_index][i];
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
            // mp_sched_schedule(self->callback_for_non_blocking, MP_OBJ_FROM_PTR(self));
        }
    }
}
#endif // MICROPY_PY_MACHINE_PDM_PCM_RING_BUF

/*========================================================================================================================*/
// PDM_PCM higher level MPY functions (extmod/machine_pdm_pcm.c)

static void machine_pdm_pcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print,
        "PDM_PCM(id=%u,\n"
        "sample_rate=%ld,\n"
        "decimation_rate=%d,\n"
        "bits=%u,\n"
        "format=%u,\n"
        "left_gain=%d,\n"
        "right_gain=%d)",
        self->pdm_pcm_id,
        self->sample_rate,
        self->decimation_rate,
        self->bits,
        self->format,
        self->left_gain,
        self->right_gain
        );
}

// PDM_PCM.init(...)
static mp_obj_t machine_pdm_pcm_init(mp_obj_t self_in) {
    mplogger_print("machine_pdm_pcm_init \r\n");
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_pdm_pcm_init(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pdm_pcm_init_obj, machine_pdm_pcm_init);

// PDM_PCM.deinit()
static mp_obj_t machine_pdm_pcm_deinit(mp_obj_t self_in) {
    mplogger_print("machine_pdm_pcm_deinit \r\n");
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_pdm_pcm_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pdm_pcm_deinit_obj, machine_pdm_pcm_deinit);

// PDM_PCM.set_gain()
static mp_obj_t machine_pdm_pcm_set_gain(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mplogger_print("machine_pdm_pcm_set_gain \r\n");

    enum { ARG_left_gain, ARG_right_gain};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_left_gain,       MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = DEFAULT_LEFT_GAIN} },
        { MP_QSTR_right_gain,      MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = DEFAULT_RIGHT_GAIN} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    machine_pdm_pcm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    self->left_gain = args[ARG_left_gain].u_int;
    self->right_gain = args[ARG_right_gain].u_int;

    mp_machine_pdm_pcm_set_gain(self, self->left_gain, self->right_gain);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pdm_pcm_set_gain_obj, 1, machine_pdm_pcm_set_gain);

// PDM_PCM.irq(handler)
static mp_obj_t machine_pdm_pcm_irq(mp_obj_t self_in, mp_obj_t handler) {
    mplogger_print("machine_pdm_pcm_irq \r\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pdm_pcm_irq_obj, machine_pdm_pcm_irq);


cy_en_pdm_pcm_ch_fir1_decimcode_t pdm_pcm_convert_decim_rate(uint8_t decim_rate) {
    cy_en_pdm_pcm_ch_fir1_decimcode_t pdl_code = CY_PDM_PCM_CHAN_FIR1_DECIM_1;

    switch (decim_rate)
    {
        case 32:
            pdl_code = CY_PDM_PCM_CHAN_FIR1_DECIM_1;
            break;
        case 64:
            pdl_code = CY_PDM_PCM_CHAN_FIR1_DECIM_2;
            break;
        case 96:
            pdl_code = CY_PDM_PCM_CHAN_FIR1_DECIM_3;
            break;
        case 128:
            pdl_code = CY_PDM_PCM_CHAN_FIR1_DECIM_4;
            break;
        default:
            break;
    }

    return pdl_code;
}

// IRQ Handler
static void pdm_pcm_irq_handler(void) {

}

// Helper: Configure PDM PCM interrupts
static void pdm_pcm_configure_interrupts(format_t format) {
    mplogger_print("pdm_pcm_configure_interrupts \r\n");
    if ((format == MONO_LEFT) || (format == STEREO)) {
        Cy_PDM_PCM_Channel_ClearInterrupt(PDM0, LEFT_CH_INDEX, CY_PDM_PCM_INTR_MASK);
        Cy_PDM_PCM_Channel_SetInterruptMask(PDM0, LEFT_CH_INDEX, CY_PDM_PCM_INTR_MASK);
    }
    if ((format == MONO_RIGHT) || (format == STEREO)) {
        Cy_PDM_PCM_Channel_ClearInterrupt(PDM0, RIGHT_CH_INDEX, CY_PDM_PCM_INTR_MASK);
        Cy_PDM_PCM_Channel_SetInterruptMask(PDM0, RIGHT_CH_INDEX, CY_PDM_PCM_INTR_MASK);
    }
    /* Register the IRQ handler */
    if (CY_SYSINT_SUCCESS != Cy_SysInt_Init(&PDM_IRQ_cfg, &pdm_pcm_irq_handler)) {
        printf("PDM PCM Initialization has failed! \r\n");
        CY_ASSERT(0);
    }
    NVIC_ClearPendingIRQ(PDM_IRQ_cfg.intrSrc);
    NVIC_EnableIRQ(PDM_IRQ_cfg.intrSrc);
}

// Helper: Configure DPLL for audio clock generation
static cy_rslt_t dpll_lp_configure(uint32_t target_freq) {
    mplogger_print("dpll_lp_configure \r\n");
    cy_rslt_t result;

    /* Check if the current DPLL_LP_1 frequency is already set to target */
    uint32_t current_freq = Cy_SysClk_PllGetFrequency(SRSS_DPLL_LP_1_PATH_NUM);
    if (current_freq == target_freq) {
        mplogger_print("DPLL_LP_1 already at target frequency\r\n");
        return CY_RSLT_SUCCESS;
    }

    /* Configure DPLL_LP_1 PLL */
    cy_stc_pll_config_t dpll_config = {
        .inputFreq = DPLL_INPUT_FREQ_HZ,
        .outputFreq = target_freq,
        .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
        .lfMode = false,
    };

    /* Disable the DPLL_LP_1 PLL path */
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_1_PATH_NUM);

    /* Configure the PLL with the specified settings */
    result = Cy_SysClk_PllConfigure(SRSS_DPLL_LP_1_PATH_NUM, &dpll_config);
    if (result != CY_SYSCLK_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("DPLL LP1 configuration failed with return code %lx"),
            result);
    }

    /* Enable the DPLL_LP_1 PLL path with a timeout */
    result = Cy_SysClk_PllEnable(SRSS_DPLL_LP_1_PATH_NUM, DPLL_ENABLE_TIMEOUT_MS);
    if (result != CY_SYSCLK_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("DPLL LP1 enable failed with return code %lx"),
            result);
    }

    return CY_RSLT_SUCCESS;
}

// Helper: Determine appropriate DPLL frequency for sample rate
static uint32_t pdm_pcm_get_dpll_freq(uint32_t sample_rate) {
    mplogger_print("pdm_pcm_get_dpll_freq \r\n");
    switch (sample_rate) {
        case SAMPLE_RATE_8000:
        case SAMPLE_RATE_16000:
        case SAMPLE_RATE_48000:
            return DPLL_OUTPUT1_FREQ_HZ;

        case SAMPLE_RATE_22050:
        case SAMPLE_RATE_44100:
            return DPLL_OUTPUT2_FREQ_HZ;

        default:
            return 0;  // Invalid sample rate
    }
}

// Helper: Calculate optimal clock divider with rounding
static uint16_t pdm_pcm_calculate_clk_div(uint32_t dpll_freq, uint32_t sample_rate, uint8_t decimation_rate) {
    mplogger_print("pdm_pcm_calculate_clk_div \r\n");
    uint32_t required_clk = sample_rate * decimation_rate * PDM_PCM_CLK_OVERSAMPLING_FACTOR;

    /* Calculate divider with rounding for better accuracy */
    uint16_t clk_div = (dpll_freq + (required_clk / 2)) / required_clk;

    /* Ensure divider is at least 1 */
    return (clk_div > 0) ? clk_div : 1;
}

// Helper: Configure peripheral clock divider
static void pdm_pcm_configure_clock_divider(uint16_t clk_div) {
    mplogger_print("pdm_pcm_configure_clock_divider \r\n");
    /* Disable the clock divider before reconfiguration */
    Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM,
        CY_SYSCLK_DIV_16_5_BIT, 1U);

    /* Set the fractional divider (integer part only, fractional part = 0) */
    Cy_SysClk_PeriPclkSetFracDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM,
        CY_SYSCLK_DIV_16_5_BIT, 1U, clk_div, 0U);

    /* Enable the configured clock divider */
    Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PDM_CLK_DIV_GRP_NUM,
        CY_SYSCLK_DIV_16_5_BIT, 1U);
}

// Initialize PDM PCM sample rate and clock configuration
static PDM_PCM_STATUS_t pdm_pcm_init_sample_rate(uint32_t sample_rate, uint8_t decimation_rate) {
    mplogger_print("pdm_pcm_init_sample_rate \r\n");
    /* Determine the appropriate DPLL frequency for the sample rate */
    uint32_t dpll_freq = pdm_pcm_get_dpll_freq(sample_rate);
    if (dpll_freq == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported sample rate"));
        return PDM_PCM_STATUS_BAD_PARAM;
    }

    /* Configure DPLL to the required frequency */
    dpll_lp_configure(dpll_freq);

    /* Calculate optimal clock divider */
    uint16_t clk_div = pdm_pcm_calculate_clk_div(dpll_freq, sample_rate, decimation_rate);

    /* Configure the peripheral clock divider */
    pdm_pcm_configure_clock_divider(clk_div);

    mplogger_print("PDM PCM clock configured: DPLL=%lu Hz, div=%u\r\n", dpll_freq, clk_div);

    return PDM_PCM_STATUS_SUCCESS;
}

// Init hardware block
static void pdm_pcm_init(machine_pdm_pcm_obj_t *self) {
    cy_rslt_t result;
    mplogger_print("pdm_pcm_init \r\n");

    // Configure PDM PCM block
    cy_stc_pdm_pcm_config_v2_t PDM_config = {
        .clkDiv = 7,
        .clksel = CY_PDM_PCM_SEL_SRSS_CLOCK,
        .halverate = CY_PDM_PCM_RATE_FULL,
        .route = 4,
        .fir0_coeff_user_value = false,
        .fir1_coeff_user_value = false,
    };

    // Initialize PDM PCM block
    result = Cy_PDM_PCM_Init(PDM0, &PDM_config);
    pdm_pcm_assert_raise_val("PDM_PCM initialisation failed with return code %lx !", result);
    pdm_pcm_init_sample_rate(self->sample_rate, self->decimation_rate);
    // Initialize left channel if needed
    if ((self->format == MONO_LEFT) || (self->format == STEREO)) {
        Cy_PDM_PCM_Channel_Enable(PDM0, LEFT_CH_INDEX);
        result = Cy_PDM_PCM_Channel_Init(PDM0, &channel_config, LEFT_CH_INDEX);
        Cy_PDM_PCM_SetGain(PDM0, LEFT_CH_INDEX, self->left_gain);
    }

    // Initialize right channel if needed
    if ((self->format == MONO_RIGHT) || (self->format == STEREO)) {
        channel_config.sampledelay = 5;
        Cy_PDM_PCM_Channel_Enable(PDM0, RIGHT_CH_INDEX);
        result = Cy_PDM_PCM_Channel_Init(PDM0, &channel_config, RIGHT_CH_INDEX);
        Cy_PDM_PCM_SetGain(PDM0, RIGHT_CH_INDEX, self->right_gain);
    }
    pdm_pcm_configure_interrupts(self->format);
}

int8_t get_frame_mapping_index(int8_t bits, format_t format) {
    if ((format == MONO_LEFT) | (format == MONO_RIGHT)) {
        if (bits == 16) {
            return 0;
        }
    } else { // STEREO
        if (bits == 16) {
            return 2;
        }
    }
    return -1;
}

// First read RX buffer
static void pdm_pcm_read_rxbuf(machine_pdm_pcm_obj_t *self) {

}

// First read and start the hw block
static void pdm_pcm_rx_init(machine_pdm_pcm_obj_t *self) {

}

// =======================================================================================
// MPY bindings for PDM_PCM

MP_NOINLINE static void machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mplogger_print("machine_pdm_pcm_init_helper \r\n");
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample_rate,     MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 16000} },
        { MP_QSTR_decimation_rate, MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 96} },
        { MP_QSTR_bits,            MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 16} },
        { MP_QSTR_format,          MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = MONO_LEFT} },
        { MP_QSTR_left_gain,       MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = DEFAULT_LEFT_GAIN} },
        { MP_QSTR_right_gain,      MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = DEFAULT_RIGHT_GAIN} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_machine_pdm_pcm_init_helper(self, args);
}

// PDM_PCM(...) Constructor
static mp_obj_t machine_pdm_pcm_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mplogger_print("machine_pdm_pcm_make_new \r\n");
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t pdm_pcm_id = mp_obj_get_int(args[0]);

    machine_pdm_pcm_obj_t *self = mp_machine_pdm_pcm_make_new_instance(pdm_pcm_id);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_pdm_pcm_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// constructor()
static machine_pdm_pcm_obj_t *mp_machine_pdm_pcm_make_new_instance(mp_int_t pdm_pcm_id) {
    mplogger_print("mp_machine_pdm_pcm_make_new_instance \r\n");
    (void)pdm_pcm_id;
    machine_pdm_pcm_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_HW_MAX_PDM_PCM; i++) {
        if (MP_STATE_PORT(machine_pdm_pcm_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_pdm_pcm_obj_t, &machine_pdm_pcm_type);
            MP_STATE_PORT(machine_pdm_pcm_obj[i]) = self;
            self->pdm_pcm_id = i;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("all available pdm pcm instances are allocated"));
    }
    return self;
}

// init.helper()
static void mp_machine_pdm_pcm_init_helper(machine_pdm_pcm_obj_t *self, mp_arg_val_t *args) {
    mplogger_print("mp_machine_pdm_pcm_init_helper \r\n");

    // Assign configurable parameters
    // PDM_PCM Mode
    format_t pdm_pcm_format = args[ARG_format].u_int;
    if ((pdm_pcm_format != MONO_LEFT) &&
        (pdm_pcm_format != MONO_RIGHT) &&
        (pdm_pcm_format != STEREO)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }
    self->format = pdm_pcm_format;

    // Check word length
    uint8_t pdm_pcm_word_length = args[ARG_bits].u_int;
    if (pdm_pcm_word_length < BITS_16 || pdm_pcm_word_length > BITS_24) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid word length"));
    }
    self->bits = args[ARG_bits].u_int;

    // Set gains
    self->left_gain = args[ARG_left_gain].u_int;
    self->right_gain = args[ARG_right_gain].u_int;

    // Set sampling and decimation rates (as given by user)
    self->sample_rate = args[ARG_sample_rate].u_int;

    if (self->sample_rate == 8000 ||
        self->sample_rate == 16000 ||
        self->sample_rate == 48000) {
        if (PLL0_freq != AUDIO_SYS_CLOCK_73_728_000_HZ) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid clock frequency set for the sample rate/ PDM_PCM Clock not set . Set the right clock before initialising PDM_PCM"));
        }
    } else if (self->sample_rate == 22050 ||
               self->sample_rate == 44100) {
        if (PLL0_freq != AUDIO_SYS_CLOCK_169_344_000_HZ) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid clock frequency set for the sample rate/ PDM_PCM Clock not set. Set the right clock before initialising PDM_PCM"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("rate not supported"));
    }

    self->decimation_rate = args[ARG_decimation_rate].u_int;
    if (self->decimation_rate < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid decimation rate"));
    }

    int32_t ring_buffer_len = 20000;
    if (ring_buffer_len < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ibuf"));
    }
    self->ibuf = ring_buffer_len;
    self->callback_for_non_blocking = MP_OBJ_NULL;
    self->io_mode = BLOCKING;

    mplogger_print("PDM_PCM configured: sample_rate=%ld, decimation_rate=%d, bits=%u, format=%u, left_gain=%d, right_gain=%d\r\n",
        args[ARG_sample_rate].u_int,
        args[ARG_decimation_rate].u_int,
        args[ARG_bits].u_int,
        args[ARG_format].u_int,
        args[ARG_left_gain].u_int,
        args[ARG_right_gain].u_int);

    ringbuf_init(&self->ring_buffer, ring_buffer_len);
    pdm_pcm_init(self);
    /* This function is not yet declared. */
    // pdm_pcm_irq_configure(self);
}

// init()
static void mp_machine_pdm_pcm_init(machine_pdm_pcm_obj_t *self) {
    mplogger_print("mp_machine_pdm_pcm_init \r\n");
    pdm_pcm_rx_init(self);
}

// deinit()
static void mp_machine_pdm_pcm_deinit(machine_pdm_pcm_obj_t *self) {
    mplogger_print("mp_machine_pdm_pcm_deinit \r\n");

    if ((self->format == MONO_LEFT) || (self->format == STEREO)) {
        Cy_PDM_PCM_DeActivate_Channel(PDM0, LEFT_CH_INDEX);
    }
    if ((self->format == MONO_RIGHT) || (self->format == STEREO)) {
        Cy_PDM_PCM_DeActivate_Channel(PDM0, RIGHT_CH_INDEX);
    }
    MP_STATE_PORT(machine_pdm_pcm_obj[self->pdm_pcm_id]) = NULL;
}

// set_gain()
static void mp_machine_pdm_pcm_set_gain(machine_pdm_pcm_obj_t *self, int16_t left_gain, int16_t right_gain) {
    mplogger_print("mp_machine_pdm_pcm_set_gain \r\n");
    Cy_PDM_PCM_SetGain(PDM0, LEFT_CH_INDEX, left_gain);
    Cy_PDM_PCM_SetGain(PDM0, RIGHT_CH_INDEX, right_gain);
}

// irq update
static void mp_machine_pdm_pcm_irq_update(machine_pdm_pcm_obj_t *self) {
    (void)self;
}

// =======================================================================================
// Implementation for stream protocol
static mp_uint_t machine_pdm_pcm_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    return 0;
}

static const mp_stream_p_t pdm_pcm_stream_p = {
    .read = machine_pdm_pcm_stream_read,
    .is_text = false,
};

static const mp_rom_map_elem_t machine_pdm_pcm_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_pdm_pcm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_pdm_pcm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_gain),        MP_ROM_PTR(&machine_pdm_pcm_set_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_pdm_pcm_irq_obj) },

    #if MICROPY_PY_MACHINE_PDM_PCM_FINALISER
    { MP_ROM_QSTR(MP_QSTR___del__),         MP_ROM_PTR(&machine_pdm_pcm_deinit_obj) },
    #endif

    // Constants
    // Word lengths
    { MP_ROM_QSTR(MP_QSTR_BITS_16),          MP_ROM_INT(BITS_16) },

    // Modes
    { MP_ROM_QSTR(MP_QSTR_STEREO),          MP_ROM_INT(STEREO) },
    { MP_ROM_QSTR(MP_QSTR_MONO_LEFT),       MP_ROM_INT(MONO_LEFT) },
    { MP_ROM_QSTR(MP_QSTR_MONO_RIGHT),      MP_ROM_INT(MONO_RIGHT) },
};
MP_DEFINE_CONST_DICT(machine_pdm_pcm_locals_dict, machine_pdm_pcm_locals_dict_table);

MP_REGISTER_ROOT_POINTER(struct _machine_pdm_pcm_obj_t *machine_pdm_pcm_obj[MICROPY_HW_MAX_PDM_PCM]);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pdm_pcm_type,
    MP_QSTR_PDM_PCM,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_pdm_pcm_make_new,
    print, machine_pdm_pcm_print,
    protocol, &pdm_pcm_stream_p,
    locals_dict, &machine_pdm_pcm_locals_dict
    );

#endif // MICROPY_PY_MACHINE_PDM_PCM
