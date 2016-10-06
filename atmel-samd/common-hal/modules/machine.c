/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

 // This file contains all of the port specific HAL functions for the machine
 // module.

#include "shared-bindings/modules/machine.h"
#include "py/nlr.h"

#include "asf/sam0/drivers/sercom/i2c/i2c_master.h"


// Number of times to try to send packet if failed.
#define TIMEOUT 1

void mp_hal_i2c_construct(machine_i2c_obj_t *self, const pin_obj_t* scl,
                          const pin_obj_t* sda, uint32_t freq) {
    struct i2c_master_config config_i2c_master;
    i2c_master_get_config_defaults(&config_i2c_master);
    // Struct takes the argument in Khz not Hz.
    config_i2c_master.baud_rate = freq / 1000;
    // TODO(tannewt): Utilize the secondary sercom if the first is already being
    // used.
    if (sda->primary_sercom.sercom == 0 || sda->primary_sercom.pad != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "SDA pin must be on SERCOM pad 0"));
    }
    if (scl->primary_sercom.sercom == 0 || scl->primary_sercom.pad != 1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "SCL pin must be on SERCOM pad 1"));
    }
    if (sda->primary_sercom.sercom != scl->primary_sercom.sercom) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "SDA and SCL pins must share a SERCOM"));
    }
    config_i2c_master.pinmux_pad0 = sda->primary_sercom.pinmux; // SDA
    config_i2c_master.pinmux_pad1 = scl->primary_sercom.pinmux; // SCL
    config_i2c_master.buffer_timeout = 10000;

    enum status_code status = i2c_master_init(&self->i2c_master_instance,
      sda->primary_sercom.sercom, &config_i2c_master);
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus init error"));
    }

}

void mp_hal_i2c_init(machine_i2c_obj_t *self) {
    i2c_master_enable(&self->i2c_master_instance);
}

void mp_hal_i2c_deinit(machine_i2c_obj_t *self) {
    i2c_master_disable(&self->i2c_master_instance);
}

void mp_hal_i2c_write(machine_i2c_obj_t *self, uint8_t addr, uint8_t *data,
                      size_t len) {
	struct i2c_master_packet packet = {
		.address     = addr,
		.data_length = len,
		.data        = data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        status = i2c_master_write_packet_wait(&self->i2c_master_instance,
                                              &packet);
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }
    if (status != STATUS_OK) {
      nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }
}

bool mp_hal_i2c_probe(machine_i2c_obj_t *self, uint8_t addr) {
    uint8_t buf;
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = 0,
        .data        = &buf,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };

    enum status_code status = i2c_master_write_packet_wait(
        &self->i2c_master_instance, &packet);
    return status == STATUS_OK;
}

void mp_hal_i2c_read(machine_i2c_obj_t *self, uint8_t addr, uint8_t *data,
                     size_t len) {
    struct i2c_master_packet packet = {
		.address     = addr,
		.data_length = len,
		.data        = data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        status = i2c_master_read_packet_wait(&self->i2c_master_instance,
                                             &packet);
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }
    if (status != STATUS_OK) {
      nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }
}

void mp_hal_i2c_write_mem(machine_i2c_obj_t *self, uint8_t addr,
                          uint16_t memaddr, const uint8_t *src, size_t len) {
    uint8_t buffer[len+1];
    buffer[0] = (uint8_t) memaddr;
    for (int i = 0; i < len; i++) {
        buffer[i+1] = src[i];
    }
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = len + 1,
        .data        = buffer,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };

    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        status = i2c_master_write_packet_wait(&self->i2c_master_instance,
                                              &packet);
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }
}

void mp_hal_i2c_read_mem(machine_i2c_obj_t *self, uint8_t addr, uint16_t memaddr, uint8_t *dest, size_t len) {
    // Write the memory address.
    struct i2c_master_packet packet = {
        .address     = addr,
        .data_length = 1,
        .data        = (uint8_t *)&memaddr,
        .ten_bit_address = false,
        .high_speed      = false,
        .hs_master_code  = 0x0,
    };
    uint16_t timeout = 0;
    enum status_code status = STATUS_BUSY;
    while (status != STATUS_OK) {
        status = i2c_master_write_packet_wait_no_stop(
            &self->i2c_master_instance, &packet);
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == TIMEOUT) {
            break;
        }
    }

    if (status != STATUS_OK) {
        i2c_master_send_stop(&self->i2c_master_instance);
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }

    // i2c_read will do a repeated start, and then read the I2C memory
    mp_hal_i2c_read(self, addr, dest, len);
    return;
}
