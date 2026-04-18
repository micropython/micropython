/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Andrew Leech
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

#if MICROPY_PY_MACHINE_MEM_BACKUP

#include STM32_HAL_H
#include "py/obj.h"
#include "py/objarray.h"
#include "extmod/modmachine.h"
#include "machine_mem_backup.h"
#include "mpu.h"

// Detect BKPSRAM availability from CMSIS macros.
#if defined(BKPSRAM_BASE)
// F4, F7, H5, U5
#define STM32_BKPSRAM_BASE BKPSRAM_BASE
#elif defined(D3_BKPSRAM_BASE)
// H7 (non-7Ax/7Bx)
#define STM32_BKPSRAM_BASE D3_BKPSRAM_BASE
#elif defined(SRD_BKPSRAM_BASE)
// H7A3/H7B3/H7B0
#define STM32_BKPSRAM_BASE SRD_BKPSRAM_BASE
#elif defined(BKPSRAM_BASE_NS)
// N6 (no unsuffixed alias)
#define STM32_BKPSRAM_BASE BKPSRAM_BASE_NS
#endif

#if defined(STM32_BKPSRAM_BASE)
#if defined(BKPSRAM_SIZE)
// Families with BKPSRAM_SIZE in CMSIS (H5: 2-4 KB, N6: 8 KB).
#define STM32_BKPSRAM_BYTES BKPSRAM_SIZE
#else
// F4, F7, H7, U5: 4 KB.
#define STM32_BKPSRAM_BYTES (4096)
#endif
#endif

// Detect RTC/TAMP BKP registers (present on all STM32 families).
#if defined(RTC_BKP_NUMBER)
#define STM32_BKP_REG_COUNT RTC_BKP_NUMBER
#elif defined(TAMP_BKP_NUMBER)
#define STM32_BKP_REG_COUNT TAMP_BKP_NUMBER
#elif defined(RTC_BACKUP_NB)
#define STM32_BKP_REG_COUNT RTC_BACKUP_NB
#elif defined(RTC_BKP_NB)
#define STM32_BKP_REG_COUNT RTC_BKP_NB
#endif

#if defined(STM32_BKP_REG_COUNT)
#if defined(TAMP)
#define STM32_BKP_REG_ADDR ((void *)&TAMP->BKP0R)
#else
#define STM32_BKP_REG_ADDR ((void *)&RTC->BKP0R)
#endif
#endif

#if !defined(STM32_BKPSRAM_BASE) && !defined(STM32_BKP_REG_COUNT)
#error "STM32: MICROPY_PY_MACHINE_MEM_BACKUP enabled but no backup storage found"
#endif

// On families with both BKPSRAM and BKP registers, expose both:
//   region 0: BKPSRAM (byte-accessible, 2-8 KB)
//   region 1: RTC/TAMP BKP registers (word-accessible, 80-128 B)
// On families without BKPSRAM, BKP registers are region 0 only.
#if defined(STM32_BKPSRAM_BASE)
const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('B', STM32_BKPSRAM_BYTES, (void *)STM32_BKPSRAM_BASE),
    #if defined(STM32_BKP_REG_COUNT)
    BACKUP_MV('I', STM32_BKP_REG_COUNT, STM32_BKP_REG_ADDR),
    #endif
};
#else
const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('I', STM32_BKP_REG_COUNT, STM32_BKP_REG_ADDR),
};
#endif

const size_t machine_mem_backup_region_count = MP_ARRAY_SIZE(machine_mem_backup_regions);

void machine_mem_backup_init(void) {
    #if defined(STM32_BKPSRAM_BASE)
    #if defined(RCC_AHB1ENR_BKPSRAMEN)
    // F4, F7, U5: BKPSRAM clock on AHB1.
    __HAL_RCC_BKPSRAM_CLK_ENABLE();
    #elif defined(RCC_AHB4ENR_BKPRAMEN)
    // H7: BKPRAM clock on AHB4.
    __HAL_RCC_BKPRAM_CLK_ENABLE();
    #elif defined(RCC_MEMENR_BKPSRAMEN)
    // N6: memory enable register.
    LL_MEM_EnableClock(LL_MEM_BKPSRAM);
    #endif
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    HAL_PWREx_EnableBkUpReg();
    #elif defined(PWR_BDCR_BREN)
    SET_BIT(PWR->BDCR, PWR_BDCR_BREN);
    // Wait for the backup regulator to become ready, with a 1s fallback so
    // a stuck BRRDY bit can't hang the boot.
    {
        uint32_t ticks = HAL_GetTick();
        while (!READ_BIT(PWR->BDSR, PWR_BDSR_BRRDY)) {
            if (HAL_GetTick() - ticks > 1000) {
                break;
            }
        }
    }
    #elif defined(PWR_BDCR1_BREN)
    SET_BIT(PWR->BDCR1, PWR_BDCR1_BREN);
    #elif defined(PWR_BDCR2_BKPRBSEN)
    SET_BIT(PWR->BDCR2, PWR_BDCR2_BKPRBSEN);
    #endif
    // F4/F7/H5/U5 BKPSRAM is in the peripheral address range (non-cacheable
    // by default) so no MPU region is needed. H7 and N6 map BKPSRAM into the
    // SRAM cacheable region and require an explicit non-cacheable MPU entry.
    #if defined(STM32H7)
    // H7 MPU region encoding uses MPU_REGION_SIZE_4KB; all current H7 variants
    // expose 4 KB BKPSRAM. If a future variant differs, update both
    // STM32_BKPSRAM_BYTES detection and this MPU size constant together.
    MP_STATIC_ASSERT(STM32_BKPSRAM_BYTES == 4096);
    {
        uint32_t irq_state = mpu_config_start();
        mpu_config_region(MPU_REGION_BKPSRAM, STM32_BKPSRAM_BASE,
            MPU_CONFIG_UNCACHED(MPU_REGION_SIZE_4KB));
        mpu_config_end(irq_state);
    }
    #elif defined(STM32N6)
    {
        uint32_t irq_state = mpu_config_start();
        mpu_config_region(MPU_REGION_BKPSRAM, STM32_BKPSRAM_BASE,
            STM32_BKPSRAM_BYTES);
        mpu_config_end(irq_state);
    }
    #endif
    #endif // STM32_BKPSRAM_BASE
}

#endif // MICROPY_PY_MACHINE_MEM_BACKUP
