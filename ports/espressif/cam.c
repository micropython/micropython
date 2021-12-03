// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/i2s_struct.h"
#include "soc/apb_ctrl_reg.h"
#ifdef CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/lldesc.h"
#include "esp32s2/rom/cache.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/rom/lldesc.h"
#include "esp32s3/rom/cache.h"
#endif
#include "soc/dport_access.h"
#include "soc/dport_reg.h"
#include "driver/ledc.h"
#include "cam.h"
#include "hal/gpio_ll.h"

static const char *TAG = "cam";

#define CAM_DMA_MAX_SIZE     (4095)

typedef enum {
    CAM_IN_SUC_EOF_EVENT = 0,
    CAM_VSYNC_EVENT
} cam_event_t;

typedef struct {
    uint8_t *frame_buffer;
    size_t len;
} frame_buffer_event_t;

typedef struct {
    uint32_t buffer_size;
    uint32_t half_buffer_size;
    uint32_t node_cnt;
    uint32_t half_node_cnt;
    uint32_t dma_size;
    uint32_t cnt;
    uint32_t total_cnt;
    lldesc_t *dma;
    uint8_t *buffer;
    uint8_t *frame1_buffer;
    uint8_t *frame2_buffer;
    uint8_t frame1_buffer_en;
    uint8_t frame2_buffer_en;
    uint8_t jpeg_mode;
    uint8_t vsync_pin;
    uint8_t vsync_invert;
    uint8_t hsync_invert;
    QueueHandle_t event_queue;
    QueueHandle_t frame_buffer_queue;
    TaskHandle_t task_handle;
    intr_handle_t intr_handle;
} cam_obj_t;

static cam_obj_t *cam_obj = NULL;

static void IRAM_ATTR cam_isr(void *arg) {
    cam_event_t cam_event = {0};
    BaseType_t HPTaskAwoken = pdFALSE;
    typeof(I2S0.int_st) int_st = I2S0.int_st;
    I2S0.int_clr.val = int_st.val;

    if (int_st.in_suc_eof) {
        cam_event = CAM_IN_SUC_EOF_EVENT;
        xQueueSendFromISR(cam_obj->event_queue, (void *)&cam_event, &HPTaskAwoken);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR cam_vsync_isr(void *arg) {
    cam_event_t cam_event = {0};
    BaseType_t HPTaskAwoken = pdFALSE;
    /*!< filter */
    ets_delay_us(1);

    if (gpio_ll_get_level(&GPIO, cam_obj->vsync_pin) == !cam_obj->vsync_invert) {
        cam_event = CAM_VSYNC_EVENT;
        xQueueSendFromISR(cam_obj->event_queue, (void *)&cam_event, &HPTaskAwoken);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void cam_config(const cam_config_t *config) {
    /*!<Enable I2S periph */
    periph_module_enable(PERIPH_I2S0_MODULE);

    /*!< Configure the clock */
    #if 0
    I2S0.clkm_conf.val = 0;
    I2S0.clkm_conf.clkm_div_num = 2; // <--- this should set the low 8 bits, but doesn't somehow
    I2S0.clkm_conf.clkm_div_b = 0;
    I2S0.clkm_conf.clkm_div_a = 0;
    I2S0.clkm_conf.clk_sel = 2;
    I2S0.clkm_conf.clk_en = 1;
    #else
    // The above code should set the clock to this value .. however, it sets it to 0x0050_0000 instead, the "clkm_div_num" value missing!
    // it's not clear why this behavior arises
    I2S0.clkm_conf.val = 0x00500002;
    #endif
    /*!< Configuration sampling rate */
    I2S0.sample_rate_conf.val = 0;
    I2S0.sample_rate_conf.tx_bck_div_num = 2;
    I2S0.sample_rate_conf.tx_bits_mod = 8;
    I2S0.sample_rate_conf.rx_bck_div_num = 1;
    I2S0.sample_rate_conf.rx_bits_mod = config->bit_width;

    /*!< Configuration data format */
    I2S0.conf.val = 0;
    I2S0.conf.tx_right_first = 1;
    I2S0.conf.tx_msb_right = 1;
    I2S0.conf.tx_dma_equal = 1;
    I2S0.conf.rx_right_first = 1;
    I2S0.conf.rx_msb_right = 1;
    I2S0.conf.rx_dma_equal = 1;

    I2S0.conf1.val = 0;
    I2S0.conf1.tx_pcm_bypass = 1;
    I2S0.conf1.tx_stop_en = 1;
    I2S0.conf1.rx_pcm_bypass = 1;

    I2S0.conf2.val = 0;
    I2S0.conf2.cam_sync_fifo_reset = 1;
    I2S0.conf2.cam_sync_fifo_reset = 0;
    I2S0.conf2.lcd_en = 1;
    I2S0.conf2.camera_en = 1;
    I2S0.conf2.i_v_sync_filter_en = 1;
    I2S0.conf2.i_v_sync_filter_thres = 1;

    I2S0.conf_chan.val = 0;
    I2S0.conf_chan.tx_chan_mod = 1;
    I2S0.conf_chan.rx_chan_mod = 1;

    I2S0.fifo_conf.val = 0;
    I2S0.fifo_conf.rx_fifo_mod_force_en = 1;
    I2S0.fifo_conf.rx_data_num = 32;
    I2S0.fifo_conf.rx_fifo_mod = 2;
    I2S0.fifo_conf.tx_fifo_mod_force_en = 1;
    I2S0.fifo_conf.tx_data_num = 32;
    I2S0.fifo_conf.tx_fifo_mod = 2;
    I2S0.fifo_conf.dscr_en = 1;

    I2S0.lc_conf.out_rst = 1;
    I2S0.lc_conf.out_rst = 0;
    I2S0.lc_conf.in_rst = 1;
    I2S0.lc_conf.in_rst = 0;

    I2S0.timing.val = 0;

    I2S0.int_ena.val = 0;
    I2S0.int_clr.val = ~0;

    I2S0.lc_conf.check_owner = 0;
    I2S0.conf.rx_start = 1;
}

static void cam_set_pin(const cam_config_t *config) {
    gpio_config_t io_conf = {0};
    io_conf.intr_type = config->vsync_invert ? GPIO_PIN_INTR_NEGEDGE : GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = 1 << config->pin.vsync;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(config->pin.vsync, cam_vsync_isr, NULL);
    gpio_intr_disable(config->pin.vsync);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.pclk], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.pclk, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.pclk, GPIO_FLOATING);
    gpio_matrix_in(config->pin.pclk, I2S0I_WS_IN_IDX, false);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.vsync], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.vsync, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.vsync, GPIO_FLOATING);
    gpio_matrix_in(config->pin.vsync, I2S0I_V_SYNC_IDX, config->vsync_invert);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin.hsync], PIN_FUNC_GPIO);
    gpio_set_direction(config->pin.hsync, GPIO_MODE_INPUT);
    gpio_set_pull_mode(config->pin.hsync, GPIO_FLOATING);
    gpio_matrix_in(config->pin.hsync, I2S0I_H_SYNC_IDX, config->hsync_invert);

    for (int i = 0; i < config->bit_width; i++) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[config->pin_data[i]], PIN_FUNC_GPIO);
        gpio_set_direction(config->pin_data[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(config->pin_data[i], GPIO_FLOATING);
        /*!< High position alignment, IN16 is always the highest bit */
        /*!< Fifo accesses data by bit, and when rx_bits_mod is 8, the data needs to be aligned by 8 bits */
        gpio_matrix_in(config->pin_data[i], I2S0I_DATA_IN0_IDX + (16 - config->bit_width) + i, false);
    }

    gpio_matrix_in(GPIO_FUNC_IN_HIGH, I2S0I_H_ENABLE_IDX, false);
    ESP_LOGI(TAG, "cam_set_pin\n");
}

static void cam_vsync_intr_enable(uint8_t en) {
    if (en) {
        gpio_intr_enable(cam_obj->vsync_pin);
    } else {
        gpio_intr_disable(cam_obj->vsync_pin);
    }
}

static void cam_dma_stop(void) {
    if (I2S0.int_ena.in_suc_eof == 1) {
        I2S0.conf.rx_start = 0;
        I2S0.int_ena.in_suc_eof = 0;
        I2S0.int_clr.in_suc_eof = 1;
        I2S0.in_link.stop = 1;
    }
}

static void cam_dma_start(void) {
    if (I2S0.int_ena.in_suc_eof == 0) {
        I2S0.int_clr.in_suc_eof = 1;
        I2S0.int_ena.in_suc_eof = 1;
        I2S0.conf.rx_reset = 1;
        I2S0.conf.rx_reset = 0;
        I2S0.conf.rx_fifo_reset = 1;
        I2S0.conf.rx_fifo_reset = 0;
        I2S0.lc_conf.in_rst = 1;
        I2S0.lc_conf.in_rst = 0;
        I2S0.lc_conf.ahbm_fifo_rst = 1;
        I2S0.lc_conf.ahbm_fifo_rst = 0;
        I2S0.lc_conf.ahbm_rst = 1;
        I2S0.lc_conf.ahbm_rst = 0;
        I2S0.in_link.start = 1;
        I2S0.conf.rx_start = 1;

        if (cam_obj->jpeg_mode) {
            /*!< Manually give the vsync data for the first frame */
            gpio_matrix_in(cam_obj->vsync_pin, I2S0I_V_SYNC_IDX, !cam_obj->vsync_invert);
            gpio_matrix_in(cam_obj->vsync_pin, I2S0I_V_SYNC_IDX, cam_obj->vsync_invert);
        }
    }
}

void cam_stop(void) {
    cam_vsync_intr_enable(0);
    cam_dma_stop();
}

void cam_start(void) {
    cam_vsync_intr_enable(1);
}

typedef enum {
    CAM_STATE_IDLE = 0,
    CAM_STATE_READ_BUF1 = 1,
    CAM_STATE_READ_BUF2 = 2,
} cam_state_t;

/*!<Copy fram from DMA buffer to fram buffer */
static void cam_task(void *arg) {
    int state = CAM_STATE_IDLE;
    cam_event_t cam_event = {0};
    frame_buffer_event_t frame_buffer_event = {0};
    xQueueReset(cam_obj->event_queue);

    while (1) {
        xQueueReceive(cam_obj->event_queue, (void *)&cam_event, portMAX_DELAY);

        switch (state) {
            case CAM_STATE_IDLE: {
                if (cam_event == CAM_VSYNC_EVENT) {
                    if (cam_obj->frame1_buffer_en) {
                        cam_dma_start();
                        cam_vsync_intr_enable(0);
                        state = CAM_STATE_READ_BUF1;
                    } else if (cam_obj->frame2_buffer_en) {
                        cam_dma_start();
                        cam_vsync_intr_enable(0);
                        state = CAM_STATE_READ_BUF2;
                    }

                    cam_obj->cnt = 0;
                }
            }
            break;

            case CAM_STATE_READ_BUF1: {
                if (cam_event == CAM_IN_SUC_EOF_EVENT) {
                    if (cam_obj->cnt == 0) {
                        cam_vsync_intr_enable(1); /*!< CAM real start is required to receive the first buf data and then turn on the vsync interrupt */
                    }

                    memcpy(&cam_obj->frame1_buffer[cam_obj->cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cam_obj->cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);

                    if (cam_obj->jpeg_mode) {
                        if (cam_obj->frame1_buffer_en == 0) {
                            cam_dma_stop();
                        }
                    } else {
                        if (cam_obj->cnt == cam_obj->total_cnt - 1) {
                            cam_obj->frame1_buffer_en = 0;
                        }
                    }

                    if (cam_obj->frame1_buffer_en == 0) {
                        frame_buffer_event.frame_buffer = cam_obj->frame1_buffer;
                        frame_buffer_event.len = (cam_obj->cnt + 1) * cam_obj->half_buffer_size;
                        xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_event, portMAX_DELAY);
                        state = CAM_STATE_IDLE;
                    } else {
                        cam_obj->cnt++;
                    }
                } else if (cam_event == CAM_VSYNC_EVENT) {
                    if (cam_obj->jpeg_mode) {
                        cam_obj->frame1_buffer_en = 0;
                    }
                }
            }
            break;

            case CAM_STATE_READ_BUF2: {
                if (cam_event == CAM_IN_SUC_EOF_EVENT) {
                    if (cam_obj->cnt == 0) {
                        cam_vsync_intr_enable(1); /*!< CAM real start is required to receive the first buf data and then turn on the vsync interrupt */
                    }

                    memcpy(&cam_obj->frame2_buffer[cam_obj->cnt * cam_obj->half_buffer_size], &cam_obj->buffer[(cam_obj->cnt % 2) * cam_obj->half_buffer_size], cam_obj->half_buffer_size);

                    if (cam_obj->jpeg_mode) {
                        if (cam_obj->frame2_buffer_en == 0) {
                            cam_dma_stop();
                        }
                    } else {
                        if (cam_obj->cnt == cam_obj->total_cnt - 1) {
                            cam_obj->frame2_buffer_en = 0;
                        }
                    }

                    if (cam_obj->frame2_buffer_en == 0) {
                        frame_buffer_event.frame_buffer = cam_obj->frame2_buffer;
                        frame_buffer_event.len = (cam_obj->cnt + 1) * cam_obj->half_buffer_size;
                        xQueueSend(cam_obj->frame_buffer_queue, (void *)&frame_buffer_event, portMAX_DELAY);
                        state = CAM_STATE_IDLE;
                    } else {
                        cam_obj->cnt++;
                    }
                } else if (cam_event == CAM_VSYNC_EVENT) {
                    if (cam_obj->jpeg_mode) {
                        cam_obj->frame2_buffer_en = 0;
                    }
                }
            }
            break;
        }
    }
}

size_t cam_take(uint8_t **buffer_p) {
    frame_buffer_event_t frame_buffer_event;
    xQueueReceive(cam_obj->frame_buffer_queue, (void *)&frame_buffer_event, portMAX_DELAY);
    *buffer_p = frame_buffer_event.frame_buffer;
    return frame_buffer_event.len;
}

bool cam_ready(void) {
    frame_buffer_event_t frame_buffer_event;
    BaseType_t result = xQueuePeek(cam_obj->frame_buffer_queue, (void *)&frame_buffer_event, 0);
    return result != pdFALSE;
}


void cam_give(uint8_t *buffer) {
    if (buffer == cam_obj->frame1_buffer) {
        cam_obj->frame1_buffer_en = 1;
    } else if (buffer == cam_obj->frame2_buffer) {
        cam_obj->frame2_buffer_en = 1;
    }
}

static void cam_dma_config(const cam_config_t *config) {
    int cnt = 0;

    if (config->mode.jpeg) {
        cam_obj->buffer_size = 2048;
        cam_obj->half_buffer_size = cam_obj->buffer_size / 2;
        cam_obj->dma_size = 1024;
    } else {
        for (cnt = 0;; cnt++) { /*!< Find the divisible buffer size */
            if ((config->size * 2) % (config->max_buffer_size - cnt) == 0) {
                break;
            }
        }

        cam_obj->buffer_size = config->max_buffer_size - cnt;

        cam_obj->half_buffer_size = cam_obj->buffer_size / 2;

        for (cnt = 0;; cnt++) { /*!< Look for divisible dma sizes */
            if ((cam_obj->half_buffer_size) % (CAM_DMA_MAX_SIZE - cnt) == 0) {
                break;
            }
        }

        cam_obj->dma_size = CAM_DMA_MAX_SIZE - cnt;
    }

    cam_obj->node_cnt = (cam_obj->buffer_size) / cam_obj->dma_size; /*!< Number of DMA nodes */
    cam_obj->half_node_cnt = cam_obj->node_cnt / 2;
    cam_obj->total_cnt = (config->size * 2) / cam_obj->half_buffer_size; /*!< Number of interrupt copies produced. Ping pong copies */

    ESP_LOGI(TAG, "cam_buffer_size: %d, cam_dma_size: %d, cam_dma_node_cnt: %d, cam_total_cnt: %d\n", cam_obj->buffer_size, cam_obj->dma_size, cam_obj->node_cnt, cam_obj->total_cnt);

    cam_obj->dma = (lldesc_t *)heap_caps_realloc(cam_obj->dma, cam_obj->node_cnt * sizeof(lldesc_t), MALLOC_CAP_DMA);
    cam_obj->buffer = (uint8_t *)heap_caps_realloc(cam_obj->buffer, cam_obj->buffer_size * sizeof(uint8_t), MALLOC_CAP_DMA);

    for (uint32_t x = 0; x < cam_obj->node_cnt; x++) {
        cam_obj->dma[x].size = cam_obj->dma_size;
        cam_obj->dma[x].length = cam_obj->dma_size;
        cam_obj->dma[x].eof = 0;
        cam_obj->dma[x].owner = 1;
        cam_obj->dma[x].buf = (cam_obj->buffer + cam_obj->dma_size * x);
        cam_obj->dma[x].empty = (intptr_t)&cam_obj->dma[(x + 1) % cam_obj->node_cnt];
    }

    I2S0.in_link.addr = ((uint32_t)&cam_obj->dma[0]) & 0xfffff;
    I2S0.rx_eof_num = cam_obj->half_buffer_size; /*!< Ping-pong operation */
}

esp_err_t cam_deinit() {
    if (!cam_obj) {
        return ESP_FAIL;
    }

    cam_stop();
    gpio_isr_handler_remove(cam_obj->vsync_pin);
    esp_intr_free(cam_obj->intr_handle);
    vTaskDelete(cam_obj->task_handle);
    vQueueDelete(cam_obj->event_queue);
    vQueueDelete(cam_obj->frame_buffer_queue);
    free(cam_obj->dma);
    free(cam_obj->buffer);
    free(cam_obj);

    cam_obj = NULL;

    return ESP_OK;
}

esp_err_t cam_init(const cam_config_t *config) {
    cam_obj = (cam_obj_t *)heap_caps_calloc(1, sizeof(cam_obj_t), MALLOC_CAP_DMA);

    if (!cam_obj) {
        ESP_LOGI(TAG, "camera object malloc error\n");
        return ESP_FAIL;
    }

    cam_obj->frame1_buffer = config->frame1_buffer;
    cam_obj->frame2_buffer = config->frame2_buffer;
    cam_obj->jpeg_mode = config->mode.jpeg;
    cam_obj->vsync_pin = config->pin.vsync;
    cam_obj->vsync_invert = config->vsync_invert;
    cam_obj->hsync_invert = config->hsync_invert;
    cam_set_pin(config);
    cam_config(config);
    cam_dma_config(config);

    cam_obj->event_queue = xQueueCreate(2, sizeof(cam_event_t));
    cam_obj->frame_buffer_queue = xQueueCreate(2, sizeof(frame_buffer_event_t));

    if (cam_obj->frame1_buffer != NULL) {
        ESP_LOGI(TAG, "frame1_buffer_en\n");
        cam_obj->frame1_buffer_en = 1;
    } else {
        cam_obj->frame1_buffer_en = 0;
    }

    if (cam_obj->frame2_buffer != NULL) {
        ESP_LOGI(TAG, "frame2_buffer_en\n");
        cam_obj->frame2_buffer_en = 1;
    } else {
        cam_obj->frame2_buffer_en = 0;
    }

    esp_intr_alloc(ETS_I2S0_INTR_SOURCE, 0, cam_isr, NULL, &cam_obj->intr_handle);
    xTaskCreate(cam_task, "cam_task", config->task_stack, NULL, config->task_pri, &cam_obj->task_handle);
    return ESP_OK;
}
