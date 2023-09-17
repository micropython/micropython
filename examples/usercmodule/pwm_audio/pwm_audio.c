// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
// Include the header file to get access to the MicroPython API
#include "py/runtime.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include "soc/ledc_struct.h"
#include "soc/ledc_reg.h"
#include "pwm_audio.h"
#include "sdkconfig.h"

#define LEFT_CHANNEL_GPIO  26
#define RIGHT_CHANNEL_GPIO 25
#if (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3)

static const char *TAG = "pwm_audio";

#define PWM_AUDIO_CHECK(a, str, ret_val)                          \
    if (!(a))                                                     \
    {                                                             \
        ESP_LOGE(TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val);                                         \
    }

static const char *PWM_AUDIO_PARAM_ADDR_ERROR = "PWM AUDIO PARAM ADDR ERROR";
static const char *PWM_AUDIO_FRAMERATE_ERROR  = "PWM AUDIO FRAMERATE ERROR";
static const char *PWM_AUDIO_STATUS_ERROR     = "PWM AUDIO STATUS ERROR";
static const char *PWM_AUDIO_TG_NUM_ERROR     = "PWM AUDIO TIMER GROUP NUMBER ERROR";
static const char *PWM_AUDIO_TIMER_NUM_ERROR  = "PWM AUDIO TIMER NUMBER ERROR";
static const char *PWM_AUDIO_ALLOC_ERROR      = "PWM AUDIO ALLOC ERROR";
static const char *PWM_AUDIO_RESOLUTION_ERROR = "PWM AUDIO RESOLUTION ERROR";
static const char *PWM_AUDIO_NOT_INITIALIZED  = "PWM AUDIO Uninitialized";

#define BUFFER_MIN_SIZE     (256UL)
#define SAMPLE_RATE_MAX     (48000)
#define SAMPLE_RATE_MIN     (8000)
#define CHANNEL_LEFT_INDEX  (0)
#define CHANNEL_RIGHT_INDEX (1)
#define CHANNEL_LEFT_MASK   (0x01)
#define CHANNEL_RIGHT_MASK  (0x02)
#define VOLUME_0DB          (16)

/**
 * Debug Configuration
 **/
#define ISR_DEBUG 0  /**< INDEBUG SWITCH */
#define ISR_DEBUG_IO_MASK 0x8000

typedef struct {
    char *buf;                         /**< Original pointer */
    volatile uint32_t head;            /**< ending pointer */
    volatile uint32_t tail;            /**< Read pointer */
    volatile uint32_t size;            /**< Buffer size */
    volatile uint32_t is_give;         /**< semaphore give flag */
    SemaphoreHandle_t semaphore_rb;    /**< Semaphore for ringbuffer */
} ringbuf_handle_t;

typedef struct {
    pwm_audio_config_t    config;                          /**< pwm audio config struct */
    ledc_channel_config_t ledc_channel[PWM_AUDIO_CH_MAX];  /**< ledc channel config */
    ledc_timer_config_t   ledc_timer;                      /**< ledc timer config  */
    timg_dev_t            *timg_dev;                       /**< timer group register pointer */
    ringbuf_handle_t      *ringbuf;                        /**< audio ringbuffer pointer */
    uint32_t              channel_mask;                    /**< channel gpio mask */
    uint32_t              channel_set_num;                 /**< channel audio set number */
    int32_t               framerate;                       /*!< frame rates in Hz */
    int32_t               bits_per_sample;                 /*!< bits per sample (8, 16, 32) */
    int32_t               volume;                          /*!< the volume(-VOLUME_0DB ~ VOLUME_0DB) */
    pwm_audio_status_t status;
    mp_obj_t callback;
    /**< ledc some register pointer */
     volatile uint32_t *g_ledc_left_conf0_val;
     volatile uint32_t *g_ledc_left_conf1_val;
     volatile uint32_t *g_ledc_left_duty_val;
     volatile uint32_t *g_ledc_right_conf0_val;
     volatile uint32_t *g_ledc_right_conf1_val;
     volatile uint32_t *g_ledc_right_duty_val;

} pwm_audio_data_t;

typedef struct{
    int index;
    ledc_timer_t ledc_timer_sel;
    timer_group_t tg_num;                /*!< timer group number (0 - 1) */
    timer_idx_t timer_num;               /*!< timer number  (0 - 1) */
    ledc_channel_t ledc_channel_left;    /*!< LEDC channel (0 - 7), Corresponding to left channel*/
    ledc_channel_t ledc_channel_right;   /*!< LEDC channel (0 - 7), Corresponding to right channel*/

}pwm_channel_t;

const pwm_channel_t pwm_channel[4]={
                                 {0,LEDC_TIMER_0,TIMER_GROUP_0,TIMER_0,LEDC_CHANNEL_0,LEDC_CHANNEL_1},
                                 {1,LEDC_TIMER_1,TIMER_GROUP_0,TIMER_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3},
                                 {2,LEDC_TIMER_2,TIMER_GROUP_1,TIMER_0,LEDC_CHANNEL_4,LEDC_CHANNEL_5},
                                 {3,LEDC_TIMER_3,TIMER_GROUP_1,TIMER_1,LEDC_CHANNEL_6,LEDC_CHANNEL_7}
                                };

/**< pwm audio handle pointer */
static pwm_audio_data_t *g_pwm_audio_handle[4]={0};

//static int ch_index=0;


static portMUX_TYPE timer_spinlock;

/**
 * Ringbuffer for pwm audio
 */
static esp_err_t rb_destroy(ringbuf_handle_t *rb)
{
    if (rb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (rb->buf) {
        heap_caps_free(rb->buf);
    }

    if (rb->semaphore_rb) {
        vSemaphoreDelete(rb->semaphore_rb);
    }

    heap_caps_free(rb);
    rb = NULL;
    return ESP_OK;
}
static ringbuf_handle_t *rb_create(uint32_t size)
{
    if (size < (BUFFER_MIN_SIZE << 2)) {
        ESP_LOGE(TAG, "Invalid buffer size, Minimum = %d", (int32_t)(BUFFER_MIN_SIZE << 2));
        return NULL;
    }

    ringbuf_handle_t *rb = NULL;
    char *buf = NULL;

    do {
        bool _success =
            (
                (rb             = heap_caps_malloc(sizeof(ringbuf_handle_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL)) &&
                (buf            = heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL))   &&
                (rb->semaphore_rb   = xSemaphoreCreateBinary())

            );

        if (!_success) {
            break;
        }

        rb->is_give = 0;
        rb->buf = buf;
        rb->head = rb->tail = 0;
        rb->size = size;
        return rb;

    } while (0);

    rb_destroy(rb);
    return NULL;
}

static uint32_t IRAM_ATTR rb_get_count(ringbuf_handle_t *rb)
{
    uint32_t tail = rb->tail;

    if (rb->head >= tail) {
        return (rb->head - tail);
    }

    return (rb->size - (tail - rb->head));
}

static uint32_t IRAM_ATTR rb_get_free(ringbuf_handle_t *rb)
{
    /** < Free a byte to judge the ringbuffer direction */
    return (rb->size - rb_get_count(rb) - 1);
}

static esp_err_t rb_flush(ringbuf_handle_t *rb)
{
    rb->tail = rb->head = 0;
    return ESP_OK;
}

static esp_err_t IRAM_ATTR rb_read_byte(ringbuf_handle_t *rb, uint8_t *outdata)
{
    uint32_t tail = rb->tail;

    if (tail == rb->head) {
        return ESP_FAIL;
    }

    // Send a byte from the buffer
    *outdata = rb->buf[tail];

    // Update tail position
    tail++;

    if (tail == rb->size) {
        tail = 0;
    }

    rb->tail = tail;
    return ESP_OK;
}

static esp_err_t rb_write_byte(ringbuf_handle_t *rb, const uint8_t indata)
{
    // Calculate next head
    uint32_t next_head = rb->head + 1;

    if (next_head == rb->size) {
        next_head = 0;
    }

    if (next_head == rb->tail) {
        return ESP_FAIL;
    }

    // Store data and advance head
    rb->buf[rb->head] = indata;
    rb->head = next_head;
    return ESP_OK;
}

static esp_err_t rb_wait_semaphore(ringbuf_handle_t *rb, TickType_t ticks_to_wait)
{
    rb->is_give = 0; /**< As long as it's written, it's allowed to give semaphore again */

    if (xSemaphoreTake(rb->semaphore_rb, ticks_to_wait) == pdTRUE) {
        return ESP_OK;
    }

    return ESP_FAIL;
}


/*
 * Note:
 * In order to improve efficiency, register is operated directly
 */
static inline void ledc_set_left_duty_fast(int ch_index,uint32_t duty_val)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];

    *handle->g_ledc_left_duty_val = (duty_val) << 4; /* Discard decimal part */
    *handle->g_ledc_left_conf0_val |= 0x00000014;
    *handle->g_ledc_left_conf1_val |= 0x80000000;
}
static inline void ledc_set_right_duty_fast(int ch_index,uint32_t duty_val)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    *handle->g_ledc_right_duty_val = (duty_val) << 4; /* Discard decimal part */
    *handle->g_ledc_right_conf0_val |= 0x00000014;
    *handle->g_ledc_right_conf1_val |= 0x80000000;
}


/*
 * Timer group ISR handler
 */
void IRAM_ATTR timer_group_isr(void *para)
{
    int ch_index=(int)para;
    if(ch_index > 3 || ch_index < 0)return;
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];

    if (handle == NULL) {
        return;
    }
    /* Clear the interrupt */
  //  if (REG_GET_BIT(TIMG_INT_ST_TIMERS_REG(handle->config.timer_num), BIT(handle->config.timer_num))) {
   //     REG_SET_BIT(TIMG_INT_CLR_TIMERS_REG(handle->config.timer_num), BIT(handle->config.timer_num));
 //   }
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(handle->config.tg_num);
    timer_group_clr_intr_status_in_isr(handle->config.tg_num, handle->config.timer_num);
    /* After the alarm has been triggered
        we need enable it again, so it is triggered the next time */
   // REG_SET_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_ALARM_EN_M);

    timer_group_enable_alarm_in_isr(handle->config.tg_num, handle->config.timer_num);

    static uint8_t wave_h, wave_l;
    static uint16_t value;
    ringbuf_handle_t *rb = handle->ringbuf;
#if (1==ISR_DEBUG)
    GPIO.out_w1ts = ISR_DEBUG_IO_MASK;
#endif

    /**
     * It is believed that the channel configured with GPIO needs to output sound
    */
    if (handle->channel_mask & CHANNEL_LEFT_MASK) {
        if (handle->config.duty_resolution > 8) {
            if (rb_get_count(rb) > 1) {
                rb_read_byte(rb, &wave_l);
                rb_read_byte(rb, &wave_h);
                value = ((wave_h << 8) | wave_l);
                ledc_set_left_duty_fast(ch_index,value);/**< set the PWM duty */
            }
        } else {
            if (ESP_OK == rb_read_byte(rb, &wave_h)) {
                ledc_set_left_duty_fast(ch_index,wave_h);/**< set the PWM duty */
            }
        }
    }

    /**
     * If two gpios are configured, but the audio data has only one channel, copy the data to the right channel
     * Instead, the right channel data is discarded
    */
    if (handle->channel_mask & CHANNEL_RIGHT_MASK) {
        if (handle->channel_set_num == 1) {
            if (handle->config.duty_resolution > 8) {
                ledc_set_right_duty_fast(ch_index,value);/**< set the PWM duty */
            } else {
                ledc_set_right_duty_fast(ch_index,wave_h);/**< set the PWM duty */
            }
        } else {
            if (handle->config.duty_resolution > 8) {
                if (rb_get_count(rb) > 1) {
                    rb_read_byte(rb, &wave_l);
                    rb_read_byte(rb, &wave_h);
                    value = ((wave_h << 8) | wave_l);
                    ledc_set_right_duty_fast(ch_index,value);/**< set the PWM duty */
                }
            } else {
                if (ESP_OK == rb_read_byte(rb, &wave_h)) {
                    ledc_set_right_duty_fast(ch_index,wave_h);/**< set the PWM duty */
                }
            }
        }
    } else {
        if (handle->channel_set_num == 2) {
            /**
             * Discard the right channel data only if the right channel is configured but the audio data is stereo
             * Read buffer but do nothing
             */
            if (handle->config.duty_resolution > 8) {
                if (rb_get_count(rb) > 1) {
                    rb_read_byte(rb, &wave_h);
                    rb_read_byte(rb, &wave_h);
                }
            } else {
                rb_read_byte(rb, &wave_h);
            }
        }
    }

    /**
     * Send semaphore when buffer free is more than BUFFER_MIN_SIZE
     */
    uint32_t free_size = rb_get_free(rb);
    if (0 == rb->is_give && free_size > BUFFER_MIN_SIZE) {
        /**< The execution time of the following code is 2.71 microsecond */
        rb->is_give = 1; /**< To prevent multiple give semaphores */
        if(handle->callback)mp_sched_schedule(handle->callback, mp_const_none);
        BaseType_t xHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR(rb->semaphore_rb, &xHigherPriorityTaskWoken);

        if (pdFALSE != xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
#if (1==ISR_DEBUG)
    GPIO.out_w1tc = ISR_DEBUG_IO_MASK;
#endif
}

esp_err_t pwm_audio_get_status(int ch_index,pwm_audio_status_t *status)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    *status = handle->status;
    return ESP_OK;
}

esp_err_t pwm_audio_get_param(int ch_index,int *rate, int *bits, int *ch)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);

    if (NULL != rate) {
        *rate = handle->framerate;
    }

    if (NULL != bits) {
        *bits = (int)handle->bits_per_sample;
    }

    if (NULL != ch) {
        *ch = handle->channel_set_num;
    }

    return ESP_OK;
}

esp_err_t pwm_audio_init(int ch_index,const pwm_audio_config_t *cfg)
{
    esp_err_t res = ESP_OK;
    PWM_AUDIO_CHECK(cfg != NULL, PWM_AUDIO_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(cfg->tg_num < TIMER_GROUP_MAX, PWM_AUDIO_TG_NUM_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(cfg->timer_num < TIMER_MAX, PWM_AUDIO_TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(cfg->duty_resolution <= 10 && cfg->duty_resolution >= 8, PWM_AUDIO_RESOLUTION_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(NULL == g_pwm_audio_handle[ch_index], "Already initiate", ESP_ERR_INVALID_STATE);
    vPortCPUInitializeMutex(&timer_spinlock);
    ESP_LOGI(TAG, "timer: %d:%d | left io: %d | right io: %d | resolution: %dBIT",
             cfg->tg_num, cfg->timer_num, cfg->gpio_num_left, cfg->gpio_num_right, cfg->duty_resolution);

    pwm_audio_data_t *handle = NULL;

    handle = heap_caps_malloc(sizeof(pwm_audio_data_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    PWM_AUDIO_CHECK(handle != NULL, PWM_AUDIO_ALLOC_ERROR, ESP_ERR_NO_MEM);
    memset(handle, 0, sizeof(pwm_audio_data_t));

    handle->ringbuf = rb_create(cfg->ringbuf_len);
    PWM_AUDIO_CHECK(handle->ringbuf != NULL, PWM_AUDIO_ALLOC_ERROR, ESP_ERR_NO_MEM);

    handle->config = *cfg;
    g_pwm_audio_handle[ch_index] = handle;
#if (1==ISR_DEBUG)
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = ISR_DEBUG_IO_MASK;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
#endif

    /**< Get timer group register pointer */
    if (cfg->tg_num == TIMER_GROUP_0) {
        handle->timg_dev = &TIMERG0;
    } else {
        handle->timg_dev = &TIMERG1;
    }

    /**
     * config ledc to generate pwm
     */
    handle->channel_mask = 0;
    handle->ledc_channel[CHANNEL_LEFT_INDEX].gpio_num = -1;
    handle->ledc_channel[CHANNEL_RIGHT_INDEX].gpio_num = -1;

    if (handle->config.gpio_num_left >= 0) {
        handle->ledc_channel[CHANNEL_LEFT_INDEX].channel = handle->config.ledc_channel_left;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].duty = 0;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].gpio_num = handle->config.gpio_num_left;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].speed_mode = LEDC_LOW_SPEED_MODE;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].hpoint = 0;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].timer_sel = handle->config.ledc_timer_sel;
        handle->ledc_channel[CHANNEL_LEFT_INDEX].intr_type = LEDC_INTR_DISABLE;
        res = ledc_channel_config(&handle->ledc_channel[CHANNEL_LEFT_INDEX]);
        PWM_AUDIO_CHECK(ESP_OK == res, "LEDC channel left configuration failed", ESP_ERR_INVALID_ARG);
        handle->channel_mask |= CHANNEL_LEFT_MASK;
    }

    if (handle->config.gpio_num_right >= 0) {
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel = handle->config.ledc_channel_right;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].duty = 0;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].gpio_num = handle->config.gpio_num_right;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].speed_mode = LEDC_LOW_SPEED_MODE;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].hpoint = 0;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].timer_sel = handle->config.ledc_timer_sel;
        handle->ledc_channel[CHANNEL_RIGHT_INDEX].intr_type = LEDC_INTR_DISABLE;
        res = ledc_channel_config(&handle->ledc_channel[CHANNEL_RIGHT_INDEX]);
        PWM_AUDIO_CHECK(ESP_OK == res, "LEDC channel right configuration failed", ESP_ERR_INVALID_ARG);
        handle->channel_mask |= CHANNEL_RIGHT_MASK;
    }

    PWM_AUDIO_CHECK(0 != handle->channel_mask, "Assign at least one channel gpio", ESP_ERR_INVALID_ARG);

#ifdef CONFIG_IDF_TARGET_ESP32S2
    handle->ledc_timer.clk_cfg = LEDC_USE_APB_CLK;
#endif
    handle->ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    handle->ledc_timer.duty_resolution = handle->config.duty_resolution;
    handle->ledc_timer.timer_num = handle->config.ledc_timer_sel;
    uint32_t freq = (APB_CLK_FREQ / (1 << handle->ledc_timer.duty_resolution));
    handle->ledc_timer.freq_hz = freq - (freq % 1000); // fixed PWM frequency ,It's a multiple of 1000
    res = ledc_timer_config(&handle->ledc_timer);
    PWM_AUDIO_CHECK(res == ESP_OK, "LEDC timer configuration failed", ESP_ERR_INVALID_ARG);

    /**
     * Get the address of LEDC register to reduce the addressing time
     */
    handle->g_ledc_left_duty_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                           channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].duty.val;
    handle->g_ledc_left_conf0_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                            channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].conf0.val;
    handle->g_ledc_left_conf1_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                            channel[handle->ledc_channel[CHANNEL_LEFT_INDEX].channel].conf1.val;
    handle->g_ledc_right_duty_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                            channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].duty.val;
    handle->g_ledc_right_conf0_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                             channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].conf0.val;
    handle->g_ledc_right_conf1_val = &LEDC.channel_group[handle->ledc_timer.speed_mode].\
                             channel[handle->ledc_channel[CHANNEL_RIGHT_INDEX].channel].conf1.val;

    /**
     * Select and initialize basic parameters of the timer
     */
    timer_config_t config = {0};
    config.divider = 16;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = 1;
#ifdef TIMER_GROUP_SUPPORTS_XTAL_CLOCK
    config.clk_src = TIMER_SRC_CLK_APB;  /* ESP32-S2 specific control bit !!!*/
#endif
    res = timer_init(handle->config.tg_num, handle->config.timer_num, &config);
    PWM_AUDIO_CHECK(ESP_OK == res, "Timer group configuration failed", ESP_FAIL);
    timer_isr_register(handle->config.tg_num, handle->config.timer_num, timer_group_isr, (void *)ch_index, ESP_INTR_FLAG_IRAM, NULL);

    /**< set a initial parameter */
    res = pwm_audio_set_param(ch_index,16000, 8, 2);
    PWM_AUDIO_CHECK(ESP_OK == res, "Set parameter failed", ESP_FAIL);

    pwm_audio_set_volume(ch_index,0);

    handle->status = PWM_AUDIO_STATUS_IDLE;

    return res;
}

esp_err_t pwm_audio_set_param(int ch_index,int rate, ledc_timer_bit_t bits, int ch)
{
    esp_err_t res = ESP_OK;

    PWM_AUDIO_CHECK(g_pwm_audio_handle[ch_index]->status != PWM_AUDIO_STATUS_BUSY, PWM_AUDIO_STATUS_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(rate <= SAMPLE_RATE_MAX && rate >= SAMPLE_RATE_MIN, PWM_AUDIO_FRAMERATE_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(bits == 32 || bits == 16 || bits == 8, " Unsupported Bit width, only support 8, 16, 32", ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(ch <= 2 && ch >= 1, "Unsupported channel number, only support mono and stereo", ESP_ERR_INVALID_ARG);

    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];

    handle->framerate = rate;
    handle->bits_per_sample = bits;
    handle->channel_set_num = ch;

    // timer_disable_intr(handle->config.tg_num, handle->config.timer_num);
    /* Timer's counter will initially start from value below.
    Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(handle->config.tg_num, handle->config.timer_num, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
  //  uint32_t divider = REG_GET_FIELD(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_DIVIDER);
    timer_config_t config;
    timer_get_config(handle->config.tg_num, handle->config.timer_num,&config);
    timer_set_alarm_value(handle->config.tg_num, handle->config.timer_num, (TIMER_BASE_CLK /config.divider) / handle->framerate);
    // timer_enable_intr(handle->config.tg_num, handle->config.timer_num);
    return res;
}

esp_err_t pwm_audio_set_sample_rate(int ch_index,int rate)
{
    esp_err_t res;
    PWM_AUDIO_CHECK(NULL != g_pwm_audio_handle[ch_index], PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    PWM_AUDIO_CHECK(g_pwm_audio_handle[ch_index]->status != PWM_AUDIO_STATUS_BUSY, PWM_AUDIO_STATUS_ERROR, ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(rate <= SAMPLE_RATE_MAX && rate >= SAMPLE_RATE_MIN, PWM_AUDIO_FRAMERATE_ERROR, ESP_ERR_INVALID_ARG);

    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    handle->framerate = rate;
   // uint32_t divider = REG_GET_FIELD(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_DIVIDER);
    timer_config_t config;
    timer_get_config(handle->config.tg_num, handle->config.timer_num,&config);
    res = timer_set_alarm_value(handle->config.tg_num, handle->config.timer_num, (TIMER_BASE_CLK / config.divider) / handle->framerate);
    return res;
}

esp_err_t pwm_audio_set_volume(int ch_index,int8_t volume)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    if (volume < 0) {
        PWM_AUDIO_CHECK(-volume <= VOLUME_0DB, "Volume is too small", ESP_ERR_INVALID_ARG);
    } else {
        PWM_AUDIO_CHECK(volume <= VOLUME_0DB, "Volume is too large", ESP_ERR_INVALID_ARG);
    }

    handle->volume = volume + VOLUME_0DB;
    return ESP_OK;
}

esp_err_t pwm_audio_get_volume(int ch_index,int8_t *volume)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    PWM_AUDIO_CHECK(NULL != volume, PWM_AUDIO_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    *volume = handle->volume;
    return ESP_OK;
}

esp_err_t IRAM_ATTR pwm_audio_write(int ch_index,uint8_t *inbuf, size_t inbuf_len, size_t *bytes_written, TickType_t ticks_to_wait)
{
    esp_err_t res = ESP_OK;
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    PWM_AUDIO_CHECK(inbuf != NULL && bytes_written != NULL, "Invalid pointer", ESP_ERR_INVALID_ARG);
    PWM_AUDIO_CHECK(inbuf_len != 0, "Length should not be zero", ESP_ERR_INVALID_ARG);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    *bytes_written = 0;
    ringbuf_handle_t *rb = handle->ringbuf;
    TickType_t start_ticks = xTaskGetTickCount();

    while (inbuf_len) {
        uint32_t free = rb_get_free(rb);
        if (ESP_OK == rb_wait_semaphore(rb, ticks_to_wait)) {
            uint32_t bytes_can_write = inbuf_len;

            if (inbuf_len > free) {
                bytes_can_write = free;
            }

            bytes_can_write &= 0xfffffffc;/**< Aligned data, bytes_can_write should be an integral multiple of 4 */

            if (0 == bytes_can_write) {
                *bytes_written += inbuf_len;  /**< Discard the last misaligned bytes of data directly */
                return ESP_OK;
            }

            /**< Get the difference between PWM resolution and audio samplewidth */
            int8_t shift = handle->bits_per_sample - handle->config.duty_resolution;
            uint32_t len = bytes_can_write;

            switch (handle->bits_per_sample) {
            case 8: {
                if (shift < 0) {
                    /**< When the PWM resolution is greater than 8 bits, the value needs to be expanded */
                    uint16_t value;
                    uint8_t temp;
                    shift = -shift;
                    len >>= 1;
                    bytes_can_write >>= 1;

                    for (size_t i = 0; i < len; i++) {
                        temp = (inbuf[i] * handle->volume / VOLUME_0DB) + 0x7f; /**< offset */
                        value = temp << shift;
                        rb_write_byte(rb, value);
                        rb_write_byte(rb, value >> 8);
                    }
                } else {
                    uint8_t value;

                    for (size_t i = 0; i < len; i++) {
                        value = (inbuf[i] * handle->volume / VOLUME_0DB) + 0x7f; /**< offset */
                        rb_write_byte(rb, value);
                    }
                }
            }
            break;

            case 16: {
                len >>= 1;
                uint16_t *buf_16b = (uint16_t *)inbuf;
                static uint16_t value_16b;
                int16_t temp;

                if (handle->config.duty_resolution > 8) {
                    for (size_t i = 0; i < len; i++) {
                        temp = buf_16b[i];
                        temp = temp * handle->volume / VOLUME_0DB;
                        value_16b = temp + 0x7fff; /**< offset */
                        value_16b >>= shift;
                        rb_write_byte(rb, value_16b);
                        rb_write_byte(rb, value_16b >> 8);
                    }
                } else {
                    /**
                     * When the PWM resolution is 8 bit, only one byte is transmitted
                     */
                    for (size_t i = 0; i < len; i++) {
                        temp = buf_16b[i];
                        temp = temp * handle->volume / VOLUME_0DB;
                        value_16b = temp + 0x7fff; /**< offset */
                        value_16b >>= shift;
                        rb_write_byte(rb, value_16b);
                    }
                }
            }
            break;

            case 32: {
                len >>= 2;
                uint32_t *buf_32b = (uint32_t *)inbuf;
                uint32_t value;
                int32_t temp;

                if (handle->config.duty_resolution > 8) {
                    for (size_t i = 0; i < len; i++) {
                        temp = buf_32b[i];
                        temp = temp * handle->volume / VOLUME_0DB;
                        value = temp + 0x7fffffff; /**< offset */
                        value >>= shift;
                        rb_write_byte(rb, value);
                        rb_write_byte(rb, value >> 8);
                    }
                } else {
                    /**
                     * When the PWM resolution is 8 bit, only one byte is transmitted
                     */
                    for (size_t i = 0; i < len; i++) {
                        temp = buf_32b[i];
                        temp = temp * handle->volume / VOLUME_0DB;
                        value = temp + 0x7fffffff; /**< offset */
                        value >>= shift;
                        rb_write_byte(rb, value);
                    }
                }
            }
            break;

            default:
                break;
            }

            inbuf += bytes_can_write;
            inbuf_len -= bytes_can_write;
            *bytes_written += bytes_can_write;

        } else {
            res = ESP_FAIL;
            ESP_LOGD(TAG, "tg config=%x\n", handle->timg_dev->hw_timer[handle->config.timer_num].config.val);
        }

        if ((xTaskGetTickCount() - start_ticks) >= ticks_to_wait) {
            return res;
        }
    }

    return res;
}

esp_err_t pwm_audio_start(int ch_index)
{
    esp_err_t res;
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    PWM_AUDIO_CHECK(handle->status == PWM_AUDIO_STATUS_IDLE, PWM_AUDIO_STATUS_ERROR, ESP_ERR_INVALID_STATE);

    handle->status = PWM_AUDIO_STATUS_BUSY;

    /**< timer enable interrupt */
    portENTER_CRITICAL_SAFE(&timer_spinlock);
  //  REG_SET_BIT(TIMG_INT_ENA_TIMERS_REG(handle->config.timer_num), BIT(handle->config.timer_num));
    timer_enable_intr(handle->config.tg_num, handle->config.timer_num);
  //  REG_SET_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_EN_M);
  //  REG_SET_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_ALARM_EN_M);
#ifdef TIMG_T0_LEVEL_INT_EN_M
 //   REG_SET_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_LEVEL_INT_EN_M);
#endif
    portEXIT_CRITICAL_SAFE(&timer_spinlock);

    res = timer_start(handle->config.tg_num, handle->config.timer_num);
    return res;
}

esp_err_t pwm_audio_stop(int ch_index)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(NULL != handle, PWM_AUDIO_NOT_INITIALIZED, ESP_ERR_INVALID_STATE);
    PWM_AUDIO_CHECK(handle->status == PWM_AUDIO_STATUS_BUSY, PWM_AUDIO_STATUS_ERROR, ESP_ERR_INVALID_STATE);

    /**< just disable timer ,keep pwm output to reduce switching nosie */
    /**< timer disable interrupt */
    portENTER_CRITICAL_SAFE(&timer_spinlock);
    REG_CLR_BIT(TIMG_INT_ENA_TIMERS_REG(handle->config.timer_num), BIT(handle->config.timer_num));
    REG_CLR_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_EN_M);
    REG_CLR_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_ALARM_EN_M);
#ifdef TIMG_T0_LEVEL_INT_EN_M
    REG_CLR_BIT(TIMG_T0CONFIG_REG(handle->config.timer_num), TIMG_T0_LEVEL_INT_EN_M);
#endif
    portEXIT_CRITICAL_SAFE(&timer_spinlock);

    // timer_pause(handle->config.tg_num, handle->config.timer_num);
    rb_flush(handle->ringbuf);  /**< flush ringbuf, avoid play noise */
    handle->status = PWM_AUDIO_STATUS_IDLE;
    return ESP_OK;
}

esp_err_t pwm_audio_deinit(int ch_index)
{
    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
    PWM_AUDIO_CHECK(handle != NULL, PWM_AUDIO_PARAM_ADDR_ERROR, ESP_FAIL);

    handle->status = PWM_AUDIO_STATUS_UN_INIT;
    timer_pause(handle->config.tg_num, handle->config.timer_num);
    timer_disable_intr(handle->config.tg_num, handle->config.timer_num);

    for (size_t i = 0; i < PWM_AUDIO_CH_MAX; i++) {
        if (handle->ledc_channel[i].gpio_num >= 0) {
            ledc_stop(handle->ledc_channel[i].speed_mode, handle->ledc_channel[i].channel, 0);
        }
    }

    /**< set the channel gpios input mode */
    for (size_t i = 0; i < PWM_AUDIO_CH_MAX; i++) {
        if (handle->ledc_channel[i].gpio_num >= 0) {
            gpio_set_direction(handle->ledc_channel[i].gpio_num, GPIO_MODE_INPUT);
        }
    }

    rb_destroy(handle->ringbuf);
    heap_caps_free(handle);
    g_pwm_audio_handle[ch_index] = NULL;
    return ESP_OK;
}
// This is the function which will be called from Python, as factorial(x)
STATIC mp_obj_t init(size_t n_args, const mp_obj_t *args) {
    // Extract the integer from the MicroPython input object
   
    int ch_index=mp_obj_get_int(args[0]);//0

    int gpio_num_left=mp_obj_get_int(args[1]);//25
    int gpio_num_right=mp_obj_get_int(args[2]);//26
    int resolution = mp_obj_get_int(args[3]);//16
  

    pwm_audio_config_t pac;
    pac.duty_resolution    = resolution;
    pac.gpio_num_left      = gpio_num_left;
    pac.ledc_channel_left  = pwm_channel[ch_index].ledc_channel_left;
    pac.gpio_num_right     = gpio_num_right;
    pac.ledc_channel_right = pwm_channel[ch_index].ledc_channel_right;

    pac.ledc_timer_sel     = pwm_channel[ch_index].ledc_timer_sel;
    pac.tg_num             = pwm_channel[ch_index].tg_num;
    pac.timer_num          = pwm_channel[ch_index].timer_num;//TIMER_0;
    pac.ringbuf_len        = 1024 * 8;

    pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
   
    if(handle!=NULL)pwm_audio_deinit(ch_index);

    pwm_audio_init(ch_index,&pac);
  
    // Convert the result to a MicroPython integer object and return it
    return mp_const_none;
}
//esp_err_t IRAM_ATTR pwm_audio_write(uint8_t *inbuf, size_t inbuf_len, size_t *bytes_written, TickType_t ticks_to_wait)

STATIC mp_obj_t write(mp_obj_t ch_index_obj,mp_obj_t wr_buf){
  //  unsigned l;
    int ch_index=mp_obj_get_int(ch_index_obj);
    unsigned bytes_written;
  //  int len = mp_obj_get_int(len_obj);
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
 //   if(len > src.len || len<0)len=src.len;
    pwm_audio_write(ch_index,src.buf,src.len,&bytes_written,1000 / portTICK_PERIOD_MS);
    return mp_obj_new_int(bytes_written);
}
STATIC mp_obj_t set_param(size_t n_args, const mp_obj_t *args){

    int ch_index=mp_obj_get_int(args[0]);
    int rate = mp_obj_get_int(args[1]);
    int bits = mp_obj_get_int(args[2]);
    int ch = mp_obj_get_int(args[3]);
    pwm_audio_set_param(ch_index,rate, bits, ch);
    return mp_const_none;

}
STATIC mp_obj_t start(mp_obj_t ch_index_obj){
 int ch_index=mp_obj_get_int(ch_index_obj);
pwm_audio_start(ch_index);
return mp_const_none;
}

STATIC mp_obj_t stop(mp_obj_t ch_index_obj){
 int ch_index=mp_obj_get_int(ch_index_obj);
pwm_audio_stop(ch_index);
return mp_const_none;
}

STATIC mp_obj_t callback(mp_obj_t ch_index_obj,mp_obj_t callback_obj){
int ch_index=mp_obj_get_int(ch_index_obj);
pwm_audio_data_t *handle = g_pwm_audio_handle[ch_index];
handle->callback=callback_obj;
return mp_const_none;
}

// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(init_obj,4,6, init);

STATIC MP_DEFINE_CONST_FUN_OBJ_2(write_obj, write);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(set_param_obj,4,4 ,set_param);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(start_obj, start);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stop_obj, stop);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(callback_obj, callback);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t audiopwm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiopwm) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&init_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&write_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_param), MP_ROM_PTR(&set_param_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&callback_obj) },
};

STATIC MP_DEFINE_CONST_DICT(audiopwm_module_globals, audiopwm_module_globals_table);

// Define module object.
const mp_obj_module_t audiopwm_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiopwm_module_globals,
};

// Register the module to make it available in Python.
// Note: the "1" in the third argument means this module is always enabled.
// This "1" can be optionally replaced with a macro like MODULE_CEXAMPLE_ENABLED
// which can then be used to conditionally enable this module.
MP_REGISTER_MODULE(MP_QSTR_audiopwm, audiopwm_user_cmodule, 1);
#endif
