/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
 * Copyright (c) 2019 Artur Pacholec
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "periph.h"

#include "fsl_lpi2c.h"

#define I2C_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (1+CLOCK_GetDiv(kCLOCK_Lpi2cDiv)))


static void config_periph_pin(const mcu_periph_obj_t *periph) {
    IOMUXC_SetPinMux(
        periph->pin->mux_reg, periph->mux_mode,
        periph->input_reg, periph->input_idx,
        0,
        1);

    IOMUXC_SetPinConfig(0, 0, 0, 0,
        periph->pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(0)
            | IOMUXC_SW_PAD_CTL_PAD_PUS(3)
            | IOMUXC_SW_PAD_CTL_PAD_PUE(0)
            | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
            | IOMUXC_SW_PAD_CTL_PAD_ODE(1)
            | IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
            | IOMUXC_SW_PAD_CTL_PAD_DSE(4)
            | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
        const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    const uint32_t sda_count = sizeof(mcu_i2c_sda_list) / sizeof(mcu_periph_obj_t);
    const uint32_t scl_count = sizeof(mcu_i2c_scl_list) / sizeof(mcu_periph_obj_t);

    for (uint32_t i = 0; i < sda_count; ++i) {
        if (mcu_i2c_sda_list[i].pin != sda)
            continue;

        for (uint32_t j = 0; j < scl_count; ++j) {
            if (mcu_i2c_scl_list[j].pin != scl)
                continue;

            if (mcu_i2c_scl_list[j].bank_idx != mcu_i2c_sda_list[i].bank_idx)
                continue;

            self->sda_pin = &mcu_i2c_sda_list[i];
            self->scl_pin = &mcu_i2c_scl_list[j];

            break;
        }
    }

    if(self->sda_pin == NULL || self->scl_pin == NULL) {
        mp_raise_RuntimeError(translate("Invalid I2C pin selection"));
    } else {
        self->i2c = mcu_i2c_banks[self->sda_pin->bank_idx - 1];
    }

    config_periph_pin(self->sda_pin);
    config_periph_pin(self->scl_pin);

    lpi2c_master_config_t config = { 0 };
    LPI2C_MasterGetDefaultConfig(&config);

    config.baudRate_Hz = frequency;

    LPI2C_MasterInit(self->i2c, &config, I2C_CLOCK_FREQ);

#if CIRCUITPY_REQUIRE_I2C_PULLUPS
//    if (!gpio_get_pin_level(sda->number) || !gpio_get_pin_level(scl->number)) {
//        reset_pin_number(sda->number);
//        reset_pin_number(scl->number);
//        mp_raise_RuntimeError(translate("SDA or SCL needs a pull up"));
//    }
#endif

    claim_pin(self->sda_pin->pin);
    claim_pin(self->scl_pin->pin);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    LPI2C_MasterDeinit(self->i2c);

//    reset_pin_number(self->sda_pin);
//    reset_pin_number(self->scl_pin);

    self->sda_pin = NULL;
    self->scl_pin = NULL;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    lpi2c_master_transfer_t xfer = { 0 };
    xfer.slaveAddress = addr;

    return LPI2C_MasterTransferBlocking(self->i2c, &xfer) == kStatus_Success;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;
//    CRITICAL_SECTION_ENTER()
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
//    CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
                                   const uint8_t *data, size_t len, bool transmit_stop_bit) {

    lpi2c_master_transfer_t xfer = { 0 };
    xfer.flags = transmit_stop_bit ? kLPI2C_TransferDefaultFlag : kLPI2C_TransferNoStopFlag;
    xfer.slaveAddress = addr;
    xfer.data = (uint8_t*)data;
    xfer.dataSize = len;

    const status_t status = LPI2C_MasterTransferBlocking(self->i2c, &xfer);
    if (status == kStatus_Success)
        return 0;

    return MP_EIO;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
        uint8_t *data, size_t len) {

    lpi2c_master_transfer_t xfer = { 0 };
    xfer.direction = kLPI2C_Read;
    xfer.slaveAddress = addr;
    xfer.data = data;
    xfer.dataSize = len;

    const status_t status = LPI2C_MasterTransferBlocking(self->i2c, &xfer);
    if (status == kStatus_Success)
        return 0;

    return MP_EIO;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
//    never_reset_sercom(self->i2c_desc.device.hw);
//
//    never_reset_pin_number(self->scl_pin);
//    never_reset_pin_number(self->sda_pin);
}
