// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"
#include "fsl_common.h"

#define INTERNAL_FLASH_SYSTICK_MASK     (0x1ff) // 512ms
#define INTERNAL_FLASH_IDLE_TICK(tick)  (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

#define FLASH_PAGE_SIZE 256

#define ROM_INDEX_WRITEENABLE 3
#define ROM_INDEX_ERASESECTOR 5
#define ROM_INDEX_PAGEPROGRAM 9
#define ROM_INDEX_READSTATUSREG 1

extern void flexspi_nor_init(void);
extern status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
extern status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src);
extern status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base);
