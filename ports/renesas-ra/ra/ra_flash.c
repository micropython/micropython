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

#include <stdbool.h>
#include "hal_data.h"
#include "ra_config.h"
#include "ra_utils.h"
#include "ra_flash.h"

#if !defined(USE_FSP_FLASH)
#define USE_FSP_FLASH
#endif

#if defined(USE_FSP_FLASH)

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#endif

/* Flags, set from Callback function */
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;

static uint8_t flash_buf[FLASH_BUF_SIZE] __attribute__((aligned(2)));

void *FLASH_SECTION lmemset(void *dst, int c, size_t len) {
    char *p;
    for (p = (char *)dst; len > 0; len--) {
        *(p++) = (char)c;
    }
    return (void *)dst;
}

void *FLASH_SECTION lmemcpy(void *dst, const void *src, size_t len) {
    char *d = (char *)dst;
    const char *s = (const char *)src;
    for (; len > 0; len--) {
        *(d++) = *(s++);
    }
    return (void *)dst;
}

int FLASH_SECTION lmemcmp(const void *p1, const void *p2, size_t len) {
    unsigned char *a, *b;
    size_t i;

    a = (unsigned char *)p1;
    b = (unsigned char *)p2;
    for (i = 0; i < len; i++) {
        if (*a != *b) {
            return (*a < *b) ? -1 : 1;
        }
        a++;
        b++;
    }
    return (int)0;
}

#if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)

uint32_t FLASH_SECTION sector_size(uint32_t addr) {
    return FLASH_SECTOR_SIZE;
}

uint32_t FLASH_SECTION sector_start(uint32_t addr) {
    return addr & ~(FLASH_SECTOR_SIZE - 1);
}

uint32_t FLASH_SECTION sector_index(uint32_t addr) {
    return (addr - 0x00000000) / FLASH_SECTOR_SIZE;
}

#elif defined(RA6M1) | defined(RA6M2)

#define REGION1_SECTOR_SIZE 0x8000  // 32K
#define REGION1_SECTOR_MAX  14
#define REGION0_SECTOR_SIZE 0x2000  // 8K
#define REGION0_SECTOR_MAX  8

uint32_t FLASH_SECTION sector_size(uint32_t addr) {
    if (addr <= 0x00010000) {
        return REGION0_SECTOR_SIZE;
    } else {
        return REGION1_SECTOR_SIZE;
    }
}

uint32_t FLASH_SECTION sector_start(uint32_t addr) {
    if (addr <= 0x00010000) {
        return addr & ~(REGION0_SECTOR_SIZE - 1);
    } else {
        return addr & ~(REGION1_SECTOR_SIZE - 1);
    }
}

uint32_t FLASH_SECTION sector_index(uint32_t addr) {
    if (addr <= 0x00010000) {
        return (addr - 0x00010000) / REGION0_SECTOR_SIZE;
    } else {
        return (addr - 0x000100000) / REGION1_SECTOR_SIZE;
    }
}

#else
#error "CMSIS MCU Series is not specified."
#endif

bool internal_flash_read(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff) {
    CHIP_WORD *startaddr = (CHIP_WORD *)addr;
    CHIP_WORD *endaddr = (CHIP_WORD *)(addr + NumBytes);
    while (startaddr < endaddr) {
        *pSectorBuff++ = *startaddr++;
    }
    return true;
}

bool internal_flash_write(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff, bool ReadModifyWrite) {
    return internal_flash_writex(addr, NumBytes, pSectorBuff, ReadModifyWrite, true);
}

bool internal_flash_writex(uint8_t *addr, uint32_t NumBytes, uint8_t *pSectorBuff, bool ReadModifyWrite, bool fIncrementDataPtr) {
    fsp_err_t err = FSP_SUCCESS;
    bool flag;
    uint32_t count;
    uint8_t *buf_addr = (uint8_t *)&flash_buf[0];
    uint32_t startaddr = (uint32_t)addr & FLASH_BUF_ADDR_MASK;
    uint32_t offset = (uint32_t)addr & FLASH_BUF_OFF_MASK;
    uint32_t endaddr = (uint32_t)addr + NumBytes;
    uint32_t error_code = 0;
    while (startaddr < endaddr) {
        // copy from dst rom addr to flash buffer to keep current data
        lmemcpy(flash_buf, (void *)startaddr, FLASH_BUF_SIZE);
        // memset(flash_buf, 0xff, FLASH_BUF_SIZE);
        if (NumBytes + offset > FLASH_BUF_SIZE) {
            count = FLASH_BUF_SIZE - offset;
            NumBytes -= count;
        } else {
            count = NumBytes;
        }
        // overwrite data from src addr to flash buffer
        if (fIncrementDataPtr) {
            lmemcpy(flash_buf + offset, pSectorBuff, count);
        } else {
            lmemset(flash_buf + offset, (int)*pSectorBuff, count);
        }
        g_b_flash_event_write_complete = false;
        uint8_t *flash_addr = (uint8_t *)((uint32_t)startaddr & FLASH_BUF_ADDR_MASK);
        uint32_t state = ra_disable_irq();
        #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
        err = R_FLASH_LP_Write(&g_flash0_ctrl, (uint32_t const)buf_addr, (uint32_t)flash_addr, FLASH_SECTOR_SIZE);
        #elif defined(RA6M1) | defined(RA6M2)
        err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t const)buf_addr, (uint32_t)flash_addr, FLASH_SECTOR_SIZE);
        #else
        #error "CMSIS MCU Series is not specified."
        #endif
        ra_enable_irq(state);
        if (FSP_SUCCESS != err) {
            error_code = 1;
            goto WriteX_exit;
        }
        if (fIncrementDataPtr) {
            flag = (lmemcmp((void *)(startaddr + offset), flash_buf + offset, count) == 0);
            if (!flag) {
                error_code = 2;
                break;
            }
        }
        offset = 0;
        startaddr += FLASH_BUF_SIZE;
        pSectorBuff += count;
    }
WriteX_exit:
    if (error_code == 0) {
        return true;
    }
    return false;
}

bool internal_flash_memset(uint8_t *addr, uint8_t Data, uint32_t NumBytes) {
    return true;
}

bool internal_flash_isblockerased(uint8_t *addr, uint32_t BlockLength) {
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blankCheck = FLASH_RESULT_BLANK;
    g_b_flash_event_not_blank = false;
    g_b_flash_event_blank = false;
    uint32_t state = ra_disable_irq();
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, (uint32_t const)((uint32_t)addr & FLASH_BUF_ADDR_MASK), FLASH_SECTOR_SIZE, &blankCheck);
    #elif defined(RA6M1) | defined(RA6M2)
    err = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, (uint32_t const)((uint32_t)addr & FLASH_BUF_ADDR_MASK), FLASH_SECTOR_SIZE, &blankCheck);
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    ra_enable_irq(state);
    if (err == FSP_SUCCESS) {
        if (FLASH_RESULT_BLANK == blankCheck) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool internal_flash_eraseblock(uint8_t *addr) {
    uint32_t error_code = 0;
    fsp_err_t err = FSP_SUCCESS;
    g_b_flash_event_erase_complete = false;
    uint32_t state = ra_disable_irq();
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_Erase(&g_flash0_ctrl, (uint32_t const)addr, 1);
    #elif defined(RA6M1) | defined(RA6M2)
    err = R_FLASH_HP_Erase(&g_flash0_ctrl, (uint32_t const)addr, 1);
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    ra_enable_irq(state);
    if (err == FSP_SUCCESS) {
        error_code = 0;
    } else {
        error_code = 1;
    }
    if (error_code == 0) {
        return true;
    }
    return false;
}

/* Callback function */
void callback_flash(flash_callback_args_t *p_args) {
    /* TODO: add your own code here */
    switch (p_args->event) {
        case FLASH_EVENT_NOT_BLANK:
            g_b_flash_event_not_blank = true;
            break;
        case FLASH_EVENT_BLANK:
            g_b_flash_event_blank = true;
            break;
        case FLASH_EVENT_ERASE_COMPLETE:
            g_b_flash_event_erase_complete = true;
            break;
        case FLASH_EVENT_WRITE_COMPLETE:
            g_b_flash_event_write_complete = true;
            break;
        default:
            /* Do nothing */
            break;
    }
}

bool internal_flash_init(void) {
    fsp_err_t err = FSP_SUCCESS;
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    #elif defined(RA6M1) | defined(RA6M2)
    err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    if (err == FSP_SUCCESS) {
        return true;
    } else {
        return false;
    }
}

#else

// ToDo: implementation

#endif
