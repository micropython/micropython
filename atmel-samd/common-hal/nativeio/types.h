/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

// This defines the types used to underly the standard nativeio Python objects.
// The shared API is defined in terms of these types.

#ifndef __MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_NATIVEIO_TYPES_H__
#define __MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_NATIVEIO_TYPES_H__

#include "common-hal/microcontroller/types.h"

// Don't reorder these includes because they are dependencies of adc_feature.h.
// They should really be included by adc_feature.h.
#include <compiler.h>
#include "asf/sam0/drivers/system/clock/gclk.h"
#include "asf/sam0/utils/cmsis/samd21/include/component/adc.h"
#include "asf/sam0/drivers/adc/adc_sam_d_r/adc_feature.h"

#include "asf/sam0/drivers/dac/dac.h"
#include "asf/sam0/drivers/sercom/i2c/i2c_master.h"
#include "asf/sam0/drivers/sercom/spi/spi.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "asf/sam0/drivers/tc/tc.h"
#include "asf/sam0/drivers/tcc/tcc.h"
// Only support TouchIn when external SPI flash is used.
#ifdef SPI_FLASH_SECTOR_SIZE
#include "QTouch/touch_api_ptc.h"
#endif

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t * pin;
    struct adc_module * adc_instance;
} nativeio_analogin_obj_t;

typedef struct {
    mp_obj_base_t base;
    struct dac_module dac_instance;
} nativeio_analogout_obj_t;

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t * pin;
    bool output;
    bool open_drain;
} nativeio_digitalinout_obj_t;

typedef struct {
    mp_obj_base_t base;
    struct i2c_master_module i2c_master_instance;
    bool has_lock;
    uint8_t scl_pin;
    uint8_t sda_pin;
} nativeio_i2c_obj_t;

typedef struct {
    mp_obj_base_t base;
    struct spi_module spi_master_instance;
    bool has_lock;
    uint8_t clock_pin;
    uint8_t MOSI_pin;
    uint8_t MISO_pin;
    uint32_t current_baudrate;
} nativeio_spi_obj_t;

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    const pin_timer_t* timer;
    bool variable_frequency;
    union {
        struct tc_module tc_instance;
        struct tcc_module tcc_instance;
    };
} nativeio_pwmout_obj_t;

typedef struct {
    mp_obj_base_t base;
    // Only support TouchIn when external SPI flash is used.
    #ifdef SPI_FLASH_SECTOR_SIZE
    const mcu_pin_obj_t * pin;
    sensor_id_t sensor_id;
    #endif
} nativeio_touchin_obj_t;

typedef struct {
  mp_obj_base_t base;
  struct usart_module uart_instance;
  uint8_t rx_pin;
  uint8_t tx_pin;
  uint32_t timeout_ms;
  bool rx_error;
  // Index of the oldest received character.
  uint32_t buffer_start;
  // Index of the next available spot to store a character.
  uint32_t buffer_size;
  uint32_t buffer_length;
  uint8_t* buffer;
} nativeio_uart_obj_t;

#endif // __MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_NATIVEIO_TYPES_H__
