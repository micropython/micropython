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

#include "py/mpconfig.h"
#include "irq.h"
#include "mpu.h"
#include ALIF_CMSIS_H

static const ARM_MPU_Region_t mpu_table[] __STARTUP_RO_DATA_ATTRIBUTE = {
    [MP_MPU_REGION_SRAM0] = {   /* SRAM0 - 4MB : RO-0, NP-1, XN-0 */
        .RBAR = ARM_MPU_RBAR(0x02000000, ARM_MPU_SH_NON, 0, 1, 0),
        .RLAR = ARM_MPU_RLAR(0x023FFFFF, MP_MPU_ATTR_NORMAL_WT_RA_TRANSIENT)
    },
    [MP_MPU_REGION_SRAM1] = {   /* SRAM1 - 2.5MB : RO-0, NP-1, XN-0 */
        .RBAR = ARM_MPU_RBAR(0x08000000, ARM_MPU_SH_NON, 0, 1, 0),
        .RLAR = ARM_MPU_RLAR(0x0827FFFF, MP_MPU_ATTR_NORMAL_WB_RA_WA)
    },
    [MP_MPU_REGION_HOST_PERIPHERALS] = {   /* Host Peripherals - 16MB : RO-0, NP-1, XN-1 */
        .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 1, 1),
        .RLAR = ARM_MPU_RLAR(0x1AFFFFFF, MP_MPU_ATTR_DEVICE_nGnRE)
    },
    [MP_MPU_REGION_MRAM] = {   /* MRAM - 5.5MB : RO-1, NP-1, XN-0  */
        .RBAR = ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_NON, 1, 1, 0),
        .RLAR = ARM_MPU_RLAR(0x8057FFFF, MP_MPU_ATTR_NORMAL_WT_RA)
    },
    [MP_MPU_REGION_OSPI_REGISTERS] = {   /* OSPI Regs - 16MB : RO-0, NP-1, XN-1  */
        .RBAR = ARM_MPU_RBAR(0x83000000, ARM_MPU_SH_NON, 0, 1, 1),
        .RLAR = ARM_MPU_RLAR(0x83FFFFFF, MP_MPU_ATTR_DEVICE_nGnRE)
    },
    [MP_MPU_REGION_OSPI0_XIP] = {   /* OSPI0 XIP flash - 512MB : RO-1, NP-1, XN-0  */
        .RBAR = ARM_MPU_RBAR(0xA0000000, ARM_MPU_SH_NON, 1, 1, 0),
        .RLAR = ARM_MPU_RLAR(0xBFFFFFFF, MP_MPU_ATTR_NORMAL_NON_CACHEABLE)
    },
    [MP_MPU_REGION_OSPI1_XIP] = {   /* OSPI1 XIP flash - 512MB : RO-1, NP-1, XN-0  */
        .RBAR = ARM_MPU_RBAR(0xC0000000, ARM_MPU_SH_NON, 1, 1, 0),
        .RLAR = ARM_MPU_RLAR(0xDFFFFFFF, MP_MPU_ATTR_NORMAL_NON_CACHEABLE)
    },
};

void MPU_Load_Regions(void) {
    // Configure memory attributes.

    ARM_MPU_SetMemAttr(MP_MPU_ATTR_NORMAL_WT_RA_TRANSIENT,
        ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0), ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0)));

    ARM_MPU_SetMemAttr(MP_MPU_ATTR_DEVICE_nGnRE,
        ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRE));

    ARM_MPU_SetMemAttr(MP_MPU_ATTR_NORMAL_WB_RA_WA,
        ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1), ARM_MPU_ATTR_MEMORY_(1, 1, 1, 1)));

    ARM_MPU_SetMemAttr(MP_MPU_ATTR_NORMAL_WT_RA,
        ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0), ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0)));

    ARM_MPU_SetMemAttr(MP_MPU_ATTR_NORMAL_NON_CACHEABLE,
        ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

    // Load the MPU regions from the table.
    ARM_MPU_Load(0, mpu_table, sizeof(mpu_table) / sizeof(ARM_MPU_Region_t));
}

void mpu_config_mram(bool read_only) {
    uintptr_t atomic = disable_irq();
    ARM_MPU_Disable();
    MPU->RNR = MP_MPU_REGION_MRAM;
    MPU->RBAR = ARM_MPU_RBAR(MRAM_BASE, ARM_MPU_SH_NON, read_only, 1, 0);
    MPU->RLAR = ARM_MPU_RLAR(MRAM_BASE + MRAM_SIZE - 1, MP_MPU_ATTR_NORMAL_WT_RA);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    enable_irq(atomic);
}
