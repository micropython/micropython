/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#include <stdbool.h>
#include "boardctrl.h"
#include "qspi.h"

#if BUILDING_MBOOT
#include "mboot/mboot.h"
#endif

// Use PYBD_SF2 as base configuration.
#include "boards/PYBD_SF2/board_init.c"

// Adesto AT25SF161 16-MBit.
static const mp_spiflash_chip_params_t chip_params_at25sf161 = {
    .jedec_id = 0x01861f,
    .memory_size_bytes_log2 = 21,
    .qspi_prescaler = 3, // maximum frequency 104MHz
    .qread_num_dummy = 2,
};

// Infineon S25FL064 64-MBit.
static const mp_spiflash_chip_params_t chip_params_s25fl064 = {
    .jedec_id = 0x176001,
    .memory_size_bytes_log2 = 23,
    .qspi_prescaler = 2, // maximum frequency 108MHz
    .qread_num_dummy = 4,
};

// Selection of possible SPI flash chips.
static const mp_spiflash_chip_params_t *const chip_params_table[] = {
    &chip_params_at25sf161,
    &chip_params_s25fl064,
};

void board_early_init_sf6(void) {
    // Initialise default SPI flash parameters.
    MICROPY_BOARD_SPIFLASH_CHIP_PARAMS0 = &chip_params_at25sf161;
    MICROPY_BOARD_SPIFLASH_CHIP_PARAMS1 = &chip_params_at25sf161;

    // Continue with standard board early init.
    board_early_init();
}

int mp_spiflash_detect(mp_spiflash_t *spiflash, int ret, uint32_t devid) {
    if (ret != 0) {
        // Could not identify flash.  Succeed anyway using default chip parameters.
        return 0;
    }

    // Try to detect the SPI flash based on the JEDEC id.
    for (size_t i = 0; i < MP_ARRAY_SIZE(chip_params_table); ++i) {
        if (devid == chip_params_table[i]->jedec_id) {
            spiflash->chip_params = chip_params_table[i];
            if (spiflash->config->bus_kind == MP_SPIFLASH_BUS_QSPI) {
                // Reinitialise the QSPI but to set new size, prescaler and dummy bytes.
                uint8_t num_dummy = spiflash->chip_params->qread_num_dummy;
                spiflash->config->bus.u_qspi.proto->ioctl(spiflash->config->bus.u_qspi.data, MP_QSPI_IOCTL_INIT, num_dummy);
            }
            break;
        }
    }

    return 0;
}
