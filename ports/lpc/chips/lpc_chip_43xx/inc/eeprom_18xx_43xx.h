/*
 * @brief LPC18xx/43xx EEPROM driver
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

#ifndef _EEPROM_18XX_43XX_H_
#define _EEPROM_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EEPROM_18XX_43XX CHIP: LPC18xx/43xx EEPROM driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/* FIX ME: Move to chip.h */
/** EEPROM start address */
#define EEPROM_START                    (0x20040000)
/** EEPROM byes per page */
#define EEPROM_PAGE_SIZE                (128)
/**The number of EEPROM pages. The last page is not writable.*/
#define EEPROM_PAGE_NUM                 (128)
/** Get the eeprom address */
#define EEPROM_ADDRESS(page, offset)     (EEPROM_START + (EEPROM_PAGE_SIZE * (page)) + offset)
#define EEPROM_CLOCK_DIV                 1500000
#define EEPROM_READ_WAIT_STATE_VAL       0x58
#define EEPROM_WAIT_STATE_VAL            0x232

/**
 * @brief EEPROM register block structure
 */
typedef struct {				/* EEPROM Structure */
	__IO uint32_t CMD;			/*!< EEPROM command register */
	uint32_t RESERVED0;
	__IO uint32_t RWSTATE;		/*!< EEPROM read wait state register */
	__IO uint32_t AUTOPROG;		/*!< EEPROM auto programming register */
	__IO uint32_t WSTATE;		/*!< EEPROM wait state register */
	__IO uint32_t CLKDIV;		/*!< EEPROM clock divider register */
	__IO uint32_t PWRDWN;		/*!< EEPROM power-down register */
	uint32_t RESERVED2[1007];
	__O  uint32_t INTENCLR;		/*!< EEPROM interrupt enable clear */
	__O  uint32_t INTENSET;		/*!< EEPROM interrupt enable set */
	__I  uint32_t INTSTAT;		/*!< EEPROM interrupt status */
	__I  uint32_t INTEN;		/*!< EEPROM interrupt enable */
	__O  uint32_t INTSTATCLR;	/*!< EEPROM interrupt status clear */
	__O  uint32_t INTSTATSET;	/*!< EEPROM interrupt status set */
} LPC_EEPROM_T;

/*
 * @brief Macro defines for EEPROM command register
 */
#define EEPROM_CMD_ERASE_PRG_PAGE       (6)		/*!< EEPROM erase/program command */

/*
 * @brief Macro defines for EEPROM Auto Programming register
 */
#define EEPROM_AUTOPROG_OFF     (0)		/*!<Auto programming off */
#define EEPROM_AUTOPROG_AFT_1WORDWRITTEN     (1)		/*!< Erase/program cycle is triggered after 1 word is written */
#define EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN  (2)		/*!< Erase/program cycle is triggered after a write to AHB
														   address ending with ......1111100 (last word of a page) */

/*
 * @brief Macro defines for EEPROM power down register
 */
#define EEPROM_PWRDWN                   (1 << 0)

/*
 * @brief Macro defines for EEPROM interrupt related registers
 */
#define EEPROM_INT_ENDOFPROG            (1 << 2)

/**
 * @brief	Put EEPROM device in power down mode
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_EnablePowerDown(LPC_EEPROM_T *pEEPROM)
{
	pEEPROM->PWRDWN = EEPROM_PWRDWN;
}

/**
 * @brief	Bring EEPROM device out of power down mode
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_DisablePowerDown(LPC_EEPROM_T *pEEPROM)
{
	pEEPROM->PWRDWN = 0;
}

/**
 * @brief	Initializes EEPROM
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	Nothing
 */
void Chip_EEPROM_Init(LPC_EEPROM_T *pEEPROM);

/**
 * @brief	De-initializes EEPROM
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_DeInit(LPC_EEPROM_T *pEEPROM)
{
	/* Enable EEPROM power down mode */
	Chip_EEPROM_EnablePowerDown(pEEPROM);
}

/**
 * @brief	Set Auto program mode
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mode	: Auto Program Mode (One of EEPROM_AUTOPROG_* value)
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_SetAutoProg(LPC_EEPROM_T *pEEPROM, uint32_t mode)
{
	pEEPROM->AUTOPROG = mode;
}

/**
 * @brief	Set EEPROM Read Wait State
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	ws      : Wait State value
 * @return	Nothing
 * @note    Bits 7:0 represents wait state for Read Phase 2 and
 *          Bits 15:8 represents wait state for Read Phase1
 */
STATIC INLINE void Chip_EEPROM_SetReadWaitState(LPC_EEPROM_T *pEEPROM, uint32_t ws)
{
	pEEPROM->RWSTATE = ws;
}

/**
 * @brief	Set EEPROM wait state
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	ws	    : Wait State value
 * @return	Nothing
 * @note    Bits 7:0 represents wait state for Phase 3,
 *          Bits 15:8 represents wait state for Phase2, and
 *          Bits 23:16 represents wait state for Phase1
 */
STATIC INLINE void Chip_EEPROM_SetWaitState(LPC_EEPROM_T *pEEPROM, uint32_t ws)
{
	pEEPROM->WSTATE = ws;
}

/**
 * @brief	Select an EEPROM command
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	cmd	    : EEPROM command
 * @return	Nothing
 * @note	The cmd is OR-ed bits value of  EEPROM_CMD_*
 */
STATIC INLINE void Chip_EEPROM_SetCmd(LPC_EEPROM_T *pEEPROM, uint32_t cmd)
{
	pEEPROM->CMD = cmd;
}

/**
 * @brief	Erase/Program an EEPROM page
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	Nothing
 */
void Chip_EEPROM_EraseProgramPage(LPC_EEPROM_T *pEEPROM);

/**
 * @brief	Wait for interrupt occurs
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mask	: Expected interrupt
 * @return	Nothing
 */
void Chip_EEPROM_WaitForIntStatus(LPC_EEPROM_T *pEEPROM, uint32_t mask);

/**
 * @brief	Enable EEPROM interrupt
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mask	: Interrupt mask (or-ed bits value of EEPROM_INT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_EnableInt(LPC_EEPROM_T *pEEPROM, uint32_t mask)
{
	pEEPROM->INTENSET =  mask;
}

/**
 * @brief	Disable EEPROM interrupt
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mask	: Interrupt mask (or-ed bits value of EEPROM_INT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_DisableInt(LPC_EEPROM_T *pEEPROM, uint32_t mask)
{
	pEEPROM->INTENCLR =  mask;
}

/**
 * @brief	Get the value of the EEPROM interrupt enable register
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	OR-ed bits value of EEPROM_INT_*
 */
STATIC INLINE uint32_t Chip_EEPROM_GetIntEnable(LPC_EEPROM_T *pEEPROM)
{
	return pEEPROM->INTEN;
}

/**
 * @brief	Get EEPROM interrupt status
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @return	OR-ed bits value of EEPROM_INT_*
 */
STATIC INLINE uint32_t Chip_EEPROM_GetIntStatus(LPC_EEPROM_T *pEEPROM)
{
	return pEEPROM->INTSTAT;
}

/**
 * @brief	Set EEPROM interrupt status
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mask	: Interrupt mask (or-ed bits value of EEPROM_INT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_SetIntStatus(LPC_EEPROM_T *pEEPROM, uint32_t mask)
{
	pEEPROM->INTSTATSET =  mask;
}

/**
 * @brief	Clear EEPROM interrupt status
 * @param	pEEPROM	: Pointer to EEPROM peripheral block structure
 * @param	mask	: Interrupt mask (or-ed bits value of EEPROM_INT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_EEPROM_ClearIntStatus(LPC_EEPROM_T *pEEPROM, uint32_t mask)
{
	pEEPROM->INTSTATCLR =  mask;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _EEPROM_18XX_43XX_H_ */






