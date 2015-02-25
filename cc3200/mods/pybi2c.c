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
///     i2c = I2C()                          # create
///     i2c = I2C(I2C.MASTER)                # create and init as a master
///     i2c.init(I2C.MASTER, baudrate=20000) # init as a master
///     i2c.deinit()                         # turn off the peripheral
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
/// You can specify a timeout (in ms):
///
///     i2c.send(b'123', timeout=20)   # timout after 20ms if the other end is not responding
///
/// A master must specify the recipient's address:
///
///     i2c.init(I2C.MASTER)
///     i2c.send('123', 0x42)        # send 3 bytes to slave with address 0x42
///     i2c.send(b'456', addr=0x42)  # keyword for address
///
/// Master also has other methods:
///
///     i2c.is_ready(0x42)           # check if slave 0x42 is ready
///     i2c.scan()                   # scan for slaves on the bus, returning
///                                  #   a list of valid addresses
///     i2c.mem_read(3, 0x42, 2)     # read 3 bytes from memory of slave 0x42,
///                                  #   starting at address 2 in the slave
///     i2c.mem_write('abc', 0x42, 2, timeout=10)


typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    int mode;
    union {
        uint baudrate;
        byte slvaddr;
    };
} pyb_i2c_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBI2C_MODE_DISABLED                   (-1)
#define PYBI2C_MODE_MASTER                     (0)
#define PYBI2C_MODE_SLAVE                      (1)

#define PYBI2C_DEF_TIMEOUT_MS                  (7)

#define PYBI2C_TIMEOUT_TO_COUNT(to_ms, baud)   (((baud) * to_ms) / 16000)

#define RET_IF_ERR(Func)                        {                           \
                                                    if (!Func) {            \
                                                        return  false;      \
                                                    }                       \
                                                }

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
// only master mode is available for the moment
void i2c_init (uint mode, uint slvaddr, uint baudrate) {
    // Enable the I2C Peripheral
    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_I2CA0);

    // Configure I2C module with the specified baudrate
    MAP_I2CMasterInitExpClk(I2CA0_BASE, baudrate);
}

void i2c_deinit(void) {
    MAP_I2CMasterDisable(I2CA0_BASE);
    MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
}

/******************************************************************************/
/* Micro Python bindings                                                      */
/******************************************************************************/

STATIC bool pybI2C_transaction(uint cmd, uint timeout) {
    // Clear all interrupts
    MAP_I2CMasterIntClearEx(I2CA0_BASE, MAP_I2CMasterIntStatusEx(I2CA0_BASE, false));
    // Set the time-out. Not to be used with breakpoints.
    MAP_I2CMasterTimeoutSet(I2CA0_BASE, timeout);
    // Initiate the transfer.
    MAP_I2CMasterControl(I2CA0_BASE, cmd);
    // Wait until the current byte has been transferred.
    // Poll on the raw interrupt status.
    while ((MAP_I2CMasterIntStatusEx(I2CA0_BASE, false) & (I2C_MASTER_INT_DATA | I2C_MASTER_INT_TIMEOUT)) == 0);

    // Check for any errors in transfer
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

STATIC bool pybI2C_Write(byte devAddr, byte *data, uint len, bool stop, uint timeout) {

    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, false);
    // Write the first byte to the controller.
    MAP_I2CMasterDataPut(I2CA0_BASE, *data++);
    // Initiate the transfer.
    RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_BURST_SEND_START, timeout));

    // Loop until the completion of transfer or error
    while (--len) {
        // Write the next byte of data
        MAP_I2CMasterDataPut(I2CA0_BASE, *data++);
        // Transact over I2C to send the byte
        RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_BURST_SEND_CONT, timeout));
    }

    // If a stop bit is to be sent, send it.
    if (stop) {
        RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_BURST_SEND_STOP, timeout));
    }

    return true;
}

STATIC bool pybI2C_Read(byte devAddr, byte *data, uint len, uint timeout) {
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
    RET_IF_ERR(pybI2C_transaction(cmd, timeout));
    // Decrement the count
    len--;
    // Loop until the completion of reception or error
    while (len) {
        // Receive the byte over I2C
        *data++ = MAP_I2CMasterDataGet(I2CA0_BASE);
        if (--len) {
            // Continue with reception
            RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_BURST_RECEIVE_CONT, timeout));
        }
        else {
            // Complete the last reception
            RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_BURST_RECEIVE_FINISH, timeout));
        }
    }

    // Receive the last byte over I2C
    *data = MAP_I2CMasterDataGet(I2CA0_BASE);

    return true;
}

STATIC bool pybI2C_ScanDevice(byte devAddr, uint timeout) {
    // Set I2C codec slave address
    MAP_I2CMasterSlaveAddrSet(I2CA0_BASE, devAddr, true);
    // Initiate the transfer.
    RET_IF_ERR(pybI2C_transaction(I2C_MASTER_CMD_SINGLE_RECEIVE, timeout));
    // Since this is a hack, send the stop bit anyway
    MAP_I2CMasterControl(I2CA0_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    return true;
}

STATIC void pyb_i2c_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;

    print(env, "<I2C0");
    if (self->mode == PYBI2C_MODE_MASTER) {
        print(env, ", I2C.MASTER, baudrate=%u>)", self->baudrate);
    }
    else if (self->mode == PYBI2C_MODE_SLAVE) {
        print(env, ", I2C.SLAVE, addr=0x%02x>)", self->slvaddr);
    }
    else {
        print(env, ">");
    }
}

/// \method init(mode, *, addr=0x12, baudrate=400000)
///
/// Initialise the I2C bus with the given parameters:
///
///   - `mode` must be either `I2C.MASTER` or `I2C.SLAVE`
///   - `addr` is the 7-bit address (only sensible for a slave)
///   - `baudrate` is the SCL clock rate (only sensible for a master)
STATIC const mp_arg_t pyb_i2c_init_args[] = {
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int  = PYBI2C_MODE_MASTER} },
    { MP_QSTR_addr,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 0} },
    { MP_QSTR_baudrate, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int  = 400000} },
};
#define PYB_I2C_INIT_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_init_args)

STATIC mp_obj_t pyb_i2c_init_helper(pyb_i2c_obj_t *self_in, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = self_in;

    // parse args
    mp_arg_val_t vals[PYB_I2C_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_I2C_INIT_NUM_ARGS, pyb_i2c_init_args, vals);

    self->mode = vals[0].u_int;
    if (self->mode != PYBI2C_MODE_MASTER) {
        // thrown an exception since only master mode is supported for now
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    if (self->mode == PYBI2C_MODE_SLAVE) {
        self->slvaddr = vals[1].u_int;
    }
    else {
        self->baudrate = MIN(vals[2].u_int, 400000);
    }

    // init the I2C bus
    i2c_init(self->mode, self->slvaddr, self->baudrate);

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct an I2C object on the given bus.  `bus` can only be 0.
/// With no additional parameters, the I2C object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get i2c number
    mp_int_t i2c_id = mp_obj_get_int(args[0]);

    // check the i2c number
    if (i2c_id != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    // create and setup the object
    pyb_i2c_obj_t *self = m_new_obj_with_finaliser(pyb_i2c_obj_t);
    self->base.type = &pyb_i2c_type;
    self->mode = PYBI2C_MODE_DISABLED;

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
    i2c_deinit();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_deinit_obj, pyb_i2c_deinit);

/// \method is_ready(addr)
/// Check if an I2C device responds to the given address.  Only valid when in master mode.
STATIC mp_obj_t pyb_i2c_is_ready(mp_obj_t self_in, mp_obj_t i2c_addr_o) {
    pyb_i2c_obj_t *self = self_in;

    if (self->mode != PYBI2C_MODE_MASTER) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    mp_uint_t i2c_addr = mp_obj_get_int(i2c_addr_o);

    for (int i = 0; i < 10; i++) {
        if (pybI2C_ScanDevice(i2c_addr, PYBI2C_TIMEOUT_TO_COUNT(PYBI2C_DEF_TIMEOUT_MS, self->baudrate))) {
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
    pyb_i2c_obj_t *self = self_in;

    if (self->mode != PYBI2C_MODE_MASTER) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (uint addr = 1; addr <= 127; addr++) {
        for (int i = 0; i < 10; i++) {
            if (pybI2C_ScanDevice(addr, PYBI2C_TIMEOUT_TO_COUNT(PYBI2C_DEF_TIMEOUT_MS, self->baudrate))) {
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
///   - `timeout` is the timeout in milliseconds to wait for the other end to respond
/// Return value: `None`.
STATIC const mp_arg_t pyb_i2c_send_args[] = {
    { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,                      MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 5} },
};
#define PYB_I2C_SEND_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_send_args)

STATIC mp_obj_t pyb_i2c_send(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_I2C_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_SEND_NUM_ARGS, pyb_i2c_send_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    if (self->mode == PYBI2C_MODE_MASTER) {
        uint timeout = MIN(PYBI2C_TIMEOUT_TO_COUNT(vals[2].u_int, self->baudrate), 255);
        if (!pybI2C_Write(vals[1].u_int, bufinfo.buf, bufinfo.len, true, timeout)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_send_obj, 1, pyb_i2c_send);

/// \method recv(recv, addr=0x00, timeout=10)
///
/// Receive data on the bus:
///
///   - `recv` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes
///   - `addr` is the address to receive from (only required in master mode)
///   - `timeout` is the timeout in milliseconds to wait for the other end to respond
///
/// Return value: if `recv` is an integer then a new buffer of the bytes received,
/// otherwise the same buffer that was passed in to `recv`.
STATIC const mp_arg_t pyb_i2c_recv_args[] = {
    { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,                      MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 5} },
};
#define PYB_I2C_RECV_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_recv_args)

STATIC mp_obj_t pyb_i2c_recv(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    // parse args
    mp_arg_val_t vals[PYB_I2C_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_RECV_NUM_ARGS, pyb_i2c_recv_args, vals);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // receive the data
    if (self->mode == PYBI2C_MODE_MASTER) {
        uint timeout = MIN(PYBI2C_TIMEOUT_TO_COUNT(vals[2].u_int, self->baudrate), 255);
        if (!pybI2C_Read(vals[1].u_int, (byte *)vstr.buf, vstr.len, timeout)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_recv_obj, 1, pyb_i2c_recv);

/// \method mem_read(data, addr, memaddr, timeout=10, addr_size=8)
///
/// Read from the memory of an I2C device:
///
///   - `data` can be an integer or a buffer to read into
///   - `addr` is the I2C device address
///   - `memaddr` is the memory location within the I2C device
///   - `timeout` is the timeout in milliseconds to wait for the other end to respond
///   - `addr_size` selects width of memaddr: 8 or 16 bits
///
/// Returns the read data.
/// This is only valid in master mode.
STATIC const mp_arg_t pyb_i2c_mem_read_args[] = {
    { MP_QSTR_data,      MP_ARG_REQUIRED  | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,      MP_ARG_REQUIRED  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_memaddr,   MP_ARG_REQUIRED  | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_timeout,   MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 5} },
    { MP_QSTR_addr_size, MP_ARG_KW_ONLY   | MP_ARG_INT, {.u_int = 8} },
};
#define PYB_I2C_MEM_READ_NUM_ARGS MP_ARRAY_SIZE(pyb_i2c_mem_read_args)

STATIC mp_obj_t pyb_i2c_mem_read(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    if (self->mode != PYBI2C_MODE_MASTER) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    // parse args
    mp_arg_val_t vals[PYB_I2C_MEM_READ_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_MEM_READ_NUM_ARGS, pyb_i2c_mem_read_args, vals);

    // get the buffer to read into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // get the addresses
    mp_uint_t i2c_addr = vals[1].u_int;
    mp_uint_t mem_addr = vals[2].u_int;
    // determine width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = vals[4].u_int >> 3;

    // Write the register address to be read from.
    uint timeout = MIN(PYBI2C_TIMEOUT_TO_COUNT(vals[3].u_int, self->baudrate), 255);
    if (pybI2C_Write (i2c_addr, (byte *)&mem_addr, mem_addr_size, false, timeout)) {
        // Read the specified length of data
        if (pybI2C_Read (i2c_addr, (byte *)vstr.buf, vstr.len, timeout)) {
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

/// \method mem_write(data, addr, memaddr, timeout=10, addr_size=8)
///
/// Write to the memory of an I2C device:
///
///   - `data` can be an integer or a buffer to write from
///   - `addr` is the I2C device address
///   - `memaddr` is the memory location within the I2C device
///   - `timeout` is the timeout in milliseconds to wait for the other end to respond
///   - `addr_size` selects width of memaddr: 8 or 16 bits
///
/// Returns `None`.
/// This is only valid in master mode.
STATIC mp_obj_t pyb_i2c_mem_write(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    if (self->mode != PYBI2C_MODE_MASTER) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

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
    // determine width of mem_addr (1 or 2 bytes)
    mp_uint_t mem_addr_size = vals[4].u_int >> 3;

    // Write the register address to be write to.
    uint timeout = MIN(PYBI2C_TIMEOUT_TO_COUNT(vals[2].u_int, self->baudrate), 255);
    if (pybI2C_Write (i2c_addr, (byte *)&mem_addr, mem_addr_size, false, timeout)) {
        // Write the specified length of data
        if (pybI2C_Write (i2c_addr, bufinfo.buf, bufinfo.len, true, timeout)) {
            return mp_const_none;
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_mem_write_obj, 1, pyb_i2c_mem_write);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__),         (mp_obj_t)&pyb_i2c_deinit_obj },
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
    /// \constant SLAVE  - for initialising the bus to slave mode
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),          MP_OBJ_NEW_SMALL_INT(PYBI2C_MODE_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),           MP_OBJ_NEW_SMALL_INT(PYBI2C_MODE_SLAVE) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
