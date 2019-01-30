/*
 * (C) Copyright 2015-2016 - Analog Devices, Inc.  All rights reserved.
 *
 * FILE:     rsi.h
 *
 * CHANGES:  1.00.0  - initial release
 */

#ifndef __RSI_H__
#define __RSI_H__

#include <sys/platform.h>
#include "sd_card_registers.h"
#include "mmc_card_registers.h"
#include <drivers/rsi/adi_rsi.h>

/*********************************************************************

Macros specific to SD/SDIO/Multimedia card configuration

*********************************************************************/

/* Vdd operating range for ADSP-BF707 EZ-Board */
#define     SD_MMC_VDD_OPERATING_RANGE              0x00FF8000      /* range - 2.7 to 3.6 volts (used as default) */
#define     SD_MMC_SECTOR_MODE_SUPPORT              0x40000000
#define     SD_MMC_BYTE_MODE_SUPPORT                0x00000000
#define     CLOCK_IN                                25000000
#define     MMC_RCA                                 0x5A7E

#include "sd_mmc_commands.h"

typedef enum
{
    SD_MMC_CARD_TYPE_SD_V1X,
    SD_MMC_CARD_TYPE_SD_V2X,
    SD_MMC_CARD_TYPE_SD_V2X_HIGH_CAPACITY,
    SD_MMC_CARD_TYPE_MMC,
    SD_MMC_CARD_TYPE_MMC_CARD_HIGH_CAPACITY,
    UNUSABLE_CARD,
    CARD_SLOT_NOT_INITIALIZED
} SD_MMC_CARD_TYPE;

typedef enum
{
    MMC_SPEED_GRADE_20MHZ = 20000000,
    MMC_SPEED_GRADE_26MHZ = 26000000,
    MMC_SPEED_GRADE_52MHZ = 52000000,
    SD_SPEED_GRADE_CLASS0 = 25000000,
    SD_SPEED_GRADE_CLASS2 = 30000000,
    SD_SPEED_GRADE_CLASS4 = 40000000,
    SD_SPEED_GRADE_CLASS6 = 50000000,
    SD_MMC_SPEED_GRADE_ERROR
} SD_MMC_SPEED_GRADE;

typedef enum
{
    RSI_DATA_BUS_WIDTH_ERROR = 0,
    RSI_DATA_BUS_WIDTH_1 = 1,
    RSI_DATA_BUS_WIDTH_4 = 4,
    RSI_DATA_BUS_WIDTH_8 = 8
} RSI_DATA_BUS_WIDTH;

extern ADI_RSI_HANDLE hDevice;

void RSI_PortSetup(void);
void RSI_WaitForCard(void);
#if defined(__ADSPSC573__)
void RSI_WaitForCardRemoved(void);
#endif
void RSI_InitIrqHandler(void);
SD_MMC_CARD_TYPE GetCardType(void);
RSI_DATA_BUS_WIDTH GetMaxBusWidth(void);
SD_MMC_SPEED_GRADE GetMMCTransferSpeed(void);
SD_MMC_SPEED_GRADE GetMMCHighTransferSpeed(void);
void MMCSetOptimumSpeed();
void RSI_GetClocks(void);

SD_MMC_CARD_TYPE sd_mmc_identification(void);
uint32_t sd_mmc_read_block_dma(uint32_t card_address, void * pDestination);
uint32_t sd_mmc_read_block_core(uint32_t card_address, void * pDestination);
uint32_t sd_mmc_read_mblocks_dma(uint32_t card_address, void * pDestination, uint32_t num_blocks);
uint32_t sd_mmc_read_mblocks_core(uint32_t card_address, void * pDestination, uint32_t num_blocks);
uint32_t sd_mmc_write_block_dma(uint32_t card_address, void * pDestination);
uint32_t sd_mmc_write_block_core(uint32_t card_address, void * pDestination);
uint32_t sd_mmc_write_mblocks_dma(uint32_t card_address, void * pDestination, uint32_t num_blocks);
uint32_t sd_mmc_write_mblocks_core(uint32_t card_address, void * pDestination, uint32_t num_blocks);
void mmc_disable_open_drain_mode(void);
uint32_t mmc_set_high_speed(void);
RSI_DATA_BUS_WIDTH mmc_bus_test(void);
uint32_t sd_mmc_select(void);
uint32_t sd_mmc_stop_transmission(void);

void get_clocks(void);
uint32_t sd_mmc_set_bus_width(unsigned short width);

uint32_t RSI_ReadBlock(uint32_t, void *);
uint32_t RSI_WriteBlock(uint32_t , void *);
uint32_t RSI_ReadMultipleBlocks(uint32_t, void *, uint32_t);
uint32_t RSI_WriteMultipleBlocks(uint32_t , void *, uint32_t);
uint64_t RSI_GetSDCapacity();

#endif /* __RSI_H__ */
