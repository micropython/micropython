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

#if defined(STM32F7) || defined(STM32H7)

#define MPU_REGION_ETH      (MPU_REGION_NUMBER0)
#define MPU_REGION_SDRAM1   (MPU_REGION_NUMBER4)
#define MPU_REGION_SDRAM2   (MPU_REGION_NUMBER5)

#define MPU_CONFIG_DISABLE(srd, size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE  << MPU_RASR_XN_Pos \
    | MPU_REGION_NO_ACCESS          << MPU_RASR_AP_Pos \
    | MPU_TEX_LEVEL0                << MPU_RASR_TEX_Pos \
    | MPU_ACCESS_NOT_SHAREABLE      << MPU_RASR_S_Pos \
    | MPU_ACCESS_NOT_CACHEABLE      << MPU_RASR_C_Pos \
    | MPU_ACCESS_NOT_BUFFERABLE     << MPU_RASR_B_Pos \
    | (srd)                         << MPU_RASR_SRD_Pos \
    | (size)                        << MPU_RASR_SIZE_Pos \
    | MPU_REGION_ENABLE             << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_ETH(size) ( \
    MPU_INSTRUCTION_ACCESS_DISABLE  << MPU_RASR_XN_Pos \
    | MPU_REGION_FULL_ACCESS        << MPU_RASR_AP_Pos \
    | MPU_TEX_LEVEL1                << MPU_RASR_TEX_Pos \
    | MPU_ACCESS_SHAREABLE          << MPU_RASR_S_Pos \
    | MPU_ACCESS_NOT_CACHEABLE      << MPU_RASR_C_Pos \
    | MPU_ACCESS_NOT_BUFFERABLE     << MPU_RASR_B_Pos \
    | 0x00                          << MPU_RASR_SRD_Pos \
    | (size)                        << MPU_RASR_SIZE_Pos \
    | MPU_REGION_ENABLE             << MPU_RASR_ENABLE_Pos \
    )

#define MPU_CONFIG_SDRAM(size) ( \
    MPU_INSTRUCTION_ACCESS_ENABLE   << MPU_RASR_XN_Pos \
    | MPU_REGION_FULL_ACCESS        << MPU_RASR_AP_Pos \
    | MPU_TEX_LEVEL1                << MPU_RASR_TEX_Pos \
    | MPU_ACCESS_NOT_SHAREABLE      << MPU_RASR_S_Pos \
    | MPU_ACCESS_CACHEABLE          << MPU_RASR_C_Pos \
    | MPU_ACCESS_BUFFERABLE         << MPU_RASR_B_Pos \
    | 0x00                          << MPU_RASR_SRD_Pos \
    | (size)                        << MPU_RASR_SIZE_Pos \
    | MPU_REGION_ENABLE             << MPU_RASR_ENABLE_Pos \
    )

static inline void mpu_init(void) {
    MPU->CTRL = MPU_PRIVILEGED_DEFAULT | MPU_CTRL_ENABLE_Msk;
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DSB();
    __ISB();
}

static inline void mpu_config_start(void) {
    __disable_irq();
}

static inline void mpu_config_region(uint32_t region, uint32_t base_addr, uint32_t attr_size) {
    MPU->RNR = region;
    MPU->RBAR = base_addr;
    MPU->RASR = attr_size;
}

static inline void mpu_config_end(void) {
    __ISB();
    __DSB();
    __DMB();
    __enable_irq();
}

#else

static inline void mpu_init(void) {
}

#endif

#endif // MICROPY_INCLUDED_STM32_MPU_H
