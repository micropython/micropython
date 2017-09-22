/*
 * @brief Common FLASH IAP support functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initializes the IAP command interface */
uint8_t Chip_IAP_Init(void)
{
	uint32_t command[5], result[4];

	command[0] = 49; /* IAP_INIT */
	result[0] = IAP_CMD_SUCCESS;
	iap_entry(command, result);
	return result[0];
}

/* Prepare sector for write operation */
uint8_t Chip_IAP_PreSectorForReadWrite(uint32_t strSector, uint32_t endSector, uint8_t flashBank)
{
	uint32_t command[5], result[4];

	command[0] = IAP_PREWRRITE_CMD;
	command[1] = strSector;
	command[2] = endSector;
	command[3] = flashBank;
	iap_entry(command, result);

	return result[0];
}

/* Copy RAM to flash */
uint8_t Chip_IAP_CopyRamToFlash(uint32_t dstAdd, uint32_t *srcAdd, uint32_t byteswrt)
{
	uint32_t command[5], result[4];

	command[0] = IAP_WRISECTOR_CMD;
	command[1] = dstAdd;
	command[2] = (uint32_t) srcAdd;
	command[3] = byteswrt;
	command[4] = SystemCoreClock / 1000;
	iap_entry(command, result);

	return result[0];
}

/* Erase sector */
uint8_t Chip_IAP_EraseSector(uint32_t strSector, uint32_t endSector, uint8_t flashBank)
{
	uint32_t command[5], result[4];

	command[0] = IAP_ERSSECTOR_CMD;
	command[1] = strSector;
	command[2] = endSector;
	command[3] = SystemCoreClock / 1000;
	command[4] = flashBank;
	iap_entry(command, result);

	return result[0];
}

/* Blank check sector */
uint8_t Chip_IAP_BlankCheckSector(uint32_t strSector, uint32_t endSector, uint8_t flashBank)
{
	uint32_t command[5], result[4];

	command[0] = IAP_BLANK_CHECK_SECTOR_CMD;
	command[1] = strSector;
	command[2] = endSector;
	command[3] = flashBank;
	iap_entry(command, result);

	return result[0];
}

/* Read part identification number */
uint32_t Chip_IAP_ReadPID()
{
	uint32_t command[5], result[4];

	command[0] = IAP_REPID_CMD;
	iap_entry(command, result);

	return result[1];
}

/* Read boot code version number */
uint8_t Chip_IAP_ReadBootCode()
{
	uint32_t command[5], result[4];

	command[0] = IAP_READ_BOOT_CODE_CMD;
	iap_entry(command, result);

	return result[0];
}

/* IAP compare */
uint8_t Chip_IAP_Compare(uint32_t dstAdd, uint32_t srcAdd, uint32_t bytescmp)
{
	uint32_t command[5], result[4];

	command[0] = IAP_COMPARE_CMD;
	command[1] = dstAdd;
	command[2] = srcAdd;
	command[3] = bytescmp;
	iap_entry(command, result);

	return result[0];
}

/* Reinvoke ISP */
uint8_t Chip_IAP_ReinvokeISP()
{
	uint32_t command[5], result[4];

	command[0] = IAP_REINVOKE_ISP_CMD;
	iap_entry(command, result);

	return result[0];
}

/* Read the unique ID */
uint32_t Chip_IAP_ReadUID(uint32_t uid[])
{
	uint32_t command[5], result[5], i;

	command[0] = IAP_READ_UID_CMD;
	iap_entry(command, result);
    for(i = 0; i < 4; i++) {
      uid[i] = result[i + 1];
    }

	return result[0];
}

/* Erase page */
uint8_t Chip_IAP_ErasePage(uint32_t strPage, uint32_t endPage)
{
	uint32_t command[5], result[4];

	command[0] = IAP_ERASE_PAGE_CMD;
	command[1] = strPage;
	command[2] = endPage;
	command[3] = SystemCoreClock / 1000;
	iap_entry(command, result);

	return result[0];
}

/* Set active boot flash bank */
uint8_t Chip_IAP_SetBootFlashBank(uint8_t bankNum)
{
	uint32_t command[5], result[4];

	command[0] = IAP_SET_BOOT_FLASH;
	command[1] = bankNum;
	command[2] = SystemCoreClock / 1000;
	iap_entry(command, result);

	return result[0];
}






