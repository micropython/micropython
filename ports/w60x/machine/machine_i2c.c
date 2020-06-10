/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include <stdint.h>
#include <string.h>

#include "wm_include.h"
#include "wm_gpio_afsel.h"
#include "wm_i2c.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    enum tls_io_name scl;
    enum tls_io_name sda;
    uint32_t freq;
} machine_hard_i2c_obj_t;

STATIC const mp_obj_type_t machine_hard_i2c_type;

STATIC void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(scl=%d, sda=%d, freq=%u)",
              self->scl, self->sda, self->freq);
}

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args)  {
    enum { ARG_id, ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 200000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 255} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hard_i2c_obj_t *self = m_new_obj(machine_hard_i2c_obj_t);

    self->base.type = &machine_hard_i2c_type;
    self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    self->freq = args[ARG_freq].u_int;

    wm_i2c_scl_config(self->scl);
    wm_i2c_sda_config(self->sda);

    tls_i2c_init(self->freq);

    return MP_OBJ_FROM_PTR(self);
}

// return value:
//    WM_SUCCESS - success
//    WM_FAILED - Error: No ack received
STATIC int w600_i2c_send_address(uint16_t addr, bool bit10, bool read) {
    u8 flags = 0;
    u8 addr_msb;
    u8 addr_lsb;
    int ret;

    if (bit10) {
        addr_msb = 0xf0 | ((addr >> 7) & 0x06);
        addr_lsb = addr & 0xff;

        if (read) {
            addr_msb |= 0x01;
        }

        tls_i2c_write_byte(addr_msb, 1);
        ret = tls_i2c_wait_ack();

        if (!ret) {
            tls_i2c_write_byte(addr_lsb, 0);
            ret = tls_i2c_wait_ack();
        }
    } else {
        if (read) {
            flags |= 0x01;
        }
        tls_i2c_write_byte((addr << 1) | flags, 1);
        ret = tls_i2c_wait_ack();
    }

    return ret;
}

// return value:
// 0: Success
STATIC int machine_hard_i2c_read(uint16_t addr, uint8_t *dest, size_t len, bool nack) {

    while (len > 1) {
        *dest++ = tls_i2c_read_byte(1, 0);
        len--;
    }
    *dest = tls_i2c_read_byte(! nack, 0);
    return 0; // success
}

// return value:
//   >=0 - len bytes written and # acks received
//   <0 - error, with errno being the negative of the return value
STATIC int machine_hard_i2c_write(uint16_t addr, const uint8_t *src, size_t len) {
    int ret;
    int ack_received = 0;
    uint8_t *buf = (uint8_t *)src;

    while (len > 0) {
        tls_i2c_write_byte(*buf, 0);
        ret = tls_i2c_wait_ack();
        if (ret != WM_SUCCESS) {
            return -MP_EIO;
        } else {
            ack_received++;  // 1 ACK received
        }
        len--;
        buf++;
    }
    return ack_received;
}

// required return value:
//  >=0 - success; for read it's 0, for write it's number of acks received
//   <0 - error, with errno being the negative of the return value
int machine_hard_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    int ret = 0;
    int wait;
    int transfer_ret = 0;

    wait = 1000; // Experimental value, just guessed.
    while((tls_reg_read32(HR_I2C_CR_SR) & I2C_SR_BUSY) && wait > 0) { // Wait while the device is busy
        wait--;      // but not forever
    }

    ret = w600_i2c_send_address(addr, false, flags & MP_MACHINE_I2C_FLAG_READ);

    if (ret != WM_SUCCESS) {  // Send address fails
        tls_i2c_stop();
        return -MP_ENODEV;
    }

    for (; n--; ++bufs) {
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            machine_hard_i2c_read(addr, bufs->buf, bufs->len, n == 0);
        } else {
            if (bufs->len != 0) {
                ret = machine_hard_i2c_write(addr, bufs->buf, bufs->len);
                if (ret < 0) {
                    return ret;
                }
                transfer_ret += ret; // count the number of acks
            }
        }
    }

    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        tls_i2c_stop();
    }

    return transfer_ret;
}

STATIC const mp_machine_i2c_p_t machine_hard_i2c_p = {
    .transfer = machine_hard_i2c_transfer,
};

STATIC const mp_obj_type_t machine_hard_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .protocol = &machine_hard_i2c_p,
    .locals_dict = (mp_obj_dict_t *) &mp_machine_soft_i2c_locals_dict,
};

