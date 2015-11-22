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

#include <board.h>
#include <iap.h>

#include <ciaa-nxp/lpc43_flash.h>
#include <py/mphal.h>

#define __RAM_FUNC __attribute__ ((section(".ramfunc")))

#ifndef IAP_INIT
#define IAP_INIT                                    49
#endif

#define F_ASSERT(exp, msg) do { \
    if (exp) { \
        __fatal(msg); \
    } \
} while(0)

#if 0
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...) do {} while(0)
#endif

/* Base address of the Flash sectors */
#define BANK					IAP_FLASH_BANK_B
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x1B000000) /* Base @ of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x1B002000) /* Base @ of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x1B004000) /* Base @ of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x1B006000) /* Base @ of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x1B008000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x1B00A000) /* Base @ of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x1B00C000) /* Base @ of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x1B00E000) /* Base @ of Sector 7, 256 Kbytes */
#define ADDR_FLASH_END          ((uint32_t)0x1B00E000) /* 1 Mbytes total */

static const uint32_t flash_info_table[26] = {
ADDR_FLASH_SECTOR_0, 0,
ADDR_FLASH_SECTOR_1, 1,
ADDR_FLASH_SECTOR_2, 2,
ADDR_FLASH_SECTOR_3, 3,
ADDR_FLASH_SECTOR_4, 4,
ADDR_FLASH_SECTOR_5, 5,
ADDR_FLASH_SECTOR_6, 6,
ADDR_FLASH_SECTOR_7, 7,
ADDR_FLASH_END, 0, };

uint32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr,
		uint32_t *size) {
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

static void __fatal(char *msg) {
    __enable_irq();
    Board_UARTPutSTR(msg);
	while(1) {
		__BKPT(0);
	}
}

/*
 * F*ck you NXP!!!! I hate this
 */
void flash_init(void) { 
    unsigned int command[5] = { IAP_INIT, 0, 0, 0, 0 };
    unsigned int result[4];
    iap_entry(command, result);
}

void flash_erase(uint32_t f_dst, const uint32_t *src, uint32_t n_words) {
	uint8_t e;
	// check there is something to write
	if (n_words == 0) {
		return;
	}
	__disable_irq();

    // erase the sector(s)
	uint32_t Start = flash_get_sector_info(f_dst, NULL, NULL);
	uint32_t End = flash_get_sector_info(f_dst + 4 * n_words - 1, NULL, NULL);

    e = Chip_IAP_PreSectorForReadWrite(Start, End, BANK);
	F_ASSERT(e != IAP_CMD_SUCCESS, "Prepare for erase FAIL");
    DBG("ERASE Prepare ok %d:%d\n", Start, End);
    e = Chip_IAP_EraseSector(Start, End, BANK);
    F_ASSERT(e != IAP_CMD_SUCCESS, "Erase FAIL");
    DBG("ERASE Operation ok %d:%d\n", Start, End);
    e = Chip_IAP_BlankCheckSector(Start, End, BANK);
    F_ASSERT(e != IAP_CMD_SUCCESS, "Blank check FAIL");
    DBG("ERASE Check ok %d:%d\n", Start, End);
    __enable_irq();
}

void flash_write(uint32_t f_dst, const uint32_t *src, uint32_t n_words) {
	uint8_t e;
	uint32_t Start = flash_get_sector_info(f_dst, NULL, NULL);
	uint32_t End = flash_get_sector_info(f_dst + (n_words - 1) * 4, NULL, NULL);
    __disable_irq();
	e = Chip_IAP_PreSectorForReadWrite(Start, End, BANK);
	F_ASSERT(e != IAP_CMD_SUCCESS, "Prepare for erase FAIL");
    DBG("Write Prepare ok %d:%d:%08X:%08Xx%d\n", Start, End, f_dst, src, n_words);
	e = Chip_IAP_CopyRamToFlash(f_dst, (uint32_t*) src, n_words * 4);
    F_ASSERT(e != IAP_CMD_SUCCESS, "Write FAIL");
    DBG("Write ok %d:%d:%08X:%08Xx%d\n", Start, End, f_dst, src, n_words);
    e = Chip_IAP_Compare(f_dst, (uint32_t) src, n_words * 4);
    F_ASSERT(e != IAP_CMD_SUCCESS, "Verify FAIL");
    DBG("Write Check ok %d:%d:%08X:%08Xx%d\n", Start, End, f_dst, src, n_words);
	__enable_irq();
}
