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

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t bit_width;
    union {
        struct {
            uint32_t pclk :   8;
            uint32_t vsync :  8;
            uint32_t hsync :  8;
        };
        uint32_t val;
    } pin;                    /*!< Pin of camera */
    uint8_t pin_data[16];     /*!< Data pin of camera*/
    uint8_t vsync_invert;
    uint8_t hsync_invert;
    uint32_t size;            /*!< size of camera frame (width times height) */
    uint32_t max_buffer_size; /*!< DMA used */
    uint32_t task_stack;      /*!< The priority of the task at run time */
    uint8_t task_pri;
    union {
        struct {
            uint32_t jpeg :   1;
        };
        uint32_t val;
    } mode;
    uint8_t *frame1_buffer; /*!< PingPang buffers , cache the image*/
    uint8_t *frame2_buffer; /*!< PingPang buffers , cache the image*/
} cam_config_t;

/**
 * @brief enable camera
 */
void cam_start(void);

/**
 * @brief disable camera
 */
void cam_stop(void);

/**
 * @brief Accepts frame data and returns a pointer to the frame data.
 *
 * @param buffer_p  The address of the frame buffer pointer
 *
 * @return - len of buffer
 */
size_t cam_take(uint8_t **buffer_p);

/**
 * @brief Check whether a frame is available
 *
 * @return - true if cam_take() will complete immediately, false if it will block
 */
bool cam_ready(void);

/**
 * @brief enable frame buffer to get the next frame data.
 *
 * @param buffer The address of the frame buffer pointer
 *
 */
void cam_give(uint8_t *buffer);

/**
 * @brief Initialize camera
 *
 * @param config Parameter configuration, including pin, buffer, output image size, and so on.
 *
 * @return - ESP_OK :Initialize success
 *           ESP_FAIL: Initialize fails
 */
esp_err_t cam_init(const cam_config_t *config);

/**
 * @brief  delete the camera
 *
 * @return - ESP_OK :Delete success
 *           ESP_FAIL: Delete fails
 */
esp_err_t cam_deinit(void);

#ifdef __cplusplus
}
#endif
