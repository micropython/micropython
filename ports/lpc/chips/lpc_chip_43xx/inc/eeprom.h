/*
 * @brief Common EEPROM support functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
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

#ifndef __EEPROM_H_
#define __EEPROM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup COMMON_EEPROM CHIP: Common Chip EEPROM commands
 * @ingroup CHIP_Common
 * @{
 */

/**
 * @brief	Write data to EEPROM
 * @param	dstAdd		: EEPROM address to be written to
 * @param	ptr			: Pointer to buffer to write from
 * @param	byteswrt	: Number of bytes to write to EEPROM
 * @return	An IAP response definition from iap.h
 */
uint8_t Chip_EEPROM_Write(uint32_t dstAdd, uint8_t *ptr, uint32_t byteswrt);

/**
 * @brief	Read data from EEPROM
 * @param	srcAdd	: EEPROM address to be read from
 * @param	ptr		: Pointer to buffer to read to
 * @param	bytesrd	: Number of bytes to read from EEPROM
 * @return	An IAP response definition from iap.h
 */
uint8_t Chip_EEPROM_Read(uint32_t srcAdd, uint8_t *ptr, uint32_t bytesrd);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_H_ */






