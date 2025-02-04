/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "board.h"

/* SDK drivers */
#include "fsl_lpi2c.h"

#include "drv_i2c.h"

int drv_i2c_init(drv_i2c_t *i2c, uint8_t id, drv_i2c_config_t *cfg) {
    i2c->instance = MCX_BoardGetI2CInstance(id);
    if (i2c->instance == NULL) {
        return -EINVAL;
    }

    i2c->id = id;

    int i2c_inputfreq = MCX_BoardConfigureI2CClock(id);
    if (i2c_inputfreq <= 0) {
        return -EINVAL;
    }

    lpi2c_master_config_t i2c_cfg;
    LPI2C_MasterGetDefaultConfig(&i2c_cfg);

    i2c_cfg.baudRate_Hz = cfg->frequency;

    LPI2C_Type *lpi2c = i2c->instance;
    LPI2C_MasterInit(lpi2c, &i2c_cfg, i2c_inputfreq);

    return 0;
}

int drv_i2c_write(drv_i2c_t *i2c, uint16_t addr, uint8_t *data, uint32_t len, uint32_t timeout, bool stop) {
    LPI2C_Type *lpi2c = i2c->instance;

    lpi2c_master_transfer_t xfer = {
        .direction = kLPI2C_Write,
        .slaveAddress = addr,
        .subaddress = 0,
        .subaddressSize = 0U,
        .data = data,
        .dataSize = len,
        .flags = stop ? kLPI2C_TransferDefaultFlag : kLPI2C_TransferNoStopFlag,
    };

    if (LPI2C_MasterTransferBlocking(lpi2c, &xfer) != kStatus_Success) {
        return -EIO;
    }

    return 0;
}

int drv_i2c_read(drv_i2c_t *i2c, uint16_t addr, uint8_t *data, uint32_t len, uint32_t timeout, bool stop) {
    LPI2C_Type *lpi2c = i2c->instance;

    lpi2c_master_transfer_t xfer = {
        .direction = kLPI2C_Read,
        .slaveAddress = addr,
        .subaddress = 0,
        .subaddressSize = 0U,
        .data = data,
        .dataSize = len,
        .flags = stop ? kLPI2C_TransferDefaultFlag : kLPI2C_TransferNoStopFlag,
    };

    if (LPI2C_MasterTransferBlocking(lpi2c, &xfer) != kStatus_Success) {
        return -EIO;
    }

    return 0;
}
