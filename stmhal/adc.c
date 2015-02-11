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
#include <stm32f4xx_hal.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "adc.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "timer.h"

/// \moduleref pyb
/// \class ADC - analog to digital conversion: read analog values on a pin
///
/// Usage:
///
///     adc = pyb.ADC(pin)              # create an analog object from a pin
///     val = adc.read()                # read an analog value
///
///     adc = pyb.ADCAll(resolution)    # creale an ADCAll object
///     val = adc.read_channel(channel) # read the given channel
///     val = adc.read_core_temp()      # read MCU temperature
///     val = adc.read_core_vbat()      # read MCU VBAT
///     val = adc.read_core_vref()      # read MCU VREF

/* ADC defintions */
#define ADCx                    (ADC1)
#define ADCx_CLK_ENABLE         __ADC1_CLK_ENABLE
#define ADC_NUM_CHANNELS        (19)
#define ADC_NUM_GPIO_CHANNELS   (16)

#if defined(STM32F405xx) || defined(STM32F415xx) || \
    defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F401xC) || defined(STM32F401xE)
#define VBAT_DIV (2)
#elif defined(STM32F427xx) || defined(STM32F429xx) || \
      defined(STM32F437xx) || defined(STM32F439xx)
#define VBAT_DIV (4)
#endif

/* Core temperature sensor definitions */
#define CORE_TEMP_V25          (943)  /* (0.76v/3.3v)*(2^ADC resoultion) */
#define CORE_TEMP_AVG_SLOPE    (3)    /* (2.5mv/3.3v)*(2^ADC resoultion) */

typedef struct _pyb_obj_adc_t {
    mp_obj_base_t base;
    mp_obj_t pin_name;
    int channel;
    ADC_HandleTypeDef handle;
} pyb_obj_adc_t;

STATIC void adc_init_single(pyb_obj_adc_t *adc_obj) {
    if (!IS_ADC_CHANNEL(adc_obj->channel)) {
        return;
    }

    if (adc_obj->channel < ADC_NUM_GPIO_CHANNELS) {
      // Channels 0-16 correspond to real pins. Configure the GPIO pin in
      // ADC mode.
      const pin_obj_t *pin = pin_adc1[adc_obj->channel];
      GPIO_InitTypeDef GPIO_InitStructure;
      GPIO_InitStructure.Pin = pin->pin_mask;
      GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStructure.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);
    }

    ADCx_CLK_ENABLE();

    ADC_HandleTypeDef *adcHandle = &adc_obj->handle;
    adcHandle->Instance                   = ADCx;
    adcHandle->Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
    adcHandle->Init.Resolution            = ADC_RESOLUTION12b;
    adcHandle->Init.ScanConvMode          = DISABLE;
    adcHandle->Init.ContinuousConvMode    = DISABLE;
    adcHandle->Init.DiscontinuousConvMode = DISABLE;
    adcHandle->Init.NbrOfDiscConversion   = 0;
    adcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    adcHandle->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adcHandle->Init.NbrOfConversion       = 1;
    adcHandle->Init.DMAContinuousRequests = DISABLE;
    adcHandle->Init.EOCSelection          = DISABLE;

    HAL_ADC_Init(adcHandle);
}

STATIC void adc_config_channel(pyb_obj_adc_t *adc_obj) {
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = adc_obj->channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    sConfig.Offset = 0;

    HAL_ADC_ConfigChannel(&adc_obj->handle, &sConfig);
}

STATIC uint32_t adc_read_channel(ADC_HandleTypeDef *adcHandle) {
    uint32_t rawValue = 0;

    HAL_ADC_Start(adcHandle);
    if (HAL_ADC_PollForConversion(adcHandle, 10) == HAL_OK && HAL_ADC_GetState(adcHandle) == HAL_ADC_STATE_EOC_REG) {
        rawValue = HAL_ADC_GetValue(adcHandle);
    }
    HAL_ADC_Stop(adcHandle);

    return rawValue;
}

/******************************************************************************/
/* Micro Python bindings : adc object (single channel)                        */

STATIC void adc_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_obj_adc_t *self = self_in;
    print(env, "<ADC on ");
    mp_obj_print_helper(print, env, self->pin_name, PRINT_STR);
    print(env, " channel=%lu>", self->channel);
}

/// \classmethod \constructor(pin)
/// Create an ADC object associated with the given pin.
/// This allows you to then read analog values on that pin.
STATIC mp_obj_t adc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin name
    mp_obj_t pin_obj = args[0];

    uint32_t channel;

    if (MP_OBJ_IS_INT(pin_obj)) {
        channel = mp_obj_get_int(pin_obj);
    } else {
        const pin_obj_t *pin = pin_find(pin_obj);
        if ((pin->adc_num & PIN_ADC1) == 0) {
            // No ADC1 function on that pin
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s does not have ADC capabilities", qstr_str(pin->name)));
        }
        channel = pin->adc_channel;
    }

    if (!IS_ADC_CHANNEL(channel)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "not a valid ADC Channel: %d", channel));
    }
    if (pin_adc1[channel] == NULL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "channel %d not available on this board", channel));
    }

    pyb_obj_adc_t *o = m_new_obj(pyb_obj_adc_t);
    memset(o, 0, sizeof(*o));
    o->base.type = &pyb_adc_type;
    o->pin_name = pin_obj;
    o->channel = channel;
    adc_init_single(o);

    return o;
}

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
STATIC mp_obj_t adc_read(mp_obj_t self_in) {
    pyb_obj_adc_t *self = self_in;

    adc_config_channel(self);
    uint32_t data = adc_read_channel(&self->handle);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

/// \method read_timed(buf, freq)
/// Read analog values into the given buffer at the given frequency. Buffer
/// can be bytearray or array.array for example. If a buffer with 8-bit elements
/// is used, sample resolution will be reduced to 8 bits.
///
/// Example:
///
///     adc = pyb.ADC(pyb.Pin.board.X19)    # create an ADC on pin X19
///     buf = bytearray(100)                # create a buffer of 100 bytes
///     adc.read_timed(buf, 10)             # read analog values into buf at 10Hz
///                                         #   this will take 10 seconds to finish
///     for val in buf:                     # loop over all values
///         print(val)                      # print the value out
///
/// This function does not allocate any memory.
STATIC mp_obj_t adc_read_timed(mp_obj_t self_in, mp_obj_t buf_in, mp_obj_t freq_in) {
    pyb_obj_adc_t *self = self_in;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    int typesize = mp_binary_get_size('@', bufinfo.typecode, NULL);

    // Init TIM6 at the required frequency (in Hz)
    timer_tim6_init(mp_obj_get_int(freq_in));

    // Start timer
    HAL_TIM_Base_Start(&TIM6_Handle);

    // This uses the timer in polling mode to do the sampling
    // We could use DMA, but then we can't convert the values correctly for the buffer
    adc_config_channel(self);
    for (uint index = 0; index < bufinfo.len; index++) {
        // Wait for the timer to trigger
        while (__HAL_TIM_GET_FLAG(&TIM6_Handle, TIM_FLAG_UPDATE) == RESET) {
        }
        __HAL_TIM_CLEAR_FLAG(&TIM6_Handle, TIM_FLAG_UPDATE);
        uint value = adc_read_channel(&self->handle);
        if (typesize == 1) {
            value >>= 4;
        }
        mp_binary_set_val_array_from_int(bufinfo.typecode, bufinfo.buf, index, value);
    }

    // Stop timer
    HAL_TIM_Base_Stop(&TIM6_Handle);

    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(adc_read_timed_obj, adc_read_timed);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&adc_read_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_timed), (mp_obj_t)&adc_read_timed_obj},
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};

/******************************************************************************/
/* adc all object                                                             */

typedef struct _pyb_adc_all_obj_t {
    mp_obj_base_t base;
    ADC_HandleTypeDef handle;
} pyb_adc_all_obj_t;

void adc_init_all(pyb_adc_all_obj_t *adc_all, uint32_t resolution) {

    switch (resolution) {
        case 6:  resolution = ADC_RESOLUTION6b;  break;
        case 8:  resolution = ADC_RESOLUTION8b;  break;
        case 10: resolution = ADC_RESOLUTION10b; break;
        case 12: resolution = ADC_RESOLUTION12b; break;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "resolution %d not supported", resolution));
    }

    for (uint32_t channel = 0; channel < ADC_NUM_GPIO_CHANNELS; channel++) {
        // Channels 0-16 correspond to real pins. Configure the GPIO pin in
        // ADC mode.
        const pin_obj_t *pin = pin_adc1[channel];
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.Pin = pin->pin_mask;
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);
    }

    ADCx_CLK_ENABLE();

    ADC_HandleTypeDef *adcHandle = &adc_all->handle;
    adcHandle->Instance = ADCx;
    adcHandle->Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
    adcHandle->Init.Resolution            = resolution;
    adcHandle->Init.ScanConvMode          = DISABLE;
    adcHandle->Init.ContinuousConvMode    = DISABLE;
    adcHandle->Init.DiscontinuousConvMode = DISABLE;
    adcHandle->Init.NbrOfDiscConversion   = 0;
    adcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    adcHandle->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adcHandle->Init.NbrOfConversion       = 1;
    adcHandle->Init.DMAContinuousRequests = DISABLE;
    adcHandle->Init.EOCSelection          = DISABLE;

    HAL_ADC_Init(adcHandle);
}

uint32_t adc_config_and_read_channel(ADC_HandleTypeDef *adcHandle, uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    sConfig.Offset = 0;
    HAL_ADC_ConfigChannel(adcHandle, &sConfig);

    return adc_read_channel(adcHandle);
}

int adc_get_resolution(ADC_HandleTypeDef *adcHandle) {
    uint32_t res_reg = __HAL_ADC_GET_RESOLUTION(adcHandle);

    switch (res_reg) {
        case ADC_RESOLUTION6b:  return 6;
        case ADC_RESOLUTION8b:  return 8;
        case ADC_RESOLUTION10b: return 10;
    }
    return 12;
}

int adc_read_core_temp(ADC_HandleTypeDef *adcHandle) {
    int32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_TEMPSENSOR);

    // Note: constants assume 12-bit resolution, so we scale the raw value to
    //       be 12-bits.
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    return ((raw_value - CORE_TEMP_V25) / CORE_TEMP_AVG_SLOPE) + 25;
}

#if MICROPY_PY_BUILTINS_FLOAT
float adc_read_core_vbat(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_VBAT);

    // Note: constants assume 12-bit resolution, so we scale the raw value to
    //       be 12-bits.
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    // multiplier is 3.3/4095
    return raw_value * VBAT_DIV * 0.8058608058608059e-3f;
}

float adc_read_core_vref(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_VREFINT);

    // Note: constants assume 12-bit resolution, so we scale the raw value to
    //       be 12-bits.
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    // multiplier is 3.3/4095
    return raw_value * 0.8058608058608059e-3f;
}
#endif

/******************************************************************************/
/* Micro Python bindings : adc_all object                                     */

STATIC mp_obj_t adc_all_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // make ADCAll object
    pyb_adc_all_obj_t *o = m_new_obj(pyb_adc_all_obj_t);
    o->base.type = &pyb_adc_all_type;
    adc_init_all(o, mp_obj_get_int(args[0])); // args[0] is the resolution

    return o;
}

STATIC mp_obj_t adc_all_read_channel(mp_obj_t self_in, mp_obj_t channel) {
    pyb_adc_all_obj_t *self = self_in;
    uint32_t chan = mp_obj_get_int(channel);
    uint32_t data = adc_config_and_read_channel(&self->handle, chan);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(adc_all_read_channel_obj, adc_all_read_channel);

STATIC mp_obj_t adc_all_read_core_temp(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = self_in;
    int data  = adc_read_core_temp(&self->handle);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_temp_obj, adc_all_read_core_temp);

#if MICROPY_PY_BUILTINS_FLOAT
STATIC mp_obj_t adc_all_read_core_vbat(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = self_in;
    float data = adc_read_core_vbat(&self->handle);
    return mp_obj_new_float(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vbat_obj, adc_all_read_core_vbat);

STATIC mp_obj_t adc_all_read_core_vref(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = self_in;
    float data  = adc_read_core_vref(&self->handle);
    return mp_obj_new_float(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vref_obj, adc_all_read_core_vref);
#endif

STATIC const mp_map_elem_t adc_all_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_channel),   (mp_obj_t)&adc_all_read_channel_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_temp), (mp_obj_t)&adc_all_read_core_temp_obj},
#if MICROPY_PY_BUILTINS_FLOAT
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_vbat), (mp_obj_t)&adc_all_read_core_vbat_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_vref), (mp_obj_t)&adc_all_read_core_vref_obj},
#endif
};

STATIC MP_DEFINE_CONST_DICT(adc_all_locals_dict, adc_all_locals_dict_table);

const mp_obj_type_t pyb_adc_all_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADCAll,
    .make_new = adc_all_make_new,
    .locals_dict = (mp_obj_t)&adc_all_locals_dict,
};
