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
#include MICROPY_HAL_H
#include "py/runtime.h"
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

/// \moduleref pyb
/// \class I2C - a two-wire serial protocol
///
/// I2C is a two-wire protocol for communicating between devices.  At the physical
/// level it consists of 2 wires: SCL and SDA, the clock and data lines respectively.
///
/// I2C objects are created attached to a specific bus.  They can be initialised
/// when created, or initialised later on:
///
///     from pyb import I2C
///
///     i2c = I2C(1)                               # create
///     i2c = I2C(1, I2C.MASTER, baudrate=50000)   # create and init with a 50KHz baudrate
///     i2c.init(I2C.MASTER, baudrate=100000)      # init with a 100KHz baudrate
///     i2c.deinit()                               # turn off the peripheral
///
/// Printing the i2c object gives you information about its configuration.
///
/// Basic methods for slave are send and recv:
///
///     i2c.send('abc')      # send 3 bytes
///     i2c.send(0x42)       # send a single byte, given by the number
///     data = i2c.recv(3)   # receive 3 bytes
///
/// To receive inplace, first create a bytearray:
///
///     data = bytearray(3)  # create a buffer
///     i2c.recv(data)       # receive 3 bytes, writing them into data
///
/// A master must specify the recipient's address:
///
///     i2c.send('123', 0x42)        # send 3 bytes to slave with address 0x42
///     i2c.send(b'456', addr=0x42)  # keyword for address
///
/// Master also has other methods:
///
///     i2c.is_ready(0x42)              # check if slave 0x42 is ready
///     i2c.scan()                      # scan for slaves on the bus, returning
///                                     #   a list of valid addresses
///     i2c.mem_read(3, 0x42, 2)        # read 3 bytes from memory of slave 0x42,
///                                     #   starting at address 2 in the slave
///     i2c.mem_write('abc', 0x42, 2)   # write 3 bytes to memory of slave 0x42,
///                                     #   starting at address 2 in the slave

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    uint baudrate;
} pyb_i2c_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBI2C_MASTER                          (0)

#define PYBI2C_MIN_BAUD_RATE_HZ                (50000)
#define PYBI2C_MAX_BAUD_RATE_HZ                (400000)

#define PYBI2C_TRANSC_TIMEOUT_MS               (10)
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
        // wait for a few microseconds
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(PYBI2C_TRANSAC_WAIT_DELAY_US));
        timeout -= PYBI2C_TRANSAC_WAIT_DELAY_US;
        if (timeout < 0) {
            // the peripheral is not responding, so stop
            return false;
        }
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

STATIC bool pyb_i2c_write(byte devAddr, byte *data, uint len, bool stop) {
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, false);
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

    // If a stop bit is to be sent, send it.
    if (stop) {
        RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_SEND_STOP));
    }

    return true;
}

STATIC bool pyb_i2c_read(byte devAddr, byte *data, uint len) {
    uint cmd;

    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, true);

    // Check if its a single receive or burst receive
    if (len > 1) {
        // Initiate a burst receive sequence
        cmd = I2C_MASTER_CMD_BURST_RECEIVE_START;
    }
    else {
        // Configure for a single receive
        cmd = I2C_MASTER_CMD_SINGLE_RECEIVE;
    }

    // Initiate the transfer.
    RET_IF_ERR(pyb_i2c_transaction(cmd));
    // Decrement the count
    len--;
    // Loop until the completion of reception or error
    while (len) {
        // Receive the byte over I2C
        *data++ = MAP_I2CMasterDataGet(I2CA0_BASE);
        if (--len) {
            // Continue with reception
            RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_RECEIVE_CONT));
        }
        else {
            // Complete the last reception
            RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_BURST_RECEIVE_FINISH));
        }
    }

    // Receive the last byte over I2C
    *data = MAP_I2CMasterDataGet(I2CA0_BASE);

    return true;
}

STATIC bool pyb_i2c_scan_device(byte devAddr) {
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, true);
    // Initiate the transfer.
    RET_IF_ERR(pyb_i2c_transaction(I2C_MASTER_CMD_SINGLE_RECEIVE));
    // Since this is a hack, send the stop bit anyway
    MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    return true;
}

/******************************************************************************/
/* Micro Python bindings                                                      */
/******************************************************************************/
STATIC void pyb_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;
    if (self->baudrate > 0) {
        mp_printf(print, "<I2C1, I2C.MASTER, baudrate=%u>)", self->baudrate);
    }
    else {
        mp_print_str(print, "<I2C1>");
    }
}

/// \method init(mode, *, baudrate=100000)
///
/// Initialise the I2C bus with the given parameters:
///
///   - `mode` must be either `I2C.MASTER` or `I2C.SLAVE`
///   - `baudrate` is the SCL clock rate (only sensible for a master)
STATIC const mp_arg_t pyb_i2c_init_args[] = {
    { MP_QSTR_mode,     MP_ARG_REQUIRED  | MP_ARG_INT, },
    { MP_QSTR_baudrate, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 100000} },
};
#define PYB_I2C_INIT_NUM_ARGS   MP_ARRAY_SIZE(pyb_i2c_init_args)

STATIC mp_obj_t pyb_i2c_init_helper(pyb_i2c_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_I2C_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_I2C_INIT_NUM_ARGS, pyb_i2c_init_args, vals);

    // verify that mode is master
    if (vals[0].u_int != PYBI2C_MASTER) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    // make sure the baudrate is between the valid range
    self->baudrate = MIN(MAX(vals[1].u_int, PYBI2C_MIN_BAUD_RATE_HZ), PYBI2C_MAX_BAUD_RATE_HZ);

    // init the I2C bus
    i2c_init(self);

    // register it with the sleep module
    pybsleep_add ((const mp_obj_t)self, (WakeUpCB_t)i2c_init);

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct an I2C object on the given bus.  `bus` can only be 1.
/// With no additional parameters, the I2C object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // setup the object
    pyb_i2c_obj_t *self = &pyb_i2c_obj;
    self->base.type = &pyb_i2c_type;

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_i2c_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)self;
}

STATIC mp_obj_t pyb_i2c_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_i2c_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_init_obj, 1, pyb_i2c_init);

/// \method deinit()
/// Turn off the I2C bus.
STATIC mp_obj_t pyb_i2c_deinit(mp_obj_t self_in) {
    // disable the peripheral
    MAP_I2CMasterDisable(I2CA0_BASE);
    MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // invalidate the baudrate
    pyb_i2c_obj.baudrate = 0;
    // unregister it with the sleep module
    pybsleep_remove ((const mp_obj_t)self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_deinit_obj, pyb_i2c_deinit);

/// \method is_ready(addr)
/// Check if an I2C device responds to the given address.  Only valid when in master mode.
STATIC mp_obj_t pyb_i2c_is_ready(mp_obj_t self_in, mp_obj_t i2c_addr_o) {
    mp_uint_t i2c_addr = mp_obj_get_int(i2c_addr_o);
    for (int i = 0; i < 7; i++) {
        if (pyb_i2c_scan_device(i2c_addr)) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_i2c_is_ready_obj, pyb_i2c_is_ready);

/// \method scan()
/// Scan all I2C addresses from 0x01 to 0x7f and return a list of those that respond.
/// Only valid when in master mode.
STATIC mp_obj_t pyb_i2c_scan(mp_obj_t self_in) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (uint addr = 1; addr <= 127; addr++) {
        for (int i = 0; i < 7; i++) {
            if (pyb_i2c_scan_device(addr)) {
                mp_obj_list_append(list, mp_obj_new_int(addr));
                break;
            }
        }
    }

    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_scan_obj, pyb_i2c_scan);

/// \method send(send, addr=0x00)
/// Send data on the bus:
///
///   - `send` is the data to send (an integer to send, or a buffer object)
///   - `addr` is the address to send to (only required in master mode)
/// Return value: `None`.
STATIC const mp_arg_t pyb_i2c_send_args[] = {
    { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, },
    { MP_QSTR_addr,                      MP_ARG_INT, {.u_int = 0} },
};
#define PYB_I2C_SEND_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_send_args)

STATIC mp_obj_t pyb_i2c_send(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_I2C_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_SEND_NUM_ARGS, pyb_i2c_send_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    if (!pyb_i2c_write(vals[1].u_int, bufinfo.buf, bufinfo.len, true)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_send_obj, 1, pyb_i2c_send);

/// \method recv(recv, addr=0x00)
///
/// Receive data on the bus:
///
///   - `recv` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes
///   - `addr` is the address to receive from (only required in master mode)
///
/// Return value: if `recv` is an integer then a new buffer of the bytes received,
/// otherwise the same buffer that was passed in to `recv`.
STATIC const mp_arg_t pyb_i2c_recv_args[] = {
    { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, },
    { MP_QSTR_addr,                      MP_ARG_INT, {.u_int = 0} },
};
#define PYB_I2C_RECV_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_recv_args)

STATIC mp_obj_t pyb_i2c_recv(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_I2C_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_RECV_NUM_ARGS, pyb_i2c_recv_args, vals);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // receive the data
    if (!pyb_i2c_read(vals[1].u_int, (byte *)vstr.buf, vstr.len)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    }
    else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_recv_obj, 1, pyb_i2c_recv);

/// \method mem_read(data, addr, memaddr, addr_size=8)
///
/// Read from the memory of an I2C device:
///
///   - `data` can be an integer or a buffer to read into
///   - `addr` is the I2C device address
///   - `memaddr` is the memory location within the I2C device
///   - `addr_size` selects the width of memaddr: 8 or 16 bits
///
/// Returns the read data.
/// This is only valid in master mode.
STATIC const mp_arg_t pyb_i2c_mem_read_args[] = {
    { MP_QSTR_data,      MP_ARG_REQUIRED  | MP_ARG_OBJ, },
    { MP_QSTR_addr,      MP_ARG_REQUIRED  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_memaddr,   MP_ARG_REQUIRED  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_addr_size, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
};
#define PYB_I2C_MEM_READ_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_mem_read_args)

STATIC mp_obj_t pyb_i2c_mem_read(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_I2C_MEM_READ_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_MEM_READ_NUM_ARGS, pyb_i2c_mem_read_args, vals);

    // get the buffer to read into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // get the addresses
    mp_uint_t i2c_addr = vals[1].u_int;
    mp_uint_t mem_addr = vals[2].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = vals[3].u_int >> 3;

    // Write the register address to be read from.
    if (pyb_i2c_write (i2c_addr, (byte *)&mem_addr, mem_addr_size, false)) {
        // Read the specified length of data
        if (pyb_i2c_read (i2c_addr, (byte *)vstr.buf, vstr.len)) {
            // return the read data
            if (o_ret != MP_OBJ_NULL) {
                return o_ret;
            } else {
                return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
            }
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_mem_read_obj, 1, pyb_i2c_mem_read);

/// \method mem_write(data, addr, memaddr, addr_size=8)
///
/// Write to the memory of an I2C device:
///
///   - `data` can be an integer or a buffer to write from
///   - `addr` is the I2C device address
///   - `memaddr` is the memory location within the I2C device
///   - `addr_size` selects the width of memaddr: 8 or 16 bits
///
/// Returns `None`.
/// This is only valid in master mode.
STATIC mp_obj_t pyb_i2c_mem_write(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args (same as mem_read)
    mp_arg_val_t vals[PYB_I2C_MEM_READ_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_MEM_READ_NUM_ARGS, pyb_i2c_mem_read_args, vals);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // get the addresses
    mp_uint_t i2c_addr = vals[1].u_int;
    mp_uint_t mem_addr = vals[2].u_int;
    // determine the width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = vals[3].u_int >> 3;

    // Write the register address to write to.
    if (pyb_i2c_write (i2c_addr, (byte *)&mem_addr, mem_addr_size, false)) {
        // Write the specified length of data
        if (pyb_i2c_write (i2c_addr, bufinfo.buf, bufinfo.len, true)) {
            return mp_const_none;
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_mem_write_obj, 1, pyb_i2c_mem_write);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),            (mp_obj_t)&pyb_i2c_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),          (mp_obj_t)&pyb_i2c_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_ready),        (mp_obj_t)&pyb_i2c_is_ready_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),            (mp_obj_t)&pyb_i2c_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send),            (mp_obj_t)&pyb_i2c_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv),            (mp_obj_t)&pyb_i2c_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_read),        (mp_obj_t)&pyb_i2c_mem_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_write),       (mp_obj_t)&pyb_i2c_mem_write_obj },

    // class constants
    /// \constant MASTER - for initialising the bus to master mode
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),          MP_OBJ_NEW_SMALL_INT(PYBI2C_MASTER) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
