/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

#include "fsl_iomuxc.h"
#include "fsl_lpi2c.h"

#define DEFAULT_I2C_FREQ        (400000)
#define DEFAULT_I2C_DRIVE       (6)

// Select USB1 PLL (480 MHz) as master lpi2c clock source
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
// Clock divider for master lpi2c clock source
#define LPI2C_CLOCK_SOURCE_DIVIDER (1U)
// Get frequency of lpi2c clock = 30 MHz
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    LPI2C_Type *i2c_inst;
    uint8_t i2c_id;
    uint8_t i2c_hw_id;
    bool transfer_busy;
    status_t transfer_status;
    lpi2c_master_config_t *master_config;
} machine_i2c_obj_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

STATIC const uint8_t i2c_index_table[] = MICROPY_HW_I2C_INDEX;
STATIC LPI2C_Type *i2c_base_ptr_table[] = LPI2C_BASE_PTRS;
static const iomux_table_t iomux_table[] = { IOMUX_TABLE_I2C };

#define MICROPY_HW_I2C_NUM     ARRAY_SIZE(i2c_index_table)

#define SCL (iomux_table[index])
#define SDA (iomux_table[index + 1])

bool lpi2c_set_iomux(int8_t hw_i2c, uint8_t drive) {
    int index = (hw_i2c - 1) * 2;
    uint16_t pad_config = pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OPEN_DRAIN, drive, SCL.configRegister);

    if (SCL.muxRegister != 0) {
        IOMUXC_SetPinMux(SCL.muxRegister, SCL.muxMode, SCL.inputRegister, SCL.inputDaisy, SCL.configRegister, 1U);
        IOMUXC_SetPinConfig(SCL.muxRegister, SCL.muxMode, SCL.inputRegister, SCL.inputDaisy, SCL.configRegister, pad_config);

        IOMUXC_SetPinMux(SDA.muxRegister, SDA.muxMode, SDA.inputRegister, SDA.inputDaisy, SDA.configRegister, 1U);
        IOMUXC_SetPinConfig(SDA.muxRegister, SDA.muxMode, SDA.inputRegister, SDA.inputDaisy, SDA.configRegister, pad_config);
        return true;
    } else {
        return false;
    }
}

STATIC void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u)",
        self->i2c_id, self->master_config->baudRate_Hz);
}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_freq, ARG_drive};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_DRIVE} },
    };

    static bool clk_init = true;

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (i2c_id < 0 || i2c_id >= MICROPY_HW_I2C_NUM || i2c_index_table[i2c_id] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    // Get I2C Object.
    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);
    self->i2c_id = i2c_id;
    self->i2c_hw_id = i2c_index_table[i2c_id];  // the hw i2c number 1..n
    self->i2c_inst = i2c_base_ptr_table[self->i2c_hw_id];

    uint8_t drive = args[ARG_drive].u_int;
    if (drive < 1 || drive > 7) {
        drive = DEFAULT_I2C_DRIVE;
    }

    if (clk_init) {
        clk_init = false;
        // Set clock source for LPI2C
        CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT); // USB1 PLL (480 MHz)
        CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);
    }

    // Initialise the I2C peripheral if any arguments given, or it was not initialised previously.
    lpi2c_set_iomux(self->i2c_hw_id, drive);
    self->master_config = m_new_obj(lpi2c_master_config_t);
    LPI2C_MasterGetDefaultConfig(self->master_config);
    // Initialise the I2C peripheral.
    self->master_config->baudRate_Hz = args[ARG_freq].u_int;
    LPI2C_MasterInit(self->i2c_inst, self->master_config, LPI2C_CLOCK_FREQUENCY);

    return MP_OBJ_FROM_PTR(self);
}

static void lpi2c_master_callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *self_in) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;

    self->transfer_busy = false;
    self->transfer_status = status;
}

STATIC int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;
    status_t ret;
    lpi2c_master_handle_t g_master_handle;
    lpi2c_master_transfer_t masterXfer = {0};
    LPI2C_MasterTransferCreateHandle(self->i2c_inst, &g_master_handle, lpi2c_master_callback, self);

    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        masterXfer.direction = kLPI2C_Read;
    } else {
        masterXfer.direction = kLPI2C_Write;
    }
    if (addr < 0x80) {  // 7 or 10 bit address?
        masterXfer.slaveAddress = addr;
    } else {
        masterXfer.slaveAddress = 0x78 | ((addr >> 8) & 0x03);
        masterXfer.subaddress = addr & 0xff;
        masterXfer.subaddressSize = 1;
    }
    masterXfer.data = buf;
    masterXfer.dataSize = len;
    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        masterXfer.flags = kLPI2C_TransferDefaultFlag;
    } else {
        masterXfer.flags = kLPI2C_TransferNoStopFlag;
    }
    self->transfer_busy = true;

    // Send master data to slave in non-blocking mode
    ret = LPI2C_MasterTransferNonBlocking(self->i2c_inst, &g_master_handle, &masterXfer);
    if (ret != kStatus_Success) {
        return -MP_EIO;
    }
    //  Wait for the transfer to complete
    while (self->transfer_busy) {
        MICROPY_EVENT_POLL_HOOK
    }

    // Transfer will not send a stop in case of errors like NAK. So it's done here.
    if (flags & MP_MACHINE_I2C_FLAG_STOP && self->transfer_status != kStatus_Success) {
        LPI2C_MasterStop(self->i2c_inst);
    }

    if (self->transfer_status == kStatus_Success) {
        return len;
    } else if (self->transfer_status == kStatus_LPI2C_Nak) {
        return -MP_ENODEV;
    } else {
        return -MP_EIO;
    }
}

STATIC const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

const mp_obj_type_t machine_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_i2c_print,
    .make_new = machine_i2c_make_new,
    .protocol = &machine_i2c_p,
    .locals_dict = (mp_obj_dict_t *)&mp_machine_i2c_locals_dict,
};
