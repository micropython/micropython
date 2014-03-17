#include <stm32f4xx_hal.h>

#include "flash.h"

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

static const uint32_t flash_info_table[26] = {
    ADDR_FLASH_SECTOR_0, FLASH_SECTOR_0,
    ADDR_FLASH_SECTOR_1, FLASH_SECTOR_1,
    ADDR_FLASH_SECTOR_2, FLASH_SECTOR_2,
    ADDR_FLASH_SECTOR_3, FLASH_SECTOR_3,
    ADDR_FLASH_SECTOR_4, FLASH_SECTOR_4,
    ADDR_FLASH_SECTOR_5, FLASH_SECTOR_5,
    ADDR_FLASH_SECTOR_6, FLASH_SECTOR_6,
    ADDR_FLASH_SECTOR_7, FLASH_SECTOR_7,
    ADDR_FLASH_SECTOR_8, FLASH_SECTOR_8,
    ADDR_FLASH_SECTOR_9, FLASH_SECTOR_9,
    ADDR_FLASH_SECTOR_10, FLASH_SECTOR_10,
    ADDR_FLASH_SECTOR_11, FLASH_SECTOR_11,
    ADDR_FLASH_SECTOR_11 + 0x20000, 0,
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

void flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
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
        return;
    }

    // program the flash word by word
    for (int i = 0; i < num_word32; i++) {
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, flash_dest, *src) != HAL_OK) {
            // error occurred during flash write
            return;
        }
        flash_dest += 4;
        src += 1;
    }

    // lock the flash
    HAL_FLASH_Lock();
}
