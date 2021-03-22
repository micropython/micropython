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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "periph.h"

#include "fsl_lpi2c.h"
#include "fsl_gpio.h"

#define I2C_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (1 + CLOCK_GetDiv(kCLOCK_Lpi2cDiv)))
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 5U

// arrays use 0 based numbering: I2C1 is stored at index 0
#define MAX_I2C 4
STATIC bool reserved_i2c[MAX_I2C];
STATIC bool never_reset_i2c[MAX_I2C];

void i2c_reset(void) {
    for (uint i = 0; i < MP_ARRAY_SIZE(mcu_i2c_banks); i++) {
        if (!never_reset_i2c[i]) {
            reserved_i2c[i] = false;
            LPI2C_MasterDeinit(mcu_i2c_banks[i]);
        }
    }
}

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

static void i2c_check_pin_config(const mcu_pin_obj_t *pin, uint32_t pull) {
    IOMUXC_SetPinConfig(0, 0, 0, 0, pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(1)
        | IOMUXC_SW_PAD_CTL_PAD_PUS(0)     // Pulldown
        | IOMUXC_SW_PAD_CTL_PAD_PUE(pull)     // 0=nopull (keeper), 1=pull
        | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
        | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
        | IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
        | IOMUXC_SW_PAD_CTL_PAD_DSE(1)
        | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    // Test that the pins are in a high state. (Hopefully indicating they are pulled up.)
    IOMUXC_SetPinMux(sda->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);
    IOMUXC_SetPinMux(scl->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);
    i2c_check_pin_config(sda, 1);
    i2c_check_pin_config(scl, 1);
    const gpio_pin_config_t check_config = { kGPIO_DigitalInput, 0, kGPIO_NoIntmode };
    GPIO_PinInit(sda->gpio, sda->number, &check_config);
    GPIO_PinInit(scl->gpio, scl->number, &check_config);

    common_hal_mcu_delay_us(10);

    i2c_check_pin_config(sda, 0);
    i2c_check_pin_config(scl, 0);

    // We must pull up within 3us to achieve 400khz.
    common_hal_mcu_delay_us(3);

    if (!GPIO_PinRead(sda->gpio, sda->number) || !GPIO_PinRead(scl->gpio, scl->number)) {
        common_hal_reset_pin(sda);
        common_hal_reset_pin(scl);
        mp_raise_RuntimeError(translate("No pull up found on SDA or SCL; check your wiring"));
    }
    #endif

    const uint32_t sda_count = MP_ARRAY_SIZE(mcu_i2c_sda_list);
    const uint32_t scl_count = MP_ARRAY_SIZE(mcu_i2c_scl_list);

    for (uint32_t i = 0; i < sda_count; ++i) {
        if (mcu_i2c_sda_list[i].pin != sda) {
            continue;
        }

        for (uint32_t j = 0; j < scl_count; ++j) {
            if (mcu_i2c_scl_list[j].pin != scl) {
                continue;
            }

            if (mcu_i2c_scl_list[j].bank_idx != mcu_i2c_sda_list[i].bank_idx) {
                continue;
            }

            self->sda = &mcu_i2c_sda_list[i];
            self->scl = &mcu_i2c_scl_list[j];

            break;
        }
    }

    if (self->sda == NULL || self->scl == NULL) {
        mp_raise_ValueError(translate("Invalid pins"));
    } else {
        self->i2c = mcu_i2c_banks[self->sda->bank_idx - 1];
    }

    config_periph_pin(self->sda);
    config_periph_pin(self->scl);

    lpi2c_master_config_t config = { 0 };
    LPI2C_MasterGetDefaultConfig(&config);

    config.baudRate_Hz = frequency;

    LPI2C_MasterInit(self->i2c, &config, I2C_CLOCK_FREQ);

    claim_pin(self->sda->pin);
    claim_pin(self->scl->pin);
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset_i2c[self->sda->bank_idx - 1] = true;

    common_hal_never_reset_pin(self->sda->pin);
    common_hal_never_reset_pin(self->scl->pin);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    reserved_i2c[self->sda->bank_idx - 1] = false;
    never_reset_i2c[self->sda->bank_idx - 1] = false;

    LPI2C_MasterDeinit(self->i2c);

    common_hal_reset_pin(self->sda->pin);
    common_hal_reset_pin(self->scl->pin);

    self->sda = NULL;
    self->scl = NULL;
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
    xfer.data = (uint8_t *)data;
    xfer.dataSize = len;

    const status_t status = LPI2C_MasterTransferBlocking(self->i2c, &xfer);
    if (status == kStatus_Success) {
        return 0;
    }

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
    if (status == kStatus_Success) {
        return 0;
    }

    return MP_EIO;
}
