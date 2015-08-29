/*
 * @brief LPC18xx/43xx AES Engine driver
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

#define BOOTROM_BASE			0x10400100
#define AES_API_TABLE_OFFSET	0x2

typedef	void		(*V_FP_V)(void);
typedef	uint32_t	(*U32_FP_V)(void);

static unsigned long *BOOTROM_API_TABLE;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static uint32_t (*aes_SetMode)(CHIP_AES_OP_MODE_T AesMode);
static void (*aes_LoadKey1)(void);
static void (*aes_LoadKey2)(void);
static void (*aes_LoadKeyRNG)(void);
static void (*aes_LoadKeySW)(uint8_t *pKey);
static void (*aes_LoadIV_SW)(uint8_t *pVector);
static void (*aes_LoadIV_IC)(void);
static uint32_t (*aes_Operate)(uint8_t *pDatOut, uint8_t *pDatIn, uint32_t size);
static uint32_t (*aes_ProgramKey1)(uint8_t *pKey);
static uint32_t (*aes_ProgramKey2)(uint8_t *pKey);
static uint32_t (*aes_Config_DMA) (uint32_t channel_id);
static uint32_t (*aes_Operate_DMA)(uint32_t channel_id, uint8_t *dataOutAddr, uint8_t *dataInAddr, uint32_t size);
static uint32_t (*aes_Get_Status_DMA) (uint32_t channel_id);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* CHIP AES Initialisation function */
void Chip_AES_Init(void)
{
	uint32_t (*ROM_aes_Init)(void);

	BOOTROM_API_TABLE = *((unsigned long * *) BOOTROM_BASE + AES_API_TABLE_OFFSET);

	ROM_aes_Init		= (uint32_t (*)(void))BOOTROM_API_TABLE[0];
	aes_SetMode			= (uint32_t (*)(CHIP_AES_OP_MODE_T AesMode))BOOTROM_API_TABLE[1];
	aes_LoadKey1		= (void (*)(void))BOOTROM_API_TABLE[2];
	aes_LoadKey2		= (void (*)(void))BOOTROM_API_TABLE[3];
	aes_LoadKeyRNG		= (void (*)(void))BOOTROM_API_TABLE[4];
	aes_LoadKeySW		= (void (*)(uint8_t *pKey))BOOTROM_API_TABLE[5];
	aes_LoadIV_SW		= (void (*)(uint8_t *pVector))BOOTROM_API_TABLE[6];
	aes_LoadIV_IC		= (void (*)(void))BOOTROM_API_TABLE[7];
	aes_Operate			= (uint32_t (*)(uint8_t *pDatOut, uint8_t *pDatIn, uint32_t Size))BOOTROM_API_TABLE[8];
	aes_ProgramKey1		= (uint32_t (*)(uint8_t *pKey))BOOTROM_API_TABLE[9];
	aes_ProgramKey2		= (uint32_t (*)(uint8_t *pKey))BOOTROM_API_TABLE[10];
	aes_Config_DMA		= (uint32_t (*)(uint32_t channel_id))BOOTROM_API_TABLE[11];
	aes_Operate_DMA		= (uint32_t (*)(uint32_t channel_id, uint8_t *dataOutAddr, uint8_t *dataInAddr, uint32_t size))BOOTROM_API_TABLE[12];
	aes_Get_Status_DMA	= (uint32_t (*) (uint32_t channel_id))BOOTROM_API_TABLE[13];
	
	ROM_aes_Init();
}

/* Set Operation mode in AES Engine */
uint32_t Chip_AES_SetMode(CHIP_AES_OP_MODE_T AesMode)
{
	return aes_SetMode(AesMode);
}

/* Load 128-bit user key in AES Engine */
void Chip_AES_LoadKey(uint32_t keyNum)
{
	if (keyNum) {
		aes_LoadKey2();
	}
	else {
		aes_LoadKey1();
	}
}

/* Load randomly generated key in AES engine */
void Chip_AES_LoadKeyRNG(void)
{
	aes_LoadKeyRNG();
}

/* Load 128-bit AES software defined user key */
void Chip_AES_LoadKeySW(uint8_t *pKey)
{
	aes_LoadKeySW(pKey);
}

/* Load 128-bit AES initialization vector */
void Chip_AES_LoadIV_SW(uint8_t *pVector)
{
	aes_LoadIV_SW(pVector);
}

/* Load IC specific 128-bit AES initialization vector */
void Chip_AES_LoadIV_IC(void)
{
	aes_LoadIV_IC();
}

/* Operate AES Engine */
uint32_t Chip_AES_Operate(uint8_t *pDatOut, uint8_t *pDatIn, uint32_t Size)
{
	return aes_Operate(pDatOut, pDatIn, Size);
}

/* Program 128-bit AES Key in OTP */
uint32_t Chip_AES_ProgramKey(uint32_t KeyNum, uint8_t *pKey)
{
	uint32_t status;

	if (KeyNum) {
		status = aes_ProgramKey2(pKey);
	}
	else {
		status = aes_ProgramKey1(pKey);
	}
	return status;
}

/* Configure DMA channel to process AES block */
uint32_t Chip_AES_Config_DMA(uint32_t channel_id)
{
	return aes_Config_DMA(channel_id);
}

/* Enables DMA channel and Operates AES Engine */
uint32_t Chip_AES_OperateDMA(uint32_t channel_id, uint8_t *dataOutAddr, uint8_t *dataInAddr, uint32_t size)
{
	return aes_Operate_DMA(channel_id,dataOutAddr,dataInAddr,size);
}

/* Read status of DMA channels that process an AES data block. */
uint32_t Chip_AES_GetStatusDMA(uint32_t channel_id)
{
	return aes_Get_Status_DMA(channel_id);
}
