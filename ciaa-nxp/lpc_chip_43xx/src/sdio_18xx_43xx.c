/*
 * @brief LPC18xx/43xx SDIO Card driver
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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SDIO_CMD_INT_MSK    0xA146       /* Interrupts to be enabled for CMD */
#define SDIO_DATA_INT_MSK   0xBE88       /* Interrupts to enable for data transfer */
#define SDIO_CARD_INT_MSK   (1UL << 16)  /* SDIO Card interrupt */

static struct
{
	void (*wake_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg);
	uint32_t (*wait_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg);
	uint32_t flag;
	uint32_t response[4];
	int fnum;
	uint16_t blkSz[8];     /* Block size setting for the 8- function blocks */
	sdif_device sdev;      /* SDIO interface device structure */
}sdio_context, *sdioif = &sdio_context;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Set the SDIO Card voltage level to 3v3 */
static int SDIO_Card_SetVoltage(LPC_SDMMC_T *pSDMMC)
{
	int ret, i;
	uint32_t val;

	ret = SDIO_Send_Command(pSDMMC, CMD5, 0);
	if (ret) return ret;
	val = sdioif->response[0];

	/* Number of functions supported by the card */
	sdioif->fnum = (val >> 28) & 7;

	/* Check number of I/O functions*/
	if(sdioif->fnum == 0) {
		/* Number of I/O functions */
		return SDIO_ERR_FNUM;
	}

	/* ---- check OCR ---- */
	if((val & SDIO_VOLT_3_3) == 0){
		/* invalid voltage */
		return SDIO_ERR_VOLT;
	}

	/* ==== send CMD5 write new voltage  === */
	for(i = 0; i < 100; i++){
		ret = SDIO_Send_Command(pSDMMC, CMD5, SDIO_VOLT_3_3);
		if (ret) return ret;
		val = sdioif->response[0];

		/* Is card ready ? */
		if(val & (1UL << 31)){
			break;
		}

		sdioif->wait_evt(pSDMMC, SDIO_WAIT_DELAY, (void *)10);
	}

	/* ==== Check C bit  ==== */
	if(val & (1UL << 31)){
		return 0;
	}

	return SDIO_ERR_VOLT; /* error end */
}

/* Set SDIO Card RCA */
static int SDIO_CARD_SetRCA(LPC_SDMMC_T *pSDMMC)
{
	int ret;

	/* ==== send CMD3 get RCA  ==== */
	ret = SDIO_Send_Command(pSDMMC, CMD3, 0);
	if (ret) return ret;

	/* R6 response to CMD3 */
	if((sdioif->response[0] & 0x0000e000) != 0){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
		return SDIO_ERR_RCA;
	}

	/* change card state */
	sdioif->flag |= SDIO_POWER_INIT;

	/* New published RCA */
	sdioif->response[0] &= 0xffff0000;

	/* ==== change state to Stanby State ==== */
	return SDIO_Send_Command(pSDMMC, CMD7, sdioif->response[0]);
}

/* Set the Clock speed and mode [1/4 bit] of the card */
static int SDIO_Card_SetMode(LPC_SDMMC_T *pSDMMC, uint32_t clk, int mode_4bit)
{
	int ret;
	uint32_t val;

	Chip_SDIF_SetClock(pSDMMC, Chip_Clock_GetBaseClocktHz(CLK_BASE_SDIO), clk);

	if (!mode_4bit)
		return 0;

	val = 0x02;
	ret = SDIO_WriteRead_Direct(pSDMMC, SDIO_AREA_CIA, 0x07, &val);
	if (ret) return ret;

	if (val & 0x02) {
		Chip_SDIF_SetCardType(pSDMMC, MCI_CTYPE_4BIT);
	}
	return 0;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Set the block size of a function */
int SDIO_Card_SetBlockSize(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t blkSize)
{
	int ret;
	uint32_t tmp, asz;
	if (func > sdioif->fnum)
		return SDIO_ERR_INVFUNC;

	if (blkSize > 2048)
		return SDIO_ERR_INVARG;

	tmp = blkSize & 0xFF;
	ret = SDIO_WriteRead_Direct(pSDMMC, SDIO_AREA_CIA, (func << 8) + 0x10, &tmp);
	if (ret) return ret;
	asz = tmp;

	tmp = blkSize >> 8;
	ret = SDIO_WriteRead_Direct(pSDMMC, SDIO_AREA_CIA, (func << 8) + 0x11, &tmp);
	if (ret) return ret;
	asz |= tmp << 8;
	sdioif->blkSz[func] = asz;
	return 0;
}

/* Get the block size of a particular function */
uint32_t SDIO_Card_GetBlockSize(LPC_SDMMC_T *pSDMMC, uint32_t func)
{
	if (func > sdioif->fnum)
		return 0;

	return sdioif->blkSz[func];
}

/* Write data to SDIO Card */
int SDIO_Card_WriteData(LPC_SDMMC_T *pSDMMC, uint32_t func,
	uint32_t dest_addr, const uint8_t *src_addr,
	uint32_t size, uint32_t flags)
{
	int ret;
	uint32_t bs = size, bsize = size;
	uint32_t cmd = CMD53 | (1UL << 10);

	if (func > sdioif->fnum)
		return SDIO_ERR_INVFUNC;

	if (bsize > 512 || bsize == 0)
		return SDIO_ERR_INVARG;

	if (flags & SDIO_MODE_BLOCK) {
		uint32_t bs = SDIO_Card_GetBlockSize(pSDMMC, func);
		if (!bs) return SDIO_ERR_INVARG;
		size *= bs;
	}

	/* Set Block Size */
	Chip_SDIF_SetBlkSize(pSDMMC, bs);

	/* set number of bytes to read */
	Chip_SDIF_SetByteCnt(pSDMMC, size);

	sdioif->wait_evt(pSDMMC, SDIO_START_DATA, 0);
	Chip_SDIF_DmaSetup(pSDMMC, &sdioif->sdev, (uint32_t) src_addr, size);

	ret = SDIO_Send_Command(pSDMMC, cmd, (func << 28) | (dest_addr << 9) | (bsize & 0x1FF) | (1UL << 31) | (flags & (0x3 << 26)));
	if (ret) return ret;

	/* Check response for errors */
	if(sdioif->response[0] & 0xcb00){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
						/* RFU FUNCTION_NUMBER */
						/* OUT_OF_RANGE */
		/* Response flag error */
		return SDIO_ERR_READWRITE;
	}
	return sdioif->wait_evt(pSDMMC, SDIO_WAIT_DATA, 0);
}

/* Write data to SDIO Card */
int SDIO_Card_ReadData(LPC_SDMMC_T *pSDMMC, uint32_t func, uint8_t *dest_addr, uint32_t src_addr, uint32_t size, uint32_t flags)
{
	int ret;
	uint32_t bs = size, bsize = size;
	uint32_t cmd = CMD53;

	if (func > sdioif->fnum)
		return SDIO_ERR_INVFUNC;

	if (bsize > 512 || bsize == 0)
		return SDIO_ERR_INVARG;

	if (flags & SDIO_MODE_BLOCK) {
		bs = SDIO_Card_GetBlockSize(pSDMMC, func);
		if (!bs) return SDIO_ERR_INVARG;
		size *= bs;
	}
	/* Set the block size */
	Chip_SDIF_SetBlkSize(pSDMMC, bs);

	/* set number of bytes to read */
	Chip_SDIF_SetByteCnt(pSDMMC, size);

	sdioif->wait_evt(pSDMMC, SDIO_START_DATA, 0);
	Chip_SDIF_DmaSetup(pSDMMC, &sdioif->sdev, (uint32_t) dest_addr, size);

	ret = SDIO_Send_Command(pSDMMC, cmd | (1 << 13), (func << 28) | (src_addr << 9) | (bsize & 0x1FF) | (flags & (0x3 << 26)));
	if (ret) return ret;

	/* Check response for errors */
	if(sdioif->response[0] & 0xcb00){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
						/* RFU FUNCTION_NUMBER */
						/* OUT_OF_RANGE */
		/* Response flag error */
		return SDIO_ERR_READWRITE;
	}

	return sdioif->wait_evt(pSDMMC, SDIO_WAIT_DATA, 0);
}

/* Enable SDIO function interrupt */
int SDIO_Card_EnableInt(LPC_SDMMC_T *pSDMMC, uint32_t func)
{
	int ret;
	uint32_t val;

	if (func > sdioif->fnum)
		return SDIO_ERR_INVFUNC;

	ret = SDIO_Read_Direct(pSDMMC, SDIO_AREA_CIA, 0x04, &val);
	if (ret) return ret;
	val |= (1 << func) | 1;
	ret = SDIO_Write_Direct(pSDMMC, SDIO_AREA_CIA, 0x04, val);
	if (ret) return ret;
	pSDMMC->INTMASK |= SDIO_CARD_INT_MSK;

	return 0;
}

/* Disable SDIO function interrupt */
int SDIO_Card_DisableInt(LPC_SDMMC_T *pSDMMC, uint32_t func)
{
	int ret;
	uint32_t val;

	if (func > sdioif->fnum)
		return SDIO_ERR_INVFUNC;

	ret = SDIO_Read_Direct(pSDMMC, SDIO_AREA_CIA, 0x04, &val);
	if (ret) return ret;
	val &= ~(1 << func);

	/* Disable master interrupt if it is the only thing enabled */
	if (val == 1)
		val = 0;
	ret = SDIO_Write_Direct(pSDMMC, SDIO_AREA_CIA, 0x04, val);
	if (ret) return ret;
	if (!val)
		pSDMMC->INTMASK &= ~SDIO_CARD_INT_MSK;

	return 0;
}

/* Initialize the SDIO card */
int SDIO_Card_Init(LPC_SDMMC_T *pSDMMC, uint32_t freq)
{
	int ret;
	uint32_t val;

	/* Set Clock to 400KHz */
	Chip_SDIF_SetClock(pSDMMC, Chip_Clock_GetBaseClocktHz(CLK_BASE_SDIO), freq);
	Chip_SDIF_SetCardType(pSDMMC, 0);

	sdioif->wait_evt(pSDMMC, SDIO_WAIT_DELAY, (void *) 100); /* Wait for card to wake up */

	if (sdioif->flag & SDIO_POWER_INIT) {
		/* Write to the Reset Bit */
		ret = SDIO_Write_Direct(pSDMMC, SDIO_AREA_CIA, 0x06, 0x08);
		if (ret) return ret;
	}

	/* Set Voltage level to 3v3 */
	ret = SDIO_Card_SetVoltage(pSDMMC);
	if (ret) return ret;

	/* Set the RCA */
	ret = SDIO_CARD_SetRCA(pSDMMC);
	if (ret) return ret;

	/* ==== check card capability ==== */
	val = 0x02;
	ret = SDIO_WriteRead_Direct(pSDMMC, SDIO_AREA_CIA, 0x13, &val);
	if (ret) return ret;

	/* FIXME: Verify */
	/* FIFO threshold settings for DMA, DMA burst of 4,   FIFO watermark at 16 */
	pSDMMC->FIFOTH = MCI_FIFOTH_DMA_MTS_1 | MCI_FIFOTH_RX_WM(0) | MCI_FIFOTH_TX_WM(1);

	/* Enable internal DMA, burst size of 4, fixed burst */
	pSDMMC->BMOD = MCI_BMOD_DE | MCI_BMOD_PBL1 | MCI_BMOD_DSL(0);

	/* High Speed Support? */
	if ((val & 0x03) == 3) {
		return SDIO_Card_SetMode(pSDMMC, SDIO_CLK_HISPEED, 1);
	}

	ret = SDIO_Read_Direct(pSDMMC, SDIO_AREA_CIA, 0x08, &val);
	if (ret) return ret;

	/* Full Speed Support? */
	if (val & SDIO_CCCR_LSC) {
		return SDIO_Card_SetMode(pSDMMC, SDIO_CLK_FULLSPEED, 1);
	}

	/* Low Speed Card */
	return SDIO_Card_SetMode(pSDMMC, SDIO_CLK_LOWSPEED, val & SDIO_CCCR_4BLS);
}

/* Write given data to register space of the CARD */
int SDIO_Write_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t data)
{
	int ret;

	ret = SDIO_Send_Command(pSDMMC, CMD52, (func << 28) | (addr << 9) | (data & 0xFF) | (1UL << 31));
	if (ret) return ret;

	/* Check response for errors */
	if(sdioif->response[0] & 0xcb00){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
						/* RFU FUNCTION_NUMBER */
						/* OUT_OF_RANGE */
		/* Response flag error */
		return SDIO_ERR_READWRITE;
	}
	return data != (sdioif->response[0] & 0xFF);
}

/* Write given data to register, and read back the register into data */
int SDIO_WriteRead_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t *data)
{
	int ret;

	ret = SDIO_Send_Command(pSDMMC, CMD52, (func << 28) | (1 << 27) | (addr << 9) | ((*data) & 0xFF) | (1UL << 31));
	if (ret) return ret;

	/* Check response for errors */
	if(sdioif->response[0] & 0xcb00){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
						/* RFU FUNCTION_NUMBER */
						/* OUT_OF_RANGE */
		/* Response flag error */
		return SDIO_ERR_READWRITE;
	}
	*data = sdioif->response[0] & 0xFF;
	return 0;
}

/* Read a register from the register address space of the CARD */
int SDIO_Read_Direct(LPC_SDMMC_T *pSDMMC, uint32_t func, uint32_t addr, uint32_t *data)
{
	int ret;
	ret = SDIO_Send_Command(pSDMMC, CMD52, ((func & 7) << 28) | ((addr & 0x1FFFF) << 9));
	if (ret) return ret;

	/* Check response for errors */
	if(sdioif->response[0] & 0xcb00){
						/* COM_CRC_ERROR */
						/* ILLEGAL_CRC_ERROR */
						/* ERROR */
						/* RFU FUNCTION_NUMBER */
						/* OUT_OF_RANGE */
		/* Response flag error */
		return SDIO_ERR_READWRITE;
	}
	*data = sdioif->response[0] & 0xFF;
	return 0;
}

/* Set up the wait and wake call-back functions */
void SDIO_Setup_Callback(LPC_SDMMC_T *pSDMMC,
	void (*wake_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg),
	uint32_t (*wait_evt)(LPC_SDMMC_T *pSDMMC, uint32_t event, void *arg))
{
	sdioif->wake_evt = wake_evt;
	sdioif->wait_evt = wait_evt;
}

/* Send and SD Command to the SDIO Card */
uint32_t SDIO_Send_Command(LPC_SDMMC_T *pSDMMC, uint32_t cmd, uint32_t arg)
{
	uint32_t ret, ival;
	uint32_t imsk = pSDMMC->INTMASK;
	ret = sdioif->wait_evt(pSDMMC, SDIO_START_COMMAND, (void *)(cmd & 0x3F));
	ival = SDIO_CMD_INT_MSK & ~ret;

	/* Set data interrupts for data commands */
	if (cmd & SDIO_CMD_DATA) {
		ival |= SDIO_DATA_INT_MSK;
		imsk |= SDIO_DATA_INT_MSK;
	}

	Chip_SDIF_SetIntMask(pSDMMC, ival);
	Chip_SDIF_SendCmd(pSDMMC, cmd, arg);
	ret = sdioif->wait_evt(pSDMMC, SDIO_WAIT_COMMAND, 0);
	if (!ret && (cmd & SDIO_CMD_RESP_R1)) {
		Chip_SDIF_GetResponse(pSDMMC, &sdioif->response[0]);
	}

	Chip_SDIF_SetIntMask(pSDMMC, imsk);
	return ret;
}

/* SDIO Card interrupt handler */
void SDIO_Handler(LPC_SDMMC_T *pSDMMC)
{
	uint32_t status = pSDMMC->MINTSTS;
	uint32_t iclr = 0;

	/* Card Detected */
	if (status & 1) {
		sdioif->wake_evt(pSDMMC, SDIO_CARD_DETECT, 0);
		iclr = 1;
	}

	/* Command event error */
	if (status & (SDIO_CMD_INT_MSK & ~4)) {
		sdioif->wake_evt(pSDMMC, SDIO_CMD_ERR, (void *) (status & (SDIO_CMD_INT_MSK & ~4)));
		iclr |= status & SDIO_CMD_INT_MSK;
	} else if (status & 4) {
		/* Command event done */
		sdioif->wake_evt(pSDMMC, SDIO_CMD_DONE, (void *) status);
		iclr |= status & SDIO_CMD_INT_MSK;
	}

	/* Command event error */
	if (status & (SDIO_DATA_INT_MSK & ~8)) {
		sdioif->wake_evt(pSDMMC, SDIO_DATA_ERR, (void *) (status & (SDIO_DATA_INT_MSK & ~8)));
		iclr |= (status & SDIO_DATA_INT_MSK) | (3 << 4);
	} else if (status & 8) {
		/* Command event done */
		sdioif->wake_evt(pSDMMC, SDIO_DATA_DONE, (void *) status);
		iclr |= (status & SDIO_DATA_INT_MSK) | (3 << 4);
	}

	/* Handle Card interrupt */
	if (status & SDIO_CARD_INT_MSK) {
		sdioif->wake_evt(pSDMMC, SDIO_CARD_INT, 0);
		iclr |= status & SDIO_CARD_INT_MSK;
	}

	/* Clear the interrupts */
	pSDMMC->RINTSTS = iclr;
}
