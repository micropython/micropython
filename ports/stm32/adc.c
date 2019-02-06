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
#include "timer.h"

#if MICROPY_HW_ENABLE_ADC

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

#if defined(STM32H7)
#define ADCx                    (ADC3)
#define PIN_ADC_MASK            PIN_ADC3
#define pin_adc_table           pin_adc3
#else
#define ADCx                    (ADC1)
#define PIN_ADC_MASK            PIN_ADC1
#define pin_adc_table           pin_adc1
#endif

#define ADCx_CLK_ENABLE         __HAL_RCC_ADC1_CLK_ENABLE
#define ADC_NUM_CHANNELS        (19)

#if defined(STM32F0)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (15)
#define ADC_SCALE_V             (3.3f)
#define ADC_CAL_ADDRESS         (0x1ffff7ba)
#define ADC_CAL1                ((uint16_t*)0x1ffff7b8)
#define ADC_CAL2                ((uint16_t*)0x1ffff7c2)
#define ADC_CAL_BITS            (12)

#elif defined(STM32F4)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (15)
#define ADC_SCALE_V             (3.3f)
#define ADC_CAL_ADDRESS         (0x1fff7a2a)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 4))
#define ADC_CAL_BITS            (12)

#elif defined(STM32F7)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (15)
#define ADC_SCALE_V             (3.3f)
#if defined(STM32F722xx) || defined(STM32F723xx) || \
    defined(STM32F732xx) || defined(STM32F733xx)
#define ADC_CAL_ADDRESS         (0x1ff07a2a)
#else
#define ADC_CAL_ADDRESS         (0x1ff0f44a)
#endif

#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS + 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 4))
#define ADC_CAL_BITS            (12)

#elif defined(STM32H7)

#define ADC_FIRST_GPIO_CHANNEL  (0)
#define ADC_LAST_GPIO_CHANNEL   (16)
#define ADC_SCALE_V             (3.3f)
#define ADC_CAL_ADDRESS         (0x1FF1E860)
#define ADC_CAL1                ((uint16_t*)(0x1FF1E820))
#define ADC_CAL2                ((uint16_t*)(0x1FF1E840))
#define ADC_CAL_BITS            (16)
#define ADC_CHANNEL_VBAT        ADC_CHANNEL_VBAT_DIV4

#elif defined(STM32L4)

#define ADC_FIRST_GPIO_CHANNEL  (1)
#define ADC_LAST_GPIO_CHANNEL   (16)
#define ADC_SCALE_V             (3.0f)
#define ADC_CAL_ADDRESS         (0x1fff75aa)
#define ADC_CAL1                ((uint16_t*)(ADC_CAL_ADDRESS - 2))
#define ADC_CAL2                ((uint16_t*)(ADC_CAL_ADDRESS + 0x20))
#define ADC_CAL_BITS            (12)

#else

#error Unsupported processor

#endif

#if defined(STM32F091xC)
#define VBAT_DIV (2)
#elif defined(STM32F405xx) || defined(STM32F415xx) || \
    defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || \
    defined(STM32F411xE)
#define VBAT_DIV (2)
#elif defined(STM32F427xx) || defined(STM32F429xx) || \
      defined(STM32F437xx) || defined(STM32F439xx) || \
      defined(STM32F446xx) || \
      defined(STM32F722xx) || defined(STM32F723xx) || \
      defined(STM32F732xx) || defined(STM32F733xx) || \
      defined(STM32F746xx) || defined(STM32F765xx) || \
      defined(STM32F767xx) || defined(STM32F769xx)
#define VBAT_DIV (4)
#elif defined(STM32H743xx)
#define VBAT_DIV (4)
#elif defined(STM32L432xx) || defined(STM32L475xx) || \
      defined(STM32L476xx) || defined(STM32L496xx)
#define VBAT_DIV (3)
#else
#error Unsupported processor
#endif

// Timeout for waiting for end-of-conversion, in ms
#define EOC_TIMEOUT (10)

/* Core temperature sensor definitions */
#define CORE_TEMP_V25          (943)  /* (0.76v/3.3v)*(2^ADC resoultion) */
#define CORE_TEMP_AVG_SLOPE    (3)    /* (2.5mv/3.3v)*(2^ADC resoultion) */

// scale and calibration values for VBAT and VREF
#define ADC_SCALE (ADC_SCALE_V / ((1 << ADC_CAL_BITS) - 1))
#define VREFIN_CAL ((uint16_t *)ADC_CAL_ADDRESS)

typedef struct _pyb_obj_adc_t {
    mp_obj_base_t base;
    mp_obj_t pin_name;
    int channel;
    ADC_HandleTypeDef handle;
} pyb_obj_adc_t;

// convert user-facing channel number into internal channel number
static inline uint32_t adc_get_internal_channel(uint32_t channel) {
    #if defined(STM32F4) || defined(STM32F7)
    // on F4 and F7 MCUs we want channel 16 to always be the TEMPSENSOR
    // (on some MCUs ADC_CHANNEL_TEMPSENSOR=16, on others it doesn't)
    if (channel == 16) {
        channel = ADC_CHANNEL_TEMPSENSOR;
    }
    #endif
    return channel;
}

STATIC bool is_adcx_channel(int channel) {
#if defined(STM32F411xE)
    // The HAL has an incorrect IS_ADC_CHANNEL macro for the F411 so we check for temp
    return IS_ADC_CHANNEL(channel) || channel == ADC_CHANNEL_TEMPSENSOR;
#elif defined(STM32F0) || defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    return IS_ADC_CHANNEL(channel);
#elif defined(STM32L4)
    ADC_HandleTypeDef handle;
    handle.Instance = ADCx;
    return IS_ADC_CHANNEL(&handle, channel);
#else
    #error Unsupported processor
#endif
}

STATIC void adc_wait_for_eoc_or_timeout(int32_t timeout) {
    uint32_t tickstart = HAL_GetTick();
#if defined(STM32F4) || defined(STM32F7)
    while ((ADCx->SR & ADC_FLAG_EOC) != ADC_FLAG_EOC) {
#elif defined(STM32F0) || defined(STM32H7) || defined(STM32L4)
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
#if defined(STM32F0) || defined(STM32F4) || defined(STM32F7)
    ADCx_CLK_ENABLE();
#elif defined(STM32H7)
    __HAL_RCC_ADC3_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
#elif defined(STM32L4)
    __HAL_RCC_ADC_CLK_ENABLE();
#else
    #error Unsupported processor
#endif
}

STATIC void adcx_init_periph(ADC_HandleTypeDef *adch, uint32_t resolution) {
    adcx_clock_enable();

    adch->Instance                   = ADCx;
    adch->Init.Resolution            = resolution;
    adch->Init.ContinuousConvMode    = DISABLE;
    adch->Init.DiscontinuousConvMode = DISABLE;
    #if !defined(STM32F0)
    adch->Init.NbrOfDiscConversion   = 0;
    adch->Init.NbrOfConversion       = 1;
    #endif
    adch->Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    adch->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    adch->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    #if defined(STM32F0) || defined(STM32F4) || defined(STM32F7)
    adch->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
    adch->Init.ScanConvMode          = DISABLE;
    adch->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adch->Init.DMAContinuousRequests = DISABLE;
    #elif defined(STM32H7)
    adch->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
    adch->Init.BoostMode             = ENABLE;
    adch->Init.ScanConvMode          = DISABLE;
    adch->Init.LowPowerAutoWait      = DISABLE;
    adch->Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
    adch->Init.OversamplingMode      = DISABLE;
    adch->Init.LeftBitShift          = ADC_LEFTBITSHIFT_NONE;
    adch->Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
    #elif defined(STM32L4)
    adch->Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
    adch->Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adch->Init.LowPowerAutoWait      = DISABLE;
    adch->Init.Overrun               = ADC_OVR_DATA_PRESERVED;
    adch->Init.OversamplingMode      = DISABLE;
    adch->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adch->Init.DMAContinuousRequests = DISABLE;
    #else
    #error Unsupported processor
    #endif

    #if defined(STM32F0)
    adch->Init.SamplingTimeCommon = ADC_SAMPLETIME_71CYCLES_5;
    #endif

    HAL_ADC_Init(adch);

    #if defined(STM32H7)
    HAL_ADCEx_Calibration_Start(adch, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    #endif
    #if defined(STM32L4)
    HAL_ADCEx_Calibration_Start(adch, ADC_SINGLE_ENDED);
    #endif
}

STATIC void adc_init_single(pyb_obj_adc_t *adc_obj) {
    if (!is_adcx_channel(adc_obj->channel)) {
        return;
    }

    if (ADC_FIRST_GPIO_CHANNEL <= adc_obj->channel && adc_obj->channel <= ADC_LAST_GPIO_CHANNEL) {
        // Channels 0-16 correspond to real pins. Configure the GPIO pin in ADC mode.
        const pin_obj_t *pin = pin_adc_table[adc_obj->channel];
        mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ADC, MP_HAL_PIN_PULL_NONE, 0);
    }

    adcx_init_periph(&adc_obj->handle, ADC_RESOLUTION_12B);

#if defined(STM32L4) && defined(ADC_DUALMODE_REGSIMULT_INJECSIMULT)
    ADC_MultiModeTypeDef multimode;
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&adc_obj->handle, &multimode) != HAL_OK)
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Can not set multimode on ADC1 channel: %d", adc_obj->channel));
    }
#endif
}

STATIC void adc_config_channel(ADC_HandleTypeDef *adc_handle, uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = channel;
    sConfig.Rank = 1;
#if defined(STM32F0)
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
#elif defined(STM32F4) || defined(STM32F7)
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
#elif defined(STM32H7)
    if (channel == ADC_CHANNEL_VREFINT
        || channel == ADC_CHANNEL_TEMPSENSOR
        || channel == ADC_CHANNEL_VBAT) {
        sConfig.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
    } else {
        sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
    }
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.OffsetRightShift = DISABLE;
    sConfig.OffsetSignedSaturation = DISABLE;
#elif defined(STM32L4)
    if (channel == ADC_CHANNEL_VREFINT
        || channel == ADC_CHANNEL_TEMPSENSOR
        || channel == ADC_CHANNEL_VBAT) {
        sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    } else {
        sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
    }
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
#else
    #error Unsupported processor
#endif

    #if defined(STM32F0)
    // On the STM32F0 we must select only one channel at a time to sample, so clear all
    // channels before calling HAL_ADC_ConfigChannel, which will select the desired one.
    adc_handle->Instance->CHSELR = 0;
    #endif

    HAL_ADC_ConfigChannel(adc_handle, &sConfig);
}

STATIC uint32_t adc_read_channel(ADC_HandleTypeDef *adcHandle) {
    HAL_ADC_Start(adcHandle);
    adc_wait_for_eoc_or_timeout(EOC_TIMEOUT);
    uint32_t value = ADCx->DR;
    HAL_ADC_Stop(adcHandle);
    return value;
}

STATIC uint32_t adc_config_and_read_channel(ADC_HandleTypeDef *adcHandle, uint32_t channel) {
    adc_config_channel(adcHandle, channel);
    uint32_t raw_value = adc_read_channel(adcHandle);

    #if defined(STM32F4) || defined(STM32F7)
    // ST docs say that (at least on STM32F42x and STM32F43x), VBATE must
    // be disabled when TSVREFE is enabled for TEMPSENSOR and VREFINT
    // conversions to work.  VBATE is enabled by the above call to read
    // the channel, and here we disable VBATE so a subsequent call for
    // TEMPSENSOR or VREFINT works correctly.
    if (channel == ADC_CHANNEL_VBAT) {
        ADC->CCR &= ~ADC_CCR_VBATE;
    }
    #endif

    return raw_value;
}

/******************************************************************************/
/* MicroPython bindings : adc object (single channel)                         */

STATIC void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_obj_adc_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "<ADC on ");
    mp_obj_print_helper(print, self->pin_name, PRINT_STR);
    mp_printf(print, " channel=%u>", self->channel);
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
        if ((pin->adc_num & PIN_ADC_MASK) == 0) {
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
        if (pin_adc_table[channel] == NULL) {
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

    return MP_OBJ_FROM_PTR(o);
}

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
STATIC mp_obj_t adc_read(mp_obj_t self_in) {
    pyb_obj_adc_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(adc_config_and_read_channel(&self->handle, self->channel));
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
    pyb_obj_adc_t *self = MP_OBJ_TO_PTR(self_in);

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
#if defined(STM32F4) || defined(STM32F7)
            ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
#elif defined(STM32F0) || defined(STM32H7) || defined(STM32L4)
            SET_BIT(ADCx->CR, ADC_CR_ADSTART);
#else
            #error Unsupported processor
#endif
        }

        // wait for sample to complete
        adc_wait_for_eoc_or_timeout(EOC_TIMEOUT);

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

// read_timed_multi((adcx, adcy, ...), (bufx, bufy, ...), timer)
//
// Read analog values from multiple ADC's into buffers at a rate set by the
// timer.  The ADC values have 12-bit resolution and are stored directly into
// the corresponding buffer if its element size is 16 bits or greater, otherwise
// the sample resolution will be reduced to 8 bits.
//
// This function should not allocate any heap memory.
STATIC mp_obj_t adc_read_timed_multi(mp_obj_t adc_array_in, mp_obj_t buf_array_in, mp_obj_t tim_in) {
    size_t nadcs, nbufs;
    mp_obj_t *adc_array, *buf_array;
    mp_obj_get_array(adc_array_in, &nadcs, &adc_array);
    mp_obj_get_array(buf_array_in, &nbufs, &buf_array);

    if (nadcs < 1) {
        mp_raise_ValueError("need at least 1 ADC");
    }
    if (nadcs != nbufs) {
        mp_raise_ValueError("length of ADC and buffer lists differ");
    }

    // Get buf for first ADC, get word size, check other buffers match in type
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_array[0], &bufinfo, MP_BUFFER_WRITE);
    size_t typesize = mp_binary_get_size('@', bufinfo.typecode, NULL);
    void *bufptrs[nbufs];
    for (uint array_index = 0; array_index < nbufs; array_index++) {
        mp_buffer_info_t bufinfo_curr;
        mp_get_buffer_raise(buf_array[array_index], &bufinfo_curr, MP_BUFFER_WRITE);
        if ((bufinfo.len != bufinfo_curr.len) || (bufinfo.typecode != bufinfo_curr.typecode)) {
            mp_raise_ValueError("size and type of buffers must match");
        }
        bufptrs[array_index] = bufinfo_curr.buf;
    }

    // Use the supplied timer object as the sampling time base
    TIM_HandleTypeDef *tim;
    tim = pyb_timer_get_handle(tim_in);

    // Start adc; this is slow so wait for it to start
    pyb_obj_adc_t *adc0 = MP_OBJ_TO_PTR(adc_array[0]);
    adc_config_channel(&adc0->handle, adc0->channel);
    HAL_ADC_Start(&adc0->handle);
    // Wait for sample to complete and discard
    adc_wait_for_eoc_or_timeout(EOC_TIMEOUT);
    // Read (and discard) value
    uint value = ADCx->DR;

    // Ensure first sample is on a timer tick
    __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);
    while (__HAL_TIM_GET_FLAG(tim, TIM_FLAG_UPDATE) == RESET) {
    }
    __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);

    // Overrun check: assume success
    bool success = true;
    size_t nelems = bufinfo.len / typesize;
    for (size_t elem_index = 0; elem_index < nelems; elem_index++) {
        if (__HAL_TIM_GET_FLAG(tim, TIM_FLAG_UPDATE) != RESET) {
            // Timer has already triggered
            success = false;
        } else {
            // Wait for the timer to trigger so we sample at the correct frequency
            while (__HAL_TIM_GET_FLAG(tim, TIM_FLAG_UPDATE) == RESET) {
            }
        }
        __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);

        for (size_t array_index = 0; array_index < nadcs; array_index++) {
            pyb_obj_adc_t *adc = MP_OBJ_TO_PTR(adc_array[array_index]);
            // configure the ADC channel
            adc_config_channel(&adc->handle, adc->channel);
            // for the first sample we need to turn the ADC on
            // ADC is started: set the "start sample" bit
            #if defined(STM32F4) || defined(STM32F7)
            ADCx->CR2 |= (uint32_t)ADC_CR2_SWSTART;
            #elif defined(STM32F0) || defined(STM32H7) || defined(STM32L4)
            SET_BIT(ADCx->CR, ADC_CR_ADSTART);
            #else
            #error Unsupported processor
            #endif
            // wait for sample to complete
            adc_wait_for_eoc_or_timeout(EOC_TIMEOUT);

            // read value
            value = ADCx->DR;

            // store values in buffer
            if (typesize == 1) {
                value >>= 4;
            }
            mp_binary_set_val_array_from_int(bufinfo.typecode, bufptrs[array_index], elem_index, value);
        }
    }

    // Turn the ADC off
    adc0 = MP_OBJ_TO_PTR(adc_array[0]);
    HAL_ADC_Stop(&adc0->handle);

    return mp_obj_new_bool(success);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(adc_read_timed_multi_fun_obj, adc_read_timed_multi);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(adc_read_timed_multi_obj, MP_ROM_PTR(&adc_read_timed_multi_fun_obj));

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&adc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timed), MP_ROM_PTR(&adc_read_timed_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_timed_multi), MP_ROM_PTR(&adc_read_timed_multi_obj) },
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
        #if !defined(STM32H7)
        case 6:  resolution = ADC_RESOLUTION_6B;  break;
        #endif
        case 8:  resolution = ADC_RESOLUTION_8B;  break;
        case 10: resolution = ADC_RESOLUTION_10B; break;
        case 12: resolution = ADC_RESOLUTION_12B; break;
        #if defined(STM32H7)
        case 16: resolution = ADC_RESOLUTION_16B; break;
        #endif
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "resolution %d not supported", resolution));
    }

    for (uint32_t channel = ADC_FIRST_GPIO_CHANNEL; channel <= ADC_LAST_GPIO_CHANNEL; ++channel) {
        // only initialise those channels that are selected with the en_mask
        if (en_mask & (1 << channel)) {
            // Channels 0-16 correspond to real pins. Configure the GPIO pin in
            // ADC mode.
            const pin_obj_t *pin = pin_adc_table[channel];
            if (pin) {
                mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ADC, MP_HAL_PIN_PULL_NONE, 0);
            }
        }
    }

    adcx_init_periph(&adc_all->handle, resolution);
}

int adc_get_resolution(ADC_HandleTypeDef *adcHandle) {
    uint32_t res_reg = ADC_GET_RESOLUTION(adcHandle);

    switch (res_reg) {
       #if !defined(STM32H7)
        case ADC_RESOLUTION_6B:  return 6;
        #endif
        case ADC_RESOLUTION_8B:  return 8;
        case ADC_RESOLUTION_10B: return 10;
        #if defined(STM32H7)
        case ADC_RESOLUTION_16B: return 16;
        #endif
    }
    return 12;
}

STATIC uint32_t adc_config_and_read_ref(ADC_HandleTypeDef *adcHandle, uint32_t channel) {
    uint32_t raw_value = adc_config_and_read_channel(adcHandle, channel);
    // Scale raw reading to the number of bits used by the calibration constants
    return raw_value << (ADC_CAL_BITS - adc_get_resolution(adcHandle));
}

int adc_read_core_temp(ADC_HandleTypeDef *adcHandle) {
    int32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_TEMPSENSOR);
    return ((raw_value - CORE_TEMP_V25) / CORE_TEMP_AVG_SLOPE) + 25;
}

#if MICROPY_PY_BUILTINS_FLOAT
// correction factor for reference value
STATIC volatile float adc_refcor = 1.0f;

float adc_read_core_temp_float(ADC_HandleTypeDef *adcHandle) {
    int32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_TEMPSENSOR);
    float core_temp_avg_slope = (*ADC_CAL2 - *ADC_CAL1) / 80.0;
    return (((float)raw_value * adc_refcor - *ADC_CAL1) / core_temp_avg_slope) + 30.0f;
}

float adc_read_core_vbat(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_VBAT);
    return raw_value * VBAT_DIV * ADC_SCALE * adc_refcor;
}

float adc_read_core_vref(ADC_HandleTypeDef *adcHandle) {
    uint32_t raw_value = adc_config_and_read_ref(adcHandle, ADC_CHANNEL_VREFINT);

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

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t adc_all_read_channel(mp_obj_t self_in, mp_obj_t channel) {
    pyb_adc_all_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t chan = adc_get_internal_channel(mp_obj_get_int(channel));
    uint32_t data = adc_config_and_read_channel(&self->handle, chan);
    return mp_obj_new_int(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(adc_all_read_channel_obj, adc_all_read_channel);

STATIC mp_obj_t adc_all_read_core_temp(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = MP_OBJ_TO_PTR(self_in);
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
    pyb_adc_all_obj_t *self = MP_OBJ_TO_PTR(self_in);
    float data = adc_read_core_vbat(&self->handle);
    return mp_obj_new_float(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vbat_obj, adc_all_read_core_vbat);

STATIC mp_obj_t adc_all_read_core_vref(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = MP_OBJ_TO_PTR(self_in);
    float data  = adc_read_core_vref(&self->handle);
    return mp_obj_new_float(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vref_obj, adc_all_read_core_vref);

STATIC mp_obj_t adc_all_read_vref(mp_obj_t self_in) {
    pyb_adc_all_obj_t *self = MP_OBJ_TO_PTR(self_in);
    adc_read_core_vref(&self->handle);
    return mp_obj_new_float(ADC_SCALE_V * adc_refcor);
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

#endif // MICROPY_HW_ENABLE_ADC
