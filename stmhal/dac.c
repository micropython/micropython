/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "stm32f4xx_hal.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "timer.h"
#include "dac.h"
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

#if MICROPY_HW_ENABLE_DAC

STATIC DAC_HandleTypeDef DAC_Handle;

void dac_init(void) {
    memset(&DAC_Handle, 0, sizeof DAC_Handle);
    DAC_Handle.Instance = DAC;
    DAC_Handle.State = HAL_DAC_STATE_RESET;
    HAL_DAC_Init(&DAC_Handle);
}

STATIC void TIM6_Config(uint freq) {
    // Init TIM6 at the required frequency (in Hz)
    timer_tim6_init(freq);

    // TIM6 TRGO selection
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&TIM6_Handle, &config);

    // TIM6 start counter
    HAL_TIM_Base_Start(&TIM6_Handle);
}

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_dac_obj_t {
    mp_obj_base_t base;
    uint32_t dac_channel; // DAC_CHANNEL_1 or DAC_CHANNEL_2
    DMA_Stream_TypeDef *dma_stream; // DMA1_Stream5 or DMA1_Stream6
    mp_uint_t state;
} pyb_dac_obj_t;

// create the dac object
// currently support either DAC1 on X5 (id = 1) or DAC2 on X6 (id = 2)

/// \classmethod \constructor(port)
/// Construct a new DAC object.
///
/// `port` can be a pin object, or an integer (1 or 2).
/// DAC(1) is on pin X5 and DAC(2) is on pin X6.
STATIC mp_obj_t pyb_dac_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

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
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s does not have DAC capabilities", qstr_str(pin->name)));
        }
    }

    pyb_dac_obj_t *dac = m_new_obj(pyb_dac_obj_t);
    dac->base.type = &pyb_dac_type;

    uint32_t pin;
    if (dac_id == 1) {
        pin = GPIO_PIN_4;
        dac->dac_channel = DAC_CHANNEL_1;
        dac->dma_stream = DMA1_Stream5;
    } else if (dac_id == 2) {
        pin = GPIO_PIN_5;
        dac->dac_channel = DAC_CHANNEL_2;
        dac->dma_stream = DMA1_Stream6;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "DAC %d does not exist", dac_id));
    }

    // GPIO configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC peripheral clock
    __DAC_CLK_ENABLE();

    // stop anything already going on
    HAL_DAC_Stop(&DAC_Handle, dac->dac_channel);
    if ((dac->dac_channel == DAC_CHANNEL_1 && DAC_Handle.DMA_Handle1 != NULL)
            || (dac->dac_channel == DAC_CHANNEL_2 && DAC_Handle.DMA_Handle2 != NULL)) {
        HAL_DAC_Stop_DMA(&DAC_Handle, dac->dac_channel);
    }

    dac->state = 0;

    // return object
    return dac;
}

/// \method noise(freq)
/// Generate a pseudo-random noise signal.  A new random sample is written
/// to the DAC output at the given frequency.
STATIC mp_obj_t pyb_dac_noise(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != 2) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 2;
    }

    // set noise wave generation
    HAL_DACEx_NoiseWaveGenerate(&DAC_Handle, self->dac_channel, DAC_LFSRUNMASK_BITS10_0);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_L, 0x7ff0);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_noise_obj, pyb_dac_noise);

/// \method triangle(freq)
/// Generate a triangle wave.  The value on the DAC output changes at
/// the given frequency, and the frequence of the repeating triangle wave
/// itself is 256 (or 1024, need to check) times smaller.
STATIC mp_obj_t pyb_dac_triangle(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != 2) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 2;
    }

    // set triangle wave generation
    HAL_DACEx_TriangleWaveGenerate(&DAC_Handle, self->dac_channel, DAC_TRIANGLEAMPLITUDE_1023);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_R, 0x100);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_triangle_obj, pyb_dac_triangle);

/// \method write(value)
/// Direct access to the DAC output (8 bit only at the moment).
STATIC mp_obj_t pyb_dac_write(mp_obj_t self_in, mp_obj_t val) {
    pyb_dac_obj_t *self = self_in;

    if (self->state != 1) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_NONE;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 1;
    }

    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_8B_R, mp_obj_get_int(val));
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_write_obj, pyb_dac_write);

/// \method write_timed(data, freq, *, mode=DAC.NORMAL)
/// Initiates a burst of RAM to DAC using a DMA transfer.
/// The input data is treated as an array of bytes (8 bit data).
///
/// `mode` can be `DAC.NORMAL` or `DAC.CIRCULAR`.
///
/// TIM6 is used to control the frequency of the transfer.
// TODO add callback argument, to call when transfer is finished
// TODO add double buffer argument
STATIC const mp_arg_t pyb_dac_write_timed_args[] = {
    { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DMA_NORMAL} },
};
#define PYB_DAC_WRITE_TIMED_NUM_ARGS MP_ARRAY_SIZE(pyb_dac_write_timed_args)

mp_obj_t pyb_dac_write_timed(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_dac_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_DAC_WRITE_TIMED_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_DAC_WRITE_TIMED_NUM_ARGS, pyb_dac_write_timed_args, vals);

    // get the data to write
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(vals[0].u_obj, &bufinfo, MP_BUFFER_READ);

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(vals[1].u_int);

    __DMA1_CLK_ENABLE();

    /*
    DMA_Cmd(self->dma_stream, DISABLE);
    while (DMA_GetCmdStatus(self->dma_stream) != DISABLE) {
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

    // DMA1_Stream[67] channel7 configuration
    DMA_HandleTypeDef DMA_Handle;
    DMA_Handle.Instance = self->dma_stream;

    // Need to deinit DMA first
    DMA_Handle.State = HAL_DMA_STATE_READY;
    HAL_DMA_DeInit(&DMA_Handle);

    DMA_Handle.Init.Channel = DMA_CHANNEL_7;
    DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    DMA_Handle.Init.Mode = vals[2].u_int;
    DMA_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
    DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&DMA_Handle);

    if (self->dac_channel == DAC_CHANNEL_1) {
        __HAL_LINKDMA(&DAC_Handle, DMA_Handle1, DMA_Handle);
    } else {
        __HAL_LINKDMA(&DAC_Handle, DMA_Handle2, DMA_Handle);
    }

    DAC_Handle.Instance = DAC;
    DAC_Handle.State = HAL_DAC_STATE_RESET;
    HAL_DAC_Init(&DAC_Handle);

    if (self->state != 3) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 3;
    }

    HAL_DAC_Start_DMA(&DAC_Handle, self->dac_channel, (uint32_t*)bufinfo.buf, bufinfo.len, DAC_ALIGN_8B_R);

    /*
    // enable DMA stream
    DMA_Cmd(self->dma_stream, ENABLE);
    while (DMA_GetCmdStatus(self->dma_stream) == DISABLE) {
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

STATIC const mp_map_elem_t pyb_dac_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_noise), (mp_obj_t)&pyb_dac_noise_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_triangle), (mp_obj_t)&pyb_dac_triangle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&pyb_dac_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_timed), (mp_obj_t)&pyb_dac_write_timed_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_NORMAL),      MP_OBJ_NEW_SMALL_INT(DMA_NORMAL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CIRCULAR),    MP_OBJ_NEW_SMALL_INT(DMA_CIRCULAR) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_dac_locals_dict, pyb_dac_locals_dict_table);

const mp_obj_type_t pyb_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .make_new = pyb_dac_make_new,
    .locals_dict = (mp_obj_t)&pyb_dac_locals_dict,
};

#endif // MICROPY_HW_ENABLE_DAC
