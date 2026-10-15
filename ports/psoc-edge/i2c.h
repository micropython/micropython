/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_I2C_H
#define MICROPY_INCLUDED_PSOC_EDGE_I2C_H

#include "sys_int.h"
#include "py/obj.h"

#include "scb.h"

typedef struct _i2c_obj_t {
    uint8_t id;
    scb_obj_t *scb_obj;
    pclk_div_obj_t *pclk_div;
    cy_stc_scb_i2c_context_t ctx;
} i2c_obj_t;

#define I2C_NUM_PINS (2)

uint8_t i2c_get_id(i2c_obj_t *self);

void i2c_alloc(i2c_obj_t *self, uint8_t id, mp_obj_t parent, void (*scb_isr)(mp_obj_t), bool *is_new);
void i2c_free(i2c_obj_t *self);

void i2c_hw_controller_init(i2c_obj_t *self, uint32_t freq);
cy_en_scb_i2c_status_t i2c_hw_controller_read(i2c_obj_t *self, cy_stc_scb_i2c_master_xfer_config_t *transfer);
cy_en_scb_i2c_status_t i2c_hw_controller_write(i2c_obj_t *self, cy_stc_scb_i2c_master_xfer_config_t *transfer);
uint32_t i2c_hw_controller_get_status(i2c_obj_t *self);
void i2c_hw_controller_irq(i2c_obj_t *self);

void i2c_hw_target_init(i2c_obj_t *self, uint32_t addr);
void i2c_hw_target_config_read_buff(i2c_obj_t *self, uint8_t *buffer, uint32_t size);
void i2c_hw_target_config_write_buff(i2c_obj_t *self, uint8_t *buffer, uint32_t size);
void i2c_hw_target_clear_read_status(i2c_obj_t *self);
void i2c_hw_target_clear_write_status(i2c_obj_t *self);
uint32_t i2c_hw_target_get_write_count(i2c_obj_t *self);
void i2c_hw_target_irq(i2c_obj_t *self);
void i2c_hw_target_irq_enable(i2c_obj_t *self);
void i2c_hw_target_irq_disable(i2c_obj_t *self);
void i2c_hw_target_register_callback(i2c_obj_t *self, void (*callback)(uint32_t events));

void i2c_hw_deinit(i2c_obj_t *self);

#endif // MICROPY_INCLUDED_PSOC_EDGE_I2C_H
