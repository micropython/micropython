/*
 * @brief LPC18xx_43xx ROM API declarations and functions
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

#ifndef __ROMAPI_18XX_43XX_H_
#define __ROMAPI_18XX_43XX_H_

#include "iap_18xx_43xx.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ROMAPI_18XX_43XX CHIP: LPC18xx_43xx ROM API declarations and functions
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief LPC18XX_43XX OTP API structure
 */
typedef struct {
	uint32_t (*Init)(void);					/*!< Initializes OTP controller. */
	uint32_t (*ProgBootSrc)(CHIP_OTP_BOOT_SRC_T BootSrc);
	uint32_t (*ProgJTAGDis)(void);
	uint32_t (*ProgUSBID)(uint32_t ProductID, uint32_t VendorID);
	uint32_t reserved01;
	uint32_t reserved02;
	uint32_t reserved03;
	uint32_t reserved04;
	uint32_t (*ProgGP0)(uint32_t data, uint32_t mask);
	uint32_t (*ProgGP1)(uint32_t data, uint32_t mask);
	uint32_t (*ProgGP2)(uint32_t data, uint32_t mask);
	uint32_t (*ProgKey1)(uint8_t *key);
	uint32_t (*ProgKey2)(uint8_t *key);
	uint32_t (*GenRand)(void);
} OTP_API_T;

/**
 * @brief LPC18XX_43XX AES API structure
 */
typedef struct {
	uint32_t (*Init)(void);
	uint32_t (*SetMode)(uint32_t mode);
	uint32_t (*LoadKey1)(void);
	uint32_t (*LoadKey2)(void);
	uint32_t (*LoadKeyRNG)(void);
	uint32_t (*LoadKeySW)(uint8_t *pKey);
	uint32_t (*LoadIV_SW)(uint8_t *pVector);
	uint32_t (*LoadIV_IC)(void);
	uint32_t (*Operate)(uint8_t *pOutput, uint8_t *pInput, uint32_t size);
	uint32_t (*ProgramKey1)(uint8_t *pKey);
	uint32_t (*ProgramKey2)(uint8_t *pKey);
} AES_API_T;

/**
 * @brief LPC18XX High level ROM API structure
 */
typedef struct {
	void(*const iap_entry) (uint32_t *, uint32_t *);	/*!< IAP API entry function available on Flash parts only*/
	const OTP_API_T *pOtp;
	const AES_API_T *pAes;
	uint32_t reserved[3];
	const uint32_t spifiApiBase;			/*!< SPIFI API function table base address*/
	const uint32_t usbdApiBase;				/*!< USBD API function table base address*/
	const uint32_t endMarker;				/*!< API table end marker = 0x87654321 */

} LPC_ROM_API_T;

/* Pointer to ROM API function address */
#define LPC_ROM_API_BASE_LOC    0x10400100
#define LPC_ROM_API ((LPC_ROM_API_T *) LPC_ROM_API_BASE)

/* Pointer to ROM IAP entry functions */
#define IAP_ENTRY_LOCATION        (*((uint32_t *) 0x10400100))

/**
 * @brief IAP flash bank definitions
 */
#define IAP_FLASH_BANK_A                        0
#define IAP_FLASH_BANK_B                        1

/**
 * @}
 */

static INLINE void iap_entry(unsigned int cmd_param[], unsigned int status_result[])
{
	((IAP_ENTRY_T) IAP_ENTRY_LOCATION)(cmd_param, status_result);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ROMAPI_18XX_43XX_H_ */






