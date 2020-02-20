/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "timer.h"
#include "dac.h"
#include "dma.h"
#include "pin.h"

/// \moduleref pyb
/// \class DAC - digital to analog conversion
///
/// The DAC is used to output analog values (a specific voltage) on pin X5 or pin X6.
/// The voltage will be between 0 and 3.3V.
///
/// *This module will undergo changes to the API.*
///
/// Example usage:
///
///     from pyb import DAC
///
///     dac = DAC(1)            # create DAC 1 on pin X5
///     dac.write(128)          # write a value to the DAC (makes X5 1.65V)
///
/// To output a continuous sine-wave:
///
///     import math
///     from pyb import DAC
///
///     # create a buffer containing a sine-wave
///     buf = bytearray(100)
///     for i in range(len(buf)):
///         buf[i] = 128 + int(127 * math.sin(2 * math.pi * i / len(buf)))
///
///     # output the sine-wave at 400Hz
///     dac = DAC(1)
///     dac.write_timed(buf, 400 * len(buf), mode=DAC.CIRCULAR)

#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC

#if defined(STM32H7)
#define DAC DAC1
#endif

#if defined(TIM6)
STATIC void TIM6_Config(uint freq) {
    // Init TIM6 at the required frequency (in Hz)
    TIM_HandleTypeDef *tim = timer_tim6_init(freq);

    // TIM6 TRGO selection
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(tim, &config);

    // TIM6 start counter
    HAL_TIM_Base_Start(tim);
}
#endif

STATIC uint32_t TIMx_Config(mp_obj_t timer) {
    // TRGO selection to trigger DAC
    TIM_HandleTypeDef *tim = pyb_timer_get_handle(timer);
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(tim, &config);

    // work out the trigger channel (only certain ones are supported)
    if (tim->Instance == TIM2) {
        return DAC_TRIGGER_T2_TRGO;
    #if defined(TIM4)
    } else if (tim->Instance == TIM4) {
        return DAC_TRIGGER_T4_TRGO;
    #endif
    #if defined(TIM5)
    } else if (tim->Instance == TIM5) {
        return DAC_TRIGGER_T5_TRGO;
    #endif
    #if defined(TIM6)
    } else if (tim->Instance == TIM6) {
        return DAC_TRIGGER_T6_TRGO;
    #endif
    #if defined(TIM7)
    } else if (tim->Instance == TIM7) {
        return DAC_TRIGGER_T7_TRGO;
    #endif
    #if defined(TIM8)
    } else if (tim->Instance == TIM8) {
        return DAC_TRIGGER_T8_TRGO;
    #endif
    } else {
        mp_raise_ValueError("Timer does not support DAC triggering");
    }
}

STATIC void dac_deinit(uint32_t dac_channel) {
    DAC->CR &= ~(DAC_CR_EN1 << dac_channel);
    #if defined(STM32H7) || defined(STM32L4)
    DAC->MCR = (DAC->MCR & ~(DAC_MCR_MODE1_Msk << dac_channel)) | (DAC_OUTPUTBUFFER_DISABLE << dac_channel);
    #else
    DAC->CR |= DAC_CR_BOFF1 << dac_channel;
    #endif
}

STATIC void dac_config_channel(uint32_t dac_channel, uint32_t trig, uint32_t outbuf) {
    DAC->CR &= ~(DAC_CR_EN1 << dac_channel);
    uint32_t cr_off = DAC_CR_DMAEN1 | DAC_CR_MAMP1 | DAC_CR_WAVE1 | DAC_CR_TSEL1 | DAC_CR_TEN1;
    uint32_t cr_on = trig;
    #if defined(STM32H7) || defined(STM32L4)
    DAC->MCR = (DAC->MCR & ~(DAC_MCR_MODE1_Msk << dac_channel)) | (outbuf << dac_channel);
    #else
    cr_off |= DAC_CR_BOFF1;
    cr_on |= outbuf;
    #endif
    DAC->CR = (DAC->CR & ~(cr_off << dac_channel)) | (cr_on << dac_channel);
}

STATIC void dac_set_value(uint32_t dac_channel, uint32_t align, uint32_t value) {
    uint32_t base;
    if (dac_channel == DAC_CHANNEL_1) {
        base = (uint32_t)&DAC->DHR12R1;
    #if !defined(STM32L452xx)
    } else {
        base = (uint32_t)&DAC->DHR12R2;
    #endif
    }
    *(volatile uint32_t*)(base + align) = value;
}

STATIC void dac_start(uint32_t dac_channel) {
    DAC->CR |= DAC_CR_EN1 << dac_channel;
}

STATIC void dac_start_dma(uint32_t dac_channel, const dma_descr_t *dma_descr, uint32_t dma_mode, uint32_t bit_size, uint32_t dac_align, size_t len, void *buf) {
    uint32_t dma_align;
    if (bit_size == 8) {
        dma_align = DMA_MDATAALIGN_BYTE | DMA_PDATAALIGN_BYTE;
    } else {
        dma_align = DMA_MDATAALIGN_HALFWORD | DMA_PDATAALIGN_HALFWORD;
    }

    uint32_t base;
    if (dac_channel == DAC_CHANNEL_1) {
        base = (uint32_t)&DAC->DHR12R1;
    #if !defined(STM32L452xx)
    } else {
        base = (uint32_t)&DAC->DHR12R2;
    #endif
    }

    dma_nohal_deinit(dma_descr);
    dma_nohal_init(dma_descr, DMA_MEMORY_TO_PERIPH | dma_mode | dma_align);
    dma_nohal_start(dma_descr, (uint32_t)buf, base + dac_align, len);

    DAC->CR |= DAC_CR_EN1 << dac_channel;
}

/******************************************************************************/
// MicroPython bindings

typedef struct _pyb_dac_obj_t {
    mp_obj_base_t base;
    uint8_t dac_channel; // DAC_CHANNEL_1 or DAC_CHANNEL_2. STM32L452 only has CHANNEL_1.
    uint8_t bits; // 8 or 12
    uint8_t outbuf_single;
    uint8_t outbuf_waveform;
} pyb_dac_obj_t;

STATIC pyb_dac_obj_t pyb_dac_obj[2];

STATIC void pyb_dac_reconfigure(pyb_dac_obj_t *self, uint32_t cr, uint32_t outbuf, uint32_t value) {
    bool restart = false;
    const uint32_t cr_mask = DAC_CR_DMAEN1 | DAC_CR_MAMP1 | DAC_CR_WAVE1 | DAC_CR_TSEL1 | DAC_CR_TEN1 | DAC_CR_EN1;
    if (((DAC->CR >> self->dac_channel) & cr_mask) != (cr | DAC_CR_EN1)) {
        const dma_descr_t *tx_dma_descr;
        if (self->dac_channel == DAC_CHANNEL_1) {
            tx_dma_descr = &dma_DAC_1_TX;
        #if !defined(STM32L452xx)
        } else {
            tx_dma_descr = &dma_DAC_2_TX;
        #endif
        }
        dma_nohal_deinit(tx_dma_descr);
        dac_config_channel(self->dac_channel, cr, outbuf);
        restart = true;
    }
    dac_set_value(self->dac_channel, DAC_ALIGN_12B_R, value);
    if (restart) {
        dac_start(self->dac_channel);
    }
}

STATIC void pyb_dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "DAC(%u, bits=%u)",
        self->dac_channel == DAC_CHANNEL_1 ? 1 : 2,
        self->bits);
}

STATIC mp_obj_t pyb_dac_init_helper(pyb_dac_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_buffering, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // GPIO configuration
    mp_hal_pin_obj_t pin;
    if (self->dac_channel == DAC_CHANNEL_1) {
        pin = pin_A4;
    #if !defined(STM32L452xx)
    } else {
        pin = pin_A5;
    #endif
    }
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);

    // DAC peripheral clock
    #if defined(STM32F4) || defined(STM32F7)
    __DAC_CLK_ENABLE();
    #elif defined(STM32H7)
    __HAL_RCC_DAC12_CLK_ENABLE();
    #elif defined(STM32F0) || defined(STM32L4)
    __HAL_RCC_DAC1_CLK_ENABLE();
    #else
    #error Unsupported Processor
    #endif

    // Stop the DAC in case it was already running
    DAC->CR &= ~(DAC_CR_EN1 << self->dac_channel);

    // set bit resolution
    if (args[0].u_int == 8 || args[0].u_int == 12) {
        self->bits = args[0].u_int;
    } else {
        mp_raise_ValueError("unsupported bits");
    }

    // set output buffer config
    if (args[1].u_obj == mp_const_none) {
        // due to legacy, default values differ for single and waveform outputs
        self->outbuf_single = DAC_OUTPUTBUFFER_DISABLE;
        self->outbuf_waveform = DAC_OUTPUTBUFFER_ENABLE;
    } else if (mp_obj_is_true(args[1].u_obj)) {
        self->outbuf_single = DAC_OUTPUTBUFFER_ENABLE;
        self->outbuf_waveform = DAC_OUTPUTBUFFER_ENABLE;
    } else {
        self->outbuf_single = DAC_OUTPUTBUFFER_DISABLE;
        self->outbuf_waveform = DAC_OUTPUTBUFFER_DISABLE;
    }

    return mp_const_none;
}

// create the dac object
// currently support either DAC1 on X5 (id = 1) or DAC2 on X6 (id = 2)

/// \classmethod \constructor(port)
/// Construct a new DAC object.
///
/// `port` can be a pin object, or an integer (1 or 2).
/// DAC(1) is on pin X5 and DAC(2) is on pin X6.
STATIC mp_obj_t pyb_dac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get pin/channel to output on
    mp_int_t dac_id;
    if (mp_obj_is_int(args[0])) {
        dac_id = mp_obj_get_int(args[0]);
    } else {
        const pin_obj_t *pin = pin_find(args[0]);
        if (pin == pin_A4) {
            dac_id = 1;
        } else if (pin == pin_A5) {
            dac_id = 2;
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, "Pin(%q) doesn't have DAC capabilities", pin->name);
        }
    }

    uint32_t dac_channel;
    if (dac_id == 1) {
        dac_channel = DAC_CHANNEL_1;
    #if !defined(STM32L452xx)
    } else if (dac_id == 2) {
        dac_channel = DAC_CHANNEL_2;
    #endif
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, "DAC(%d) doesn't exist", dac_id);
    }

    pyb_dac_obj_t *dac = &pyb_dac_obj[dac_id - 1];
    dac->base.type = &pyb_dac_type;
    dac->dac_channel = dac_channel;

    if (dac->bits == 0 || n_args > 1 || n_kw > 0) {
        // configure the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_dac_init_helper(dac, n_args - 1, args + 1, &kw_args);
    }

    // return object
    return MP_OBJ_FROM_PTR(dac);
}

STATIC mp_obj_t pyb_dac_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_dac_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_dac_init_obj, 1, pyb_dac_init);

/// \method deinit()
/// Turn off the DAC, enable other use of pin.
STATIC mp_obj_t pyb_dac_deinit(mp_obj_t self_in) {
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    dac_deinit(self->dac_channel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_dac_deinit_obj, pyb_dac_deinit);

#if defined(TIM6)
/// \method noise(freq)
/// Generate a pseudo-random noise signal.  A new random sample is written
/// to the DAC output at the given frequency.
STATIC mp_obj_t pyb_dac_noise(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    // Configure DAC in noise mode with trigger via TIM6
    uint32_t cr = DAC_LFSRUNMASK_BITS11_0 | DAC_CR_WAVE1_0 | DAC_TRIGGER_T6_TRGO;
    pyb_dac_reconfigure(self, cr, self->outbuf_waveform, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_noise_obj, pyb_dac_noise);
#endif

#if defined(TIM6)
/// \method triangle(freq)
/// Generate a triangle wave.  The value on the DAC output changes at
/// the given frequency, and the frequency of the repeating triangle wave
/// itself is 8192 times smaller.
STATIC mp_obj_t pyb_dac_triangle(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    // Configure DAC in full-scale triangle mode with trigger via TIM6
    uint32_t cr = DAC_TRIANGLEAMPLITUDE_4095 | DAC_CR_WAVE1_1 | DAC_TRIGGER_T6_TRGO;
    pyb_dac_reconfigure(self, cr, self->outbuf_waveform, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_triangle_obj, pyb_dac_triangle);
#endif

/// \method write(value)
/// Direct access to the DAC output (8 bit only at the moment).
STATIC mp_obj_t pyb_dac_write(mp_obj_t self_in, mp_obj_t val) {
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // DAC output is always 12-bit at the hardware level, and we provide support
    // for multiple bit "resolutions" simply by shifting the input value.
    uint32_t cr = DAC_TRIGGER_NONE;
    uint32_t value = mp_obj_get_int(val) << (12 - self->bits);
    pyb_dac_reconfigure(self, cr, self->outbuf_single, value);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_write_obj, pyb_dac_write);

#if defined(TIM6)
/// \method write_timed(data, freq, *, mode=DAC.NORMAL)
/// Initiates a burst of RAM to DAC using a DMA transfer.
/// The input data is treated as an array of bytes (8 bit data).
///
/// `freq` can be an integer specifying the frequency to write the DAC
/// samples at, using Timer(6).  Or it can be an already-initialised
/// Timer object which is used to trigger the DAC sample.  Valid timers
/// are 2, 4, 5, 6, 7 and 8.
///
/// `mode` can be `DAC.NORMAL` or `DAC.CIRCULAR`.
///
// TODO add callback argument, to call when transfer is finished
// TODO add double buffer argument
//
// TODO reconsider API, eg: write_trig(data, *, trig=None, loop=False)
// Then trigger can be timer (preinitialised with desired freq) or pin (extint9),
// and we can reuse the same timer for both DACs (and maybe also ADC) without
// setting the freq twice.
// Can still do 1-liner: dac.write_trig(buf, trig=Timer(6, freq=100), loop=True)
mp_obj_t pyb_dac_write_timed(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DMA_NORMAL} },
    };

    // parse args
    pyb_dac_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the data to write
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0].u_obj, &bufinfo, MP_BUFFER_READ);

    uint32_t dac_trigger;
    if (mp_obj_is_integer(args[1].u_obj)) {
        // set TIM6 to trigger the DAC at the given frequency
        TIM6_Config(mp_obj_get_int(args[1].u_obj));
        dac_trigger = DAC_TRIGGER_T6_TRGO;
    } else {
        // set the supplied timer to trigger the DAC (timer should be initialised)
        dac_trigger = TIMx_Config(args[1].u_obj);
    }

    dac_config_channel(self->dac_channel, DAC_CR_DMAEN1 | dac_trigger, self->outbuf_waveform);

    const dma_descr_t *tx_dma_descr;
    if (self->dac_channel == DAC_CHANNEL_1) {
        tx_dma_descr = &dma_DAC_1_TX;
    #if !defined(STM32L452xx)
    } else {
        tx_dma_descr = &dma_DAC_2_TX;
    #endif
    }

    uint32_t align;
    if (self->bits == 8) {
        align = DAC_ALIGN_8B_R;
    } else {
        align = DAC_ALIGN_12B_R;
        bufinfo.len /= 2;
    }

    dac_start_dma(self->dac_channel, tx_dma_descr, args[2].u_int, self->bits, align, bufinfo.len, bufinfo.buf);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_dac_write_timed_obj, 1, pyb_dac_write_timed);
#endif

STATIC const mp_rom_map_elem_t pyb_dac_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_dac_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_dac_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&pyb_dac_write_obj) },
    #if defined(TIM6)
    { MP_ROM_QSTR(MP_QSTR_noise), MP_ROM_PTR(&pyb_dac_noise_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&pyb_dac_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_timed), MP_ROM_PTR(&pyb_dac_write_timed_obj) },
    #endif

    // class constants
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(DMA_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_CIRCULAR), MP_ROM_INT(DMA_CIRCULAR) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_dac_locals_dict, pyb_dac_locals_dict_table);

const mp_obj_type_t pyb_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .print = pyb_dac_print,
    .make_new = pyb_dac_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_dac_locals_dict,
};

#endif // MICROPY_HW_ENABLE_DAC
