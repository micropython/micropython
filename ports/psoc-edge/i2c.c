/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2026 Infineon Technologies AG
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

// std includes
#include <stdio.h>

// mpy includes
#include "py/runtime.h"

// MTB includes
#include "cy_scb_i2c.h"

// port-specific includes
#include "genhdr/pins_af.h"
#include "clk.h"
#include "i2c.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)

#define i2c_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

uint8_t i2c_get_id(i2c_obj_t *self) {
    return self->id;
}

void i2c_alloc(i2c_obj_t *self, uint8_t id, mp_obj_t parent, void (*scb_isr)(mp_obj_t), bool *is_new) {
    if (self->scb_obj == NULL) {
        *is_new = true;
        if (!scb_is_free(id)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCB %u is already in use by a machine.UART or machine.SPI instance."), id);
        }
        self->id = id;
        self->pclk_div = NULL;
        self->scb_obj = scb_obj_alloc(id, parent, scb_isr);
    } else if (self->scb_obj->parent == parent) {
        /* The scb instance is already associated with this parent.
           Notify the caller that the I2C object is not new.
         */
        *is_new = false;
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C object is already associated with another parent."));
    }
}

void i2c_free(i2c_obj_t *self) {
    if (self->scb_obj != NULL) {
        scb_obj_free(self->scb_obj);
        self->scb_obj = NULL;
    }
}

static void i2c_hw_init(i2c_obj_t *self, cy_en_scb_i2c_mode_t mode, uint8_t target_address) {
    cy_stc_scb_i2c_config_t cfg = {
        .i2cMode = mode,
        .useRxFifo = false,
        .useTxFifo = true,
        .slaveAddress = (mode == CY_SCB_I2C_SLAVE) ? target_address : 0U,
        .slaveAddressMask = 0xFEU,
        .acceptAddrInFifo = false,
        .ackGeneralAddr = false,
        .enableWakeFromSleep = false,
        .enableDigitalFilter = false,
        .lowPhaseDutyCycle = (mode == CY_SCB_I2C_MASTER) ? 8U : 0U,
        .highPhaseDutyCycle = (mode == CY_SCB_I2C_MASTER) ? 8U : 0U,
    };

    cy_rslt_t result = Cy_SCB_I2C_Init(self->scb_obj->scb, &cfg, &self->ctx);
    i2c_assert_raise_val("I2C init failed: 0x%lx", result);
    sys_int_init(&(self->scb_obj->irq));
    Cy_SCB_I2C_Enable(self->scb_obj->scb);
}

void i2c_hw_deinit(i2c_obj_t *self) {
    if (self->scb_obj != NULL) {
        Cy_SCB_I2C_Disable(self->scb_obj->scb, &self->ctx);
        sys_int_deinit(&self->scb_obj->irq);
    }
    if (self->pclk_div != NULL) {
        pclk_div_deinit(self->pclk_div);
        self->pclk_div = NULL;
    }
}

static void i2c_hw_controller_clock_init(i2c_obj_t *self, uint32_t freq) {
    /**
     * For desired data rate, clk_scb frequency must be in valid range (see TRM I2C Oversampling section)
     * For 100kHz: clk_scb range is 1.55 - 3.2 MHz (architecture reference manual 002-38331 Rev. *B P707 table 366)
     *   - target clk_scb = 2.38 MHz (mid-range)
     * For 400kHz: clk_scb range is 7.82 - 10 MHz
     *   - target clk_scb = 9.09 MHz (within range)
     */
    #define MACHINE_I2C_CLK_SCB_FREQ_100KHZ  (2380000U)
    #define MACHINE_I2C_CLK_SCB_FREQ_400KHZ  (9090000U)

    uint32_t input_freq = pclk_div_get_input_freq(self->scb_obj->clk);
    if (input_freq == 0U) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to get clock frequency for I2C(%u)"), self->scb_obj->id);
    }

    uint32_t clk_scb_freq = (freq <= 100000) ? MACHINE_I2C_CLK_SCB_FREQ_100KHZ : MACHINE_I2C_CLK_SCB_FREQ_400KHZ;
    uint32_t divider = (input_freq / clk_scb_freq) - 1U;
    DEBUG_printf("DEBUG: clk_scb_freq=%u Hz\n", clk_scb_freq);

    self->pclk_div = pclk_div_init(self->scb_obj->clk, divider, 0);
    if (self->pclk_div == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to initialize clock divider for I2C(%u)"), self->scb_obj->id);
    }

    uint32_t achieved_freq = Cy_SCB_I2C_SetDataRate(self->scb_obj->scb, freq, clk_scb_freq);
    if ((achieved_freq > freq) || (achieved_freq == 0U)) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("cannot reach desired I2C data rate %u Hz (achieved: %u Hz)"),
            freq, achieved_freq);
    }
}

void i2c_hw_controller_init(i2c_obj_t *self, uint32_t freq) {
    i2c_hw_controller_clock_init(self, freq);
    i2c_hw_init(self, CY_SCB_I2C_MASTER, 0);
}

cy_en_scb_i2c_status_t i2c_hw_controller_read(i2c_obj_t *self, cy_stc_scb_i2c_master_xfer_config_t *transfer) {
    return Cy_SCB_I2C_MasterRead(self->scb_obj->scb, transfer, &self->ctx);
}

cy_en_scb_i2c_status_t i2c_hw_controller_write(i2c_obj_t *self, cy_stc_scb_i2c_master_xfer_config_t *transfer) {
    return Cy_SCB_I2C_MasterWrite(self->scb_obj->scb, transfer, &self->ctx);
}

uint32_t i2c_hw_controller_get_status(i2c_obj_t *self) {
    return Cy_SCB_I2C_MasterGetStatus(self->scb_obj->scb, &self->ctx);
}

void i2c_hw_controller_irq(i2c_obj_t *self) {
    Cy_SCB_I2C_MasterInterrupt(self->scb_obj->scb, &self->ctx);
}

void i2c_hw_target_clock_init(i2c_obj_t *self) {
    // Configure clock for I2C slave operation
    // For 400 khz slave, clk_scb must be 7.82 – 15.38 MHz
    // For 100 khz slave, clk_scb must be 1.55 – 12.8 MHz
    // clk_peri = 100 MHz, divider = 7, clk_scb = 100/8 = 12.5 MHz
    #define I2C_TARGET_SCB_CLK_FREQ_HZ (12500000UL)

    uint32_t input_freq = pclk_div_get_input_freq(self->scb_obj->clk);
    uint32_t divider = input_freq / I2C_TARGET_SCB_CLK_FREQ_HZ - 1;
    self->pclk_div = pclk_div_init(self->scb_obj->clk, divider, 0);
    if (self->pclk_div == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to initialize clock divider for I2CTarget(%u)"), self->id);
    }
}

void i2c_hw_target_init(i2c_obj_t *self, uint32_t addr) {
    i2c_hw_target_clock_init(self);
    i2c_hw_init(self, CY_SCB_I2C_SLAVE, addr);
}

void i2c_hw_target_config_read_buff(i2c_obj_t *self, uint8_t *buffer, uint32_t size) {
    Cy_SCB_I2C_SlaveConfigReadBuf(self->scb_obj->scb, buffer, size, &self->ctx);
}

void i2c_hw_target_config_write_buff(i2c_obj_t *self, uint8_t *buffer, uint32_t size) {
    Cy_SCB_I2C_SlaveConfigWriteBuf(self->scb_obj->scb, buffer, size, &self->ctx);
}

void i2c_hw_target_clear_read_status(i2c_obj_t *self) {
    Cy_SCB_I2C_SlaveClearReadStatus(self->scb_obj->scb, &self->ctx);
}

void i2c_hw_target_clear_write_status(i2c_obj_t *self) {
    Cy_SCB_I2C_SlaveClearWriteStatus(self->scb_obj->scb, &self->ctx);
}

uint32_t i2c_hw_target_get_write_count(i2c_obj_t *self) {
    return Cy_SCB_I2C_SlaveGetWriteTransferCount(self->scb_obj->scb, &self->ctx);
}

void i2c_hw_target_irq(i2c_obj_t *self) {
    Cy_SCB_I2C_SlaveInterrupt(self->scb_obj->scb, &self->ctx);
}

void i2c_hw_target_irq_enable(i2c_obj_t *self) {
    sys_int_enable(&(self->scb_obj->irq));
}

void i2c_hw_target_irq_disable(i2c_obj_t *self) {
    sys_int_disable(&(self->scb_obj->irq));
}

void i2c_hw_target_register_callback(i2c_obj_t *self, void (*callback)(uint32_t events)) {
    Cy_SCB_I2C_RegisterEventCallback(self->scb_obj->scb, callback, &self->ctx);
}
