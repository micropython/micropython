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
#ifndef MICROPY_INCLUDED_STM32_MPU_H
#define MICROPY_INCLUDED_STM32_MPU_H

#include "irq.h"

#if (defined(STM32F4) && defined(MICROPY_HW_ETH_MDC)) || defined(STM32F7) || defined(STM32H7) || defined(STM32WB)

#define MPU_REGION_ETH      (MPU_REGION_NUMBER0)
#define MPU_REGION_QSPI1    (MPU_REGION_NUMBER1)
#define MPU_REGION_QSPI2    (MPU_REGION_NUMBER2)
#define MPU_REGION_QSPI3    (MPU_REGION_NUMBER3)
#define MPU_REGION_SDRAM1   (MPU_REGION_NUMBER4)
#define MPU_REGION_SDRAM2   (MPU_REGION_NUMBER5)
#define MPU_REGION_OPENAMP  (MPU_REGION_NUMBER15)

// Only relevant on CPUs with D-Cache, must be higher priority than SDRAM
#define MPU_REGION_DMA_UNCACHED_1 (MPU_REGION_NUMBER6)
#define MPU_REGION_DMA_UNCACHED_2 (MPU_REGION_NUMBER7)

// Attribute value to disable a region entirely, remove it from the MPU
// (i.e. the MPU_REGION_ENABLE bit is unset.)
#define MPU_CONFIG_DISABLE 0

// Configure a region with all access disabled. Can also set a Subregion Disable mask.
#define MPU_CONFIG_NOACCESS(srd, size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos \
        | MPU_REGION_NO_ACCESS << MPU_RASR_AP_Pos \
        | MPU_TEX_LEVEL0 << MPU_RASR_TEX_Pos \
        | MPU_ACCESS_NOT_SHAREABLE << MPU_RASR_S_Pos \
        | MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos \
        | MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos \
        | (srd) << MPU_RASR_SRD_Pos \
        | (size) << MPU_RASR_SIZE_Pos \
        | MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_ETH(size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos \
        | MPU_REGION_FULL_ACCESS << MPU_RASR_AP_Pos \
        | MPU_TEX_LEVEL1 << MPU_RASR_TEX_Pos \
        | MPU_ACCESS_SHAREABLE << MPU_RASR_S_Pos \
        | MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos \
        | MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos \
        | 0x00 << MPU_RASR_SRD_Pos \
        | (size) << MPU_RASR_SIZE_Pos \
        | MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_SDRAM(size) ( \
    MPU_INSTRUCTION_ACCESS_ENABLE << MPU_RASR_XN_Pos \
        | MPU_REGION_FULL_ACCESS << MPU_RASR_AP_Pos \
        | MPU_TEX_LEVEL1 << MPU_RASR_TEX_Pos \
        | MPU_ACCESS_NOT_SHAREABLE << MPU_RASR_S_Pos \
        | MPU_ACCESS_CACHEABLE << MPU_RASR_C_Pos \
        | MPU_ACCESS_BUFFERABLE << MPU_RASR_B_Pos \
        | 0x00 << MPU_RASR_SRD_Pos \
        | (size) << MPU_RASR_SIZE_Pos \
        | MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_UNCACHED(size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos \
        | MPU_REGION_FULL_ACCESS << MPU_RASR_AP_Pos \
        | MPU_TEX_LEVEL1 << MPU_RASR_TEX_Pos \
        | MPU_ACCESS_NOT_SHAREABLE << MPU_RASR_S_Pos \
        | MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos \
        | MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos \
        | 0x00 << MPU_RASR_SRD_Pos \
        | (size) << MPU_RASR_SIZE_Pos \
        | MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_SHARED_UNCACHED(size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RASR_XN_Pos \
        | MPU_REGION_FULL_ACCESS << MPU_RASR_AP_Pos \
        | MPU_TEX_LEVEL1 << MPU_RASR_TEX_Pos \
        | MPU_ACCESS_SHAREABLE << MPU_RASR_S_Pos \
        | MPU_ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos \
        | MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos \
        | 0x00 << MPU_RASR_SRD_Pos \
        | (size) << MPU_RASR_SIZE_Pos \
        | MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos \
    )

static inline void mpu_init(void) {
    MPU->CTRL = MPU_PRIVILEGED_DEFAULT | MPU_CTRL_ENABLE_Msk;
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DSB();
    __ISB();
}

static inline uint32_t mpu_config_start(void) {
    return disable_irq();
}

static inline void mpu_config_region(uint32_t region, uint32_t base_addr, uint32_t attr_size) {
    MPU->RNR = region;
    MPU->RBAR = base_addr;
    MPU->RASR = attr_size;
}

static inline void mpu_config_end(uint32_t irq_state) {
    __ISB();
    __DSB();
    __DMB();
    enable_irq(irq_state);
}

#elif defined(STM32H5)

#define MPU_REGION_SIG      (MPU_REGION_NUMBER0)
#define MPU_REGION_ETH      (MPU_REGION_NUMBER1)

#define ST_DEVICE_SIGNATURE_BASE (0x08fff800)
#define ST_DEVICE_SIGNATURE_LIMIT (0x08ffffff)

// STM32H5 Cortex-M33 MPU works differently from older cores.
// Macro only takes region size in bytes, Attributes are coded in mpu_config_region().
#define MPU_CONFIG_ETH(size) (size)

static inline void mpu_init(void) {
    // Configure attribute 0, inner-outer non-cacheable (=0x44).
    __DMB();
    MPU->MAIR0 = (MPU->MAIR0 & ~MPU_MAIR0_Attr0_Msk)
        | 0x44 << MPU_MAIR0_Attr0_Pos;

    // Configure region 0 to make device signature non-cacheable.
    // This allows the memory region at ST_DEVICE_SIGNATURE_BASE to be readable.
    __DMB();
    MPU->RNR = MPU_REGION_NUMBER0;
    MPU->RBAR = (ST_DEVICE_SIGNATURE_BASE & MPU_RBAR_BASE_Msk)
        | MPU_ACCESS_NOT_SHAREABLE << MPU_RBAR_SH_Pos
        | MPU_REGION_ALL_RW << MPU_RBAR_AP_Pos
        | MPU_INSTRUCTION_ACCESS_DISABLE << MPU_RBAR_XN_Pos;
    MPU->RLAR = (ST_DEVICE_SIGNATURE_LIMIT & MPU_RLAR_LIMIT_Msk)
        | MPU_ATTRIBUTES_NUMBER0 << MPU_RLAR_AttrIndx_Pos
        | MPU_REGION_ENABLE << MPU_RLAR_EN_Pos;

    // Enable the MPU.
    MPU->CTRL = MPU_PRIVILEGED_DEFAULT | MPU_CTRL_ENABLE_Msk;
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DMB();
    __ISB();
}

static inline uint32_t mpu_config_start(void) {
    return disable_irq();
}

static inline void mpu_config_region(uint32_t region, uint32_t base_addr, uint32_t size) {
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

static inline void mpu_config_end(uint32_t irq_state) {
    __ISB();
    __DSB();
    __DMB();
    enable_irq(irq_state);
}

#else

static inline void mpu_init(void) {
}

#endif

#endif // MICROPY_INCLUDED_STM32_MPU_H
