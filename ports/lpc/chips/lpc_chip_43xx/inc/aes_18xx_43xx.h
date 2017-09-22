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

#ifndef __AES_18XX_43XX_H_
#define __AES_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup AES_18XX_43XX CHIP: LPC18xx/43xx AES Engine driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief	AES Engine operation mode
 */
typedef enum CHIP_AES_OP_MODE {
	CHIP_AES_API_CMD_ENCODE_ECB,	/*!< ECB Encode mode */
	CHIP_AES_API_CMD_DECODE_ECB,	/*!< ECB Decode mode */
	CHIP_AES_API_CMD_ENCODE_CBC,	/*!< CBC Encode mode */
	CHIP_AES_API_CMD_DECODE_CBC,	/*!< CBC Decode mode */
} CHIP_AES_OP_MODE_T;

/**
 * @brief	Initialize the AES Engine function
 * @return	None
 * This function will initialize all the AES Engine driver function pointers
 * and call the AES Engine Initialization function.
 */
void Chip_AES_Init(void);

/**
 * @brief	Set operation mode in AES Engine
 * @param	AesMode		: AES Operation Mode
 * @return	Status
 */
uint32_t Chip_AES_SetMode(CHIP_AES_OP_MODE_T AesMode);

/**
 * @brief	Load 128-bit AES user key in AES Engine
 * @param	keyNum: 0 - Load AES 128-bit user key 1, else load user key2
 * @return	None
 */
void Chip_AES_LoadKey(uint32_t keyNum);

/**
 * @brief	Load randomly generated key in AES engine
 * @return	None
 * To update the RNG and load a new random number,
 * the API call Chip_OTP_GenRand should be used
 */
void Chip_AES_LoadKeyRNG(void);

/**
 * @brief	Load 128-bit AES software defined user key in AES Engine
 * @param	pKey		: Pointer to 16 byte user key
 * @return	None
 */
void Chip_AES_LoadKeySW(uint8_t *pKey);

/**
 * @brief Load 128-bit AES initialization vector in AES Engine
 * @param	pVector		: Pointer to 16 byte Initialisation vector
 * @return	None
 */
void Chip_AES_LoadIV_SW(uint8_t *pVector);

/**
 * @brief Load IC specific 128-bit AES initialization vector in AES Engine
 * @return	None
 * This loads 128-bit AES IC specific initialization vector,
 * which is used to decrypt a boot image
 */
void Chip_AES_LoadIV_IC(void);

/**
 * @brief Operate AES Engine
 * @param	pDatOut		: Pointer to output data stream
 * @param	pDatIn		: Pointer to input data stream
 * @param	Size		: Size of the data stream (128-bit)
 * @return	Status
 * This function performs the AES operation after the AES mode
 * has been set using Chip_AES_SetMode and the appropriate keys
 * and init vectors have been loaded
 */
uint32_t Chip_AES_Operate(uint8_t *pDatOut, uint8_t *pDatIn, uint32_t Size);

/**
 * @brief	Program 128-bit AES Key in OTP
 * @param	KeyNum		: Key Number (Select 0 or 1)
 * @param	pKey		: Pointer to AES Key (16 bytes required)
 * @return	Status
 * When calling the aes_ProgramKey2 function, ensure that VPP = 2.7 V to 3.6 V.
 */
uint32_t Chip_AES_ProgramKey(uint32_t KeyNum, uint8_t *pKey);

/**
 * @brief	Checks for valid AES configuration of the chip and setup 
 *			DMA channel to process an AES data block.
 * @param	channel_id	: channel id
 * @return	Status
 */
uint32_t Chip_AES_Config_DMA(uint32_t channel_id);

/**
 * @brief	Checks for valid AES configuration of the chip and 
 *			enables DMA channel to process an AES data block.
 * @param	channel_id	: channel_id
 * @param	dataOutAddr	: destination address(16 x size of consecutive bytes)
 * @param	dataInAddr	: source address(16 x size of consecutive bytes)
 * @param	size		: number of 128 bit AES blocks
 * @return	Status
 */
uint32_t Chip_AES_OperateDMA(uint32_t channel_id, uint8_t *dataOutAddr, uint8_t *dataInAddr, uint32_t size);

/**
 * @brief	Read status of DMA channels that process an AES data block.
 * @param	channel_id	: channel id
 * @return	Status
 */
 uint32_t Chip_AES_GetStatusDMA(uint32_t channel_id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AES_18XX_43XX_H_ */






