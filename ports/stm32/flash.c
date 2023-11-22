/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "py/misc.h"
#include "py/mphal.h"
#include "flash.h"

#if defined(STM32F0)

#define FLASH_FLAG_ALL_ERRORS (FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR)

#elif defined(STM32F4)

#define FLASH_FLAG_ALL_ERRORS (FLASH_FLAG_EOP | FLASH_FLAG_OPERR \
    | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)

#elif defined(STM32G0)

// These are not defined on the CMSIS header
#define FLASH_FLAG_SR_ERRORS      (FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | \
    FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | \
    FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR | \
    FLASH_FLAG_OPTVERR)
#if defined(FLASH_OPTR_DBANK)
#define FLASH_FLAG_ECCR_ERRORS    (FLASH_FLAG_ECCC | FLASH_FLAG_ECCD | FLASH_FLAG_ECCC2 | FLASH_FLAG_ECCD2)
#else
#define FLASH_FLAG_ECCR_ERRORS    (FLASH_FLAG_ECCC | FLASH_FLAG_ECCD)
#endif
#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_SR_ERRORS | FLASH_FLAG_ECCR_ERRORS)

#elif defined(STM32H7)

#define FLASH_FLAG_ALL_ERRORS (FLASH_FLAG_ALL_ERRORS_BANK1 | FLASH_FLAG_ALL_ERRORS_BANK2)

#elif defined(STM32L0) || defined(STM32L1)

#define FLASH_FLAG_ALL_ERRORS (FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR)

#endif

#if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
// See WB55 specific documentation in AN5289 Rev 3, and in particular, Figure 10.

#include "rfcore.h"
#include "stm32wbxx_ll_hsem.h"

// Protects all flash registers.
#define SEMID_FLASH_REGISTERS (2)
// Used by CPU1 to prevent CPU2 from writing/erasing data in flash memory.
#define SEMID_FLASH_CPU1 (6)
// Used by CPU2 to prevent CPU1 from writing/erasing data in flash memory.
#define SEMID_FLASH_CPU2 (7)

#endif

typedef struct {
    uint32_t base_address;
    uint32_t sector_size;
    uint32_t sector_count;
} flash_layout_t;

#if defined(STM32F0)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (1)
#define FLASH_LAYOUT_START_ADDR     (FLASH_BASE)
#define FLASH_LAYOUT_SECTOR_SIZE    (FLASH_PAGE_SIZE)
#define FLASH_LAYOUT_NUM_SECTORS    ((FLASH_BANK1_END + 1 - FLASH_BASE) / FLASH_PAGE_SIZE)

#elif defined(STM32F4)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (0)

static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x04000, 4 },
    { 0x08010000, 0x10000, 1 },
    { 0x08020000, 0x20000, 3 },
    #if defined(FLASH_SECTOR_8)
    { 0x08080000, 0x20000, 4 },
    #endif
    #if defined(FLASH_SECTOR_12)
    { 0x08100000, 0x04000, 4 },
    { 0x08110000, 0x10000, 1 },
    { 0x08120000, 0x20000, 7 },
    #endif
};

#elif defined(STM32F7)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (0)

#if defined(STM32F722xx) || defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x04000, 4 },
    { 0x08010000, 0x10000, 1 },
    { 0x08020000, 0x20000, 3 },
};
#else
// This is for dual-bank mode disabled
static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x08000, 4 },
    { 0x08020000, 0x20000, 1 },
    #if FLASH_SECTOR_TOTAL == 8
    { 0x08040000, 0x40000, 3 },
    #else
    { 0x08040000, 0x40000, 7 },
    #endif
};
#endif

#elif defined(STM32G0) || defined(STM32G4) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (1)
#define FLASH_LAYOUT_START_ADDR     (FLASH_BASE)
#define FLASH_LAYOUT_SECTOR_SIZE    (FLASH_PAGE_SIZE)
#define FLASH_LAYOUT_NUM_SECTORS    (512)

#elif defined(STM32L1)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (1)
#define FLASH_LAYOUT_START_ADDR     (FLASH_BASE)
#define FLASH_LAYOUT_SECTOR_SIZE    (0x200)
#define FLASH_LAYOUT_NUM_SECTORS    (1024)

#elif defined(STM32H5)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (1)
#define FLASH_LAYOUT_START_ADDR     (FLASH_BASE_NS)
#define FLASH_LAYOUT_SECTOR_SIZE    (0x2000)
#define FLASH_LAYOUT_NUM_SECTORS    (256)

#elif defined(STM32H7)

#define FLASH_LAYOUT_IS_HOMOGENEOUS (1)
#define FLASH_LAYOUT_START_ADDR     (FLASH_BASE)
#define FLASH_LAYOUT_SECTOR_SIZE    (0x20000)
#define FLASH_LAYOUT_NUM_SECTORS    (16)

#else
#error Unsupported processor
#endif

#if defined(STM32H7) && !defined(DUAL_BANK)

// get the bank of a given flash address
static uint32_t get_bank(uint32_t addr) {
    return FLASH_BANK_1;
}

// get the page of a given flash address
static uint32_t get_page(uint32_t addr) {
    return (addr - FLASH_LAYOUT_START_ADDR) / FLASH_LAYOUT_SECTOR_SIZE;
}

#elif (defined(STM32L4) && defined(SYSCFG_MEMRMP_FB_MODE)) || defined(STM32H5) || defined(STM32H7)

// get the bank of a given flash address
static uint32_t get_bank(uint32_t addr) {
    #if defined(STM32H5) || defined(STM32H7)
    if (READ_BIT(FLASH->OPTCR, FLASH_OPTCR_SWAP_BANK) == 0) {
    #else
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
        #endif
        // no bank swap
        if (addr < (FLASH_LAYOUT_START_ADDR + FLASH_BANK_SIZE)) {
            return FLASH_BANK_1;
        } else {
            return FLASH_BANK_2;
        }
    } else {
        // bank swap
        if (addr < (FLASH_LAYOUT_START_ADDR + FLASH_BANK_SIZE)) {
            return FLASH_BANK_2;
        } else {
            return FLASH_BANK_1;
        }
    }
}

// get the page of a given flash address
static uint32_t get_page(uint32_t addr) {
    if (addr < (FLASH_LAYOUT_START_ADDR + FLASH_BANK_SIZE)) {
        // bank 1
        return (addr - FLASH_LAYOUT_START_ADDR) / FLASH_LAYOUT_SECTOR_SIZE;
    } else {
        // bank 2
        return (addr - (FLASH_LAYOUT_START_ADDR + FLASH_BANK_SIZE)) / FLASH_LAYOUT_SECTOR_SIZE;
    }
}

#elif (defined(STM32L4) && !defined(SYSCFG_MEMRMP_FB_MODE)) || defined(STM32WB) || defined(STM32WL)

static uint32_t get_page(uint32_t addr) {
    return (addr - FLASH_LAYOUT_START_ADDR) / FLASH_LAYOUT_SECTOR_SIZE;
}

#elif defined(STM32G0) || defined(STM32G4)

static uint32_t get_page(uint32_t addr) {
    return (addr - FLASH_LAYOUT_START_ADDR) / FLASH_LAYOUT_SECTOR_SIZE;
}

static uint32_t get_bank(uint32_t addr) {
    // no bank swap
    if (addr < (FLASH_LAYOUT_START_ADDR + FLASH_BANK_SIZE)) {
        return FLASH_BANK_1;
    } else {
        #if defined(FLASH_OPTR_DBANK)
        return FLASH_BANK_2;
        #else
        return 0;
        #endif
    }
}

#endif

bool flash_is_valid_addr(uint32_t addr) {
    #if FLASH_LAYOUT_IS_HOMOGENEOUS
    uint32_t base = FLASH_LAYOUT_START_ADDR;
    uint32_t end_of_flash = FLASH_LAYOUT_START_ADDR + FLASH_LAYOUT_NUM_SECTORS * FLASH_LAYOUT_SECTOR_SIZE;
    #else
    uint32_t base = flash_layout[0].base_address;
    uint8_t last = MP_ARRAY_SIZE(flash_layout) - 1;
    uint32_t end_of_flash = flash_layout[last].base_address +
        flash_layout[last].sector_count * flash_layout[last].sector_size;
    #endif
    return base <= addr && addr < end_of_flash;
}

int32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    #if FLASH_LAYOUT_IS_HOMOGENEOUS
    if (addr >= FLASH_LAYOUT_START_ADDR) {
        uint32_t sector_index = (addr - FLASH_LAYOUT_START_ADDR) / FLASH_LAYOUT_SECTOR_SIZE;
        if (sector_index < FLASH_LAYOUT_NUM_SECTORS) {
            if (start_addr != NULL) {
                *start_addr = FLASH_LAYOUT_START_ADDR + sector_index * FLASH_LAYOUT_SECTOR_SIZE;
            }
            if (size != NULL) {
                *size = FLASH_LAYOUT_SECTOR_SIZE;
            }
            return sector_index;
        }
    }
    #else
    if (addr >= flash_layout[0].base_address) {
        uint32_t sector_index = 0;
        for (int i = 0; i < MP_ARRAY_SIZE(flash_layout); ++i) {
            for (int j = 0; j < flash_layout[i].sector_count; ++j) {
                uint32_t sector_start_next = flash_layout[i].base_address
                    + (j + 1) * flash_layout[i].sector_size;
                if (addr < sector_start_next) {
                    if (start_addr != NULL) {
                        *start_addr = flash_layout[i].base_address
                            + j * flash_layout[i].sector_size;
                    }
                    if (size != NULL) {
                        *size = flash_layout[i].sector_size;
                    }
                    return sector_index;
                }
                ++sector_index;
            }
        }
    }
    #endif
    return -1;
}

// Erase a single flash page which starts at the given address.
// The address will be converted to a bank and sector number.
int flash_erase(uint32_t flash_dest) {
    const unsigned int num_sectors = 1;

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Acquire lock on the flash peripheral.
    while (LL_HSEM_1StepLock(HSEM, SEMID_FLASH_REGISTERS)) {
    }
    #endif

    // Unlock the flash for erase.
    HAL_FLASH_Unlock();

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Tell the HCI controller stack we're starting an erase, so it
    // avoids radio activity for a while.
    rfcore_start_flash_erase();
    // Wait for PES.
    while (LL_FLASH_IsActiveFlag_OperationSuspended()) {
    }
    // Wait for flash lock.
    while (LL_HSEM_1StepLock(HSEM, SEMID_FLASH_CPU2)) {
    }
    #endif

    // Clear pending flags (if any).
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    // Set up EraseInitStruct...
    FLASH_EraseInitTypeDef EraseInitStruct;

    // ... the erase type and number of pages/sectors,
    #if defined(STM32F0) || defined(STM32G0) || defined(STM32G4) || defined(STM32L0) \
    || defined(STM32L1) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = num_sectors;

    #elif defined(STM32F4) || defined(STM32F7) || defined(STM32H5) || defined(STM32H7)

    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.NbSectors = num_sectors;
    #if defined(FLASH_CR_PSIZE)
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    #elif !defined(STM32H5)
    EraseInitStruct.VoltageRange = 0; // unused parameter on STM32H7A3/B3
    #endif

    #else
    #error Unsupported processor
    #endif

    // ... and the flash bank and page/sector.
    #if defined(STM32F0) || defined(STM32L0) || defined(STM32L1)
    EraseInitStruct.PageAddress = flash_dest;
    #elif defined(STM32G0) || defined(STM32G4) || (defined(STM32L4) && defined(SYSCFG_MEMRMP_FB_MODE))
    EraseInitStruct.Page = get_page(flash_dest);
    EraseInitStruct.Banks = get_bank(flash_dest);
    #elif (defined(STM32L4) && !defined(SYSCFG_MEMRMP_FB_MODE)) || defined(STM32WB) || defined(STM32WL)
    EraseInitStruct.Page = get_page(flash_dest);
    #elif defined(STM32F4) || defined(STM32F7)
    EraseInitStruct.Sector = flash_get_sector_info(flash_dest, NULL, NULL);
    #elif defined(STM32H5) || defined(STM32H7)
    EraseInitStruct.Banks = get_bank(flash_dest);
    EraseInitStruct.Sector = get_page(flash_dest);
    #else
    #error Unsupported processor
    #endif

    // Erase the sectors.
    uint32_t SectorError = 0;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Release flash lock.
    while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_CFGBSY)) {
    }
    LL_HSEM_ReleaseLock(HSEM, SEMID_FLASH_CPU2, 0);
    // Tell HCI controller that erase is over.
    rfcore_end_flash_erase();
    #endif

    // Lock the flash after erase.
    HAL_FLASH_Lock();

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Release lock on the flash peripheral.
    LL_HSEM_ReleaseLock(HSEM, SEMID_FLASH_REGISTERS, 0);
    #endif

    return mp_hal_status_to_neg_errno(status);
}

int flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Acquire lock on the flash peripheral.
    while (LL_HSEM_1StepLock(HSEM, SEMID_FLASH_REGISTERS)) {
    }
    #endif

    // Unlock the flash for write.
    HAL_FLASH_Unlock();

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Wait for PES.
    while (LL_FLASH_IsActiveFlag_OperationSuspended()) {
    }
    #endif

    HAL_StatusTypeDef status = HAL_OK;

    #if defined(STM32G0) || defined(STM32G4) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)

    // program the flash uint64 by uint64
    for (int i = 0; i < num_word32 / 2; i++) {
        uint64_t val = *(uint64_t *)src;

        #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
        // Wait for flash lock.
        while (LL_HSEM_1StepLock(HSEM, SEMID_FLASH_CPU2)) {
        }
        #endif

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_dest, val);

        #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
        // Release flash lock.
        LL_HSEM_ReleaseLock(HSEM, SEMID_FLASH_CPU2, 0);
        while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_CFGBSY)) {
        }
        #endif

        if (status != HAL_OK) {
            num_word32 = 0; // don't write any odd word after this loop
            break;
        }
        flash_dest += 8;
        src += 2;
    }
    if ((num_word32 & 0x01) == 1) {
        uint64_t val = *(uint64_t *)flash_dest;
        val = (val & 0xffffffff00000000uL) | (*src);

        #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
        // Wait for flash lock.
        while (LL_HSEM_1StepLock(HSEM, SEMID_FLASH_CPU2)) {
        }
        #endif

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_dest, val);

        #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
        // Release flash lock.
        LL_HSEM_ReleaseLock(HSEM, SEMID_FLASH_CPU2, 0);
        while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_CFGBSY)) {
        }
        #endif
    }

    #elif defined(STM32H5)

    // program the flash 128 bits (4 words) at a time
    for (int i = 0; i < num_word32 / 4; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, flash_dest, (uint64_t)(uint32_t)src);
        if (status != HAL_OK) {
            break;
        }
        flash_dest += 16;
        src += 4;
    }

    #elif defined(STM32H7)

    // program the flash 256 bits at a time
    for (int i = 0; i < num_word32 / 8; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flash_dest, (uint64_t)(uint32_t)src);
        if (status != HAL_OK) {
            break;
        }
        flash_dest += 32;
        src += 8;
    }

    #else

    // program the flash word by word
    for (int i = 0; i < num_word32; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_dest, *src);
        if (status != HAL_OK) {
            break;
        }
        flash_dest += 4;
        src += 1;
    }

    #endif

    // Lock the flash after write.
    HAL_FLASH_Lock();

    #if MICROPY_HW_STM32WB_FLASH_SYNCRONISATION
    // Release lock on the flash peripheral.
    LL_HSEM_ReleaseLock(HSEM, SEMID_FLASH_REGISTERS, 0);
    #endif

    return mp_hal_status_to_neg_errno(status);
}
