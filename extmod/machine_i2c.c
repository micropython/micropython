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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/modmachine.h"

#define SOFT_I2C_DEFAULT_TIMEOUT_US (50000) // 50ms

#if MICROPY_PY_MACHINE_SOFTI2C

typedef mp_machine_soft_i2c_obj_t machine_i2c_obj_t;

static void mp_hal_i2c_delay(machine_i2c_obj_t *self) {
    // We need to use an accurate delay to get acceptable I2C
    // speeds (eg 1us should be not much more than 1us).
    mp_hal_delay_us_fast(self->us_delay);
}

static void mp_hal_i2c_scl_low(machine_i2c_obj_t *self) {
    mp_hal_pin_od_low(self->scl);
}

static int mp_hal_i2c_scl_release(machine_i2c_obj_t *self) {
    uint32_t count = self->us_timeout;

    mp_hal_pin_od_high(self->scl);
    mp_hal_i2c_delay(self);
    // For clock stretching, wait for the SCL pin to be released, with timeout.
    for (; mp_hal_pin_read(self->scl) == 0 && count; --count) {
        mp_hal_delay_us_fast(1);
    }
    if (count == 0) {
        return -MP_ETIMEDOUT;
    }
    return 0; // success
}

static void mp_hal_i2c_sda_low(machine_i2c_obj_t *self) {
    mp_hal_pin_od_low(self->sda);
}

static void mp_hal_i2c_sda_release(machine_i2c_obj_t *self) {
    mp_hal_pin_od_high(self->sda);
}

static int mp_hal_i2c_sda_read(machine_i2c_obj_t *self) {
    return mp_hal_pin_read(self->sda);
}

static int mp_hal_i2c_start(machine_i2c_obj_t *self) {
    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
    int ret = mp_hal_i2c_scl_release(self);
    if (ret != 0) {
        return ret;
    }
    mp_hal_i2c_sda_low(self);
    mp_hal_i2c_delay(self);
    return 0; // success
}

static int mp_hal_i2c_stop(machine_i2c_obj_t *self) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_sda_low(self);
    mp_hal_i2c_delay(self);
    int ret = mp_hal_i2c_scl_release(self);
    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
    return ret;
}

static void mp_hal_i2c_init(machine_i2c_obj_t *self, uint32_t freq) {
    self->us_delay = 500000 / freq;
    if (self->us_delay == 0) {
        self->us_delay = 1;
    }
    mp_hal_pin_open_drain(self->scl);
    mp_hal_pin_open_drain(self->sda);
    mp_hal_i2c_stop(self); // ignore error
}

// return value:
//    0 - byte written and ack received
//    1 - byte written and nack received
//   <0 - error, with errno being the negative of the return value
static int mp_hal_i2c_write_byte(machine_i2c_obj_t *self, uint8_t val) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);

    for (int i = 7; i >= 0; i--) {
        if ((val >> i) & 1) {
            mp_hal_i2c_sda_release(self);
        } else {
            mp_hal_i2c_sda_low(self);
        }
        mp_hal_i2c_delay(self);
        int ret = mp_hal_i2c_scl_release(self);
        if (ret != 0) {
            mp_hal_i2c_sda_release(self);
            return ret;
        }
        mp_hal_i2c_scl_low(self);
    }

    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
    int ret = mp_hal_i2c_scl_release(self);
    if (ret != 0) {
        return ret;
    }

    int ack = mp_hal_i2c_sda_read(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);

    return ack;
}

// return value:
//    0 - success
//   <0 - error, with errno being the negative of the return value
static int mp_hal_i2c_read_byte(machine_i2c_obj_t *self, uint8_t *val, int nack) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);
    mp_hal_i2c_delay(self);

    uint8_t data = 0;
    for (int i = 7; i >= 0; i--) {
        int ret = mp_hal_i2c_scl_release(self);
        if (ret != 0) {
            return ret;
        }
        data = (data << 1) | mp_hal_i2c_sda_read(self);
        mp_hal_i2c_scl_low(self);
        mp_hal_i2c_delay(self);
    }
    *val = data;

    // send ack/nack bit
    if (!nack) {
        mp_hal_i2c_sda_low(self);
    }
    mp_hal_i2c_delay(self);
    int ret = mp_hal_i2c_scl_release(self);
    if (ret != 0) {
        mp_hal_i2c_sda_release(self);
        return ret;
    }
    mp_hal_i2c_scl_low(self);
    mp_hal_i2c_sda_release(self);

    return 0; // success
}

// return value:
//  >=0 - success; for read it's 0, for write it's number of acks received
//   <0 - error, with errno being the negative of the return value
int mp_machine_soft_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;

    // start the I2C transaction
    int ret = mp_hal_i2c_start(self);
    if (ret != 0) {
        return ret;
    }

    // write the slave address
    ret = mp_hal_i2c_write_byte(self, (addr << 1) | (flags & MP_MACHINE_I2C_FLAG_READ));
    if (ret < 0) {
        return ret;
    } else if (ret != 0) {
        // nack received, release the bus cleanly
        mp_hal_i2c_stop(self);
        return -MP_ENODEV;
    }

    int transfer_ret = 0;
    for (; n--; ++bufs) {
        size_t len = bufs->len;
        uint8_t *buf = bufs->buf;
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            // read bytes from the slave into the given buffer(s)
            while (len--) {
                ret = mp_hal_i2c_read_byte(self, buf++, (n | len) == 0);
                if (ret != 0) {
                    return ret;
                }
            }
        } else {
            // write bytes from the given buffer(s) to the slave
            while (len--) {
                ret = mp_hal_i2c_write_byte(self, *buf++);
                if (ret < 0) {
                    return ret;
                } else if (ret != 0) {
                    // nack received, stop sending
                    n = 0;
                    break;
                }
                ++transfer_ret; // count the number of acks
            }
        }
    }

    // finish the I2C transaction
    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        ret = mp_hal_i2c_stop(self);
        if (ret != 0) {
            return ret;
        }
    }

    return transfer_ret;
}

#endif // MICROPY_PY_MACHINE_SOFTI2C

/******************************************************************************/
// Generic helper functions

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

// For use by ports that require a single buffer of data for a read/write transfer
int mp_machine_i2c_transfer_adaptor(mp_obj_base_t *self, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    size_t len;
    uint8_t *buf;
    if (n == 1) {
        // Use given single buffer
        len = bufs[0].len;
        buf = bufs[0].buf;
    } else {
        // Combine buffers into a single one
        len = 0;
        for (size_t i = 0; i < n; ++i) {
            len += bufs[i].len;
        }
        buf = m_new(uint8_t, len);
        if (!(flags & MP_MACHINE_I2C_FLAG_READ)) {
            len = 0;
            for (size_t i = 0; i < n; ++i) {
                memcpy(buf + len, bufs[i].buf, bufs[i].len);
                len += bufs[i].len;
            }
        }
    }

    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    int ret = i2c_p->transfer_single(self, addr, len, buf, flags);

    if (n > 1) {
        if (flags & MP_MACHINE_I2C_FLAG_READ) {
            // Copy data from single buffer to individual ones
            len = 0;
            for (size_t i = 0; i < n; ++i) {
                memcpy(bufs[i].buf, buf + len, bufs[i].len);
                len += bufs[i].len;
            }
        }
        m_del(uint8_t, buf, len);
    }

    return ret;
}

static int mp_machine_i2c_readfrom(mp_obj_base_t *self, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    mp_machine_i2c_buf_t buf = {.len = len, .buf = dest};
    unsigned int flags = MP_MACHINE_I2C_FLAG_READ | (stop ? MP_MACHINE_I2C_FLAG_STOP : 0);
    return i2c_p->transfer(self, addr, 1, &buf, flags);
}

static int mp_machine_i2c_writeto(mp_obj_base_t *self, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    mp_machine_i2c_buf_t buf = {.len = len, .buf = (uint8_t *)src};
    unsigned int flags = stop ? MP_MACHINE_I2C_FLAG_STOP : 0;
    return i2c_p->transfer(self, addr, 1, &buf, flags);
}

/******************************************************************************/
// MicroPython bindings for generic machine.I2C

static mp_obj_t machine_i2c_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->init == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2C operation not supported"));
    }
    i2c_p->init(self, n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_init_obj, 1, machine_i2c_init);

static mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
    mp_obj_base_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (int addr = 0x08; addr < 0x78; ++addr) {
        int ret = mp_machine_i2c_writeto(self, addr, NULL, 0, true);
        if (ret == 0) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
        // This scan loop may run for some time, so process any pending events/exceptions,
        // or allow the port to run any necessary background tasks.  But do it as fast as
        // possible, in particular we are not waiting on any events.
        mp_event_handle_nowait();
    }
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

static mp_obj_t machine_i2c_start(mp_obj_t self_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->start == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2C operation not supported"));
    }
    int ret = i2c_p->start(self);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_start_obj, machine_i2c_start);

static mp_obj_t machine_i2c_stop(mp_obj_t self_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->stop == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2C operation not supported"));
    }
    int ret = i2c_p->stop(self);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_stop_obj, machine_i2c_stop);

static mp_obj_t machine_i2c_readinto(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->read == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2C operation not supported"));
    }

    // get the buffer to read into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);

    // work out if we want to send a nack at the end
    bool nack = (n_args == 2) ? true : mp_obj_is_true(args[2]);

    // do the read
    int ret = i2c_p->read(self, bufinfo.buf, bufinfo.len, nack);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readinto_obj, 2, 3, machine_i2c_readinto);

static mp_obj_t machine_i2c_write(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->write == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2C operation not supported"));
    }

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    // do the write
    int ret = i2c_p->write(self, bufinfo.buf, bufinfo.len);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    // return number of acks received
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_write_obj, machine_i2c_write);

static mp_obj_t machine_i2c_readfrom(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[2]));
    bool stop = (n_args == 3) ? true : mp_obj_is_true(args[3]);
    int ret = mp_machine_i2c_readfrom(self, addr, (uint8_t *)vstr.buf, vstr.len, stop);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_bytes_from_vstr(&vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readfrom_obj, 3, 4, machine_i2c_readfrom);

static mp_obj_t machine_i2c_readfrom_into(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    bool stop = (n_args == 3) ? true : mp_obj_is_true(args[3]);
    int ret = mp_machine_i2c_readfrom(self, addr, bufinfo.buf, bufinfo.len, stop);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readfrom_into_obj, 3, 4, machine_i2c_readfrom_into);

static mp_obj_t machine_i2c_writeto(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    bool stop = (n_args == 3) ? true : mp_obj_is_true(args[3]);
    int ret = mp_machine_i2c_writeto(self, addr, bufinfo.buf, bufinfo.len, stop);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    // return number of acks received
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_writeto_obj, 3, 4, machine_i2c_writeto);

static mp_obj_t machine_i2c_writevto(size_t n_args, const mp_obj_t *args) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);

    // Get the list of data buffer(s) to write
    size_t nitems;
    const mp_obj_t *items;
    mp_obj_get_array(args[2], &nitems, (mp_obj_t **)&items);

    // Get the stop argument
    bool stop = (n_args == 3) ? true : mp_obj_is_true(args[3]);

    // Extract all buffer data, skipping zero-length buffers
    size_t alloc = nitems == 0 ? 1 : nitems;
    size_t nbufs = 0;
    mp_machine_i2c_buf_t *bufs = mp_local_alloc(alloc * sizeof(mp_machine_i2c_buf_t));
    for (; nitems--; ++items) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(*items, &bufinfo, MP_BUFFER_READ);
        if (bufinfo.len > 0) {
            bufs[nbufs].len = bufinfo.len;
            bufs[nbufs++].buf = bufinfo.buf;
        }
    }

    // Make sure there is at least one buffer, empty if needed
    if (nbufs == 0) {
        bufs[0].len = 0;
        bufs[0].buf = NULL;
        nbufs = 1;
    }

    // Do the I2C transfer
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    int ret = i2c_p->transfer(self, addr, nbufs, bufs, stop ? MP_MACHINE_I2C_FLAG_STOP : 0);
    mp_local_free(bufs);

    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    // Return number of acks received
    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_writevto_obj, 3, 4, machine_i2c_writevto);

static size_t fill_memaddr_buf(uint8_t *memaddr_buf, uint32_t memaddr, uint8_t addrsize) {
    size_t memaddr_len = 0;
    if ((addrsize & 7) != 0 || addrsize > 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid addrsize"));
    }
    for (int16_t i = addrsize - 8; i >= 0; i -= 8) {
        memaddr_buf[memaddr_len++] = memaddr >> i;
    }
    return memaddr_len;
}

static int read_mem(mp_obj_t self_in, uint16_t addr, uint32_t memaddr, uint8_t addrsize, uint8_t *buf, size_t len) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);

    // Create buffer with memory address
    uint8_t memaddr_buf[4];
    size_t memaddr_len = fill_memaddr_buf(&memaddr_buf[0], memaddr, addrsize);

    #if MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1
    // The I2C transfer function may support the MP_MACHINE_I2C_FLAG_WRITE1 option
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    if (i2c_p->transfer_supports_write1) {
        // Create partial write and read buffers
        mp_machine_i2c_buf_t bufs[2] = {
            {.len = memaddr_len, .buf = memaddr_buf},
            {.len = len, .buf = buf},
        };

        // Do write+read I2C transfer
        return i2c_p->transfer(self, addr, 2, bufs,
            MP_MACHINE_I2C_FLAG_WRITE1 | MP_MACHINE_I2C_FLAG_READ | MP_MACHINE_I2C_FLAG_STOP);
    }
    #endif

    int ret = mp_machine_i2c_writeto(self, addr, memaddr_buf, memaddr_len, false);
    if (ret != memaddr_len) {
        // must generate STOP
        mp_machine_i2c_writeto(self, addr, NULL, 0, true);
        return ret;
    }
    return mp_machine_i2c_readfrom(self, addr, buf, len, true);
}

static int write_mem(mp_obj_t self_in, uint16_t addr, uint32_t memaddr, uint8_t addrsize, const uint8_t *buf, size_t len) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);

    // Create buffer with memory address
    uint8_t memaddr_buf[4];
    size_t memaddr_len = fill_memaddr_buf(&memaddr_buf[0], memaddr, addrsize);

    // Create partial write buffers
    mp_machine_i2c_buf_t bufs[2] = {
        {.len = memaddr_len, .buf = memaddr_buf},
        {.len = len, .buf = (uint8_t *)buf},
    };

    // Do I2C transfer
    mp_machine_i2c_p_t *i2c_p = (mp_machine_i2c_p_t *)MP_OBJ_TYPE_GET_SLOT(self->type, protocol);
    return i2c_p->transfer(self, addr, 2, bufs, MP_MACHINE_I2C_FLAG_STOP);
}

static const mp_arg_t machine_i2c_mem_allowed_args[] = {
    { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_memaddr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_arg,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
};

static mp_obj_t machine_i2c_readfrom_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_n, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_i2c_mem_allowed_args), machine_i2c_mem_allowed_args, args);

    // create the buffer to store data into
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[ARG_n].u_obj));

    // do the transfer
    int ret = read_mem(pos_args[0], args[ARG_addr].u_int, args[ARG_memaddr].u_int,
        args[ARG_addrsize].u_int, (uint8_t *)vstr.buf, vstr.len);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_readfrom_mem_obj, 1, machine_i2c_readfrom_mem);


static mp_obj_t machine_i2c_readfrom_mem_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_i2c_mem_allowed_args), machine_i2c_mem_allowed_args, args);

    // get the buffer to store data into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_WRITE);

    // do the transfer
    int ret = read_mem(pos_args[0], args[ARG_addr].u_int, args[ARG_memaddr].u_int,
        args[ARG_addrsize].u_int, bufinfo.buf, bufinfo.len);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_readfrom_mem_into_obj, 1, machine_i2c_readfrom_mem_into);

static mp_obj_t machine_i2c_writeto_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_i2c_mem_allowed_args), machine_i2c_mem_allowed_args, args);

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ);

    // do the transfer
    int ret = write_mem(pos_args[0], args[ARG_addr].u_int, args[ARG_memaddr].u_int,
        args[ARG_addrsize].u_int, bufinfo.buf, bufinfo.len);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_writeto_mem_obj, 1, machine_i2c_writeto_mem);

static const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_i2c_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },

    // primitive I2C operations
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_i2c_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_i2c_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_i2c_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_i2c_write_obj) },

    // standard bus operations
    { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&machine_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&machine_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&machine_i2c_writeto_obj) },
    { MP_ROM_QSTR(MP_QSTR_writevto), MP_ROM_PTR(&machine_i2c_writevto_obj) },

    // memory operations
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem), MP_ROM_PTR(&machine_i2c_readfrom_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem_into), MP_ROM_PTR(&machine_i2c_readfrom_mem_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_mem), MP_ROM_PTR(&machine_i2c_writeto_mem_obj) },
};
MP_DEFINE_CONST_DICT(mp_machine_i2c_locals_dict, machine_i2c_locals_dict_table);

#endif // MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_SOFTI2C

/******************************************************************************/
// Implementation of soft I2C

#if MICROPY_PY_MACHINE_SOFTI2C

static void mp_machine_soft_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_machine_soft_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SoftI2C(scl=" MP_HAL_PIN_FMT ", sda=" MP_HAL_PIN_FMT ", freq=%u)",
        mp_hal_pin_name(self->scl), mp_hal_pin_name(self->sda), 500000 / self->us_delay);
}

static void mp_machine_soft_i2c_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_scl, ARG_sda, ARG_freq, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SOFT_I2C_DEFAULT_TIMEOUT_US} },
    };

    mp_machine_soft_i2c_obj_t *self = (mp_machine_soft_i2c_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    self->us_timeout = args[ARG_timeout].u_int;
    mp_hal_i2c_init(self, args[ARG_freq].u_int);
}

static mp_obj_t mp_machine_soft_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // create new soft I2C object
    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, &mp_machine_soft_i2c_type);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_soft_i2c_init(&self->base, n_args, args, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

int mp_machine_soft_i2c_read(mp_obj_base_t *self_in, uint8_t *dest, size_t len, bool nack) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;
    while (len--) {
        int ret = mp_hal_i2c_read_byte(self, dest++, nack && (len == 0));
        if (ret != 0) {
            return ret;
        }
    }
    return 0; // success
}

int mp_machine_soft_i2c_write(mp_obj_base_t *self_in, const uint8_t *src, size_t len) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;
    int num_acks = 0;
    while (len--) {
        int ret = mp_hal_i2c_write_byte(self, *src++);
        if (ret < 0) {
            return ret;
        } else if (ret != 0) {
            // nack received, stop sending
            break;
        }
        ++num_acks;
    }
    return num_acks;
}

static const mp_machine_i2c_p_t mp_machine_soft_i2c_p = {
    .init = mp_machine_soft_i2c_init,
    .start = (int (*)(mp_obj_base_t *))mp_hal_i2c_start,
    .stop = (int (*)(mp_obj_base_t *))mp_hal_i2c_stop,
    .read = mp_machine_soft_i2c_read,
    .write = mp_machine_soft_i2c_write,
    .transfer = mp_machine_soft_i2c_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_machine_soft_i2c_type,
    MP_QSTR_SoftI2C,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_soft_i2c_make_new,
    print, mp_machine_soft_i2c_print,
    protocol, &mp_machine_soft_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SOFTI2C
