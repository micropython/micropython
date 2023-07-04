/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef RA_FLASH_H_
#define RA_FLASH_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if MICROPY_HW_HAS_QSPI_FLASH
#define SELECT(ic, attr)        ic##_##attr
#define FLASH(ic, attr)         SELECT(ic, attr)
#define CHIP                    AT25SF128A
// #define CHIP                    W25Q128JV

#define W25Q128JV_SECTOR_SIZE   4096ul
#define W25Q128JV_NUM_BLOCKS    4096ul
#define W25Q128JV_PAGE_SIZE     256ul

#define AT25SF128A_SECTOR_SIZE  4096ul
#define AT25SF128A_NUM_BLOCKS   4096ul
#define AT25SF128A_PAGE_SIZE    256ul

#define FLASH_SECTOR_SIZE       FLASH(CHIP, SECTOR_SIZE)
#define FLASH_NUM_BLOCKS        FLASH(CHIP, NUM_BLOCKS)
#define FLASH_PAGE_SIZE         FLASH(CHIP, PAGE_SIZE)
#else
#define FLASH_SECTOR_SIZE   ((uint32_t)2048)
#define FLASH_NUM_BLOCKS    ((uint32_t)128)
#endif
#define FLASH_BUF_OFF_MASK  (FLASH_SECTOR_SIZE - 1)
#define FLASH_BUF_ADDR_MASK (~FLASH_BUF_OFF_MASK)
#define FLASH_BUF_SIZE      FLASH_SECTOR_SIZE

#define FLASH_SECTION __attribute__((section("FLASH_OP")))

typedef uint8_t CHIP_WORD;

void FLASH_SECTION *lmemset(void *dst, int c, size_t len);
void FLASH_SECTION *lmemcpy(void *dst, const void *src, size_t len);
int FLASH_SECTION lmemcmp(const void *p1, const void *p2, size_t len);

uint32_t FLASH_SECTION sector_size(uint32_t addr);
uint32_t FLASH_SECTION sector_start(uint32_t addr);
uint32_t FLASH_SECTION sector_index(uint32_t addr);
bool FLASH_SECTION internal_flash_read(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff);
bool FLASH_SECTION internal_flash_write(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff, bool ReadModifyWrite);
bool FLASH_SECTION internal_flash_writex(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff, bool ReadModifyWrite, bool fIncrementDataPtr);
bool FLASH_SECTION internal_flash_memset(uint8_t *addr, uint8_t Data, uint32_t NumBytes);
bool FLASH_SECTION internal_flash_isblockerased(uint8_t *addr, uint32_t BlockLength);
bool FLASH_SECTION internal_flash_eraseblock(uint8_t *addr);
bool FLASH_SECTION internal_flash_init(void);

#endif /* RA_FLASH_H_ */
