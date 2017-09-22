/*
 * @brief LPC18xx/43xx FLASH Memory Controller (FMC) driver
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

#ifndef __FMC_18XX_43XX_H_
#define __FMC_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup FMC_18XX_43XX CHIP: LPC18xx/43xx FLASH Memory Controller driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief FLASH Memory Controller Unit register block structure
 */
typedef struct {		/*!< FMC Structure */
	__I  uint32_t  RESERVED1[8];
	__IO uint32_t  FMSSTART;
	__IO uint32_t  FMSSTOP;
	__I  uint32_t  RESERVED2;
	__I  uint32_t  FMSW[4];
	__I  uint32_t  RESERVED3[1001];
	__I  uint32_t  FMSTAT;
	__I  uint32_t  RESERVED5;
	__O  uint32_t  FMSTATCLR;
	__I  uint32_t  RESERVED4[5];
} LPC_FMC_T;

/* Flash signature start and busy status bit */
#define FMC_FLASHSIG_BUSY       (1UL << 17)

/* Flash signature clear status bit */
#define FMC_FLASHSIG_STAT       (1 << 2)

/**
 * @brief	Gets the base address of given bank
 * @param	0 - Bank 0; 1 - Bank 1
 * @return	Base address corresponding to given bank
 */
__STATIC_INLINE LPC_FMC_T * Chip_FMC_BaseAddr(uint8_t bank)
{
	if (!bank) {
		return LPC_FMCA;
	} else {
		return LPC_FMCB;
	}
}

/**
 * @brief	Start computation of a signature for a FLASH memory range
 * @param	bank	: FLASH bank, A = 0, B = 1
 * @param	start	: Starting FLASH address for computation, must be aligned on 16 byte boundary
 * @param	stop	: Ending FLASH address for computation, must be aligned on 16 byte boundary
 * @return	Nothing
 * @note	Only bits 20..4 are used for the FLASH signature computation.
 *			Use the Chip_FMC_IsSignatureBusy() function to determine when the
 *			signature computation operation is complete and use the
 *			Chip_FMC_GetSignature() function to get the computed signature.
 */
STATIC INLINE void Chip_FMC_ComputeSignature(uint8_t bank, uint32_t start, uint32_t stop)
{
	LPC_FMC_T *LPC_FMC = Chip_FMC_BaseAddr(bank);
	LPC_FMC->FMSSTART = (start >> 4);
	LPC_FMC->FMSTATCLR = FMC_FLASHSIG_STAT;
	LPC_FMC->FMSSTOP = (stop >> 4) | FMC_FLASHSIG_BUSY;
}

/**
 * @brief	Start computation of a signature for a FLASH memory address and block count
 * @param	bank	: FLASH bank, A = 0, B = 1
 * @param	start	: Starting FLASH address for computation, must be aligned on 16 byte boundary
 * @param	blocks	: Number of 16 byte blocks used for computation
 * @return	Nothing
 * @note	Only bits 20..4 are used for the FLASH signature computation.
 *			Use the Chip_FMC_IsSignatureBusy() function to determine when the
 *			signature computation operation is complete and the
 *			Chip_FMC_GetSignature() function to get the computed signature.
 */
STATIC INLINE void Chip_FMC_ComputeSignatureBlocks(uint8_t bank, uint32_t start, uint32_t blocks)
{
	Chip_FMC_ComputeSignature(bank, start, (start + (blocks * 16)));
}

/**
 * @brief	Clear signature generation completion flag
 * @param	bank	: FLASH bank, A = 0, B = 1
 * @return	Nothing
 */
STATIC INLINE void Chip_FMC_ClearSignatureBusy(uint8_t bank)
{
	Chip_FMC_BaseAddr(bank)->FMSTATCLR = FMC_FLASHSIG_STAT;
}

/**
 * @brief	Check for signature generation completion
 * @param	bank	: FLASH bank, A = 0, B = 1
 * @return	true if the signature computation is running, false if finished
 */
STATIC INLINE bool Chip_FMC_IsSignatureBusy(uint8_t bank)
{
	return (bool) ((Chip_FMC_BaseAddr(bank)->FMSTAT & FMC_FLASHSIG_STAT) == 0);
}

/**
 * @brief	Returns the generated FLASH signature value
 * @param	bank	: FLASH bank, A = 0, B = 1
 * @param	index	: Signature index to get - use 0 to FMSW0, 1 to FMSW1, etc.
 * @return	the generated FLASH signature value
 */
STATIC INLINE uint32_t Chip_FMC_GetSignature(uint8_t bank, int index)
{
	return Chip_FMC_BaseAddr(bank)->FMSW[index];
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __FMC_18XX_43XX_H_ */






