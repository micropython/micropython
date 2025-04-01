/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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
#ifndef MICROPY_INCLUDED_ALIF_OSPI_EXT_H
#define MICROPY_INCLUDED_ALIF_OSPI_EXT_H

#include <stddef.h>
#include "ospi_drv.h"

#define OSPI_ETIMEDOUT (110)

#define OSPI_INST_L_8bit    (CTRLR0_INST_L_8bit)
#define OSPI_INST_L_16bit   (CTRLR0_INST_L_16bit)

#define OSPI_ADDR_L_0bit    UINT32_C(0x0)
#define OSPI_ADDR_L_24bit   UINT32_C(0x6)
#define OSPI_ADDR_L_32bit   UINT32_C(0x8)

#define OSPI_DATA_L_8bit    (CTRLR0_DFS_8bit)
#define OSPI_DATA_L_16bit   (CTRLR0_DFS_16bit)
#define OSPI_DATA_L_32bit   (CTRLR0_DFS_32bit)

void ospi_spi_transfer(ospi_flash_cfg_t *ospi_cfg, size_t len, const uint8_t *buf_out, uint8_t *buf_in);

void ospi_setup_read_ext(ospi_flash_cfg_t *ospi_cfg, bool rxds, uint32_t inst_len, uint32_t addr_len, uint32_t data_len, uint32_t read_len, uint32_t wait_cycles);
int ospi_recv_blocking_8bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint8_t *buffer);
int ospi_recv_blocking_16bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint16_t *buffer);
int ospi_recv_blocking_32bit_data(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint32_t *buffer);

void ospi_setup_write_ext(ospi_flash_cfg_t *ospi_cfg, bool rxds, uint32_t inst_len, uint32_t addr_len, uint32_t data_len);

void ospi_xip_enter_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t inst_len, uint32_t data_len, uint16_t incr_command, uint16_t wrap_command, uint16_t read_dummy_cycles);
void ospi_xip_exit_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t inst_len, uint16_t incr_command, uint16_t wrap_command);
void ospi_xip_restore_ext(ospi_flash_cfg_t *ospi_cfg, uint32_t data_len);

#endif // MICROPY_INCLUDED_ALIF_OSPI_EXT_H
