/*
 * @brief LPC18XX/43XX CREG control functions
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

#ifndef __CREG_18XX_43XX_H_
#define __CREG_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CREG_18XX_43XX CHIP: LPC18xx/43xx CREG driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief CREG Register Block
 */
typedef struct {						/*!< CREG Structure         */
	__I  uint32_t  RESERVED0;
	__IO uint32_t  CREG0;				/*!< Chip configuration register 32 kHz oscillator output and BOD control register. */
	__I  uint32_t  RESERVED1[62];
	__IO uint32_t  MXMEMMAP;			/*!< ARM Cortex-M3/M4 memory mapping */
#if defined(CHIP_LPC18XX)
	__I  uint32_t  RESERVED2[5];
#else
	__I  uint32_t  RESERVED2;
	__I  uint32_t  CREG1;				/*!< Configuration Register 1 */
	__I  uint32_t  CREG2;				/*!< Configuration Register 2 */
	__I  uint32_t  CREG3;				/*!< Configuration Register 3 */
	__I  uint32_t  CREG4;				/*!< Configuration Register 4 */
#endif
	__IO uint32_t  CREG5;				/*!< Chip configuration register 5. Controls JTAG access. */
	__IO uint32_t  DMAMUX;				/*!< DMA muxing control     */
	__IO uint32_t  FLASHCFGA;			/*!< Flash accelerator configuration register for flash bank A */
	__IO uint32_t  FLASHCFGB;			/*!< Flash accelerator configuration register for flash bank B */
	__IO uint32_t  ETBCFG;				/*!< ETB RAM configuration  */
	__IO uint32_t  CREG6;				/*!< Chip configuration register 6. */
#if defined(CHIP_LPC18XX)
	__I  uint32_t  RESERVED4[52];
#else
	__IO uint32_t  M4TXEVENT;			/*!< M4 IPC event register */
	__I  uint32_t  RESERVED4[51];
#endif
	__I  uint32_t  CHIPID;				/*!< Part ID                */
#if defined(CHIP_LPC18XX)
	__I  uint32_t  RESERVED5[191];
#else
	__I  uint32_t  RESERVED5[65];
	__IO uint32_t  M0SUBMEMMAP;         /*!< M0SUB IPC Event memory mapping */
	__I  uint32_t  RESERVED6[2];
	__IO uint32_t  M0SUBTXEVENT;        /*!< M0SUB IPC Event register */
	__I  uint32_t  RESERVED7[58];
	__IO uint32_t  M0APPTXEVENT;		/*!< M0APP IPC Event register */
	__IO uint32_t  M0APPMEMMAP;			/*!< ARM Cortex M0APP memory mapping */
	__I  uint32_t  RESERVED8[62];
#endif
	__IO uint32_t  USB0FLADJ;			/*!< USB0 frame length adjust register */
	__I  uint32_t  RESERVED9[63];
	__IO uint32_t  USB1FLADJ;			/*!< USB1 frame length adjust register */
} LPC_CREG_T;

/**
 * @brief	Identifies whether on-chip flash is present
 * @return	true if on chip flash is available, otherwise false
 */
STATIC INLINE uint32_t Chip_CREG_OnChipFlashIsPresent(void)
{
	return LPC_CREG->CHIPID != 0x3284E02B;
}

/**
 * @brief	Configures the onboard Flash Accelerator in flash-based LPC18xx/LPC43xx parts.
 * @param	Hz	: Current frequency in Hz of the CPU
 * @return	Nothing
 * This function should be called with the higher frequency before the clock frequency is
 * increased and it should be called with the new lower value after the clock frequency is
 * decreased.
 */
STATIC INLINE void Chip_CREG_SetFlashAcceleration(uint32_t Hz)
{
	uint32_t FAValue = Hz / 21510000;

	LPC_CREG->FLASHCFGA = (LPC_CREG->FLASHCFGA & (~(0xF << 12))) | (FAValue << 12);
	LPC_CREG->FLASHCFGB = (LPC_CREG->FLASHCFGB & (~(0xF << 12))) | (FAValue << 12);
}

/**
 * @brief FLASH Access time definitions
 */
typedef enum {
	FLASHTIM_20MHZ_CPU = 0,		/*!< Flash accesses use 1 CPU clocks. Use for up to 20 MHz CPU clock */
	FLASHTIM_40MHZ_CPU = 1,		/*!< Flash accesses use 2 CPU clocks. Use for up to 40 MHz CPU clock */
	FLASHTIM_60MHZ_CPU = 2,		/*!< Flash accesses use 3 CPU clocks. Use for up to 60 MHz CPU clock */
	FLASHTIM_80MHZ_CPU = 3,		/*!< Flash accesses use 4 CPU clocks. Use for up to 80 MHz CPU clock */
	FLASHTIM_100MHZ_CPU = 4,	/*!< Flash accesses use 5 CPU clocks. Use for up to 100 MHz CPU clock */
	FLASHTIM_120MHZ_CPU = 5,	/*!< Flash accesses use 6 CPU clocks. Use for up to 120 MHz CPU clock */
	FLASHTIM_150MHZ_CPU = 6,	/*!< Flash accesses use 7 CPU clocks. Use for up to 150 Mhz CPU clock */
	FLASHTIM_170MHZ_CPU = 7,		/*!< Flash accesses use 8 CPU clocks. Use for up to 170 MHz CPU clock */
	FLASHTIM_190MHZ_CPU = 8,		/*!< Flash accesses use 9 CPU clocks. Use for up to 190 MHz CPU clock */
	FLASHTIM_SAFE_SETTING = 9,		/*!< Flash accesses use 10 CPU clocks. Safe setting for any allowed conditions */
} CREG_FLASHTIM_T;

/**
 * @brief	Set FLASH memory access time in clocks
 * @param	clks	: FLASH access speed rating
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_SetFLASHAccess(CREG_FLASHTIM_T clks)
{
	uint32_t tmpA, tmpB;

	/* Don't alter lower bits */
	tmpA = LPC_CREG->FLASHCFGA & ~(0xF << 12);
	LPC_CREG->FLASHCFGA = tmpA | ((uint32_t) clks << 12);
	tmpB = LPC_CREG->FLASHCFGB & ~(0xF << 12);
	LPC_CREG->FLASHCFGB = tmpB | ((uint32_t) clks << 12);
}

/**
 * @brief	Enables the USB0 high-speed PHY on LPC18xx/LPC43xx parts
 * @return	Nothing
 * @note	The USB0 PLL & clock should be configured before calling this function. This function
 * should be called before the USB0 registers are accessed.
 */
STATIC INLINE void Chip_CREG_EnableUSB0Phy(void)
{
	LPC_CREG->CREG0 &= ~(1 << 5);
}

/**
 * @brief	Disable the USB0 high-speed PHY on LPC18xx/LPC43xx parts
 * @return	Nothing
 * @note	The USB0 PLL & clock should be configured before calling this function. This function
 * should be called before the USB0 registers are accessed.
 */
STATIC INLINE void Chip_CREG_DisableUSB0Phy(void)
{
	LPC_CREG->CREG0 |= (1 << 5);
}

/**
 * @brief	Configures the BOD and Reset on LPC18xx/LPC43xx parts.
 * @param	BODVL	: Brown-Out Detect voltage level (0-3)
 * @param	BORVL	: Brown-Out Reset voltage level (0-3)
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_ConfigureBODaR(uint32_t BODVL, uint32_t BORVL)
{
	LPC_CREG->CREG0 = (LPC_CREG->CREG0 & ~((3 << 8) | (3 << 10))) | (BODVL << 8) | (BORVL << 10);
}

#if (defined(CHIP_LPC43XX) && defined(LPC_CREG))
/**
 * @brief	Configures base address of image to be run in the Cortex M0APP Core.
 * @param	memaddr	: Address of the image (must be aligned to 4K)
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_SetM0AppMemMap(uint32_t memaddr)
{
	LPC_CREG->M0APPMEMMAP = memaddr & ~0xFFF;
}

/**
 * @brief	Configures base address of image to be run in the Cortex M0SUB Core.
 * @param	memaddr	: Address of the image (must be aligned to 4K)
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_SetM0SubMemMap(uint32_t memaddr)
{
	LPC_CREG->M0SUBMEMMAP = memaddr & ~0xFFF;
}

/**
 * @brief	Clear M4 IPC Event
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_ClearM4Event(void)
{
	LPC_CREG->M4TXEVENT = 0;
}

/**
 * @brief	Clear M0APP IPC Event
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_ClearM0AppEvent(void)
{
	LPC_CREG->M0APPTXEVENT = 0;
}

/**
 * @brief	Clear M0APP IPC Event
 * @return	Nothing
 */
STATIC INLINE void Chip_CREG_ClearM0SubEvent(void)
{
	LPC_CREG->M0SUBTXEVENT = 0;
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CREG_18XX_43XX_H_ */
