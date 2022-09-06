/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
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

#include "py/runtime.h"

#if MICROPY_PY_MACHINE_DAC

#include <stdint.h>
#include "py/obj.h"
#include "py/mphal.h"

#include "sam.h"
#include "pin_af.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "dma_manager.h"
#include "tc_manager.h"

typedef struct _dac_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    bool initialized;
    mp_hal_pin_obj_t gpio_id;
    uint8_t vref;
    int8_t dma_channel;
    int8_t tc_index;
    uint32_t count;
} dac_obj_t;
Dac *const dac_bases[] = DAC_INSTS;
static void dac_init(dac_obj_t *self, Dac *dac);

#if defined(MCU_SAMD21)

static dac_obj_t dac_obj[] = {
    {{&machine_dac_type}, 0, PIN_PA02},
};

#define MAX_DAC_VALUE       (1023)
#define DEFAULT_DAC_VREF    (1)
#define MAX_DAC_VREF        (2)

#elif defined(MCU_SAMD51)

static dac_obj_t dac_obj[] = {
    {{&machine_dac_type}, 0, PIN_PA02},
    {{&machine_dac_type}, 1, PIN_PA05},
};
// According to Errata 2.9.2, VDDANA as ref value is not available. However it worked
// in tests. So I keep the selection here but set the default to Aref, which is usually
// connected at the Board to VDDANA
static uint8_t dac_vref_table[] = {
    DAC_CTRLB_REFSEL_INTREF_Val, DAC_CTRLB_REFSEL_VDDANA_Val,
    DAC_CTRLB_REFSEL_VREFPU_Val, DAC_CTRLB_REFSEL_VREFPB_Val
};
#define MAX_DAC_VALUE       (4095)
#define DEFAULT_DAC_VREF    (2)
#define MAX_DAC_VREF        (3)

#endif // defined SAMD21 or SAMD51


void dac_irq_handler(int dma_channel) {
    dac_obj_t *self;

    #if defined(MCU_SAMD21)
    DMAC->CHID.reg = dma_channel;
    DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
    self = &dac_obj[0];
    if (self->count > 1) {
        self->count -= 1;
        dma_desc[self->dma_channel].BTCTRL.reg |= DMAC_BTCTRL_VALID;
        DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
    }

    #elif defined(MCU_SAMD51)
    DMAC->Channel[dma_channel].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
    if (dac_obj[0].dma_channel == dma_channel) {
        self = &dac_obj[0];
    } else {
        self = &dac_obj[1];
    }
    if (self->count > 1) {
        self->count -= 1;
        dma_desc[self->dma_channel].BTCTRL.reg |= DMAC_BTCTRL_VALID;
        DMAC->Channel[self->dma_channel].CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
    }
    #endif
}

static mp_obj_t dac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *all_args) {

    enum { ARG_id, ARG_vref };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_vref,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_DAC_VREF} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t id = args[ARG_id].u_int;
    dac_obj_t *self = NULL;
    if (0 <= id && id <= MP_ARRAY_SIZE(dac_obj)) {
        self = &dac_obj[id];
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Pin for DAC"));
    }

    uint8_t vref = args[ARG_vref].u_int;
    if (0 <= vref && vref <= MAX_DAC_VREF) {
        self->vref = vref;
    }

    Dac *dac = dac_bases[0]; // Just one DAC
    dac_init(self, dac);
    // Set the port as given in self->gpio_id as DAC
    mp_hal_set_pin_mux(self->gpio_id, ALT_FCT_DAC);

    return MP_OBJ_FROM_PTR(self);
}

static void dac_init(dac_obj_t *self, Dac *dac) {
    // Init DAC
    if (self->initialized == false) {
        #if defined(MCU_SAMD21)

        // Configuration SAMD21
        // Enable APBC clocks and PCHCTRL clocks; GCLK3 at 1 MHz
        PM->APBCMASK.reg |= PM_APBCMASK_DAC;
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_DAC;
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        // Reset DAC registers
        dac->CTRLA.bit.SWRST = 1;
        while (dac->CTRLA.bit.SWRST) {
        }
        dac->CTRLB.reg = DAC_CTRLB_EOEN | DAC_CTRLB_REFSEL(self->vref);
        // Enable DAC and wait to be ready
        dac->CTRLA.bit.ENABLE = 1;
        while (dac->STATUS.bit.SYNCBUSY) {
        }

        #elif defined(MCU_SAMD51)

        // Configuration SAMD51
        // Enable APBD clocks and PCHCTRL clocks; GCLK3 at 8 MHz
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_DAC;
        GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK3 | GCLK_PCHCTRL_CHEN;

        // Reset DAC registers
        dac->CTRLA.bit.SWRST = 1;
        while (dac->CTRLA.bit.SWRST) {
        }
        dac->CTRLB.reg = DAC_CTRLB_REFSEL(dac_vref_table[self->vref]);
        dac->DACCTRL[self->id].reg = DAC_DACCTRL_ENABLE | DAC_DACCTRL_REFRESH(2) | DAC_DACCTRL_CCTRL_CC12M;

        // Enable DAC and wait to be ready
        dac->CTRLA.bit.ENABLE = 1;
        while (dac->SYNCBUSY.bit.ENABLE) {
        }

        #endif // defined SAMD21 or SAMD51
    }
    self->initialized = true;
}

static void dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dac_obj_t *self = self_in;
    mp_printf(print, "DAC(%u, Pin=%q, vref=%d)", self->id, pin_find_by_id(self->gpio_id)->name, self->vref);
}

static mp_obj_t dac_write(mp_obj_t self_in, mp_obj_t value_in) {
    Dac *dac = dac_bases[0]; // Just one DAC
    dac_obj_t *self = self_in;
    int value = mp_obj_get_int(value_in);

    if (value < 0 || value > MAX_DAC_VALUE) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range"));
    }
    // Re-init, if required
    dac_init(self, dac);
    #if defined(MCU_SAMD21)
    dac->DATA.reg = value;
    #elif defined(MCU_SAMD51)
    dac->DATA[self->id].reg = value;
    #endif

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(dac_write_obj, dac_write);

static mp_obj_t dac_write_timed(size_t n_args, const mp_obj_t *args) {
    Dac *dac = dac_bases[0]; // Just one DAC used
    dac_obj_t *self = args[0];
    mp_buffer_info_t src;
    // Re-init, if required
    dac_init(self, dac);

    mp_get_buffer_raise(args[1], &src, MP_BUFFER_READ);
    if (n_args > 3) {
        self->count = mp_obj_get_int(args[3]);
    } else {
        self->count = 1;
    }
    if (src.len >= 2) {
        int freq = mp_obj_get_int(args[2]);
        if (self->dma_channel == -1) {
            self->dma_channel = allocate_dma_channel();
            dma_init();
            dma_register_irq(self->dma_channel, dac_irq_handler);
        }
        if (self->tc_index == -1) {
            self->tc_index = allocate_tc_instance();
        }
        // Configure TC; no need to check the return value
        configure_tc(self->tc_index, freq, 0);
        // Configure DMA for halfword output to the DAC
        #if defined(MCU_SAMD21)

        dma_desc[self->dma_channel].BTCTRL.reg =
            DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT |
            DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_STEPSEL |
            DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val);
        dma_desc[self->dma_channel].BTCNT.reg = src.len / 2;
        dma_desc[self->dma_channel].SRCADDR.reg = (uint32_t)(src.buf) + src.len;
        dma_desc[self->dma_channel].DSTADDR.reg = (uint32_t)(&dac->DATA.reg);
        if (self->count >= 1) {
            dma_desc[self->dma_channel].DESCADDR.reg = 0; // ONE_SHOT
        } else {
            dma_desc[self->dma_channel].DESCADDR.reg = (uint32_t)(&dma_desc[self->dma_channel].BTCTRL.reg);
        }

        DMAC->CHID.reg = self->dma_channel;
        DMAC->CHCTRLA.reg = 0;
        while (DMAC->CHCTRLA.bit.ENABLE) {
        }
        DMAC->CHCTRLB.reg =
            DMAC_CHCTRLB_LVL(0) |
            DMAC_CHCTRLB_TRIGACT_BEAT |
            DMAC_CHCTRLB_TRIGSRC(TC3_DMAC_ID_OVF + 3 * self->tc_index);
        DMAC->CHINTENSET.reg = DMAC_CHINTFLAG_TCMPL;
        DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;

        NVIC_EnableIRQ(DMAC_IRQn);

        #elif defined(MCU_SAMD51)

        dma_desc[self->dma_channel].BTCTRL.reg =
            DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT |
            DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_STEPSEL |
            DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val);
        dma_desc[self->dma_channel].BTCNT.reg = src.len / 2;
        dma_desc[self->dma_channel].SRCADDR.reg = (uint32_t)(src.buf) + src.len;
        dma_desc[self->dma_channel].DSTADDR.reg = (uint32_t)(&dac->DATA[self->id].reg);
        if (self->count >= 1) {
            dma_desc[self->dma_channel].DESCADDR.reg = 0; // ONE_SHOT
        } else {
            dma_desc[self->dma_channel].DESCADDR.reg = (uint32_t)(&dma_desc[self->dma_channel].BTCTRL.reg);
        }

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

        #endif // defined SAMD21 or SAMD51
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dac_write_timed_obj, 3, 4, dac_write_timed);

static mp_obj_t dac_deinit(mp_obj_t self_in) {
    dac_obj_t *self = self_in;
    self->initialized = false;
    // Reset the DAC to lower the current consumption as SAMD21
    dac_bases[0]->CTRLA.bit.SWRST = 1;
    if (self->dma_channel >= 0) {
        dac_stop_dma(self->dma_channel, true);
        free_dma_channel(self->dma_channel);
        self->dma_channel = -1;
    }
    if (self->tc_index >= 0) {
        free_tc_instance(self->tc_index);
        self->tc_index = -1;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dac_deinit_obj, dac_deinit);

static const mp_rom_map_elem_t dac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&dac_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&dac_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_timed), MP_ROM_PTR(&dac_write_timed_obj) },
};

static MP_DEFINE_CONST_DICT(dac_locals_dict, dac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, dac_make_new,
    print, dac_print,
    locals_dict, &dac_locals_dict
    );

#endif
