/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_qspi.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

/* Temporary sector storage. Use uint32_t type to force 4B alignment and
 * improve copy operation */
static uint32_t g_flashm_sector[MFLASH_SECTOR_SIZE / sizeof(uint32_t)];

/*! @brief QSPI flash driver structure */
qspi_flash_config_t single_config = {.flashA1Size = FLASH_SIZE, /* 4MB */
                                     .flashA2Size = 0,
#if defined(FSL_FEATURE_QSPI_SUPPORT_PARALLEL_MODE) && (FSL_FEATURE_QSPI_SUPPORT_PARALLEL_MODE)
                                     .flashB1Size = FLASH_SIZE,
                                     .flashB2Size = 0,
#endif
#if !defined(FSL_FEATURE_QSPI_HAS_NO_TDH) || (!FSL_FEATURE_QSPI_HAS_NO_TDH)
                                     .dataHoldTime = 0,
#endif
                                     .CSHoldTime = 0,
                                     .CSSetupTime = 0,
                                     .cloumnspace = 0,
                                     .dataLearnValue = 0,
                                     .endian = kQSPI_64LittleEndian,
                                     .enableWordAddress = false};

/* LUT definition, taken from QSPI demo */
uint32_t lut[FSL_FEATURE_QSPI_LUT_DEPTH] =
    {/* Seq0 :Quad Read */
         /* CMD:        0xEB - Quad Read, Single pad */
         /* ADDR:       0x18 - 24bit address, Quad pads */
         /* DUMMY:      0x06 - 6 clock cyles, Quad pads */
         /* READ:       0x80 - Read 128 bytes, Quad pads */
         /* JUMP_ON_CS: 0 */
         [0] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0xEB, QSPI_ADDR, QSPI_PAD_4, 0x18),
         [1] = QSPI_LUT_SEQ(QSPI_DUMMY, QSPI_PAD_4, 0x06, QSPI_READ, QSPI_PAD_4, 0x80),
         [2] = QSPI_LUT_SEQ(QSPI_JMP_ON_CS, QSPI_PAD_1, 0x0, 0, 0, 0),

         /* Seq1: Write Enable */
         /* CMD:      0x06 - Write Enable, Single pad */
         [4] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x06, 0, 0, 0),

         /* Seq2: Erase All */
         /* CMD:    0x60 - Erase All chip, Single pad */
         [8] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x60, 0, 0, 0),

         /* Seq3: Read Status */
         /* CMD:    0x05 - Read Status, single pad */
         /* READ:   0x01 - Read 1 byte */
         [12] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x05, QSPI_READ, QSPI_PAD_1, 0x1),

         /* Seq4: Page Program */
         /* CMD:    0x02 - Page Program, Single pad */
         /* ADDR:   0x18 - 24bit address, Single pad */
         /* WRITE:  0x80 - Write 128 bytes at one pass, Single pad */
         [16] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x02, QSPI_ADDR, QSPI_PAD_1, 0x18),
         [17] = QSPI_LUT_SEQ(QSPI_WRITE, QSPI_PAD_1, 0x80, 0, 0, 0),

         /* Seq5: Write Register */
         /* CMD:    0x01 - Write Status Register, single pad */
         /* WRITE:  0x01 - Write 1 byte of data, single pad */
         [20] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x01, QSPI_WRITE, QSPI_PAD_1, 0x1),

         /* Seq6: Read Config Register */
         /* CMD:  0x15 - Read Config register, single pad */
         /* READ: 0x01 - Read 1 byte */
         [24] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x15, QSPI_READ, QSPI_PAD_1, 0x1),

         /* Seq7: Erase Sector */
         /* CMD:  0x20 - Sector Erase, single pad */
         /* ADDR: 0x18 - 24 bit address, single pad */
         [28] = QSPI_LUT_SEQ(QSPI_CMD, QSPI_PAD_1, 0x20, QSPI_ADDR, QSPI_PAD_1, 0x18),

         /* Match MISRA rule */
         [63] = 0};

/* API - initialize 'mflash' */
void mflash_drv_init(void)
{
    uint32_t clockSourceFreq = 0;
    qspi_config_t config = {0};
    /*Get QSPI default settings and configure the qspi */
    QSPI_GetDefaultQspiConfig(&config);

    /*Set AHB buffer size for reading data through AHB bus */
    config.AHBbufferSize[3] = MFLASH_PAGE_SIZE;
    clockSourceFreq = QSPI_CLK_FREQ;
    QSPI_Init(EXAMPLE_QSPI, &config, clockSourceFreq);

    /* Copy the LUT table */
    memcpy(single_config.lookuptable, lut, sizeof(uint32_t) * FSL_FEATURE_QSPI_LUT_DEPTH);

    /*According to serial flash feature to configure flash settings */
    QSPI_SetFlashConfig(EXAMPLE_QSPI, &single_config);
}

/* Check if serial flash erase or program finished. */
void check_if_finished(void)
{
    uint32_t val = 0;
    /* Check WIP bit */
    do
    {
        while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
        {
        }
        QSPI_ClearFifo(EXAMPLE_QSPI, kQSPI_RxFifo);
        QSPI_ExecuteIPCommand(EXAMPLE_QSPI, 12U);
        while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
        {
        }
        val = *(volatile uint32_t *)(FSL_FEATURE_QSPI_ARDB_BASE);
        /* Clear ARDB area */
        QSPI_ClearErrorFlag(EXAMPLE_QSPI, kQSPI_RxBufferDrain);
    } while (val & 0x1);
}

/* Write enable command */
void cmd_write_enable(void)
{
    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ExecuteIPCommand(EXAMPLE_QSPI, 4U);
}

#if defined(FLASH_ENABLE_QUAD_CMD)
/* Enable Quad mode */
void enable_quad_mode(void)
{
    uint32_t val[4] = {FLASH_ENABLE_QUAD_CMD, 0, 0, 0};

    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_SetIPCommandAddress(EXAMPLE_QSPI, FSL_FEATURE_QSPI_AMBA_BASE);

    /* Clear Tx FIFO */
    QSPI_ClearFifo(EXAMPLE_QSPI, kQSPI_TxFifo);

    /* Write enable */
    cmd_write_enable();

    /* Write data into TX FIFO, needs to write at least 16 bytes of data */
    QSPI_WriteBlocking(EXAMPLE_QSPI, val, 16U);

    /* Set seq id, write register */
    QSPI_ExecuteIPCommand(EXAMPLE_QSPI, 20);

    /* Wait until finished */
    check_if_finished();
}
#endif

/* Internal - erase single sector */
static void mflash_drv_sector_erase(uint32_t sector_addr)
{
    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ClearFifo(EXAMPLE_QSPI, kQSPI_TxFifo);
    QSPI_SetIPCommandAddress(EXAMPLE_QSPI, sector_addr);
    cmd_write_enable();
    QSPI_ExecuteIPCommand(EXAMPLE_QSPI, 28U);
    check_if_finished();
}

/* Program page into serial flash using QSPI polling way */
void program_page(uint32_t dest_addr, uint32_t *src_addr)
{
    uint32_t leftLongWords = 0;

    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
    {
    }
    QSPI_ClearFifo(EXAMPLE_QSPI, kQSPI_TxFifo);
    QSPI_SetIPCommandAddress(EXAMPLE_QSPI, dest_addr);
    cmd_write_enable();
    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & kQSPI_Busy)
    {
    }

    /* First write some data into TXFIFO to prevent from underrun */
    QSPI_WriteBlocking(EXAMPLE_QSPI, src_addr, FSL_FEATURE_QSPI_TXFIFO_DEPTH * 4);
    src_addr += FSL_FEATURE_QSPI_TXFIFO_DEPTH;

    /* Start the program */
    QSPI_SetIPCommandSize(EXAMPLE_QSPI, MFLASH_PAGE_SIZE);
    QSPI_ExecuteIPCommand(EXAMPLE_QSPI, 16U);

    leftLongWords = MFLASH_PAGE_SIZE - 16 * sizeof(uint32_t);
    QSPI_WriteBlocking(EXAMPLE_QSPI, src_addr, leftLongWords);

    /* Wait until flash finished program */
    check_if_finished();
    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & (kQSPI_Busy | kQSPI_IPAccess))
    {
    }
    QSPI_SoftwareReset(EXAMPLE_QSPI);
    while (QSPI_GetStatusFlags(EXAMPLE_QSPI) & (kQSPI_Busy | kQSPI_IPAccess))
    {
    }
}

/* Internal - write single sector */
static int32_t mflash_drv_sector_program(uint32_t sector_addr, uint32_t *sector_data, uint32_t sector_data_length)
{
    int32_t result = 0;
    for (int32_t i = 0; i < MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE; i++)
    {
        program_page(sector_addr + i * MFLASH_PAGE_SIZE, sector_data + i * MFLASH_PAGE_SIZE/sizeof(uint32_t));
    }
    return result;
}

/* Internal - write data of 'data_len' to single sector 'sector_addr', starting from 'sect_off' */
int32_t mflash_drv_sector_write(uint32_t sector_addr, uint32_t sect_off, uint8_t *data, uint32_t data_len)
{
    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;
    /* Offset + length exceeed sector size */
    if (sect_off + data_len > MFLASH_SECTOR_SIZE)
        return -1;

    /* Copy old sector data by 4B in each loop to buffer */
    for (uint32_t i = 0; i < sizeof(g_flashm_sector) / sizeof(g_flashm_sector[0]); i++)
    {
        g_flashm_sector[i] = *((uint32_t *)(sector_addr) + i);
    }

    /* Copy custom data ( 1B in each loop ) to buffer at specific position */
    for (uint32_t i = 0; i < data_len; i++)
    {
        ((uint8_t *)g_flashm_sector)[sect_off + i] = data[i];
    }

    /* Erase flash */
    mflash_drv_sector_erase(sector_addr);
    
    /* Program data */
    if (0 != mflash_drv_sector_program(sector_addr, g_flashm_sector, sizeof(g_flashm_sector)))
        return -2;

    return 0;
}

/* Write data to flash, cannot be invoked directly, requires calling wrapper in non XIP memory */
int32_t mflash_drv_write_internal(void *any_addr, uint8_t *data, uint32_t data_len)
{
    uint32_t sect_size = MFLASH_SECTOR_SIZE;
    /* Interval <0, sector_size) */
    uint32_t to_write = 0;
    /* Interval (data_len, 0>  */
    uint32_t to_remain = data_len;
    int32_t result = 0;

    for (
        /* Calculate address of first sector */
        uint32_t sect_a = mflash_drv_addr_to_sector_addr((uint32_t)any_addr),
                 /* and first sector offset */
        sect_of = mflash_drv_addr_to_sector_of((uint32_t)any_addr),
                 /* and set first data offset to 0*/
        data_of = 0;
        /* Continue until sector address exceed target adddress + data_length */
        sect_a < ((uint32_t)any_addr) + data_len;
        /* Move to next sector */
        sect_a += sect_size,
                 /* and move pointer to data */
        data_of += to_write)
    {
        /* If remaining data is exceed 'sector_size', write 'sector_size' length */
        if (to_remain > sect_size - sect_of)
        {
            to_write = sect_size - sect_of;
            to_remain = to_remain - to_write;
        }
        /* else write remaining data length */
        else
        {
            to_write = to_remain;
            to_remain = 0;
        }

        /* Write at 'sect_a' sector, starting at 'sect_of' using '&data[data_of]' of length 'to_write' */
        result = mflash_drv_sector_write(sect_a, sect_of, &data[data_of], to_write);
        if (0 != result)
            return -1;
        /* Only first sector is allowed to have an offset */
        sect_of = 0;
    }

    return 0;
}

/* Calling wrapper for 'mflash_drv_write_internal'.
 * Write 'data' of 'data_len' to 'any_addr' - which doesn't have to be sector aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_write(void *any_addr, uint8_t *data, uint32_t data_len)
{
    volatile int32_t result;
    result = mflash_drv_write_internal(any_addr, data, data_len);
    return result;
}
