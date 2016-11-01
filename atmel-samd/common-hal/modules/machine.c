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

// We use ENABLE registers below we don't want to treat as a macro.
#undef ENABLE

// Number of times to try to send packet if failed.
#define TIMEOUT 1

void mp_hal_i2c_construct(machine_i2c_obj_t *self, const pin_obj_t* scl,
                          const pin_obj_t* sda, uint32_t freq) {
    struct i2c_master_config config_i2c_master;
    i2c_master_get_config_defaults(&config_i2c_master);
    // Struct takes the argument in Khz not Hz.
    config_i2c_master.baud_rate = freq / 1000;
    Sercom* sercom = NULL;
    uint32_t sda_pinmux = 0;
    uint32_t scl_pinmux = 0;
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = sda->sercom[i].sercom;
        if (potential_sercom == NULL ||
            potential_sercom->I2CM.CTRLA.bit.ENABLE != 0 ||
            sda->sercom[i].pad != 0) {
            continue;
        }
        sda_pinmux = sda->sercom[i].pinmux;
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (potential_sercom == scl->sercom[j].sercom &&
                scl->sercom[j].pad == 1) {
                scl_pinmux = scl->sercom[j].pinmux;
                sercom = potential_sercom;
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "No hardware support available with those pins."));
    }

    config_i2c_master.pinmux_pad0 = sda_pinmux; // SDA
    config_i2c_master.pinmux_pad1 = scl_pinmux; // SCL
    config_i2c_master.buffer_timeout = 10000;

    enum status_code status = i2c_master_init(&self->i2c_master_instance,
        sercom, &config_i2c_master);
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus init error"));
    }

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

void mp_hal_spi_construct(machine_spi_obj_t *self, const pin_obj_t * clock,
                          const pin_obj_t * mosi, const pin_obj_t * miso,
                          uint32_t baudrate) {
    struct spi_config config_spi_master;
    spi_get_config_defaults(&config_spi_master);

    Sercom* sercom = NULL;
    uint32_t clock_pinmux = 0;
    uint32_t mosi_pinmux = 0;
    uint32_t miso_pinmux = 0;
    uint8_t clock_pad = 0;
    uint8_t mosi_pad = 0;
    uint8_t miso_pad = 0;
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = clock->sercom[i].sercom;
        if (potential_sercom == NULL ||
            potential_sercom->SPI.CTRLA.bit.ENABLE != 0) {
            continue;
        }
        clock_pinmux = clock->sercom[i].pinmux;
        clock_pad = clock->sercom[i].pad;
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            mosi_pinmux = mosi->sercom[j].pinmux;
            mosi_pad = mosi->sercom[j].pad;
            for (int k = 0; k < NUM_SERCOMS_PER_PIN; k++) {
                if (potential_sercom == miso->sercom[k].sercom) {
                    miso_pinmux = miso->sercom[k].pinmux;
                    miso_pad = miso->sercom[k].pad;
                    sercom = potential_sercom;
                    break;
                }
            }
            if (sercom != NULL) {
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "No hardware support available with those pins."));
    }

    // Depends on where MOSI and CLK are.
    uint8_t dopo = 8;
    if (clock_pad == 1) {
        if (mosi_pad == 0) {
            dopo = 0;
        } else if (mosi_pad == 3) {
            dopo = 2;
        }
    } else if (clock_pad == 3) {
        if (mosi_pad == 0) {
            dopo = 3;
        } else if (mosi_pad == 2) {
            dopo = 1;
        }
    }
    if (dopo == 8) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "SPI MOSI and clock pins incompatible."));
    }

    config_spi_master.mux_setting = (dopo << SERCOM_SPI_CTRLA_DOPO_Pos) |
        (miso_pad << SERCOM_SPI_CTRLA_DIPO_Pos);

    // Map pad to pinmux through a short array.
    uint32_t *pinmuxes[4] = {&config_spi_master.pinmux_pad0,
                             &config_spi_master.pinmux_pad1,
                             &config_spi_master.pinmux_pad2,
                             &config_spi_master.pinmux_pad3};
    *pinmuxes[clock_pad] = clock_pinmux;
    *pinmuxes[mosi_pad] = mosi_pinmux;
    *pinmuxes[miso_pad] = miso_pinmux;

    config_spi_master.mode_specific.master.baudrate = baudrate;

    spi_init(&self->spi_master_instance, sercom, &config_spi_master);

    spi_enable(&self->spi_master_instance);
}

void mp_hal_spi_deinit(machine_spi_obj_t *self) {
    spi_disable(&self->spi_master_instance);
}

void mp_hal_spi_transfer(machine_spi_obj_t *self, size_t len, const uint8_t *src,
                         uint8_t *dest) {
    // TODO(tannewt): Don't cast away the const. Change ASF to respect it instead.
    enum status_code status = spi_transceive_buffer_wait(
        &self->spi_master_instance, (uint8_t *) src, dest, len);
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "SPI bus error"));
    }
}
