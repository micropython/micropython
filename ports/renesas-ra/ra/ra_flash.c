/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
 * Copyright (c) 2023 Vekatech Ltd.
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
#include "py/mpconfig.h"
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

#ifdef USE_FSP_QSPI
#if MICROPY_HW_HAS_QSPI_FLASH
static bool r_qspi_get_QE(void) {
    /* Enter direct communication mode */
    R_QSPI->SFMCMD = 1U;

    R_QSPI->SFMCOM = 0x35;
    bool sts = (R_QSPI->SFMCOM >> 1) & 1;

    /* Close the SPI bus cycle. Reference section 39.10.3 "Generating the SPI Bus Cycle during Direct Communication"
     * in the RA6M3 manual R01UH0886EJ0100. */
    R_QSPI->SFMCMD = 1U;

    /* Exit direct communication mode */
    R_QSPI->SFMCMD = 0U;

    return sts;
}

static void r_qspi_set_QE(qspi_instance_ctrl_t *p_instance_ctrl) {
    /* Enter direct communication mode */
    R_QSPI->SFMCMD = 1U;

    /* Enable write. */
    R_QSPI->SFMCOM = p_instance_ctrl->p_cfg->write_enable_command;

    /* Close the SPI bus cycle. Reference section 39.10.3 "Generating the SPI Bus Cycle during Direct Communication"
     * in the RA6M3 manual R01UH0886EJ0100. */
    R_QSPI->SFMCMD = 1U;

    R_QSPI->SFMCOM = 0x31;
    R_QSPI->SFMCOM = 0x02;

    /* Close the SPI bus cycle. Reference section 39.10.3 "Generating the SPI Bus Cycle during Direct Communication"
     * in the RA6M3 manual R01UH0886EJ0100. */
    R_QSPI->SFMCMD = 1U;

    /* Exit direct communication mode */
    R_QSPI->SFMCMD = 0U;
}

static void r_qspi_wait_WIP(qspi_instance_ctrl_t *p_instance_ctrl) {
    /* Enter direct communication mode */
    R_QSPI->SFMCMD = 1U;

    R_QSPI->SFMCOM = p_instance_ctrl->p_cfg->status_command;
    while ((R_QSPI->SFMCOM >> p_instance_ctrl->p_cfg->write_status_bit) & 1) {
        ;
    }

    /* Close the SPI bus cycle. Reference section 39.10.3 "Generating the SPI Bus Cycle during Direct Communication"
     * in the RA6M3 manual R01UH0886EJ0100. */
    R_QSPI->SFMCMD = 1U;

    /* Exit direct communication mode */
    R_QSPI->SFMCMD = 0U;
}

static fsp_err_t R_QSPI_QuadEnable(spi_flash_ctrl_t *p_ctrl) {
    qspi_instance_ctrl_t *p_instance_ctrl = (qspi_instance_ctrl_t *)p_ctrl;

    #if QSPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_status);
    FSP_ERROR_RETURN(QSPI_PRV_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Do not enter direct communication mode from XIP mode.  Reference note in section 39.10.2 "Using Direct
     * Communication Mode" in the RA6M3 manual R01UH0886EJ0100. */
    FSP_ERROR_RETURN(0U == R_QSPI->SFMSDC_b.SFMXST, FSP_ERR_INVALID_MODE);
    #endif

    /* Read device status. */
    if (!r_qspi_get_QE()) {
        r_qspi_set_QE(p_instance_ctrl);
    }

    return FSP_SUCCESS;
}

static fsp_err_t R_QSPI_Wait_WIP(spi_flash_ctrl_t *p_ctrl) {
    qspi_instance_ctrl_t *p_instance_ctrl = (qspi_instance_ctrl_t *)p_ctrl;

    #if QSPI_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_status);
    FSP_ERROR_RETURN(QSPI_PRV_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Do not enter direct communication mode from XIP mode.  Reference note in section 39.10.2 "Using Direct
     * Communication Mode" in the RA6M3 manual R01UH0886EJ0100. */
    FSP_ERROR_RETURN(0U == R_QSPI->SFMSDC_b.SFMXST, FSP_ERR_INVALID_MODE);
    #endif

    /* Wait WIP flag to go 0 */
    r_qspi_wait_WIP(p_instance_ctrl);

    return FSP_SUCCESS;
}

static bool lmemprob(void *dst, size_t len) {
    uint8_t *p;
    for (p = (uint8_t *)dst; ((len > 0) && (p[0] == 0xFF)); len--) {
        p++;
    }

    return len? false : true;
}
#endif
#endif

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

#if defined(RA4M1) | defined(RA4M3) | defined(RA4W1) | MICROPY_HW_HAS_QSPI_FLASH

uint32_t FLASH_SECTION sector_size(uint32_t addr) {
    return FLASH_SECTOR_SIZE;
}

uint32_t FLASH_SECTION sector_start(uint32_t addr) {
    return addr & ~(FLASH_SECTOR_SIZE - 1);
}

uint32_t FLASH_SECTION sector_index(uint32_t addr) {
    return (addr - 0x00000000) / FLASH_SECTOR_SIZE;
}

#elif defined(RA6M1) | defined(RA6M2) | defined(RA6M3) | defined(RA6M5)

#define REGION1_SECTOR_SIZE 0x8000  // 32K
#define REGION1_SECTOR_MAX  14
#define REGION0_SECTOR_SIZE 0x2000  // 8K
#define REGION0_SECTOR_MAX  8

uint32_t FLASH_SECTION sector_size(uint32_t addr) {
    if (addr < 0x00010000) {
        return REGION0_SECTOR_SIZE;
    } else {
        return REGION1_SECTOR_SIZE;
    }
}

uint32_t FLASH_SECTION sector_start(uint32_t addr) {
    if (addr < 0x00010000) {
        return addr & ~(REGION0_SECTOR_SIZE - 1);
    } else {
        return addr & ~(REGION1_SECTOR_SIZE - 1);
    }
}

uint32_t FLASH_SECTION sector_index(uint32_t addr) {
    if (addr < 0x00010000) {
        return (addr - 0x00000000) / REGION0_SECTOR_SIZE;
    } else {
        return ((addr - 0x00010000) / REGION1_SECTOR_SIZE) + REGION0_SECTOR_SIZE;
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
        #if MICROPY_HW_HAS_QSPI_FLASH
        for (uint16_t idx = 0; ((err == FSP_SUCCESS) && (idx < FLASH_SECTOR_SIZE)); idx += FLASH_PAGE_SIZE)
        {
            err = R_QSPI_Write(&g_qspi0_ctrl, &buf_addr[idx], &flash_addr[idx], FLASH_PAGE_SIZE);
            err = R_QSPI_Wait_WIP(&g_qspi0_ctrl);
        }
        #else
        uint32_t state = ra_disable_irq();
        #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
        err = R_FLASH_LP_Write(&g_flash0_ctrl, (uint32_t const)buf_addr, (uint32_t)flash_addr, FLASH_SECTOR_SIZE);
        #elif defined(RA6M1) | defined(RA6M2) | defined(RA6M3) | defined(RA6M5)
        err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t const)buf_addr, (uint32_t)flash_addr, FLASH_SECTOR_SIZE);
        #else
        #error "CMSIS MCU Series is not specified."
        #endif
        ra_enable_irq(state);
        #endif
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
    g_b_flash_event_not_blank = false;
    g_b_flash_event_blank = false;
    #if MICROPY_HW_HAS_QSPI_FLASH
    return lmemprob((uint8_t *)((uint32_t)addr & FLASH_BUF_ADDR_MASK), FLASH_SECTOR_SIZE);
    #else
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blankCheck = FLASH_RESULT_BLANK;
    uint32_t state = ra_disable_irq();
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, (uint32_t const)((uint32_t)addr & FLASH_BUF_ADDR_MASK), FLASH_SECTOR_SIZE, &blankCheck);
    #elif defined(RA6M1) | defined(RA6M2) | defined(RA6M3) | defined(RA6M5)
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
    #endif
}

bool internal_flash_eraseblock(uint8_t *addr) {
    uint32_t error_code = 0;
    fsp_err_t err = FSP_SUCCESS;
    g_b_flash_event_erase_complete = false;
    #if MICROPY_HW_HAS_QSPI_FLASH
    if (!lmemprob((uint8_t *)((uint32_t)addr & FLASH_BUF_ADDR_MASK), FLASH_SECTOR_SIZE)) {
        err = R_QSPI_Erase(&g_qspi0_ctrl, (uint8_t *const)addr, FLASH_SECTOR_SIZE);
        err = R_QSPI_Wait_WIP(&g_qspi0_ctrl);
    }
    #else
    uint32_t state = ra_disable_irq();
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_Erase(&g_flash0_ctrl, (uint32_t const)addr, 1);
    #elif defined(RA6M1) | defined(RA6M2) | defined(RA6M3) | defined(RA6M5)
    err = R_FLASH_HP_Erase(&g_flash0_ctrl, (uint32_t const)addr, 1);
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    ra_enable_irq(state);
    #endif
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
    #if MICROPY_HW_HAS_QSPI_FLASH
    err = R_QSPI_Open(&g_qspi0_ctrl, &g_qspi0_cfg);
    if (err == FSP_SUCCESS) {
        R_QSPI_QuadEnable(&g_qspi0_ctrl);
        return true;
    } else {
        return false;
    }
    #else
    #if defined(RA4M1) | defined(RA4M3) | defined(RA4W1)
    err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    #elif defined(RA6M1) | defined(RA6M2) | defined(RA6M3) | defined(RA6M5)
    err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    if (err == FSP_SUCCESS) {
        return true;
    } else {
        return false;
    }
    #endif
}

#else

// ToDo: implementation

#endif
