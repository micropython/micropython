/*
 * @brief LPC18xx/43xx SD/SDIO driver
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

#include "chip.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Global instance of the current card */
static mci_card_struct *g_card_info;

/* Helper definition: all SD error conditions in the status word */
#define SD_INT_ERROR (MCI_INT_RESP_ERR | MCI_INT_RCRC | MCI_INT_DCRC | \
					  MCI_INT_RTO | MCI_INT_DTO | MCI_INT_HTO | MCI_INT_FRUN | MCI_INT_HLE | \
					  MCI_INT_SBE | MCI_INT_EBE)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Function to execute a command */
static int32_t sdmmc_execute_command(LPC_SDMMC_T *pSDMMC, uint32_t cmd, uint32_t arg, uint32_t wait_status)
{
	int32_t step = (cmd & CMD_BIT_APP) ? 2 : 1;
	int32_t status = 0;
	uint32_t cmd_reg = 0;

	if (!wait_status) {
		wait_status = (cmd & CMD_MASK_RESP) ? MCI_INT_CMD_DONE : MCI_INT_DATA_OVER;
	}

	/* Clear the interrupts & FIFOs*/
	if (cmd & CMD_BIT_DATA) {
		Chip_SDIF_SetClearIntFifo(pSDMMC);
	}

	/* also check error conditions */
	wait_status |= MCI_INT_EBE | MCI_INT_SBE | MCI_INT_HLE | MCI_INT_RTO | MCI_INT_RCRC | MCI_INT_RESP_ERR;
	if (wait_status & MCI_INT_DATA_OVER) {
		wait_status |= MCI_INT_FRUN | MCI_INT_HTO | MCI_INT_DTO | MCI_INT_DCRC;
	}

	while (step) {
		Chip_SDIF_SetClock(pSDMMC, Chip_Clock_GetBaseClocktHz(CLK_BASE_SDIO), g_card_info->card_info.speed);

		/* Clear the interrupts */
		Chip_SDIF_ClrIntStatus(pSDMMC, 0xFFFFFFFF);

		g_card_info->card_info.evsetup_cb((void *) &wait_status);

		switch (step) {
		case 1:	/* Execute command */
			cmd_reg = ((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD) |
					  ((cmd & CMD_BIT_INIT)  ? MCI_CMD_INIT : 0) |
					  ((cmd & CMD_BIT_DATA)  ? (MCI_CMD_DAT_EXP | MCI_CMD_PRV_DAT_WAIT) : 0) |
					  (((cmd & CMD_MASK_RESP) == CMD_RESP_R2) ? MCI_CMD_RESP_LONG : 0) |
					  ((cmd & CMD_MASK_RESP) ? MCI_CMD_RESP_EXP : 0) |
					  ((cmd & CMD_BIT_WRITE)  ? MCI_CMD_DAT_WR : 0) |
					  ((cmd & CMD_BIT_STREAM) ? MCI_CMD_STRM_MODE : 0) |
					  ((cmd & CMD_BIT_BUSY) ? MCI_CMD_STOP : 0) |
					  ((cmd & CMD_BIT_AUTO_STOP)  ? MCI_CMD_SEND_STOP : 0) |
					  MCI_CMD_START;

			/* wait for previos data finsh for select/deselect commands */
			if (((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD) == MMC_SELECT_CARD) {
				cmd_reg |= MCI_CMD_PRV_DAT_WAIT;
			}

			/* wait for command to be accepted by CIU */
			if (Chip_SDIF_SendCmd(pSDMMC, cmd_reg, arg) == 0) {
				--step;
			}
			break;

		case 0:
			return 0;

		case 2:	/* APP prefix */
			cmd_reg = MMC_APP_CMD | MCI_CMD_RESP_EXP |
					  ((cmd & CMD_BIT_INIT)  ? MCI_CMD_INIT : 0) |
					  MCI_CMD_START;

			if (Chip_SDIF_SendCmd(pSDMMC, cmd_reg, g_card_info->card_info.rca << 16) == 0) {
				--step;
			}
			break;
		}

		/* wait for command response */
		status = g_card_info->card_info.waitfunc_cb();

		/* We return an error if there is a timeout, even if we've fetched  a response */
		if (status & SD_INT_ERROR) {
			return status;
		}

		if (status & MCI_INT_CMD_DONE) {
			switch (cmd & CMD_MASK_RESP) {
			case 0:
				break;

			case CMD_RESP_R1:
			case CMD_RESP_R3:
			case CMD_RESP_R2:
				Chip_SDIF_GetResponse(pSDMMC, &g_card_info->card_info.response[0]);
				break;
			}
		}
	}

	return 0;
}

/* Checks whether card is acquired properly or not */
static int32_t prv_card_acquired(void)
{
	return g_card_info->card_info.cid[0] != 0;
}

/* Helper function to get a bit field withing multi-word  buffer. Used to get
   fields with-in CSD & EXT-CSD */
static uint32_t prv_get_bits(int32_t start, int32_t end, uint32_t *data)
{
	uint32_t v;
	uint32_t i = end >> 5;
	uint32_t j = start & 0x1f;

	if (i == (start >> 5)) {
		v = (data[i] >> j);
	}
	else {
		v = ((data[i] << (32 - j)) | (data[start >> 5] >> j));
	}

	return v & ((1 << (end - start + 1)) - 1);
}

/* Function to process the CSD & EXT-CSD of the card */
static void prv_process_csd(LPC_SDMMC_T *pSDMMC)
{
	int32_t status = 0;
	int32_t c_size = 0;
	int32_t c_size_mult = 0;
	int32_t mult = 0;

	/* compute block length based on CSD response */
	g_card_info->card_info.block_len = 1 << prv_get_bits(80, 83, g_card_info->card_info.csd);

	if ((g_card_info->card_info.card_type & CARD_TYPE_HC) && (g_card_info->card_info.card_type & CARD_TYPE_SD)) {
		/* See section 5.3.3 CSD Register (CSD Version 2.0) of SD2.0 spec  an explanation for the calculation of these values */
		c_size = prv_get_bits(48, 69, (uint32_t *) g_card_info->card_info.csd) + 1;
		g_card_info->card_info.blocknr = c_size << 10;	/* 512 byte blocks */
	}
	else {
		/* See section 5.3 of the 4.1 revision of the MMC specs for  an explanation for the calculation of these values */
		c_size = prv_get_bits(62, 73, (uint32_t *) g_card_info->card_info.csd);
		c_size_mult = prv_get_bits(47, 49, (uint32_t *) g_card_info->card_info.csd);
		mult = 1 << (c_size_mult + 2);
		g_card_info->card_info.blocknr = (c_size + 1) * mult;

		/* adjust blocknr to 512/block */
		if (g_card_info->card_info.block_len > MMC_SECTOR_SIZE) {
			g_card_info->card_info.blocknr = g_card_info->card_info.blocknr * (g_card_info->card_info.block_len >> 9);
		}

		/* get extended CSD for newer MMC cards CSD spec >= 4.0*/
		if (((g_card_info->card_info.card_type & CARD_TYPE_SD) == 0) &&
			(prv_get_bits(122, 125, (uint32_t *) g_card_info->card_info.csd) >= 4)) {
			/* put card in trans state */
			status = sdmmc_execute_command(pSDMMC, CMD_SELECT_CARD, g_card_info->card_info.rca << 16, 0);

			/* set block size and byte count */
			Chip_SDIF_SetBlkSizeByteCnt(pSDMMC, MMC_SECTOR_SIZE);

			/* send EXT_CSD command */
			Chip_SDIF_DmaSetup(pSDMMC,
							  &g_card_info->sdif_dev,
							  (uint32_t) g_card_info->card_info.ext_csd,
							  MMC_SECTOR_SIZE);

			status = sdmmc_execute_command(pSDMMC, CMD_SEND_EXT_CSD, 0, 0 | MCI_INT_DATA_OVER);
			if ((status & SD_INT_ERROR) == 0) {
				/* check EXT_CSD_VER is greater than 1.1 */
				if ((g_card_info->card_info.ext_csd[48] & 0xFF) > 1) {
					g_card_info->card_info.blocknr = g_card_info->card_info.ext_csd[53];/* bytes 212:215 represent sec count */

				}
				/* switch to 52MHz clock if card type is set to 1 or else set to 26MHz */
				if ((g_card_info->card_info.ext_csd[49] & 0xFF) == 1) {
					/* for type 1 MMC cards high speed is 52MHz */
					g_card_info->card_info.speed = MMC_HIGH_BUS_MAX_CLOCK;
				}
				else {
					/* for type 0 MMC cards high speed is 26MHz */
					g_card_info->card_info.speed = MMC_LOW_BUS_MAX_CLOCK;
				}
			}
		}
	}

	g_card_info->card_info.device_size = (uint64_t) g_card_info->card_info.blocknr << 9;	/* blocknr * 512 */
}

/* Puts current selected card in trans state */
static int32_t prv_set_trans_state(LPC_SDMMC_T *pSDMMC)
{
	uint32_t status;

	/* get current state of the card */
	status = sdmmc_execute_command(pSDMMC, CMD_SEND_STATUS, g_card_info->card_info.rca << 16, 0);
	if (status & MCI_INT_RTO) {
		/* unable to get the card state. So return immediatly. */
		return -1;
	}

	/* check card state in response */
	status = R1_CURRENT_STATE(g_card_info->card_info.response[0]);
	switch (status) {
	case SDMMC_STBY_ST:
		/* put card in 'Trans' state */
		status = sdmmc_execute_command(pSDMMC, CMD_SELECT_CARD, g_card_info->card_info.rca << 16, 0);
		if (status != 0) {
			/* unable to put the card in Trans state. So return immediatly. */
			return -1;
		}
		break;

	case SDMMC_TRAN_ST:
		/*do nothing */
		break;

	default:
		/* card shouldn't be in other states so return */
		return -1;
	}

	return 0;
}

/* Sets card data width and block size */
static int32_t prv_set_card_params(LPC_SDMMC_T *pSDMMC)
{
	int32_t status;

#if SDIO_BUS_WIDTH > 1
	if (g_card_info->card_info.card_type & CARD_TYPE_SD) {
		status = sdmmc_execute_command(pSDMMC, CMD_SD_SET_WIDTH, 2, 0);
		if (status != 0) {
			return -1;
		}

		/* if positive response */
		Chip_SDIF_SetCardType(pSDMMC, MCI_CTYPE_4BIT);
	}
#elif SDIO_BUS_WIDTH > 4
#error 8-bit mode not supported yet!
#endif

	/* set block length */
	Chip_SDIF_SetBlkSize(pSDMMC, MMC_SECTOR_SIZE);
	status = sdmmc_execute_command(pSDMMC, CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0);
	if (status != 0) {
		return -1;
	}

	return 0;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Get card's current state (idle, transfer, program, etc.) */
int32_t Chip_SDMMC_GetState(LPC_SDMMC_T *pSDMMC)
{
	uint32_t status;

	/* get current state of the card */
	status = sdmmc_execute_command(pSDMMC, CMD_SEND_STATUS, g_card_info->card_info.rca << 16, 0);
	if (status & MCI_INT_RTO) {
		return -1;
	}

	/* check card state in response */
	return (int32_t) R1_CURRENT_STATE(g_card_info->card_info.response[0]);
}

/* Function to enumerate the SD/MMC/SDHC/MMC+ cards */
uint32_t Chip_SDMMC_Acquire(LPC_SDMMC_T *pSDMMC, mci_card_struct *pcardinfo)
{
	int32_t status;
	int32_t tries = 0;
	uint32_t ocr = OCR_VOLTAGE_RANGE_MSK;
	uint32_t r;
	int32_t state = 0;
	uint32_t command = 0;

	g_card_info = pcardinfo;

	/* clear card type */
	Chip_SDIF_SetCardType(pSDMMC, 0);

	/* set high speed for the card as 20MHz */
	g_card_info->card_info.speed = MMC_MAX_CLOCK;

	status = sdmmc_execute_command(pSDMMC, CMD_IDLE, 0, MCI_INT_CMD_DONE);

	while (state < 100) {
		switch (state) {
		case 0:	/* Setup for SD */
			/* check if it is SDHC card */
			status = sdmmc_execute_command(pSDMMC, CMD_SD_SEND_IF_COND, SD_SEND_IF_ARG, 0);
			if (!(status & MCI_INT_RTO)) {
				/* check response has same echo pattern */
				if ((g_card_info->card_info.response[0] & SD_SEND_IF_ECHO_MSK) == SD_SEND_IF_RESP) {
					ocr |= OCR_HC_CCS;
				}
			}

			++state;
			command = CMD_SD_OP_COND;
			tries = INIT_OP_RETRIES;

			/* assume SD card */
			g_card_info->card_info.card_type |= CARD_TYPE_SD;
			g_card_info->card_info.speed = SD_MAX_CLOCK;
			break;

		case 10:	/* Setup for MMC */
			/* start fresh for MMC crds */
			g_card_info->card_info.card_type &= ~CARD_TYPE_SD;
			status = sdmmc_execute_command(pSDMMC, CMD_IDLE, 0, MCI_INT_CMD_DONE);
			command = CMD_MMC_OP_COND;
			tries = INIT_OP_RETRIES;
			ocr |= OCR_HC_CCS;
			++state;

			/* for MMC cards high speed is 20MHz */
			g_card_info->card_info.speed = MMC_MAX_CLOCK;
			break;

		case 1:
		case 11:
			status = sdmmc_execute_command(pSDMMC, command, 0, 0);
			if (status & MCI_INT_RTO) {
				state += 9;	/* Mode unavailable */
			}
			else {
				++state;
			}
			break;

		case 2:		/* Initial OCR check  */
		case 12:
			ocr = g_card_info->card_info.response[0] | (ocr & OCR_HC_CCS);
			if (ocr & OCR_ALL_READY) {
				++state;
			}
			else {
				state += 2;
			}
			break;

		case 3:		/* Initial wait for OCR clear */
		case 13:
			while ((ocr & OCR_ALL_READY) && --tries > 0) {
				g_card_info->card_info.msdelay_func(MS_ACQUIRE_DELAY);
				status = sdmmc_execute_command(pSDMMC, command, 0, 0);
				ocr = g_card_info->card_info.response[0] | (ocr & OCR_HC_CCS);
			}
			if (ocr & OCR_ALL_READY) {
				state += 7;
			}
			else {
				++state;
			}
			break;

		case 14:
			/* for MMC cards set high capacity bit */
			ocr |= OCR_HC_CCS;

		case 4:	/* Assign OCR */
			tries = SET_OP_RETRIES;
			ocr &= OCR_VOLTAGE_RANGE_MSK | OCR_HC_CCS;	/* Mask for the bits we care about */
			do {
				g_card_info->card_info.msdelay_func(MS_ACQUIRE_DELAY);
				status = sdmmc_execute_command(pSDMMC, command, ocr, 0);
				r = g_card_info->card_info.response[0];
			} while (!(r & OCR_ALL_READY) && --tries > 0);

			if (r & OCR_ALL_READY) {
				/* is it high capacity card */
				g_card_info->card_info.card_type |= (r & OCR_HC_CCS);
				++state;
			}
			else {
				state += 6;
			}
			break;

		case 5:	/* CID polling */
		case 15:
			status = sdmmc_execute_command(pSDMMC, CMD_ALL_SEND_CID, 0, 0);
			memcpy(&g_card_info->card_info.cid, &g_card_info->card_info.response[0], 16);
			++state;
			break;

		case 6:	/* RCA send, for SD get RCA */
			status = sdmmc_execute_command(pSDMMC, CMD_SD_SEND_RCA, 0, 0);
			g_card_info->card_info.rca = (g_card_info->card_info.response[0]) >> 16;
			++state;
			break;

		case 16:	/* RCA assignment for MMC set to 1 */
			g_card_info->card_info.rca = 1;
			status = sdmmc_execute_command(pSDMMC, CMD_MMC_SET_RCA, g_card_info->card_info.rca << 16, 0);
			++state;
			break;

		case 7:
		case 17:
			status = sdmmc_execute_command(pSDMMC, CMD_SEND_CSD, g_card_info->card_info.rca << 16, 0);
			memcpy(&g_card_info->card_info.csd, &g_card_info->card_info.response[0], 16);
			state = 100;
			break;

		default:
			state += 100;	/* break from while loop */
			break;
		}
	}

	/* Compute card size, block size and no. of blocks  based on CSD response recived. */
	if (prv_card_acquired()) {
		prv_process_csd(pSDMMC);

		/* Setup card data width and block size (once) */
		if (prv_set_trans_state(pSDMMC) != 0) {
			return 0;
		}
		if (prv_set_card_params(pSDMMC) != 0) {
			return 0;
		}
	}

	return prv_card_acquired();
}

/* Get the device size of SD/MMC card (after enumeration) */
uint64_t Chip_SDMMC_GetDeviceSize(LPC_SDMMC_T *pSDMMC)
{
	return g_card_info->card_info.device_size;
}

/* Get the number of blocks in SD/MMC card (after enumeration) */
int32_t Chip_SDMMC_GetDeviceBlocks(LPC_SDMMC_T *pSDMMC)
{
	return g_card_info->card_info.blocknr;
}

/* Performs the read of data from the SD/MMC card */
int32_t Chip_SDMMC_ReadBlocks(LPC_SDMMC_T *pSDMMC, void *buffer, int32_t start_block, int32_t num_blocks)
{
	int32_t cbRead = (num_blocks) * MMC_SECTOR_SIZE;
	int32_t status = 0;
	int32_t index;

	/* if card is not acquired return immediately */
	if (( start_block < 0) || ( (start_block + num_blocks) > g_card_info->card_info.blocknr) ) {
		return 0;
	}

	/* put card in trans state */
	if (prv_set_trans_state(pSDMMC) != 0) {
		return 0;
	}

	/* set number of bytes to read */
	Chip_SDIF_SetByteCnt(pSDMMC, cbRead);

	/* if high capacity card use block indexing */
	if (g_card_info->card_info.card_type & CARD_TYPE_HC) {
		index = start_block;
	}
	else {	/*fix at 512 bytes*/
		index = start_block << 9;	// \* g_card_info->card_info.block_len;

	}
	Chip_SDIF_DmaSetup(pSDMMC, &g_card_info->sdif_dev, (uint32_t) buffer, cbRead);

	/* Select single or multiple read based on number of blocks */
	if (num_blocks == 1) {
		status = sdmmc_execute_command(pSDMMC, CMD_READ_SINGLE, index, 0 | MCI_INT_DATA_OVER);
	}
	else {
		status = sdmmc_execute_command(pSDMMC, CMD_READ_MULTIPLE, index, 0 | MCI_INT_DATA_OVER);
	}

	if (status != 0) {
		cbRead = 0;
	}
	/*Wait for card program to finish*/
	while (Chip_SDMMC_GetState(pSDMMC) != SDMMC_TRAN_ST) {}

	return cbRead;
}

/* Performs write of data to the SD/MMC card */
int32_t Chip_SDMMC_WriteBlocks(LPC_SDMMC_T *pSDMMC, void *buffer, int32_t start_block, int32_t num_blocks)
{
	int32_t cbWrote = num_blocks *  MMC_SECTOR_SIZE;
	int32_t status;
	int32_t index;

	/* if card is not acquired return immediately */
	if (( start_block < 0) || ( (start_block + num_blocks) > g_card_info->card_info.blocknr) ) {
		return 0;
	}

	/*Wait for card program to finish*/
	while (Chip_SDMMC_GetState(pSDMMC) != SDMMC_TRAN_ST) {}

	/* put card in trans state */
	if (prv_set_trans_state(pSDMMC) != 0) {
		return 0;
	}

	/* set number of bytes to write */
	Chip_SDIF_SetByteCnt(pSDMMC, cbWrote);

	/* if high capacity card use block indexing */
	if (g_card_info->card_info.card_type & CARD_TYPE_HC) {
		index = start_block;
	}
	else {	/*fix at 512 bytes*/
		index = start_block << 9;	// * g_card_info->card_info.block_len;

	}

	Chip_SDIF_DmaSetup(pSDMMC, &g_card_info->sdif_dev, (uint32_t) buffer, cbWrote);

	/* Select single or multiple write based on number of blocks */
	if (num_blocks == 1) {
		status = sdmmc_execute_command(pSDMMC, CMD_WRITE_SINGLE, index, 0 | MCI_INT_DATA_OVER);
	}
	else {
		status = sdmmc_execute_command(pSDMMC, CMD_WRITE_MULTIPLE, index, 0 | MCI_INT_DATA_OVER);
	}

	/*Wait for card program to finish*/
	while (Chip_SDMMC_GetState(pSDMMC) != SDMMC_TRAN_ST) {}

	if (status != 0) {
		cbWrote = 0;
	}

	return cbWrote;
}






