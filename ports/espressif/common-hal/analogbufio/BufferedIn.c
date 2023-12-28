/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2023 Milind Movasha
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
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
#include "bindings/espidf/__init__.h"
#include "common-hal/analogbufio/BufferedIn.h"
#include "shared-bindings/analogbufio/BufferedIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// #define DEBUG_ANALOGBUFIO

#define NUM_SAMPLES_PER_INTERRUPT   256
#define NUM_ADC_CHANNELS            1
#define DMA_BUFFER_SIZE             1024
#define ATTENUATION                 ADC_ATTEN_DB_11
#define ADC_READ_TIMEOUT_MS         2000
#define ADC_PIN_MAX_VALUE           0xfff

#if defined(CONFIG_IDF_TARGET_ESP32)
#define ADC_RESULT_BYTE     2
#define ADC_CONV_LIMIT_EN   1   // For ESP32, this should always be set to 1
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define ADC_RESULT_BYTE     2
#define ADC_CONV_LIMIT_EN   0
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32H2)
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define ADC_RESULT_BYTE     4
#define ADC_CONV_LIMIT_EN   0
#else
#error No known CONFIG_IDF_TARGET_xxx found
#endif

static void start_dma(analogbufio_bufferedin_obj_t *self, adc_digi_convert_mode_t *convert_mode, adc_digi_output_format_t *output_format);
static void stop_dma(analogbufio_bufferedin_obj_t *self);

void common_hal_analogbufio_bufferedin_construct(analogbufio_bufferedin_obj_t *self, const mcu_pin_obj_t *pin, uint32_t sample_rate) {
    self->pin = pin;
    self->sample_rate = sample_rate;
    if (pin->adc_index == NO_ADC || pin->adc_channel == NO_ADC_CHANNEL) {
        raise_ValueError_invalid_pin();
    }

    #if defined(CONFIG_IDF_TARGET_ESP32)
    if (pin->adc_index != ADC_UNIT_1) {
        /*
         * ESP32 only supports ADC1 unit
         * https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
         * Table 29-3
         */
        raise_ValueError_invalid_pin();
    }
    #endif

    // C3 and S3 have errata related to ADC2 and continuous mode.
    #if (defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)) && !defined(CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3)
    if (pin->adc_index != ADC_UNIT_1) {
        raise_ValueError_invalid_pin();
    }
    #endif

    common_hal_mcu_pin_claim(pin);
}

static void start_dma(analogbufio_bufferedin_obj_t *self, adc_digi_convert_mode_t *convert_mode, adc_digi_output_format_t *output_format) {
    const mcu_pin_obj_t *pin = self->pin;
    uint32_t sample_rate = self->sample_rate;

    *output_format = ADC_DIGI_OUTPUT_FORMAT_TYPE1;
    if (pin->adc_index == ADC_UNIT_1) {
        *convert_mode = ADC_CONV_SINGLE_UNIT_1;
    } else {
        *convert_mode = ADC_CONV_SINGLE_UNIT_2;
    }

    /*
     *	Chip version	Conversion Mode		Output Format Type
     *	ESP32           1                   TYPE1
     *	ESP32S2         1,2,BOTH,ALTER      TYPE1, TYPE2
     *	ESP32C3         ALTER               TYPE2
     *	ESP32S3         1,2,BOTH,ALTER      TYPE2
     *	ESP32H3         1,2,BOTH,ALTER      TYPE2
     */

    #if defined(CONFIG_IDF_TARGET_ESP32C3)
    /* ESP32C3 only supports alter mode */
    *convert_mode = ADC_CONV_ALTER_UNIT;
    #endif

    #if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32H2)
    *output_format = ADC_DIGI_OUTPUT_FORMAT_TYPE2;
    #endif

    adc_continuous_handle_cfg_t adc_dma_config = {
        .max_store_buf_size = DMA_BUFFER_SIZE,
        .conv_frame_size = NUM_SAMPLES_PER_INTERRUPT * SOC_ADC_DIGI_DATA_BYTES_PER_CONV
    };

    #if defined(DEBUG_ANALOGBUFIO)
    mp_printf(&mp_plat_print, "pin:%d, ADC channel:%d, ADC index:%d, adc1_chan_mask:0x%x, adc2_chan_mask:0x%x\n", pin->number, pin->adc_channel, pin->adc_index, adc1_chan_mask, adc2_chan_mask);
    #endif // DEBUG_ANALOGBUFIO
    esp_err_t err = adc_continuous_new_handle(&adc_dma_config, &self->handle);
    if (ESP_OK != err) {
        stop_dma(self);
        CHECK_ESP_RESULT(err);
    }

    adc_continuous_config_t dig_cfg = {
        .pattern_num = NUM_ADC_CHANNELS,
        .sample_freq_hz = sample_rate,
        .conv_mode = *convert_mode,
        .format = *output_format,
    };

    #if defined(DEBUG_ANALOGBUFIO)
    mp_printf(&mp_plat_print, "conversion_mode:%d, format:%d, conv_limit_en:%d, sample_rate:%d\n", *convert_mode, *output_format, ADC_CONV_LIMIT_EN, sample_rate);
    #endif // DEBUG_ANALOGBUFIO

    adc_digi_pattern_config_t adc_pattern[NUM_ADC_CHANNELS] = {0};
    adc_pattern[0].atten = ATTENUATION;
    adc_pattern[0].channel = pin->adc_channel;
    if (pin->adc_index == ADC_UNIT_1) {
        adc_pattern[0].unit = 0;
    } else {
        adc_pattern[0].unit = 1;
    }
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

    dig_cfg.adc_pattern = adc_pattern;
    #if defined(DEBUG_ANALOGBUFIO)
    mp_printf(&mp_plat_print, "adc_pattern[0].channel:%d, adc_pattern[0].unit:%d, adc_pattern[0].atten:%d\n", adc_pattern[0].channel, adc_pattern[0].unit, adc_pattern[0].atten);
    #endif // DEBUG_ANALOGBUFIO

    err = adc_continuous_config(self->handle, &dig_cfg);
    if (ESP_OK != err) {
        stop_dma(self);
        CHECK_ESP_RESULT(err);
    }
    err = adc_continuous_start(self->handle);
    if (ESP_OK != err) {
        stop_dma(self);
        CHECK_ESP_RESULT(err);
    }
    self->started = true;
}

static void stop_dma(analogbufio_bufferedin_obj_t *self) {
    if (self->started) {
        adc_continuous_stop(self->handle);
        self->started = false;
    }
    if (self->handle != NULL) {
        adc_continuous_deinit(self->handle);
        self->handle = NULL;
    }
}

bool common_hal_analogbufio_bufferedin_deinited(analogbufio_bufferedin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogbufio_bufferedin_deinit(analogbufio_bufferedin_obj_t *self) {
    if (common_hal_analogbufio_bufferedin_deinited(self)) {
        return;
    }
    // Release ADC Pin
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

static bool check_valid_data(const adc_digi_output_data_t *data, const mcu_pin_obj_t *pin, adc_digi_convert_mode_t convert_mode, adc_digi_output_format_t output_format) {
    unsigned int unit;
    #if SOC_ADC_PERIPH_NUM == 1
    unit = 0;
    #else
    unit = data->type2.unit;
    #endif
    if (output_format == ADC_DIGI_OUTPUT_FORMAT_TYPE2) {
        if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) {
            return false;
        }
        if (pin->adc_channel != data->type2.channel) {
            return false;
        }
    } else {
        if (convert_mode == ADC_CONV_SINGLE_UNIT_1) {
            unit = 0;
        } else {
            unit = 1;
        }
        #if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
        if (data->type1.channel >= SOC_ADC_CHANNEL_NUM(unit)) {
            return false;
        }
        if (pin->adc_channel != data->type1.channel) {
            return false;
        }
        #endif
    }
    if (unit > 2) {
        return false;
    }
    return true;
}

uint32_t common_hal_analogbufio_bufferedin_readinto(analogbufio_bufferedin_obj_t *self, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample) {
    uint8_t result[NUM_SAMPLES_PER_INTERRUPT * SOC_ADC_DIGI_DATA_BYTES_PER_CONV] __attribute__ ((aligned(4))) = {0};
    uint32_t captured_samples = 0;
    uint32_t captured_bytes = 0;
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint32_t adc_reading = 0;
    adc_digi_convert_mode_t convert_mode = ADC_CONV_SINGLE_UNIT_2;
    adc_digi_output_format_t output_format = ADC_DIGI_OUTPUT_FORMAT_TYPE1;

    if (bytes_per_sample != 2) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be array of type 'H'"), MP_QSTR_buffer);
    }

    start_dma(self, &convert_mode, &output_format);

    #if defined(DEBUG_ANALOGBUFIO)
    mp_printf(&mp_plat_print, "Required bytes: %d\n", len);
    #endif // DEBUG_ANALOGBUFIO

    while (captured_bytes < len) {
        ret_num = 0;
        ret = adc_continuous_read(self->handle, result, NUM_SAMPLES_PER_INTERRUPT * SOC_ADC_DIGI_DATA_BYTES_PER_CONV, &ret_num, ADC_READ_TIMEOUT_MS);

        if (ret == ESP_OK) {
            for (uint32_t i = 0; i < ret_num; i += ADC_RESULT_BYTE) {
                adc_digi_output_data_t *pResult = (adc_digi_output_data_t *)(void *)&result[i];
                if (check_valid_data(pResult, self->pin, convert_mode, output_format)) {
                    if (captured_bytes < len) {
                        uint16_t *pBuffer = (uint16_t *)(void *)&buffer[captured_bytes];
                        if (output_format == ADC_DIGI_OUTPUT_FORMAT_TYPE1) {
                            #if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
                            adc_reading = pResult->type1.data;
                            #endif
                        } else {
                            adc_reading = pResult->type2.data;
                        }
                        adc_reading = adc_reading * ((1 << 16) - 1) / ADC_PIN_MAX_VALUE;
                        *pBuffer = (uint16_t)adc_reading;
                        captured_bytes += sizeof(uint16_t);
                        captured_samples++;
                    } else {
                        stop_dma(self);
                        return captured_samples;
                    }
                } else {
                    #if !defined(CONFIG_IDF_TARGET_ESP32C3)
                    // For all chips except for ESP32C3 we would receive samples only from one unit
                    // For ESP32C3 we may receive sample from alternating units and need to ignore them
                    #if defined(DEBUG_ANALOGBUFIO)
                    mp_printf(&mp_plat_print, "Invalid sample received: 0x%x\n", pResult->val);
                    #endif // DEBUG_ANALOGBUFIO
                    stop_dma(self);
                    return captured_samples;
                    #endif
                }
            }
        } else if (ret == ESP_ERR_TIMEOUT) {
            #if defined(DEBUG_ANALOGBUFIO)
            mp_printf(&mp_plat_print, "ADC Timeout\n");
            #endif // DEBUG_ANALOGBUFIO
            stop_dma(self);
            return captured_samples;
        } else {
            #if defined(DEBUG_ANALOGBUFIO)
            mp_printf(&mp_plat_print, "adc_digi_read_bytes failed error code:%d\n", ret);
            #endif // DEBUG_ANALOGBUFIO
            stop_dma(self);
            return captured_samples;
        }
    }

    stop_dma(self);
    #if defined(DEBUG_ANALOGBUFIO)
    mp_printf(&mp_plat_print, "Captured bytes: %d\n", captured_bytes);
    #endif // DEBUG_ANALOGBUFIO
    return captured_samples;
}
