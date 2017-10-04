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
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
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

#if defined(MCU_SERIES_F4)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (15)
#define ADC_CAL_ADDRESS         (0x1fff7a2a)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 4))

#elif defined(MCU_SERIES_F7)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (15)
#define ADC_CAL_ADDRESS         (0x1ff0f44a)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 4))

#elif defined(MCU_SERIES_L4)

#define ADC_FIRST_GPIO_CHANNEL  (1)
#define ADC_LAST_GPIO_CHANNEL   (16)
#define ADC_CAL_ADDRESS         (0x1fff75aa)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS - 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 0x20))

#else

#error Unsupported processor

#endif

#if defined(STM32F405xx) || defined(STM32F415xx) || \
    defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || \
    defined(STM32F411xE)
#define VBAT_DIV (2)
#elif defined(STM32F427xx) || defined(STM32F429xx) || \
      defined(STM32F437xx) || defined(STM32F439xx) || \
      defined(STM32F746xx) || defined(STM32F767xx) || \
      defined(STM32F769xx) || defined(STM32F446xx)
#define VBAT_DIV (4)
#elif defined(STM32L475xx) || defined(STM32L476xx)
#define VBAT_DIV (3)
#else
#error Unsupported processor
#endif

/* Core temperature sensor definitions */
#define CORE_TEMP_V25          (943)  /* (0.76v/3.3v)*(2^ADC resoultion) */
#define CORE_TEMP_AVG_SLOPE    (3)    /* (2.5mv/3.3v)*(2^ADC resoultion) */

// scale and calibration values for VBAT and VREF
#define ADC_SCALE (3.3f / 4095)
#define VREFIN_CAL ((uint16_t *)ADC_CAL_ADDRESS)

typedef struct _pyb_obj_adc_t {
    mp_obj_base_t base;
    mp_obj_t pin_name;
    int channel;
    ADC_HandleTypeDef handle;
} pyb_obj_adc_t;

// convert user-facing channel number into internal channel number
static inline uint32_t adc_get_internal_channel(uint32_t channel) {
    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    // on F4 and F7 MCUs we want channel 16 to always be the TEMPSENSOR
    // (on some MCUs ADC_CHANNEL_TEMPSENSOR=16, on others it doesn't)
    if (channel == 16) {
        channel = ADC_CHANNEL_TEMPSENSOR;
    }
    #endif
    return channel;
}

STATIC bool is_adcx_channel(int channel) {
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    return IS_ADC_CHANNEL(channel);
#elif defined(MCU_SERIES_L4)
    ADC_HandleTypeDef handle;
    handle.Instance = ADCx;
    return IS_ADC_CHANNEL(&handle, channel);
#else
    #error Unsupported processor
#endif
}

STATIC void adc_wait_for_eoc_or_timeout(int32_t timeout) {
    uint32_t tickstart = HAL_GetTick();
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    while ((ADCx->SR & ADC_FLAG_EOC) != ADC_FLAG_EOC) {
#elif defined(MCU_SERIES_L4)
    while (READ_BIT(ADCx->ISR, ADC_FLAG_EOC) != ADC_FLAG_EOC) {
#else
    #error Unsupported processor
#endif
        if (((HAL_GetTick() - tickstart ) > timeout)) {
            break; // timeout
        }
    }
}

STATIC void adcx_clock_enable(void) {
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    ADCx_CLK_ENABLE();
#elif defined(MCU_SERIES_L4)
    __HAL_RCC_ADC_CLK_ENABLE();
#else
    #error Unsupported processor
#endif
}

STATIC void adc_init_single(pyb_obj_adc_t *adc_obj) {
    if (!is_adcx_channel(adc_obj->channel)) {
        return;
    }

    if (ADC_FIRST_GPIO_CHANNEL <= adc_obj->channel && adc_obj->channel <= ADC_LAST_GPIO_CHANNEL) {
      // Channels 0-16 correspond to real pins. Configure the GPIO pin in
      // ADC mode.
      const pin_obj_t *pin = pin_adc1[adc_obj->channel];
      mp_hal_gpio_clock_enable(pin->gpio);
      GPIO_InitTypeDef GPIO_InitStructure;
      GPIO_InitStructure.Pin = pin->pin_mask;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
      GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
#elif defined(MCU_SERIES_L4)
      GPIO_InitStructure.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
#else
    #error Unsupported processor
#endif
      GPIO_InitStructure.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);
    }

    adcx_clock_enable();

    ADC_HandleTypeDef *adcHandle = &adc_obj->handle;
    adcHandle->Instance                   = ADCx;
    adcHandle->Init.ContinuousConvMode    = DISABLE;
    adcHandle->Init.DiscontinuousConvMode = DISABLE;
    adcHandle->Init.NbrOfDiscConversion   = 0;
    adcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adcHandle->Init.NbrOfConversion       = 1;
    adcHandle->Init.DMAContinuousRequests = DISABLE;
    adcHandle->Init.Resolution            = ADC_RESOLUTION_12B;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    adcHandle->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
    adcHandle->Init.ScanConvMode          = DISABLE;
    adcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    adcHandle->Init.EOCSelection          = DISABLE;
#elif defined(MCU_SERIES_L4)
    adcHandle->Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
    adcHandle->Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adcHandle->Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    adcHandle->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    adcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle->Init.LowPowerAutoWait      = DISABLE;
    adcHandle->Init.Overrun               = ADC_OVR_DATA_PRESERVED;
    adcHandle->Init.OversamplingMode      = DISABLE;
#else
    #error Unsupported processor
#endif

    HAL_ADC_Init(adcHandle);

#if defined(MCU_SERIES_L4)
    ADC_MultiModeTypeDef multimode;
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(adcHandle, &multimode) != HAL_OK)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Can not set multimode on ADC1 channel: %d", adc_obj->channel));
    }
#endif
}

STATIC void adc_config_channel(ADC_HandleTypeDef *adc_handle, uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = channel;
    sConfig.Rank = 1;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
#elif defined(MCU_SERIES_L4)
    sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
#else
    #error Unsupported processor
#endif
    sConfig.Offset = 0;

    HAL_ADC_ConfigChannel(adc_handle, &sConfig);
}

STATIC uint32_t adc_read_channel(ADC_HandleTypeDef *adcHandle) {
    uint32_t rawValue = 0;

    HAL_ADC_Start(adcHandle);
    if (HAL_ADC_PollForConversion(adcHandle, 10) == HAL_OK
        && (HAL_ADC_GetState(adcHandle) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG) {
        rawValue = HAL_ADC_GetValue(adcHandle);
    }
    HAL_ADC_Stop(adcHandle);

    return rawValue;
}

/******************************************************************************/
/* MicroPython bindings : adc object (single channel)                         */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_obj_adc_t *self = self_in;
    mp_print_str(print, "<ADC on ");
    mp_obj_print_helper(print, self->pin_name, PRINT_STR);
    mp_printf(print, " channel=%lu>", self->channel);
}

/// \classmethod \constructor(pin)
/// Create an ADC object associated with the given pin.
/// This allows you to then read analog values on that pin.
STATIC mp_obj_t adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // 1st argument is the pin name
    mp_obj_t pin_obj = args[0];

    uint32_t channel;

    if (MP_OBJ_IS_INT(pin_obj)) {
        channel = adc_get_internal_channel(mp_obj_get_int(pin_obj));
    } else {
        const pin_obj_t *pin = pin_find(pin_obj);
        if ((pin->adc_num & PIN_ADC1) == 0) {
            // No ADC1 function on that pin
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %q does not have ADC capabilities", pin->name));
        }
        channel = pin->adc_channel;
    }

    if (!is_adcx_channel(channel)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "not a valid ADC Channel: %d", channel));
    }


    if (ADC_FIRST_GPIO_CHANNEL <= channel && channel <= ADC_LAST_GPIO_CHANNEL) {
        // these channels correspond to physical GPIO ports so make sure they exist
        if (pin_adc1[channel] == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "channel %d not available on this board", channel));
        }
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

    adc_config_channel(&self->handle, self->channel);
    uint32_t data = adc_read_channel(&self->handle);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

/// \method read_timed(buf, timer)
///
/// Read analog values into `buf` at a rate set by the `timer` object.
///
/// `buf` can be bytearray or array.array for example.  The ADC values have
/// 12-bit resolution and are stored directly into `buf` if its element size is
/// 16 bits or greater.  If `buf` has only 8-bit elements (eg a bytearray) then
/// the sample resolution will be reduced to 8 bits.
///
/// `timer` should be a Timer object, and a sample is read each time the timer
/// triggers.  The timer must already be initialised and running at the desired
/// sampling frequency.
///
/// To support previous behaviour of this function, `timer` can also be an
/// integer which specifies the frequency (in Hz) to sample at.  In this case
/// Timer(6) will be automatically configured to run at the given frequency.
///
/// Example using a Timer object (preferred way):
///
///     adc = pyb.ADC(pyb.Pin.board.X19)    # create an ADC on pin X19
///     tim = pyb.Timer(6, freq=10)         # create a timer running at 10Hz
///     buf = bytearray(100)                # creat a buffer to store the samples
///     adc.read_timed(buf, tim)            # sample 100 values, taking 10s
///
/// Example using an integer for the frequency:
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
    size_t typesize = mp_binary_get_size('@', bufinfo.typecode, NULL);

    TIM_HandleTypeDef *tim;
    #if defined(TIM6)
    if (mp_obj_is_integer(freq_in)) {
        // freq in Hz given so init TIM6 (legacy behaviour)
        tim = timer_tim6_init(mp_obj_get_int(freq_in));
        HAL_TIM_Base_Start(tim);
    } else
    #endif
    {
        // use the supplied timer object as the sampling time base
        tim = pyb_timer_get_handle(freq_in);
    }

    // configure the ADC channel
    adc_config_channel(&self->handle, self->channel);

    // This uses the timer in polling mode to do the sampling
    // TODO use DMA

    uint nelems = bufinfo.len / typesize;
    for (uint index = 0; index < nelems; index++) {
        // Wait for the timer to trigger so we sample at the correct frequency
        while (__HAL_TIM_GET_FLAG(tim, TIM_FLAG_UPDATE) == RESET) {
        }
        __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);

        if (index == 0) {
            // for the first sample we need to turn the ADC on
            HAL_ADC_Start(&self->handle);
        } else {
            // for subsequent samples we can just set the "start sample" bit
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
            ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
#elif defined(MCU_SERIES_L4)
            SET_BIT(ADCx->CR, ADC_CR_ADSTART);
#else
            #error Unsupported processor
#endif
        }

        // wait for sample to complete
        #define READ_TIMED_TIMEOUT (10) // in ms
        adc_wait_for_eoc_or_timeout(READ_TIMED_TIMEOUT);

        // read value
        uint value = ADCx->DR;

        // store value in buffer
        if (typesize == 1) {
            value >>= 4;
        }
        mp_binary_set_val_array_from_int(bufinfo.typecode, bufinfo.buf, index, value);
    }

    // turn the ADC off
    HAL_ADC_Stop(&self->handle);

    #if defined(TIM6)
    if (mp_obj_is_integer(freq_in)) {
        // stop timer if we initialised TIM6 in this function (legacy behaviour)
        HAL_TIM_Base_Stop(tim);
    }
    #endif

    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(adc_read_timed_obj, adc_read_timed);

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&adc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timed), MP_ROM_PTR(&adc_read_timed_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .make_new = adc_make_new,
    .locals_dict = (mp_obj_dict_t*)&adc_locals_dict,
};

/******************************************************************************/
/* adc all object                                                             */

typedef struct _pyb_adc_all_obj_t {
    mp_obj_base_t base;
    ADC_HandleTypeDef handle;
} pyb_adc_all_obj_t;

void adc_init_all(pyb_adc_all_obj_t *adc_all, uint32_t resolution, uint32_t en_mask) {

    switch (resolution) {
        case 6:  resolution = ADC_RESOLUTION_6B;  break;
        case 8:  resolution = ADC_RESOLUTION_8B;  break;
        case 10: resolution = ADC_RESOLUTION_10B; break;
        case 12: resolution = ADC_RESOLUTION_12B; break;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "resolution %d not supported", resolution));
    }

    for (uint32_t channel = ADC_FIRST_GPIO_CHANNEL; channel <= ADC_LAST_GPIO_CHANNEL; ++channel) {
        // only initialise those channels that are selected with the en_mask
        if (en_mask & (1 << channel)) {
            // Channels 0-16 correspond to real pins. Configure the GPIO pin in
            // ADC mode.
            const pin_obj_t *pin = pin_adc1[channel];
            if (pin) {
                mp_hal_gpio_clock_enable(pin->gpio);
                GPIO_InitTypeDef GPIO_InitStructure;
                GPIO_InitStructure.Pin = pin->pin_mask;
                GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                HAL_GPIO_Init(pin->gpio, &GPIO_InitStructure);
            }
        }
    }

    adcx_clock_enable();

    ADC_HandleTypeDef *adcHandle = &adc_all->handle;
    adcHandle->Instance = ADCx;
    adcHandle->Init.Resolution            = resolution;
    adcHandle->Init.ContinuousConvMode    = DISABLE;
    adcHandle->Init.DiscontinuousConvMode = DISABLE;
    adcHandle->Init.NbrOfDiscConversion   = 0;
    adcHandle->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adcHandle->Init.NbrOfConversion       = 1;
    adcHandle->Init.DMAContinuousRequests = DISABLE;
    adcHandle->Init.EOCSelection          = DISABLE;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    adcHandle->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
    adcHandle->Init.ScanConvMode          = DISABLE;
    adcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
#elif defined(MCU_SERIES_L4)
    adcHandle->Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV2;
    adcHandle->Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adcHandle->Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T1_CC1;
    adcHandle->Init.LowPowerAutoWait      = DISABLE;
    adcHandle->Init.Overrun               = ADC_OVR_DATA_PRESERVED;
    adcHandle->Init.OversamplingMode      = DISABLE;
#else
    #error Unsupported processor
#endif

    HAL_ADC_Init(adcHandle);
}

uint32_t adc_config_and_read_channel(ADC_HandleTypeDef *adcHandle, uint32_t channel) {
    adc_config_channel(adcHandle, channel);
    return adc_read_channel(adcHandle);
}

int adc_get_resolution(ADC_HandleTypeDef *adcHandle) {
    uint32_t res_reg = __HAL_ADC_GET_RESOLUTION(adcHandle);

    switch (res_reg) {
        case ADC_RESOLUTION_6B:  return 6;
        case ADC_RESOLUTION_8B:  return 8;
        case ADC_RESOLUTION_10B: return 10;
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
// correction factor for reference value
STATIC volatile float adc_refcor = 1.0f;

float adc_read_core_temp_float(ADC_HandleTypeDef *adcHandle) {
    int32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_TEMPSENSOR);

    // constants assume 12-bit resolution so we scale the raw value to 12-bits
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    float core_temp_avg_slope = (*ADC_CAL2 - *ADC_CAL1) / 80.0;
    return (((float)raw_value * adc_refcor - *ADC_CAL1) / core_temp_avg_slope) + 30.0f;
}

float adc_read_core_vbat(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_VBAT);

    // Note: constants assume 12-bit resolution, so we scale the raw value to
    //       be 12-bits.
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    // ST docs say that (at least on STM32F42x and STM32F43x), VBATE must
    // be disabled when TSVREFE is enabled for TEMPSENSOR and VREFINT
    // conversions to work.  VBATE is enabled by the above call to read
    // the channel, and here we disable VBATE so a subsequent call for
    // TEMPSENSOR or VREFINT works correctly.
    ADC->CCR &= ~ADC_CCR_VBATE;
    #endif

    return raw_value * VBAT_DIV * ADC_SCALE * adc_refcor;
}

float adc_read_core_vref(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_channel(adcHandle, ADC_CHANNEL_VREFINT);

    // Note: constants assume 12-bit resolution, so we scale the raw value to
    //       be 12-bits.
    raw_value <<= (12 - adc_get_resolution(adcHandle));

    // update the reference correction factor
    adc_refcor = ((float)(*VREFIN_CAL)) / ((float)raw_value);

    return (*VREFIN_CAL) * ADC_SCALE;
}
#endif

/******************************************************************************/
/* MicroPython bindings : adc_all object                                      */

STATIC mp_obj_t adc_all_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    // make ADCAll object
    pyb_adc_all_obj_t *o = m_new_obj(pyb_adc_all_obj_t);
    o->base.type = &pyb_adc_all_type;
    mp_int_t res = mp_obj_get_int(args[0]);
    uint32_t en_mask = 0xffffffff;
    if (n_args > 1) {
        en_mask =  mp_obj_get_int(args[1]);
    }
    adc_init_all(o, res, en_mask);

    return o;
}

STATIC mp_obj_t adc_all_read_channel(mp_obj_t self_in, mp_obj_t channel) {
    pyb_adc_all_obj_t *self = self_in;
    uint32_t chan = adc_get_internal_channel(mp_obj_get_int(channel));
    uint32_t data = adc_config_and_read_channel(&self->handle, chan);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(adc_all_read_channel_obj, adc_all_read_channel);

STATIC mp_obj_t adc_all_read_core_temp(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = self_in;
    #if MICROPY_PY_BUILTINS_FLOAT
    float data = adc_read_core_temp_float(&self->handle);
    return mp_obj_new_float(data);
    #else
    int data  = adc_read_core_temp(&self->handle);
    return mp_obj_new_int(data);
    #endif
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

STATIC mp_obj_t adc_all_read_vref(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = self_in;
    adc_read_core_vref(&self->handle);
    return mp_obj_new_float(3.3 * adc_refcor);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_vref_obj, adc_all_read_vref);
#endif

STATIC const mp_rom_map_elem_t adc_all_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_channel), MP_ROM_PTR(&adc_all_read_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_core_temp), MP_ROM_PTR(&adc_all_read_core_temp_obj) },
#if MICROPY_PY_BUILTINS_FLOAT
    { MP_ROM_QSTR(MP_QSTR_read_core_vbat), MP_ROM_PTR(&adc_all_read_core_vbat_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_core_vref), MP_ROM_PTR(&adc_all_read_core_vref_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_vref), MP_ROM_PTR(&adc_all_read_vref_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(adc_all_locals_dict, adc_all_locals_dict_table);

const mp_obj_type_t pyb_adc_all_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADCAll,
    .make_new = adc_all_make_new,
    .locals_dict = (mp_obj_dict_t*)&adc_all_locals_dict,
};
