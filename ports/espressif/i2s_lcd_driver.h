/* *INDENT-OFF* */
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef   __I2S_LCD_DRIVER_H__
#define   __I2S_LCD_DRIVER_H__

#include "driver/i2s.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define LCD_CMD_LEV   (0)
#define LCD_DATA_LEV  (1)

typedef void *i2s_lcd_handle_t;  /** Handle of i2s lcd driver */

/**
 * @brief Configuration of i2s lcd mode
 *
 */
typedef struct {
    int8_t data_width;           /*!< Parallel data width, 16bit or 8bit available */
    int8_t pin_data_num[16];     /*!< Parallel data output IO*/
    int8_t pin_num_cs;           /*!< CS io num */
    int8_t pin_num_wr;           /*!< Write clk io*/
    int8_t pin_num_rs;           /*!< RS io num */
    int clk_freq;                /*!< I2s clock frequency */
    i2s_port_t i2s_port;         /*!< I2S port number */
    bool swap_data;              /*!< Swap the 2 bytes of RGB565 color */
    uint32_t buffer_size;        /*!< DMA buffer size */
} i2s_lcd_config_t;

/**
 * @brief Initialize i2s lcd driver.
 *
 * @param config configuration of i2s
 *
 * @return  A handle to the created i2s lcd driver, or NULL in case of error.
 */
i2s_lcd_handle_t i2s_lcd_driver_init(const i2s_lcd_config_t *config);

/**
 * @brief Deinit i2s lcd driver.
 *
 * @param handle i2s lcd driver handle to deinitilize
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG handle is invalid
 */
esp_err_t i2s_lcd_driver_deinit(i2s_lcd_handle_t handle);

/**
 * @brief Write a data to LCD
 *
 * @param handle i2s lcd driver handle
 * @param data Data to write
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG handle is invalid
 */
esp_err_t i2s_lcd_write_data(i2s_lcd_handle_t handle, uint16_t data);

/**
 * @brief Write a command to LCD
 *
 * @param handle Handle of i2s lcd driver
 * @param cmd command to write
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG handle is invalid
 */
esp_err_t i2s_lcd_write_cmd(i2s_lcd_handle_t handle, uint16_t cmd);

/**
 * @brief Write block data to LCD
 *
 * @param handle  Handle of i2s lcd driver
 * @param data Pointer of data
 * @param length length of data
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG handle is invalid
 */
esp_err_t i2s_lcd_write(i2s_lcd_handle_t handle, const uint8_t *data, uint32_t length);

/**
 * @brief acquire a lock
 *
 * @param handle  Handle of i2s lcd driver
 *
 * @return Always return ESP_OK
 */
esp_err_t i2s_lcd_acquire(i2s_lcd_handle_t handle);

/**
 * @brief acquire a lock, but only wait a certain period of time
 *
 * @param handle  Handle of i2s lcd driver
 *
 * @return true if the lock was acquired, false otherwise
 */
bool i2s_lcd_acquire_nonblocking(i2s_lcd_handle_t handle, TickType_t ticks_to_wait);

/**
 * @brief release a lock
 *
 * @param handle  Handle of i2s lcd driver
 *
 * @return Always return ESP_OK
 */
esp_err_t i2s_lcd_release(i2s_lcd_handle_t handle);


#ifdef __cplusplus
}
#endif

#endif
