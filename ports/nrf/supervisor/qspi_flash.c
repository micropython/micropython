/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/spi_flash_api.h"

#include <stdint.h>
#include <string.h>

#include "py/mpconfig.h" // for EXTERNAL_FLASH_QSPI_DUAL
#include "nrfx_qspi.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/shared/external_flash/common_commands.h"
#include "supervisor/shared/external_flash/qspi_flash.h"

bool spi_flash_command(uint8_t command) {
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = 0,
        .length = 0,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false
    };
    cinstr_cfg.opcode = command;
    cinstr_cfg.length = 1;
    nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    return true;
}

bool spi_flash_read_command(uint8_t command, uint8_t* response, uint32_t length) {
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = command,
        .length = length + 1,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false
    };
    nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, response);
    return true;
}

bool spi_flash_write_command(uint8_t command, uint8_t* data, uint32_t length) {
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = command,
        .length = length + 1,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false // We do this manually.
    };
    nrfx_qspi_cinstr_xfer(&cinstr_cfg, data, NULL);
    return true;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    if (command != CMD_SECTOR_ERASE) {
        return false;
    }
    return nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, address) == NRFX_SUCCESS;
}

bool spi_flash_write_data(uint32_t address, uint8_t* data, uint32_t length) {
    return nrfx_qspi_write(data, length, address) == NRFX_SUCCESS;
}

bool spi_flash_read_data(uint32_t address, uint8_t* data, uint32_t length) {
    nrfx_qspi_read(data, length, address);
    return true;
}

void spi_flash_init(void) {
    // Init QSPI flash
    nrfx_qspi_config_t qspi_cfg = {
        .xip_offset = 0,
        .pins = {
            .sck_pin = MICROPY_QSPI_SCK,
            .csn_pin = MICROPY_QSPI_CS,
            .io0_pin = MICROPY_QSPI_DATA0,
            .io1_pin = NRF_QSPI_PIN_NOT_CONNECTED,
            .io2_pin = NRF_QSPI_PIN_NOT_CONNECTED,
            .io3_pin = NRF_QSPI_PIN_NOT_CONNECTED,

        },
        .prot_if = {
            .readoc = NRF_QSPI_READOC_FASTREAD,
            .writeoc = NRF_QSPI_WRITEOC_PP,
            .addrmode = NRF_QSPI_ADDRMODE_24BIT,
            .dpmconfig = false
        },
        .phy_if = {
            .sck_freq = NRF_QSPI_FREQ_32MDIV16,
            .sck_delay = 10,    // min time CS must stay high before going low again. in unit of 62.5 ns
            .spi_mode = NRF_QSPI_MODE_0,
            .dpmen = false
        },
        .irq_priority = 7,
    };

#if EXTERNAL_FLASH_QSPI_DUAL
    qspi_cfg.pins.io1_pin = MICROPY_QSPI_DATA1;
    qspi_cfg.prot_if.readoc = NRF_QSPI_READOC_READ2O;
    qspi_cfg.prot_if.writeoc = NRF_QSPI_WRITEOC_PP2O;
#else
    qspi_cfg.pins.io1_pin = MICROPY_QSPI_DATA1;
    qspi_cfg.pins.io2_pin = MICROPY_QSPI_DATA2;
    qspi_cfg.pins.io3_pin = MICROPY_QSPI_DATA3;
    qspi_cfg.prot_if.readoc = NRF_QSPI_READOC_READ4IO;
    qspi_cfg.prot_if.writeoc = NRF_QSPI_WRITEOC_PP4IO;
#endif

    // No callback for blocking API
    nrfx_qspi_init(&qspi_cfg, NULL, NULL);
}

void spi_flash_init_device(const external_flash_device* device) {
    check_quad_enable(device);

    // TODO(tannewt): Adjust the speed for the found device.
}
