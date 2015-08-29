/*
 * @brief LPC18xx/43xx SD/MMC card driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __SDMMC_18XX_43XX_H_
#define __SDMMC_18XX_43XX_H_

#include "sdmmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SDMMC_18XX_43XX CHIP: LPC18xx/43xx SD/MMC driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#define CMD_MASK_RESP       (0x3UL << 28)
#define CMD_RESP(r)         (((r) & 0x3) << 28)
#define CMD_RESP_R0         (0 << 28)
#define CMD_RESP_R1         (1 << 28)
#define CMD_RESP_R2         (2 << 28)
#define CMD_RESP_R3         (3 << 28)
#define CMD_BIT_AUTO_STOP   (1 << 24)
#define CMD_BIT_APP         (1 << 23)
#define CMD_BIT_INIT        (1 << 22)
#define CMD_BIT_BUSY        (1 << 21)
#define CMD_BIT_LS          (1 << 20)	/* Low speed, used during acquire */
#define CMD_BIT_DATA        (1 << 19)
#define CMD_BIT_WRITE       (1 << 18)
#define CMD_BIT_STREAM      (1 << 17)
#define CMD_MASK_CMD        (0xff)
#define CMD_SHIFT_CMD       (0)

#define CMD(c, r)        ( ((c) &  CMD_MASK_CMD) | CMD_RESP((r)) )

#define CMD_IDLE            CMD(MMC_GO_IDLE_STATE, 0) | CMD_BIT_LS    | CMD_BIT_INIT
#define CMD_SD_OP_COND      CMD(SD_APP_OP_COND, 1)      | CMD_BIT_LS | CMD_BIT_APP
#define CMD_SD_SEND_IF_COND CMD(SD_CMD8, 1)      | CMD_BIT_LS
#define CMD_MMC_OP_COND     CMD(MMC_SEND_OP_COND, 3)    | CMD_BIT_LS | CMD_BIT_INIT
#define CMD_ALL_SEND_CID    CMD(MMC_ALL_SEND_CID, 2)    | CMD_BIT_LS
#define CMD_MMC_SET_RCA     CMD(MMC_SET_RELATIVE_ADDR, 1) | CMD_BIT_LS
#define CMD_SD_SEND_RCA     CMD(SD_SEND_RELATIVE_ADDR, 1) | CMD_BIT_LS
#define CMD_SEND_CSD        CMD(MMC_SEND_CSD, 2) | CMD_BIT_LS
#define CMD_SEND_EXT_CSD    CMD(MMC_SEND_EXT_CSD, 1) | CMD_BIT_LS | CMD_BIT_DATA
#define CMD_DESELECT_CARD   CMD(MMC_SELECT_CARD, 0)
#define CMD_SELECT_CARD     CMD(MMC_SELECT_CARD, 1)
#define CMD_SET_BLOCKLEN    CMD(MMC_SET_BLOCKLEN, 1)
#define CMD_SEND_STATUS     CMD(MMC_SEND_STATUS, 1)
#define CMD_READ_SINGLE     CMD(MMC_READ_SINGLE_BLOCK, 1) | CMD_BIT_DATA
#define CMD_READ_MULTIPLE   CMD(MMC_READ_MULTIPLE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_AUTO_STOP
#define CMD_SD_SET_WIDTH    CMD(SD_APP_SET_BUS_WIDTH, 1) | CMD_BIT_APP
#define CMD_STOP            CMD(MMC_STOP_TRANSMISSION, 1) | CMD_BIT_BUSY
#define CMD_WRITE_SINGLE    CMD(MMC_WRITE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_WRITE
#define CMD_WRITE_MULTIPLE  CMD(MMC_WRITE_MULTIPLE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_WRITE | CMD_BIT_AUTO_STOP

/* Card specific setup data */
typedef struct _mci_card_struct {
	sdif_device sdif_dev;
	SDMMC_CARD_T card_info;
} mci_card_struct;

/**
 * @brief	Get card's current state (idle, transfer, program, etc.)
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Current SD card transfer state
 */
int32_t Chip_SDMMC_GetState(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Function to enumerate the SD/MMC/SDHC/MMC+ cards
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	pcardinfo	: Pointer to pre-allocated card info structure
 * @return	1 if a card is acquired, otherwise 0
 */
uint32_t Chip_SDMMC_Acquire(LPC_SDMMC_T *pSDMMC, mci_card_struct *pcardinfo);

/**
 * @brief	Get the device size of SD/MMC card (after enumeration)
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Card size in number of bytes (capacity)
 */
uint64_t Chip_SDMMC_GetDeviceSize(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Get the number of device blocks of SD/MMC card (after enumeration)
 * Since Chip_SDMMC_GetDeviceSize is limited to 32 bits cards with greater than
 * 2 GBytes of data will not be correct, in such cases users can use this function
 * to get the size of the card in blocks.
 * @param	pSDMMC	: SDMMC peripheral selected
 * @return	Number of 512 bytes blocks in the card
 */
int32_t Chip_SDMMC_GetDeviceBlocks(LPC_SDMMC_T *pSDMMC);

/**
 * @brief	Performs the read of data from the SD/MMC card
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	buffer		: Pointer to data buffer to copy to
 * @param	start_block	: Start block number
 * @param	num_blocks	: Number of block to read
 * @return	Bytes read, or 0 on error
 */
int32_t Chip_SDMMC_ReadBlocks(LPC_SDMMC_T *pSDMMC, void *buffer, int32_t start_block, int32_t num_blocks);

/**
 * @brief	Performs write of data to the SD/MMC card
 * @param	pSDMMC		: SDMMC peripheral selected
 * @param	buffer		: Pointer to data buffer to copy to
 * @param	start_block	: Start block number
 * @param	num_blocks	: Number of block to write
 * @return	Number of bytes actually written, or 0 on error
 */
int32_t Chip_SDMMC_WriteBlocks(LPC_SDMMC_T *pSDMMC, void *buffer, int32_t start_block, int32_t num_blocks);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SDMMC_18XX_43XX_H_ */
