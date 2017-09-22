/*
 * @brief LPC18xx/43xx OTP Controller driver
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

#define BOOTROM_BASE            0x10400100
#define OTP_API_TABLE_OFFSET    0x1

static unsigned long *BOOTROM_API_TABLE;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static uint32_t (*Otp_ProgBootSrc)(CHIP_OTP_BOOT_SRC_T BootSrc);
static uint32_t (*Otp_ProgJTAGDis)(void);
static uint32_t (*Otp_ProgUSBID)(uint32_t ProductID, uint32_t VendorID);
static uint32_t (*Otp_ProgGP0)(uint32_t Data, uint32_t Mask);
static uint32_t (*Otp_ProgGP1)(uint32_t Data, uint32_t Mask);
static uint32_t (*Otp_ProgGP2)(uint32_t Data, uint32_t Mask);
static uint32_t (*Otp_ProgKey1)(uint8_t *key);
static uint32_t (*Otp_ProgKey2)(uint8_t *key);
static uint32_t (*Otp_GenRand)(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* CHIP OTP Initialisation function */
uint32_t Chip_OTP_Init(void)
{
	uint32_t (*ROM_otp_Init)(void);

	BOOTROM_API_TABLE = *((unsigned long * *) BOOTROM_BASE + OTP_API_TABLE_OFFSET);

	ROM_otp_Init      = (uint32_t (*)(void))BOOTROM_API_TABLE[0];
	Otp_ProgBootSrc   = (uint32_t (*)(CHIP_OTP_BOOT_SRC_T BootSrc))BOOTROM_API_TABLE[1];
	Otp_ProgJTAGDis   = (uint32_t (*)(void))BOOTROM_API_TABLE[2];
	Otp_ProgUSBID     = (uint32_t (*)(uint32_t ProductID, uint32_t VendorID))BOOTROM_API_TABLE[3];
	Otp_ProgGP0       = (uint32_t (*)(uint32_t Data, uint32_t Mask))BOOTROM_API_TABLE[8];
	Otp_ProgGP1       = (uint32_t (*)(uint32_t Data, uint32_t Mask))BOOTROM_API_TABLE[9];
	Otp_ProgGP2       = (uint32_t (*)(uint32_t Data, uint32_t Mask))BOOTROM_API_TABLE[10];
	Otp_ProgKey1      = (uint32_t (*)(uint8_t *key))BOOTROM_API_TABLE[11];
	Otp_ProgKey2      = (uint32_t (*)(uint8_t *key))BOOTROM_API_TABLE[12];
	Otp_GenRand       = (uint32_t (*)(void))BOOTROM_API_TABLE[13];

	return ROM_otp_Init();
}

/* Program boot source in OTP Controller */
uint32_t Chip_OTP_ProgBootSrc(CHIP_OTP_BOOT_SRC_T BootSrc)
{
	return Otp_ProgBootSrc(BootSrc);
}

/* Program the JTAG bit in OTP Controller */
uint32_t Chip_OTP_ProgJTAGDis(void)
{
	return Otp_ProgJTAGDis();
}

/* Program USB ID in OTP Controller */
uint32_t Chip_OTP_ProgUSBID(uint32_t ProductID, uint32_t VendorID)
{
	return Otp_ProgUSBID(ProductID, VendorID);
}

/* Program OTP GP Word memory */
uint32_t Chip_OTP_ProgGPWord(uint32_t WordNum, uint32_t Data, uint32_t Mask)
{
	uint32_t status;

	switch (WordNum) {
	case 1:
		status = Otp_ProgGP1(Data, Mask);
		break;

	case 2:
		status = Otp_ProgGP2(Data, Mask);
		break;

	case 0:
	default:
		status = Otp_ProgGP0(Data, Mask);
		break;
	}

	return status;
}

/* Program AES Key */
uint32_t Chip_OTP_ProgKey(uint32_t KeyNum, uint8_t *key)
{
	uint32_t status;

	if (KeyNum) {
		status = Otp_ProgKey2(key);
	}
	else {
		status = Otp_ProgKey1(key);
	}
	return status;
}

/* Generate Random Number using HW Random Number Generator */
uint32_t Chip_OTP_GenRand(void)
{
	return Otp_GenRand();
}






