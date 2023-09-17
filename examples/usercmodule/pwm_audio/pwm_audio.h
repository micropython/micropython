// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _PWM_AUDIO_H_
#define _PWM_AUDIO_H_
#include "esp_err.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration parameters for pwm_audio_init function
 */
typedef struct
{
    timer_group_t tg_num;                /*!< timer group number (0 - 1) */
    timer_idx_t timer_num;               /*!< timer number  (0 - 1) */
    int gpio_num_left;                   /*!< the LEDC output gpio_num, Left channel */
    int gpio_num_right;                  /*!< the LEDC output gpio_num, Right channel */
    ledc_channel_t ledc_channel_left;    /*!< LEDC channel (0 - 7), Corresponding to left channel*/
    ledc_channel_t ledc_channel_right;   /*!< LEDC channel (0 - 7), Corresponding to right channel*/
    ledc_timer_t ledc_timer_sel;         /*!< Select the timer source of channel (0 - 3) */
    ledc_timer_bit_t duty_resolution;    /*!< ledc pwm bits */
    uint32_t ringbuf_len;                /*!< ringbuffer size */
} pwm_audio_config_t;

/**
 * @brief pwm audio status
 */
typedef enum {
    PWM_AUDIO_STATUS_UN_INIT = 0,        /*!< pwm audio uninitialized */
    PWM_AUDIO_STATUS_IDLE = 1,           /*!< pwm audio idle */
    PWM_AUDIO_STATUS_BUSY = 2,           /*!< pwm audio busy */
} pwm_audio_status_t;

/**
 * @brief pwm audio channel define
 *
 */
typedef enum {
    PWM_AUDIO_CH_MONO        = 0,        /*!< 1 channel (mono)*/
    PWM_AUDIO_CH_STEREO      = 1,        /*!< 2 channel (stereo)*/
    PWM_AUDIO_CH_MAX,
} pwm_audio_channel_t;

/**
 * @brief Initializes and configure the pwm audio.
 * 
 * @param cfg configurations - see pwm_audio_config_t struct
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL timer_group or ledc initialize failed
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_ERR_INVALID_STATE The pwm audio already configure
 *     - ESP_ERR_NO_MEM Memory allocate failed
 */
esp_err_t pwm_audio_init(int ch_index, const pwm_audio_config_t *cfg);

/**
 * @brief Deinitialize LEDC timer_group and output gpio
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL pwm_audio not initialized
 */
esp_err_t pwm_audio_deinit(int ch_index);

/**
 * @brief Start to run pwm audio
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE pwm_audio not initialized or it already running 
 */
esp_err_t pwm_audio_start(int ch_index);

/**
 * @brief Stop pwm audio
 * 
 * @attention Only stop timer, and the pwm will keep to output. If you want to stop pwm output, call pwm_audio_deinit function
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_STATE pwm_audio not initialized or it already idle
 */
esp_err_t pwm_audio_stop(int ch_index);

/**
 * @brief Write data to play
 *
 * @param inbuf Pointer source data to write
 * @param len length of data in bytes
 * @param[out] bytes_written Number of bytes written, if timeout, the result will be less than the size passed in.
 * @param ticks_to_wait TX buffer wait timeout in RTOS ticks. If this
 * many ticks pass without space becoming available in the DMA
 * transmit buffer, then the function will return (note that if the
 * data is written to the DMA buffer in pieces, the overall operation
 * may still take longer than this timeout.) Pass portMAX_DELAY for no
 * timeout.
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Write encounter error
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t pwm_audio_write(int ch_index,uint8_t *inbuf, size_t len, size_t *bytes_written, TickType_t ticks_to_wait);

/**
 * @brief Set audio parameter, Similar to pwm_audio_set_sample_rate(), but also sets bit width.
 * 
 * @attention After start pwm audio, it can't modify parameters by call function pwm_audio_set_param.
 *            If you want to change the parameters, must stop pwm audio by call function pwm_audio_stop.
 *
 * @param rate sample rate (ex: 8000, 44100...)
 * @param bits bit width
 * @param ch channel number
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pwm_audio_set_param(int ch_index,int rate, ledc_timer_bit_t bits, int ch);

/**
 * @brief Set sample rate
 *
 * @param rate sample rate (ex: 8000, 44100...)
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pwm_audio_set_sample_rate(int ch_index,int rate);

/**
 * @brief Set volume for pwm audio
 * 
 * @attention when the volume is too small, it will produce serious distortion
 *
 * @param volume Volume to set (-16 ~ 16).
 *        Set to 0 for original output;
 *        Set to less then 0 for attenuation, and -16 is mute;
 *        Set to more than 0 for enlarge, and 16 is double output
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pwm_audio_set_volume(int ch_index,int8_t volume);

/**
 * @brief Get current volume
 * 
 * @param volume Pointer to volume
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t pwm_audio_get_volume(int ch_index,int8_t *volume);

/**
 * @brief Get parameter for pwm audio.
 *
 * @param rate sample rate, if you don't care about this parameter, set it to NULL
 * @param bits bit width, if you don't care about this parameter, set it to NULL
 * @param ch channel number, if you don't care about this parameter, set it to NULL
 *
 * @return
 *     - Always return ESP_OK
 */
esp_err_t pwm_audio_get_param(int ch_index,int *rate, int *bits, int *ch);

/**
 * @brief get pwm audio status
 * 
 * @param status current pwm_audio status
 * 
 * @return
 *     - ESP_OK Success
 */
esp_err_t pwm_audio_get_status(int ch_index,pwm_audio_status_t *status);

#ifdef __cplusplus
}
#endif

#endif
