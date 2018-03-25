/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "timer.h"
#include "dac.h"
#include "dma.h"
#include "pin.h"
#include "genhdr/pins.h"

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

STATIC DAC_HandleTypeDef DAC_Handle;

void dac_init(void) {
    memset(&DAC_Handle, 0, sizeof DAC_Handle);
    DAC_Handle.Instance = DAC;
    DAC_Handle.State = HAL_DAC_STATE_RESET;
    HAL_DAC_Init(&DAC_Handle);
}

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
    } else if (tim->Instance == TIM4) {
        return DAC_TRIGGER_T4_TRGO;
    } else if (tim->Instance == TIM5) {
        return DAC_TRIGGER_T5_TRGO;
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

/******************************************************************************/
// MicroPython bindings

typedef enum {
    DAC_STATE_RESET,
    DAC_STATE_WRITE_SINGLE,
    DAC_STATE_BUILTIN_WAVEFORM,
    DAC_STATE_DMA_WAVEFORM, // should be last enum since we use space beyond it
} pyb_dac_state_t;

typedef struct _pyb_dac_obj_t {
    mp_obj_base_t base;
    uint32_t dac_channel; // DAC_CHANNEL_1 or DAC_CHANNEL_2
    const dma_descr_t *tx_dma_descr;
    uint16_t pin; // GPIO_PIN_4 or GPIO_PIN_5
    uint8_t bits; // 8 or 12
    uint8_t state;
} pyb_dac_obj_t;

STATIC mp_obj_t pyb_dac_init_helper(pyb_dac_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // GPIO configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = self->pin;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC peripheral clock
    #if defined(STM32F4) || defined(STM32F7)
    __DAC_CLK_ENABLE();
    #elif defined(STM32L4)
    __HAL_RCC_DAC1_CLK_ENABLE();
    #else
    #error Unsupported Processor
    #endif

    // stop anything already going on
    __DMA1_CLK_ENABLE();
    DMA_HandleTypeDef DMA_Handle;
    /* Get currently configured dma */
    dma_init_handle(&DMA_Handle, self->tx_dma_descr, (void*)NULL);
    // Need to deinit DMA first
    DMA_Handle.State = HAL_DMA_STATE_READY;
    HAL_DMA_DeInit(&DMA_Handle);

    HAL_DAC_Stop(&DAC_Handle, self->dac_channel);
    if ((self->dac_channel == DAC_CHANNEL_1 && DAC_Handle.DMA_Handle1 != NULL)
            || (self->dac_channel == DAC_CHANNEL_2 && DAC_Handle.DMA_Handle2 != NULL)) {
        HAL_DAC_Stop_DMA(&DAC_Handle, self->dac_channel);
    }

    // set bit resolution
    if (args[0].u_int == 8 || args[0].u_int == 12) {
        self->bits = args[0].u_int;
    } else {
        mp_raise_ValueError("unsupported bits");
    }

    // reset state of DAC
    self->state = DAC_STATE_RESET;

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
    if (MP_OBJ_IS_INT(args[0])) {
        dac_id = mp_obj_get_int(args[0]);
    } else {
        const pin_obj_t *pin = pin_find(args[0]);
        if (pin == &pin_A4) {
            dac_id = 1;
        } else if (pin == &pin_A5) {
            dac_id = 2;
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Pin(%q) doesn't have DAC capabilities", pin->name));
        }
    }

    pyb_dac_obj_t *dac = m_new_obj(pyb_dac_obj_t);
    dac->base.type = &pyb_dac_type;

    if (dac_id == 1) {
        dac->pin = GPIO_PIN_4;
        dac->dac_channel = DAC_CHANNEL_1;
        dac->tx_dma_descr = &dma_DAC_1_TX;
    } else if (dac_id == 2) {
        dac->pin = GPIO_PIN_5;
        dac->dac_channel = DAC_CHANNEL_2;
        dac->tx_dma_descr = &dma_DAC_2_TX;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "DAC(%d) doesn't exist", dac_id));
    }

    // configure the peripheral
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    pyb_dac_init_helper(dac, n_args - 1, args + 1, &kw_args);

    // return object
    return dac;
}

STATIC mp_obj_t pyb_dac_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_dac_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_dac_init_obj, 1, pyb_dac_init);

/// \method deinit()
/// Turn off the DAC, enable other use of pin.
STATIC mp_obj_t pyb_dac_deinit(mp_obj_t self_in) {
    pyb_dac_obj_t *self = self_in;
    if (self->dac_channel == DAC_CHANNEL_1) {
        DAC_Handle.Instance->CR &= ~DAC_CR_EN1;
        DAC_Handle.Instance->CR |= DAC_CR_BOFF1;
    } else {
        DAC_Handle.Instance->CR &= ~DAC_CR_EN2;
        DAC_Handle.Instance->CR |= DAC_CR_BOFF2;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_dac_deinit_obj, pyb_dac_deinit);

#if defined(TIM6)
/// \method noise(freq)
/// Generate a pseudo-random noise signal.  A new random sample is written
/// to the DAC output at the given frequency.
STATIC mp_obj_t pyb_dac_noise(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != DAC_STATE_BUILTIN_WAVEFORM) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = DAC_STATE_BUILTIN_WAVEFORM;
    }

    // set noise wave generation
    HAL_DACEx_NoiseWaveGenerate(&DAC_Handle, self->dac_channel, DAC_LFSRUNMASK_BITS10_0);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_L, 0x7ff0);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_noise_obj, pyb_dac_noise);
#endif

#if defined(TIM6)
/// \method triangle(freq)
/// Generate a triangle wave.  The value on the DAC output changes at
/// the given frequency, and the frequence of the repeating triangle wave
/// itself is 256 (or 1024, need to check) times smaller.
STATIC mp_obj_t pyb_dac_triangle(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != DAC_STATE_BUILTIN_WAVEFORM) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = DAC_STATE_BUILTIN_WAVEFORM;
    }

    // set triangle wave generation
    HAL_DACEx_TriangleWaveGenerate(&DAC_Handle, self->dac_channel, DAC_TRIANGLEAMPLITUDE_1023);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_R, 0x100);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_triangle_obj, pyb_dac_triangle);
#endif

/// \method write(value)
/// Direct access to the DAC output (8 bit only at the moment).
STATIC mp_obj_t pyb_dac_write(mp_obj_t self_in, mp_obj_t val) {
    pyb_dac_obj_t *self = self_in;

    if (self->state != DAC_STATE_WRITE_SINGLE) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_NONE;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = DAC_STATE_WRITE_SINGLE;
    }

    // DAC output is always 12-bit at the hardware level, and we provide support
    // for multiple bit "resolutions" simply by shifting the input value.
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_R,
        mp_obj_get_int(val) << (12 - self->bits));

    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

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
    pyb_dac_obj_t *self = pos_args[0];
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

    __DMA1_CLK_ENABLE();

    DMA_HandleTypeDef DMA_Handle;
    /* Get currently configured dma */
    dma_init_handle(&DMA_Handle, self->tx_dma_descr, (void*)NULL);
    /*
    DMA_Cmd(DMA_Handle->Instance, DISABLE);
    while (DMA_GetCmdStatus(DMA_Handle->Instance) != DISABLE) {
    }

    DAC_Cmd(self->dac_channel, DISABLE);
    */

    /*
    // DAC channel configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1; // unused, but need to set it to a valid value
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(self->dac_channel, &DAC_InitStructure);
    */

    // Need to deinit DMA first
    DMA_Handle.State = HAL_DMA_STATE_READY;
    HAL_DMA_DeInit(&DMA_Handle);

    if (self->bits == 8) {
        DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    } else {
        DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    }
    DMA_Handle.Init.Mode = args[2].u_int;
    HAL_DMA_Init(&DMA_Handle);

    if (self->dac_channel == DAC_CHANNEL_1) {
        __HAL_LINKDMA(&DAC_Handle, DMA_Handle1, DMA_Handle);
    } else {
        __HAL_LINKDMA(&DAC_Handle, DMA_Handle2, DMA_Handle);
    }

    DAC_Handle.Instance = DAC;
    DAC_Handle.State = HAL_DAC_STATE_RESET;
    HAL_DAC_Init(&DAC_Handle);

    if (self->state != DAC_STATE_DMA_WAVEFORM + dac_trigger) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = dac_trigger;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = DAC_STATE_DMA_WAVEFORM + dac_trigger;
    }

    if (self->bits == 8) {
        HAL_DAC_Start_DMA(&DAC_Handle, self->dac_channel,
            (uint32_t*)bufinfo.buf, bufinfo.len, DAC_ALIGN_8B_R);
    } else {
        HAL_DAC_Start_DMA(&DAC_Handle, self->dac_channel,
            (uint32_t*)bufinfo.buf, bufinfo.len / 2, DAC_ALIGN_12B_R);
    }

    /*
    // enable DMA stream
    DMA_Cmd(DMA_Handle->Instance, ENABLE);
    while (DMA_GetCmdStatus(DMA_Handle->Instance) == DISABLE) {
    }

    // enable DAC channel
    DAC_Cmd(self->dac_channel, ENABLE);

    // enable DMA for DAC channel
    DAC_DMACmd(self->dac_channel, ENABLE);
    */

    //printf("DMA: %p %lu\n", bufinfo.buf, bufinfo.len);

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
    .make_new = pyb_dac_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_dac_locals_dict,
};

#endif // MICROPY_HW_ENABLE_DAC
