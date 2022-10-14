/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
 * Copyright (c) 2022 Robert Hammelrath
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
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#if MICROPY_PY_MACHINE_ADC

#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "py/mphal.h"
#include "sam.h"
#include "pin_af.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "dma_manager.h"
#include "tc_manager.h"

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    adc_config_t adc_config;
    uint8_t id;
    uint8_t avg;
    uint8_t bits;
    uint8_t vref;
    int8_t dma_channel;
    int8_t tc_index;
} machine_adc_obj_t;

#define DEFAULT_ADC_BITS    12
#define DEFAULT_ADC_AVG     16

#if defined(MCU_SAMD21)

static uint8_t adc_vref_table[] = {
    ADC_REFCTRL_REFSEL_INT1V_Val, ADC_REFCTRL_REFSEL_INTVCC0_Val,
    ADC_REFCTRL_REFSEL_INTVCC1_Val, ADC_REFCTRL_REFSEL_AREFA_Val, ADC_REFCTRL_REFSEL_AREFB_Val
};
#if MICROPY_HW_ADC_VREF
#define DEFAULT_ADC_VREF    MICROPY_HW_ADC_VREF
#else
#define DEFAULT_ADC_VREF    (3)
#endif
#define MAX_ADC_VREF        (4)

#define ADC_EVSYS_CHANNEL    0

typedef struct _device_mgmt_t {
    bool init;
    bool busy;
    mp_obj_t callback;
    mp_obj_t self;
} device_mgmt_t;

device_mgmt_t device_mgmt[ADC_INST_NUM];

#elif defined(MCU_SAMD51)

static uint8_t adc_vref_table[] = {
    ADC_REFCTRL_REFSEL_INTREF_Val, ADC_REFCTRL_REFSEL_INTVCC1_Val,
    ADC_REFCTRL_REFSEL_INTVCC0_Val, ADC_REFCTRL_REFSEL_AREFA_Val,
    ADC_REFCTRL_REFSEL_AREFB_Val, ADC_REFCTRL_REFSEL_AREFC_Val
};
#if MICROPY_HW_ADC_VREF
#define DEFAULT_ADC_VREF    MICROPY_HW_ADC_VREF
#else
#define DEFAULT_ADC_VREF    (3)
#endif
#define MAX_ADC_VREF        (5)

typedef struct _device_mgmt_t {
    bool init;
    bool busy;
    int8_t dma_channel;
    mp_obj_t callback;
    mp_obj_t self;
} device_mgmt_t;

device_mgmt_t device_mgmt[ADC_INST_NUM] = {
    { 0, 0, -1, MP_OBJ_NULL, MP_OBJ_NULL},
    { 0, 0, -1, MP_OBJ_NULL, MP_OBJ_NULL}
};

#endif  // defined(MCU_SAMD21)

// The ADC class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS

Adc *const adc_bases[] = ADC_INSTS;
uint32_t ch_busy_flags = 0;

static uint8_t resolution[] = {
    ADC_CTRLB_RESSEL_8BIT_Val, ADC_CTRLB_RESSEL_10BIT_Val, ADC_CTRLB_RESSEL_12BIT_Val
};
static void adc_init(machine_adc_obj_t *self);

extern mp_int_t log2i(mp_int_t num);

// Active just for SAMD21, stops the freerun mode
// For SAMD51, just the INT flag is reset.
void adc_irq_handler(int dma_channel) {

    #if defined(MCU_SAMD21)
    DMAC->CHID.reg = dma_channel;
    DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL | DMAC_CHINTFLAG_TERR | DMAC_CHINTFLAG_SUSP;
    ADC->EVCTRL.bit.STARTEI = 0;
    device_mgmt[0].busy = 0;
    if (device_mgmt[0].callback != MP_OBJ_NULL) {
        mp_sched_schedule(device_mgmt[0].callback, device_mgmt[0].self);
    }

    #elif defined(MCU_SAMD51)
    DMAC->Channel[dma_channel].CHINTFLAG.reg =
        DMAC_CHINTFLAG_TCMPL | DMAC_CHINTFLAG_TERR | DMAC_CHINTFLAG_SUSP;
    if (device_mgmt[0].dma_channel == dma_channel) {
        device_mgmt[0].busy = 0;
        if (device_mgmt[0].callback != MP_OBJ_NULL) {
            mp_sched_schedule(device_mgmt[0].callback, device_mgmt[0].self);
        }
    } else if (device_mgmt[1].dma_channel == dma_channel) {
        device_mgmt[1].busy = 0;
        if (device_mgmt[1].callback != MP_OBJ_NULL) {
            mp_sched_schedule(device_mgmt[1].callback, device_mgmt[1].self);
        }
    }
    #endif
}

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "ADC(%q, device=%u, channel=%u, bits=%u, average=%u, vref=%d)",
        pin_find_by_id(self->id)->name, self->adc_config.device,
        self->adc_config.channel, self->bits, 1 << self->avg, self->vref);
}

static mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *all_args) {

    enum { ARG_id, ARG_bits, ARG_average, ARG_vref, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_bits,     MP_ARG_INT, {.u_int = DEFAULT_ADC_BITS} },
        { MP_QSTR_average,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_ADC_AVG} },
        { MP_QSTR_vref,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_ADC_VREF} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Unpack and check, whether the pin has ADC capability
    int id = mp_hal_get_pin_obj(args[ARG_id].u_obj);
    adc_config_t adc_config = get_adc_config(id, ch_busy_flags);

    // Now that we have a valid device and channel, create and populate the ADC instance
    machine_adc_obj_t *self = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    self->id = id;
    self->adc_config = adc_config;
    self->bits = DEFAULT_ADC_BITS;

    uint16_t bits = args[ARG_bits].u_int;
    if (8 <= bits && bits <= 12) {
        self->bits = bits;
    }
    uint32_t avg = log2i(args[ARG_average].u_int);
    self->avg = (avg <= 10 ? avg : 10);

    uint8_t vref = args[ARG_vref].u_int;
    if (0 <= vref && vref <= MAX_ADC_VREF) {
        self->vref = vref;
    }
    device_mgmt[adc_config.device].callback = args[ARG_callback].u_obj;
    if (device_mgmt[adc_config.device].callback == mp_const_none) {
        device_mgmt[adc_config.device].callback = MP_OBJ_NULL;
    } else {
        device_mgmt[adc_config.device].self = self;
    }

    // flag the device/channel as being in use.
    ch_busy_flags |= (1 << (self->adc_config.device * 16 + self->adc_config.channel));
    self->dma_channel = -1;
    self->tc_index = -1;

    adc_init(self);

    return MP_OBJ_FROM_PTR(self);
}

// read_u16()
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    Adc *adc = adc_bases[self->adc_config.device];
    // Set the reference voltage. Default: external AREFA.
    adc->REFCTRL.reg = adc_vref_table[self->vref];
    if (device_mgmt[self->adc_config.device].busy != 0) {
        mp_raise_OSError(MP_EBUSY);
    }

    // Set Input channel and resolution
    // Select the pin as positive input and gnd as negative input reference, non-diff mode by default
    adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND | self->adc_config.channel;
    // set resolution. Scale 8-16 to 0 - 4 for table access.
    adc->CTRLB.bit.RESSEL = resolution[(self->bits - 8) / 2];

    #if defined(MCU_SAMD21)
    // Stop the ADC sampling by timer
    adc->EVCTRL.bit.STARTEI = 0;
    #elif defined(MCU_SAMD51)
    // Do not restart ADC after data has bee read
    adc->DSEQCTRL.reg = 0;
    #endif
    // Measure input voltage
    adc->SWTRIG.bit.START = 1;
    while (adc->INTFLAG.bit.RESRDY == 0) {
    }
    // Get and return the result
    return adc->RESULT.reg * (65536 / (1 << self->bits));
}

static void machine_adc_read_timed(mp_obj_t self_in, mp_obj_t values, mp_obj_t freq_in) {
    machine_adc_obj_t *self = self_in;
    Adc *adc = adc_bases[self->adc_config.device];
    mp_buffer_info_t src;
    mp_get_buffer_raise(values, &src, MP_BUFFER_READ);
    if (src.len >= 2) {
        int freq = mp_obj_get_int(freq_in);
        if (self->dma_channel == -1) {
            self->dma_channel = allocate_dma_channel();
            dma_init();
            dma_register_irq(self->dma_channel, adc_irq_handler);
        }
        if (self->tc_index == -1) {
            self->tc_index = allocate_tc_instance();
        }
        // Set Input channel and resolution
        // Select the pin as positive input and gnd as negative input reference, non-diff mode by default
        adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND | self->adc_config.channel;
        // set resolution. Scale 8-16 to 0 - 4 for table access.
        adc->CTRLB.bit.RESSEL = resolution[(self->bits - 8) / 2];

        // Configure DMA for halfword output to the DAC
        #if defined(MCU_SAMD21)
        configure_tc(self->tc_index, freq, TC_EVCTRL_OVFEO);
        // Enable APBC clock
        PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;
        // Set up the EVSYS channel
        EVSYS->CTRL.bit.SWRST = 1;
        EVSYS->USER.reg = EVSYS_USER_CHANNEL(ADC_EVSYS_CHANNEL + 1) |
            EVSYS_USER_USER(EVSYS_ID_USER_ADC_START);
        EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(ADC_EVSYS_CHANNEL) |
            EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TC3_OVF + 3 * self->tc_index) |
            EVSYS_CHANNEL_PATH_ASYNCHRONOUS;

        dma_desc[self->dma_channel].BTCTRL.reg =
            DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT |
            DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_STEPSEL |
            DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val);
        dma_desc[self->dma_channel].BTCNT.reg = src.len / 2;
        dma_desc[self->dma_channel].SRCADDR.reg = (uint32_t)(&adc->RESULT.reg);
        dma_desc[self->dma_channel].DSTADDR.reg = (uint32_t)(src.buf) + src.len;
        dma_desc[self->dma_channel].DESCADDR.reg = 0; // ONE_SHOT
        DMAC->CHID.reg = self->dma_channel;
        DMAC->CHCTRLA.reg = 0;
        while (DMAC->CHCTRLA.bit.ENABLE) {
        }
        DMAC->CHCTRLB.reg =
            DMAC_CHCTRLB_LVL(0) |
            DMAC_CHCTRLB_TRIGACT_BEAT |
            DMAC_CHCTRLB_TRIGSRC(ADC_DMAC_ID_RESRDY);

        DMAC->CHINTENSET.reg = DMAC_CHINTFLAG_TCMPL;
        DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;

        NVIC_EnableIRQ(DMAC_IRQn);
        adc->EVCTRL.bit.STARTEI = 1;
        device_mgmt[0].busy = 1;

        #elif defined(MCU_SAMD51)
        configure_tc(self->tc_index, freq, 0);
        device_mgmt[self->adc_config.device].dma_channel = self->dma_channel;

        // Restart ADC after data has bee read
        adc->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART;
        // Start the first sampling to ensure we get a proper first value.
        adc->SWTRIG.bit.START = 1;
        while (adc->INTFLAG.bit.RESRDY == 0) {
        }

        dma_desc[self->dma_channel].BTCTRL.reg =
            DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT |
            DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_STEPSEL |
            DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val);
        dma_desc[self->dma_channel].BTCNT.reg = src.len / 2;
        dma_desc[self->dma_channel].SRCADDR.reg = (uint32_t)(&adc->RESULT.reg);
        dma_desc[self->dma_channel].DSTADDR.reg = (uint32_t)(src.buf) + src.len;
        dma_desc[self->dma_channel].DESCADDR.reg = 0; // ONE_SHOT

        DMAC->Channel[self->dma_channel].CHCTRLA.reg =
            DMAC_CHCTRLA_BURSTLEN(DMAC_CHCTRLA_BURSTLEN_SINGLE_Val) |
            DMAC_CHCTRLA_TRIGACT(DMAC_CHCTRLA_TRIGACT_BURST_Val) |
            DMAC_CHCTRLA_TRIGSRC(TC0_DMAC_ID_OVF + 3 * self->tc_index);
        DMAC->Channel[self->dma_channel].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
        DMAC->Channel[self->dma_channel].CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;

        if (self->dma_channel < 4) {
            NVIC_EnableIRQ(DMAC_0_IRQn + self->dma_channel);
        } else {
            NVIC_EnableIRQ(DMAC_4_IRQn);
        }
        device_mgmt[self->adc_config.device].busy = 1;

        #endif // defined SAMD21 or SAMD51

    }
    return mp_const_none;
}

// deinit() : release the ADC channel
static void mp_machine_adc_deinit(machine_adc_obj_t *self) {
    busy_flags &= ~((1 << (self->adc_config.device * 16 + self->adc_config.channel)));
    if (self->dma_channel >= 0) {
        #if defined(MCU_SAMD51)
        if (self->dma_channel == device_mgmt[self->adc_config.device].dma_channel) {
            device_mgmt[self->adc_config.device].dma_channel = -1;
            device_mgmt[self->adc_config.device].busy = 0;
        }
        #endif
        dac_stop_dma(self->dma_channel, true);
        free_dma_channel(self->dma_channel);
        self->dma_channel = -1;
    }
    if (self->tc_index >= 0) {
        free_tc_instance(self->tc_index);
        self->tc_index = -1;
    }
}

// busy() : Report, if  the ADC device is busy
static mp_int_t machine_adc_busy(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return device_mgmt[self->adc_config.device].busy ? true : false;
}

void adc_deinit_all(void) {
    ch_busy_flags = 0;
    device_mgmt[0].init = 0;
    #if defined(MCU_SAMD51)
    device_mgmt[0].dma_channel = -1;
    device_mgmt[1].init = 0;
    device_mgmt[1].dma_channel = -1;
    #endif
}

static void adc_init(machine_adc_obj_t *self) {
    // ADC & clock init is done only once per ADC
    if (device_mgmt[self->adc_config.device].init == false) {
        Adc *adc = adc_bases[self->adc_config.device];

        device_mgmt[self->adc_config.device].init = true;

        #if defined(MCU_SAMD21)
        // Configuration SAMD21
        // Enable APBD clocks and PCHCTRL clocks; GCLK5 at 48 MHz
        PM->APBCMASK.reg |= PM_APBCMASK_ADC;
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID_ADC;
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        // Reset ADC registers
        adc->CTRLA.bit.SWRST = 1;
        while (adc->CTRLA.bit.SWRST) {
        }
        // Get the calibration data
        uint32_t bias = (*((uint32_t *)ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
        uint32_t linearity = (*((uint32_t *)ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
        linearity |= ((*((uint32_t *)ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
        /* Write the calibration data. */
        ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);
        // Divide a 48MHz clock by 32 to obtain 1.5 MHz clock to adc
        adc->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32;
        // Select external AREFA as reference voltage.
        adc->REFCTRL.reg = self->vref;
        // Average: Accumulate samples and scale them down accordingly
        adc->AVGCTRL.reg = self->avg | ADC_AVGCTRL_ADJRES(self->avg);
        // Enable ADC and wait to be ready
        adc->CTRLA.bit.ENABLE = 1;
        while (adc->STATUS.bit.SYNCBUSY) {
        }

        #elif defined(MCU_SAMD51)
        // Configuration SAMD51
        // Enable APBD clocks and PCHCTRL clocks; GCLK5 at 48 MHz
        if (self->adc_config.device == 0) {
            GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK5 | GCLK_PCHCTRL_CHEN;
            MCLK->APBDMASK.bit.ADC0_ = 1;
        } else {
            GCLK->PCHCTRL[ADC1_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK5 | GCLK_PCHCTRL_CHEN;
            MCLK->APBDMASK.bit.ADC1_ = 1;
        }
        // Reset ADC registers
        adc->CTRLA.bit.SWRST = 1;
        while (adc->CTRLA.bit.SWRST) {
        }
        // Get the calibration data
        uint32_t biascomp;
        uint32_t biasr2r;
        uint32_t biasrefbuf;
        if (self->adc_config.device == 0) {
            biascomp = (*((uint32_t *)ADC0_FUSES_BIASCOMP_ADDR) & ADC0_FUSES_BIASCOMP_Msk) >> ADC0_FUSES_BIASCOMP_Pos;
            biasr2r = (*((uint32_t *)ADC0_FUSES_BIASR2R_ADDR) & ADC0_FUSES_BIASR2R_Msk) >> ADC0_FUSES_BIASR2R_Pos;
            biasrefbuf = (*((uint32_t *)ADC0_FUSES_BIASREFBUF_ADDR) & ADC0_FUSES_BIASREFBUF_Msk) >> ADC0_FUSES_BIASREFBUF_Pos;
        } else {
            biascomp = (*((uint32_t *)ADC1_FUSES_BIASCOMP_ADDR) & ADC1_FUSES_BIASCOMP_Msk) >> ADC1_FUSES_BIASCOMP_Pos;
            biasr2r = (*((uint32_t *)ADC1_FUSES_BIASR2R_ADDR) & ADC1_FUSES_BIASR2R_Msk) >> ADC1_FUSES_BIASR2R_Pos;
            biasrefbuf = (*((uint32_t *)ADC1_FUSES_BIASREFBUF_ADDR) & ADC1_FUSES_BIASREFBUF_Msk) >> ADC1_FUSES_BIASREFBUF_Pos;
        }
        /* Write the calibration data. */
        adc->CALIB.reg = ADC_CALIB_BIASCOMP(biascomp) | ADC_CALIB_BIASR2R(biasr2r) | ADC_CALIB_BIASREFBUF(biasrefbuf);
        // Divide 48MHz clock by 4 to obtain 12 MHz clock to adc
        adc->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV4;
        // Enable the offset compensation
        adc->SAMPCTRL.reg = ADC_SAMPCTRL_OFFCOMP;
        // Set the reference voltage. Default: external AREFA.
        adc->REFCTRL.reg = adc_vref_table[self->vref];
        // Average: Accumulate samples and scale them down accordingly
        adc->AVGCTRL.reg = self->avg | ADC_AVGCTRL_ADJRES(self->avg);
        // Enable ADC and wait to be ready
        adc->CTRLA.bit.ENABLE = 1;
        while (adc->SYNCBUSY.bit.ENABLE) {
        }

        #endif
    }
    // Set the port as given in self->id as ADC
    mp_hal_set_pin_mux(self->id, ALT_FCT_ADC);
}
