/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include <stdint.h>
#include "py/mphal.h"
#include "mpu.h"

#if (defined(STM32F4) && defined(MICROPY_HW_ETH_MDC)) || defined(STM32F7) || defined(STM32H7) || defined(STM32WB)

void mpu_config_region(uint32_t region, uint32_t base_addr, uint32_t attr_size) {
    MPU->RNR = region;
    MPU->RBAR = base_addr;
    MPU->RASR = attr_size;
}

#elif defined(STM32H5)

void mpu_config_region(uint32_t region, uint32_t base_addr, uint32_t size) {
    if (region == MPU_REGION_ETH) {
        // Configure region 1 to make DMA memory non-cacheable.

        __DMB();
        // Configure attribute 1, inner-outer non-cacheable (=0x44).
        MPU->MAIR0 = (MPU->MAIR0 & ~MPU_MAIR0_Attr1_Msk)
            | 0x44 << MPU_MAIR0_Attr1_Pos;
        __DMB();

        // RBAR
        //  BASE          Bits [31:5] of base address
        //  SH[4:3]  00 = Non-shareable
        //  AP[2:1]  01 = Read/write by any privilege level
        //  XN[0]:    1 = No execution

        // RLAR
        //  LIMIT         Limit address. Contains bits[31:5] of the upper inclusive limit of the selected MPU memory region
        //  AT[3:1] 001 = Attribute 1
        //  EN[0]     1 = Enabled
        MPU->RNR = region;
        MPU->RBAR = (base_addr & MPU_RBAR_BASE_Msk)
            | MPU_ACCESS_NOT_SHAREABLE << MPU_RBAR_SH_Pos
            | MPU_REGION_ALL_RW << MPU_RBAR_AP_Pos
            | MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RBAR_XN_Pos;
        MPU->RLAR = ((base_addr + size - 1) & MPU_RLAR_LIMIT_Msk)
            | MPU_ATTRIBUTES_NUMBER1 << MPU_RLAR_AttrIndx_Pos
            | MPU_REGION_ENABLE << MPU_RLAR_EN_Pos;
    }
    __DMB();
}

#endif
