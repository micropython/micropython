/*
 * Copyright 2017-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_qspi.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

#include "fsl_debug_console.h"

#define FLASH_ENABLE_OCTAL_CMD 0x2U
#define MAX_QSPI_CLK_DIV 0xFFU

/* Ratio between clock of cpu and qspi, qspi clk must be lower than cpu clk to avoid tx fifo underflow */
#define CPU_QSPI_CLK_RATIO 6U

static uint32_t g_clk_div = 0;
static uint8_t g_tmp_data[MFLASH_PAGE_SIZE];

#ifndef XIP_IMAGE
static qspi_dqs_config_t dqsConfig = {
    .portADelayTapNum = 0x05,
    .shift = kQSPI_DQSNoPhraseShift,
    .rxSampleClock = kQSPI_ReadSampleClkExternalInputFromDqsPad,
    .enableDQSClkInverse = false,
};
#endif

/*! @brief QSPI flash driver structure */
static qspi_flash_config_t single_config = {.flashA1Size = FLASH_SIZE, /* 4MB */
                                            .flashA2Size = 0,
                                            .dataHoldTime = 1,
                                            .CSHoldTime = 3,
                                            .CSSetupTime = 3,
                                            .cloumnspace = 0,
                                            .dataLearnValue = 0,
                                            .endian = kQSPI_64LittleEndian,
                                            .enableWordAddress = false};

/* LUT definition, taken from QSPI demo */
static uint32_t lut[FSL_FEATURE_QSPI_LUT_DEPTH] = {
        /* Seq0 :Octa Read */
        /* CMD:        0xEE11 - Octa IO Read, Octa pad */
        /* ADDR:       0x20 - 32bit address, Octa pads */
        /* DUMMY:      0x04 -4 clock cyles, Octa Pad */
        /* READ:       0x80 - Read 128 bytes, Octa pads */
        [0] = 0x471147EE, [1] = 0x0F042B20, [2] = 0x3B80,

        /* Seq1: Write Enable */
        /* CMD:      0x06F9 - Write Enable, Octa pad */
        [4] = 0x47F94706,

        /* Seq2: Erase All */
        /* CMD:    0x609F - Erase All chip, Octa pad */
        [8] = 0x479F4760,

        /* Seq3: Read Status */
        /* CMD:    0x05FA - Read Status, Octa pad */
        [12] = 0x47FA4705, [13] = 0x47004700, [14] = 0x47004700, [15] = 0x3B04,

        /* Seq4: Page Program */
        /* CMD:    0x12ED - Page Program, Octa pad */
        /* ADDR:   0x20 - 32bit address, Octa pad */
        [16] = 0x47ED4712, [17] = 0x3F042B20,

        /* Seq5: Write configuration register 2 */
        /* CMD:    0x72 - Write configuration Register 2, single pad */
        /* ADDR:   0x00000000 - Use 4 commands to simulate address. */
        /* WRITE:  0x01 - Write 1 byte of data, single pad */
        [20] = 0x04000472, [21] = 0x04000400, [22] = 0x20010400,

        /* Seq6: Read Config Register */
        /* CMD:  0x718E - Read configuration register 2, Octa pad */
        /* ADDR: 0x00000000 - Use 4 commands to simulate address. */
        /* DUMMY: 0x4 */
        /* READ: 0x01 - Read 1 byte, Octa pad */
        [24] = 0x478E4771, [25] = 0x47004700, [26] = 0x47004700, [27] = 0x3B010F04,

        /* Seq7: Erase Sector */
        /* CMD:    0x21DE - Sector Erase, Octa pad */
        /* ADDR:   0x20 - 32 bit address, Octa pad */
        [28] = 0x47DE4721, [29] = 0x2B20,

        /* Seq8: Write Enable */
        /* CMD:      0x06 - Write Enable, Single pad */
        [32] = 0x0406,

        /* Match MISRA rule */
        [63] = 0};

static void setQspiClockDiv(QuadSPI_Type *qspi, uint32_t divider)
{
    int32_t i;

    if (qspi)
    {
        /* Make sure QSPI clock is enabled */
        CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_OSPI_OTFAD_CLK_MASK;
        /* Wait until QSPI is not busy */
        while (qspi->SR & QuadSPI_SR_BUSY_MASK)
        {
        }
        /* Make sure module is enabled when reset */
        qspi->MCR &= ~QuadSPI_MCR_MDIS_MASK;
        qspi->MCR |= QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK;
        /* Wait enough cycles until serial flash are reset */
        for (i = 0; i < 200; i++)
        {
            qspi->SR;
        }
        /* Disable module during the reset procedure */
        qspi->MCR |= QuadSPI_MCR_MDIS_MASK;
        /* Clear the reset bits. */
        qspi->MCR &= ~(QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);
        /* Disable clock before changing clock source */
        CLKCTL0->PSCCTL0_CLR = CLKCTL0_PSCCTL0_SET_OSPI_OTFAD_CLK_MASK;
        /* Update qspi clock div */
        CLKCTL0->OSPIFCLKDIV |= CLKCTL0_OSPIFCLKDIV_RESET_MASK; /* Reset the divider counter */
        CLKCTL0->OSPIFCLKDIV = CLKCTL0_OSPIFCLKDIV_DIV(divider - 1);
        while ((CLKCTL0->OSPIFCLKDIV) & CLKCTL0_OSPIFCLKDIV_REQFLAG_MASK)
        {
        }
        /* Enable QSPI clock again */
        CLKCTL0->PSCCTL0_SET = CLKCTL0_PSCCTL0_SET_OSPI_OTFAD_CLK_MASK;

        /* Re-enable QSPI module */
        qspi->MCR &= ~QuadSPI_MCR_MDIS_MASK;
    }
}

/* Check if serial flash erase or program finished. */
static void check_if_finished(void)
{
    uint32_t val = 0;
    /* Check WIP bit */
    do
    {
        while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
        {
        }
        QSPI_ClearFifo(MFLASH_QSPI, kQSPI_RxFifo);
        QSPI_ExecuteIPCommand(MFLASH_QSPI, 12U);
        while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
        {
        }
        val = MFLASH_QSPI->RBDR[0];
        /* Clear ARDB area */
        QSPI_ClearErrorFlag(MFLASH_QSPI, kQSPI_RxBufferDrain);
    } while (val & 0x1);
}

#ifndef XIP_IMAGE
/* Enable Quad DDR mode */
static void enable_octal_mode(void)
{
    uint32_t val[4] = {FLASH_ENABLE_OCTAL_CMD, 0, 0, 0};

    while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_SetIPCommandAddress(MFLASH_QSPI, FSL_FEATURE_QSPI_AMBA_BASE);

    /* Clear Tx FIFO */
    QSPI_ClearFifo(MFLASH_QSPI, kQSPI_TxFifo);

    /* Write enable */
    QSPI_ExecuteIPCommand(MFLASH_QSPI, 32U);

    /* Write data into TX FIFO, needs to write at least 16 bytes of data */
    QSPI_WriteBlocking(MFLASH_QSPI, val, 16U);

    /* Set seq id, write register */
    QSPI_ExecuteIPCommand(MFLASH_QSPI, 20);

    /* Wait until finished */
    check_if_finished();
}
#endif

/* Write enable command */
static void cmd_write_enable(void)
{
    while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ExecuteIPCommand(MFLASH_QSPI, 4U);
}

/* Internal - erase single sector */
static void mflash_drv_sector_erase(uint32_t sector_addr)
{
    __asm("cpsid i");

    while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ClearFifo(MFLASH_QSPI, kQSPI_TxFifo);
    QSPI_SetIPCommandAddress(MFLASH_QSPI, sector_addr);
    cmd_write_enable();
    QSPI_ExecuteIPCommand(MFLASH_QSPI, 28U);
    check_if_finished();

    __asm("cpsie i");
    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ISB();
}

/* Program page into serial flash using QSPI polling way */
void program_page(uint32_t dest_addr, uint32_t *src_addr)
{
    __asm("cpsid i");

    uint32_t old_div = 0;
    if (g_clk_div)
    {
        /* Backup qspi clk div */
        old_div = (CLKCTL0->OSPIFCLKDIV & CLKCTL0_OSPIFCLKDIV_DIV_MASK) + 1;

        /* Set new value of div */
        setQspiClockDiv(MFLASH_QSPI, g_clk_div);
    }

    uint32_t leftLongWords = 0;

    while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ClearFifo(MFLASH_QSPI, kQSPI_TxFifo);

    QSPI_SetIPCommandAddress(MFLASH_QSPI, dest_addr);
    cmd_write_enable();
    while (QSPI_GetStatusFlags(MFLASH_QSPI) & kQSPI_Busy)
    {
    }

    /* First write some data into TXFIFO to prevent from underrun */
    QSPI_WriteBlocking(MFLASH_QSPI, src_addr, FSL_FEATURE_QSPI_TXFIFO_DEPTH * 4);
    src_addr += FSL_FEATURE_QSPI_TXFIFO_DEPTH;

    /* Start the program */
    QSPI_SetIPCommandSize(MFLASH_QSPI, MFLASH_PAGE_SIZE);
    QSPI_ExecuteIPCommand(MFLASH_QSPI, 16U);

    leftLongWords = MFLASH_PAGE_SIZE - 16 * sizeof(uint32_t);
    QSPI_WriteBlocking(MFLASH_QSPI, src_addr, leftLongWords);

    /* Wait until flash finished program */
    check_if_finished();
    while (QSPI_GetStatusFlags(MFLASH_QSPI) & (kQSPI_Busy | kQSPI_IPAccess))
    {
    }
    QSPI_SoftwareReset(MFLASH_QSPI);
    while (QSPI_GetStatusFlags(MFLASH_QSPI) & (kQSPI_Busy | kQSPI_IPAccess))
    {
    }

    if (g_clk_div)
    {
        /* Set the clk div to previous vlaue */
        setQspiClockDiv(MFLASH_QSPI, old_div);
    }

    QSPI_ClearCache(MFLASH_QSPI);

    __asm("cpsie i");
    /* Flush pipeline to allow pending interrupts take place
     * before starting next loop */
    __ISB();
}

/* API - initialize 'mflash' */
void mflash_drv_init(void)
{
    __asm("cpsid i");

    uint32_t qspiClockSourceFreq = QSPI_CLK_FREQ;

#ifndef XIP_IMAGE
    BOARD_InitQSPI();

    qspi_config_t config = {0};
    /*Get QSPI default settings and configure the qspi */
    QSPI_GetDefaultQspiConfig(&config);

    /*Set AHB buffer size for reading data through AHB bus */
    config.AHBbufferSize[3] = MFLASH_PAGE_SIZE;
    QSPI_Init(MFLASH_QSPI, &config, qspiClockSourceFreq);
#endif

    /* Copy the LUT table */
    int i;
    for (i = 0; i < FSL_FEATURE_QSPI_LUT_DEPTH; i++)
    {
        single_config.lookuptable[i] = lut[i];
    }

#ifndef XIP_IMAGE
    /* Set DQS config */
    QSPI_SetDqsConfig(MFLASH_QSPI, &dqsConfig);
#endif

    /* According to serial flash feature to configure flash settings */
    QSPI_SetFlashConfig(MFLASH_QSPI, &single_config);

#ifndef XIP_IMAGE
    QSPI_EnableDDRMode(MFLASH_QSPI, true);

    /* Enable Octal mode for the flash */
    enable_octal_mode();
#endif

    /* QSPI clk must be lower than cpu clk to avoid tx fifo underflow */
    uint32_t max_qspi_clk;
    max_qspi_clk = CLOCK_GetFreq(kCLOCK_CoreSysClk) / CPU_QSPI_CLK_RATIO;
    if (qspiClockSourceFreq > max_qspi_clk)
    {
        uint32_t clk_div;
        /* current qspi clk div */
        clk_div = (CLKCTL0->OSPIFCLKDIV & CLKCTL0_OSPIFCLKDIV_DIV_MASK) + 1;
        /* compute new qspi clk div to be the result qspi clk <= max_qspi_clk */
        g_clk_div = ((qspiClockSourceFreq * clk_div) / max_qspi_clk) + 1;
        assert(g_clk_div <= MAX_QSPI_CLK_DIV);
    }

    __asm("cpsie i");
}

/* Write data to flash */
int32_t mflash_drv_write(void *sector_addr, uint8_t *a_data, uint32_t a_len, uint8_t *b_data, uint32_t b_len)
{
    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;

    uint32_t total_len, ceiling_len;
    uint32_t block_i, block_max;
    uint32_t tmp_a_len = a_len, tmp_b_len = b_len;
    uint8_t *tmp_a_data = a_data, *tmp_b_data = b_data;
    uint32_t tmp_addr = (uint32_t)sector_addr;

    total_len = a_len + b_len;
    ceiling_len = (total_len + (MFLASH_PAGE_MASK)) & (~((uint32_t)(MFLASH_PAGE_MASK)));
    block_max = ceiling_len / (MFLASH_PAGE_SIZE);

    for (block_i = 0; block_i < block_max; block_i++)
    {
        /* Erase sector */
        if ((tmp_addr & MFLASH_SECTOR_MASK) == 0)
        {
            mflash_drv_sector_erase(tmp_addr);
        }

        /* Process word-aligned a_data */
        if (tmp_a_len >= (MFLASH_PAGE_SIZE))
        {
            memcpy(g_tmp_data, tmp_a_data, MFLASH_PAGE_SIZE);
            program_page(tmp_addr, (uint32_t *)g_tmp_data);
            tmp_a_data += (MFLASH_PAGE_SIZE);
            tmp_a_len -= (MFLASH_PAGE_SIZE);
        }
        /* Process word-aligned b_data */
        else if (tmp_a_len == 0 && tmp_b_len >= (MFLASH_PAGE_SIZE))
        {
            memcpy(g_tmp_data, tmp_b_data, MFLASH_PAGE_SIZE);
            program_page(tmp_addr, (uint32_t *)g_tmp_data);
            tmp_b_data += (MFLASH_PAGE_SIZE);
            tmp_b_len -= (MFLASH_PAGE_SIZE);
        }
        /* Process word-unaligned a/b_data */
        else if ((tmp_a_len && tmp_a_len < (MFLASH_PAGE_SIZE)) || (tmp_b_len && tmp_b_len < (MFLASH_PAGE_SIZE)))
        {
            uint32_t tmp_i = 0, tmp_len = 0;
            /* Trailing a_data */
            if (tmp_a_len)
            {
                memcpy(&g_tmp_data[tmp_i], tmp_a_data, tmp_a_len);
                tmp_i = tmp_a_len;
                tmp_a_len = 0;
            }
            /* Leading or trailing b_data */
            if (tmp_b_len)
            {
                tmp_len = (MFLASH_PAGE_SIZE)-tmp_i;
                tmp_len = tmp_len > tmp_b_len ? tmp_b_len : tmp_len;
                memcpy(&g_tmp_data[tmp_i], tmp_b_data, tmp_len);
                tmp_b_data += tmp_len;
                tmp_b_len -= tmp_len;
            }
            program_page(tmp_addr, (uint32_t *)g_tmp_data);
        }
        else
        {
            /* Should not happen */
            assert(0);
            return -1;
        }
        tmp_addr += (MFLASH_PAGE_SIZE);
    }

    return 0;
}
