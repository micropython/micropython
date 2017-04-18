/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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
#include <string.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "i2c.h"
#include "pybi2c.h"
#include "mpexception.h"
#include "pybsleep.h"
#include "utils.h"
#include "pybpin.h"
#include "pins.h"

/// \moduleref pyb
/// \class I2C - a two-wire serial protocol

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    uint baudrate;
} pyb_i2c_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBI2C_MIN_BAUD_RATE_HZ                (50000)
#define PYBI2C_MAX_BAUD_RATE_HZ                (400000)

#define PYBI2C_TRANSC_TIMEOUT_MS               (20)
#define PYBI2C_TRANSAC_WAIT_DELAY_US           (10)

#define PYBI2C_TIMEOUT_TO_COUNT(to_us, baud)   (((baud) * to_us) / 16000000)

#define RET_IF_ERR(Func)                        {                           \
                                                    if (!Func) {            \
                                                        return  false;      \
                                                    }                       \
                                                }

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_i2c_obj_t pyb_i2c_obj = {.baudrate = 0};

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC bool pyb_i2c_write(byte addr, byte *data, uint len, bool stop);

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
// only master mode is available for the moment
STATIC void i2c_init (pyb_i2c_obj_t *self) {
    // Enable the I2C Peripheral
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_I2CA0);
    // Configure I2C module with the specified baudrate
    MAP_I2CMasterInitExpClk(I2CA0_BASE, self->baudrate);
}

STATIC bool pyb_i2c_transaction(uint cmd) {
    // Convert the timeout to microseconds
    int32_t timeout = PYBI2C_TRANSC_TIMEOUT_MS * 1000;
    // Sanity check, t_timeout must be between 1 and 255
    uint t_timeout = MIN(PYBI2C_TIMEOUT_TO_COUNT(timeout, pyb_i2c_obj.baudrate), 255);
    // Clear all interrupts
    MAP_I2CMasterIntClearEx(I2CA0_BASE, MAP_I2CMasterIntStatusEx(I2CA0_BASE, false));
    // Set the time-out in terms of clock cycles. Not to be used with breakpoints.
    MAP_I2CMasterTimeoutSet(I2CA0_BASE, t_timeout);
    // Initiate the transfer.
    MAP_I2CMasterControl(I2CA0_BASE, cmd);
    // Wait until the current byte has been transferred.
    // Poll on the raw interrupt status.
    while ((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & (I2C_MASTER_INT_DATA | I2C_MASTER_INT_TIMEOUT)) == 0) {
        if (timeout < 0) {
            // the peripheral is not responding, so stop
            return false;
        }
        // wait for a few microseconds
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(PYBI2C_TRANSAC_WAIT_DELAY_US));
        timeout -= PYBI2C_TRANSAC_WAIT_DELAY_US;
    }

    // Check for any errors in the transfer
    if (MAP_I2CMasterErr(I2CA0_BASE) != I2C_MASTER_ERR_NONE) {
        switch(cmd) {
        case I2C_MASTER_CMD_BURST_SEND_START:
        case I2C_MASTER_CMD_BURST_SEND_CONT:
        case I2C_MASTER_CMD_BURST_SEND_STOP:
            MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
            break;
        case I2C_MASTER_CMD_BURST_RECEIVE_START:
        case I2C_MASTER_CMD_BURST_RECEIVE_CONT:
        case I2C_MASTER_CMD_BURST_RECEIVE_FINISH:
            MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
            break;
        default:
            break;
        }
        return false;
    }
    return true;
}

STATIC void pyb_i2c_check_init(pyb_i2c_obj_t *self) {
    // not initialized
    if (!self->baudrate) {
        mp_raise_OSError(MP_EPERM);
    }
}

STATIC bool pyb_i2c_scan_device(byte devAddr) {
    bool ret = false;
    // Set the I2C slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, true);
    // Initiate the transfer.
    if (pyb_i2c_transaction(I2C_MASTER_CMD_SINGLE_RECEIVE)) {
        ret = true;
    }
    // Send the stop bit to cancel the read transaction
    MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
    if (!ret) {
        uint8_t data = 0;
        if (pyb_i2c_write(devAddr, &data, sizeof(data), true)) {
            ret = true;
        }
    }
    return ret;
}

STATIC bool pyb_i2c_mem_addr_write (byte addr, byte *mem_addr, uint mem_addr_len) {
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, addr, false);
    // Write the first byte to the controller.
    MAP_I2CMasterDataPut(I2CA0_BASE, *mem_addr++);
    // Initiate the transfer.
    RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_START));

    // Loop until the completion of transfer or error
    while (--mem_addr_len) {
        // Write the next byte of data
        MAP_I2CMasterDataPut(I2CA0_BASE, *mem_addr++);
        // Transact over I2C to send the next byte
        RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_CONT));
    }
    return true;
}

STATIC bool pyb_i2c_mem_write (byte addr, byte *mem_addr, uint mem_addr_len, byte *data, uint data_len) {
    if (pyb_i2c_mem_addr_write (addr, mem_addr, mem_addr_len)) {
        // Loop until the completion of transfer or error
        while (data_len--) {
            // Write the next byte of data
            MAP_I2CMasterDataPut(I2CA0_BASE, *data++);
            // Transact over I2C to send the byte
            RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_CONT));
        }
        // send the stop bit
        RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_STOP));
        return true;
    }
    return false;
}

STATIC bool pyb_i2c_write(byte addr, byte *data, uint len, bool stop) {
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, addr, false);
    // Write the first byte to the controller.
    MAP_I2CMasterDataPut(I2CA0_BASE, *data++);
    // Initiate the transfer.
    RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_START));

    // Loop until the completion of transfer or error
    while (--len) {
        // Write the next byte of data
        MAP_I2CMasterDataPut(I2CA0_BASE, *data++);
        // Transact over I2C to send the byte
        RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_CONT));
    }

    // If a stop bit is to be sent, do it.
    if (stop) {
        RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_STOP));
    }
    return true;
}

STATIC bool pyb_i2c_read(byte addr, byte *data, uint len) {
    // Initiate a burst or single receive sequence
    uint cmd = --len > 0 ? I2C_MASTER_CMD_BURST_RECEIVE_START : I2C_MASTER_CMD_SINGLE_RECEIVE;
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, addr, true);
    // Initiate the transfer.
    RET_IF_ERR(pyb_i2c_transaction(cmd));
    // Loop until the completion of reception or error
    while (len) {
        // Receive the byte over I2C
        *data++ = MAP_I2CMasterDataGet(I2CA0_BASE);
        if (--len) {
            // Continue with reception
            RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_RECEIVE_CONT));
        } else {
            // Complete the last reception
            RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_RECEIVE_FINISH));
        }
    }

    // Receive the last byte over I2C
    *data = MAP_I2CMasterDataGet(I2CA0_BASE);
    return true;
}

STATIC void pyb_i2c_read_into (mp_arg_val_t *args, vstr_t *vstr) {
    pyb_i2c_check_init(&pyb_i2c_obj);
    // get the buffer to receive into
    pyb_buf_get_for_recv(args[1].u_obj, vstr);

    // receive the data
    if (!pyb_i2c_read(args[0].u_int, (byte *)vstr->buf, vstr->len)) {
        mp_raise_OSError(MP_EIO);
    }
}

STATIC void pyb_i2c_readmem_into (mp_arg_val_t *args, vstr_t *vstr) {
    pyb_i2c_check_init(&pyb_i2c_obj);
    // get the buffer to receive into
    pyb_buf_get_for_recv(args[2].u_obj, vstr);

    // get the addresses
    mp_uint_t i2c_addr = args[0].u_int;
    mp_uint_t mem_addr = args[1].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = args[3].u_int >> 3;

    // write the register address to be read from
    if (pyb_i2c_mem_addr_write (i2c_addr, (byte *)&mem_addr, mem_addr_size)) {
        // Read the specified length of data
        if (!pyb_i2c_read (i2c_addr, (byte *)vstr->buf, vstr->len)) {
            mp_raise_OSError(MP_EIO);
        }
    } else {
        mp_raise_OSError(MP_EIO);
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */
/******************************************************************************/
STATIC void pyb_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;
    if (self->baudrate > 0) {
        mp_printf(print, "I2C(0, baudrate=%u)", self->baudrate);
    } else {
        mp_print_str(print, "I2C(0)");
    }
}

STATIC mp_obj_t pyb_i2c_init_helper(pyb_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_scl, ARG_sda, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // make sure the baudrate is between the valid range
    self->baudrate = MIN(MAX(args[ARG_freq].u_int, PYBI2C_MIN_BAUD_RATE_HZ), PYBI2C_MAX_BAUD_RATE_HZ);

    // assign the pins
    mp_obj_t pins[2] = {&pin_GP13, &pin_GP23}; // default (SDA, SCL) pins
    if (args[ARG_scl].u_obj != MP_OBJ_NULL) {
        pins[1] = args[ARG_scl].u_obj;
    }
    if (args[ARG_sda].u_obj != MP_OBJ_NULL) {
        pins[0] = args[ARG_sda].u_obj;
    }
    pin_assign_pins_af(pins, 2, PIN_TYPE_STD_PU, PIN_FN_I2C, 0);

    // init the I2C bus
    i2c_init(self);

    // register it with the sleep module
    pyb_sleep_add ((const mp_obj_t)self, (WakeUpCB_t)i2c_init);

    return mp_const_none;
}

STATIC mp_obj_t pyb_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // check the id argument, if given
    if (n_args > 0) {
        if (all_args[0] != MP_OBJ_NEW_SMALL_INT(0)) {
            mp_raise_OSError(MP_ENODEV);
        }
        --n_args;
        ++all_args;
    }

    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // setup the object
    pyb_i2c_obj_t *self = &pyb_i2c_obj;
    self->base.type = &pyb_i2c_type;

    // start the peripheral
    pyb_i2c_init_helper(self, n_args, all_args, &kw_args);

    return (mp_obj_t)self;
}

STATIC mp_obj_t pyb_i2c_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return pyb_i2c_init_helper(pos_args[0], n_args - 1, pos_args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_init_obj, 1, pyb_i2c_init);

STATIC mp_obj_t pyb_i2c_deinit(mp_obj_t self_in) {
    // disable the peripheral
    MAP_I2CMasterDisable(I2CA0_BASE);
    MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // invalidate the baudrate
    pyb_i2c_obj.baudrate = 0;
    // unregister it with the sleep module
    pyb_sleep_remove ((const mp_obj_t)self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_deinit_obj, pyb_i2c_deinit);

STATIC mp_obj_t pyb_i2c_scan(mp_obj_t self_in) {
    pyb_i2c_check_init(&pyb_i2c_obj);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (uint addr = 0x08; addr <= 0x77; addr++) {
        for (int i = 0; i < 3; i++) {
            if (pyb_i2c_scan_device(addr)) {
                mp_obj_list_append(list, mp_obj_new_int(addr));
                break;
            }
        }
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_scan_obj, pyb_i2c_scan);

STATIC mp_obj_t pyb_i2c_readfrom(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, },
        { MP_QSTR_nbytes,  MP_ARG_REQUIRED | MP_ARG_OBJ, },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_args, args);

    vstr_t vstr;
    pyb_i2c_read_into(args, &vstr);

    // return the received data
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_obj, 3, pyb_i2c_readfrom);

STATIC mp_obj_t pyb_i2c_readfrom_into(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_into_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, },
        { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ, },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_into_args, args);

    vstr_t vstr;
    pyb_i2c_read_into(args, &vstr);

    // return the number of bytes received
    return mp_obj_new_int(vstr.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_into_obj, 1, pyb_i2c_readfrom_into);

STATIC mp_obj_t pyb_i2c_writeto(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_writeto_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT,  },
        { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ,  },
        { MP_QSTR_stop,    MP_ARG_KW_ONLY  | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_writeto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_writeto_args, args);

    pyb_i2c_check_init(&pyb_i2c_obj);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[1].u_obj, &bufinfo, data);

    // send the data
    if (!pyb_i2c_write(args[0].u_int, bufinfo.buf, bufinfo.len, args[2].u_bool)) {
        mp_raise_OSError(MP_EIO);
    }

    // return the number of bytes written
    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_writeto_obj, 1, pyb_i2c_writeto);

STATIC mp_obj_t pyb_i2c_readfrom_mem(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_readfrom_mem_args[] = {
        { MP_QSTR_addr,     MP_ARG_REQUIRED  | MP_ARG_INT, },
        { MP_QSTR_memaddr,  MP_ARG_REQUIRED  | MP_ARG_INT, },
        { MP_QSTR_nbytes,   MP_ARG_REQUIRED  | MP_ARG_OBJ, },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_mem_args, args);

    vstr_t vstr;
    pyb_i2c_readmem_into (args, &vstr);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_mem_obj, 1, pyb_i2c_readfrom_mem);

STATIC const mp_arg_t pyb_i2c_readfrom_mem_into_args[] = {
    { MP_QSTR_addr,     MP_ARG_REQUIRED  | MP_ARG_INT, },
    { MP_QSTR_memaddr,  MP_ARG_REQUIRED  | MP_ARG_INT, },
    { MP_QSTR_buf,      MP_ARG_REQUIRED  | MP_ARG_OBJ, },
    { MP_QSTR_addrsize, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
};

STATIC mp_obj_t pyb_i2c_readfrom_mem_into(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_readfrom_mem_into_args, args);

    // get the buffer to read into
    vstr_t vstr;
    pyb_i2c_readmem_into (args, &vstr);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_mem_into_obj, 1, pyb_i2c_readfrom_mem_into);

STATIC mp_obj_t pyb_i2c_writeto_mem(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(pyb_i2c_readfrom_mem_into_args), pyb_i2c_readfrom_mem_into_args, args);

    pyb_i2c_check_init(&pyb_i2c_obj);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[2].u_obj, &bufinfo, data);

    // get the addresses
    mp_uint_t i2c_addr = args[0].u_int;
    mp_uint_t mem_addr = args[1].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = args[3].u_int >> 3;

    // write the register address to write to.
    if (pyb_i2c_mem_write (i2c_addr, (byte *)&mem_addr, mem_addr_size, bufinfo.buf, bufinfo.len)) {
        return mp_const_none;
    }

    mp_raise_OSError(MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_writeto_mem_obj, 1, pyb_i2c_writeto_mem);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&pyb_i2c_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),              (mp_obj_t)&pyb_i2c_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),                (mp_obj_t)&pyb_i2c_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom),            (mp_obj_t)&pyb_i2c_readfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_into),       (mp_obj_t)&pyb_i2c_readfrom_into_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeto),             (mp_obj_t)&pyb_i2c_writeto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_mem),        (mp_obj_t)&pyb_i2c_readfrom_mem_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_mem_into),   (mp_obj_t)&pyb_i2c_readfrom_mem_into_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeto_mem),         (mp_obj_t)&pyb_i2c_writeto_mem_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
