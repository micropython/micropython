/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include STM32_HAL_H

#include "flash.h"

#if defined(MCU_SERIES_F7)

// FLASH_FLAG_PGSERR (Programming Sequence Error) was renamed to
// FLASH_FLAG_ERSERR (Erasing Sequence Error) in STM32F7
#define FLASH_FLAG_PGSERR FLASH_FLAG_ERSERR

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) /* Base @ of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) /* Base @ of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) /* Base @ of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base @ of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) /* Base @ of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) /* Base @ of Sector 7, 256 Kbytes */
#define ADDR_FLASH_END          ((uint32_t)0x08100000) /* 1 Mbytes total */

#else

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#if !defined(FLASH_SECTOR_8)
#define ADDR_FLASH_END          ((uint32_t)0x08080000) /* 512 Kbytes total */
#else
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define ADDR_FLASH_END          ((uint32_t)0x08100000) /* 1 Mbytes total */
#endif

#endif // MCU_SERIES_F7

static const uint32_t flash_info_table[26] = {
    ADDR_FLASH_SECTOR_0, FLASH_SECTOR_0,
    ADDR_FLASH_SECTOR_1, FLASH_SECTOR_1,
    ADDR_FLASH_SECTOR_2, FLASH_SECTOR_2,
    ADDR_FLASH_SECTOR_3, FLASH_SECTOR_3,
    ADDR_FLASH_SECTOR_4, FLASH_SECTOR_4,
    ADDR_FLASH_SECTOR_5, FLASH_SECTOR_5,
    ADDR_FLASH_SECTOR_6, FLASH_SECTOR_6,
    ADDR_FLASH_SECTOR_7, FLASH_SECTOR_7,
    #if defined(FLASH_SECTOR_8)
    ADDR_FLASH_SECTOR_8, FLASH_SECTOR_8,
    ADDR_FLASH_SECTOR_9, FLASH_SECTOR_9,
    ADDR_FLASH_SECTOR_10, FLASH_SECTOR_10,
    ADDR_FLASH_SECTOR_11, FLASH_SECTOR_11,
    #endif
    ADDR_FLASH_END, 0,
};

uint32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    if (addr >= flash_info_table[0]) {
        for (int i = 0; i < 24; i += 2) {
            if (addr < flash_info_table[i + 2]) {
                if (start_addr != NULL) {
                    *start_addr = flash_info_table[i];
                }
                if (size != NULL) {
                    *size = flash_info_table[i + 2] - flash_info_table[i];
                }
                return flash_info_table[i + 1];
            }
        }
    }
    return 0;
}

void flash_erase(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    // check there is something to write
    if (num_word32 == 0) {
        return;
    }

    // unlock
    HAL_FLASH_Unlock();

    // Clear pending flags (if any)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    // erase the sector(s)
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    EraseInitStruct.Sector = flash_get_sector_info(flash_dest, NULL, NULL);
    EraseInitStruct.NbSectors = flash_get_sector_info(flash_dest + 4 * num_word32 - 1, NULL, NULL) - EraseInitStruct.Sector + 1;
    uint32_t SectorError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        // error occurred during sector erase
        HAL_FLASH_Lock(); // lock the flash
        return;
    }
}

/*
// erase the sector using an interrupt
void flash_erase_it(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    // check there is something to write
    if (num_word32 == 0) {
        return;
    }

    // unlock
    HAL_FLASH_Unlock();

    // Clear pending flags (if any)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

    // erase the sector(s)
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    EraseInitStruct.Sector = flash_get_sector_info(flash_dest, NULL, NULL);
    EraseInitStruct.NbSectors = flash_get_sector_info(flash_dest + 4 * num_word32 - 1, NULL, NULL) - EraseInitStruct.Sector + 1;
    if (HAL_FLASHEx_Erase_IT(&EraseInitStruct) != HAL_OK) {
        // error occurred during sector erase
        HAL_FLASH_Lock(); // lock the flash
        return;
    }
}
*/

void flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    // program the flash word by word
    for (int i = 0; i < num_word32; i++) {
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, flash_dest, *src) != HAL_OK) {
            // error occurred during flash write
            HAL_FLASH_Lock(); // lock the flash
            return;
        }
        flash_dest += 4;
        src += 1;
    }

    // lock the flash
    HAL_FLASH_Lock();
}

/*
 use erase, then write
void flash_erase_and_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    // check there is something to write
    if (num_word32 == 0) {
        return;
    }

    // unlock
    HAL_FLASH_Unlock();

    // Clear pending flags (if any)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

    // erase the sector(s)
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    EraseInitStruct.Sector = flash_get_sector_info(flash_dest, NULL, NULL);
    EraseInitStruct.NbSectors = flash_get_sector_info(flash_dest + 4 * num_word32 - 1, NULL, NULL) - EraseInitStruct.Sector + 1;
    uint32_t SectorError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        // error occurred during sector erase
        HAL_FLASH_Lock(); // lock the flash
        return;
    }

    // program the flash word by word
    for (int i = 0; i < num_word32; i++) {
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, flash_dest, *src) != HAL_OK) {
            // error occurred during flash write
            HAL_FLASH_Lock(); // lock the flash
            return;
        }
        flash_dest += 4;
        src += 1;
    }

    // lock the flash
    HAL_FLASH_Lock();
}
*/
