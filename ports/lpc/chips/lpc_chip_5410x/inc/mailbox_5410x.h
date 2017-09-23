/*
 * @brief LPC5410X Mailbox M4/M0+ driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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

#ifndef __MAILBOX_5410X_H_
#define __MAILBOX_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MAILBOX_5410X CHIP: LPC5410X Mailbox M4/M0+ driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/* Mailbox indexes */
typedef enum {
	MAILBOX_CM0PLUS = 0,
	MAILBOX_CM4
} MBOX_IDX_T;
#define MAILBOX_AVAIL       (MAILBOX_CM4 + 1)	/* Number of available mailboxes */

/** Individual mailbox IRQ structure */
typedef struct {
	__IO    uint32_t        IRQ;		/*!< Mailbox data */
	__O     uint32_t        IRQSET;		/*!< Mailbox data set bits only */
	__O     uint32_t        IRQCLR;		/*!< Mailbox dataclearset bits only */
	__I     uint32_t        RESERVED;
} LPC_MBOXIRQ_T;

/** Mailbox register structure */
typedef struct {						/*!< Mailbox register structure */
	LPC_MBOXIRQ_T           BOX[MAILBOX_AVAIL];	/*!< Mailbox, offset 0 = M0+, offset 1 = M4 */
	LPC_MBOXIRQ_T           RESERVED1[15 - MAILBOX_AVAIL];
	__I     uint32_t        RESERVED2[2];
	__IO    uint32_t        MUTEX;		/*!< Mutex */
} LPC_MBOX_T;

/**
 * @brief	Initialize mailbox
 * @param	pMBOX	: Pointer to the mailbox register structure
 * @return	Nothing
 * @note	Even if both cores use the amilbox, only 1 core should initialize it.
 */
__STATIC_INLINE void Chip_MBOX_Init(LPC_MBOX_T *pMBOX)
{
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_MAILBOX);
	Chip_SYSCON_PeriphReset(RESET_MAILBOX);
}

/**
 * @brief	Shutdown mailbox
 * @param	pMBOX	: Pointer to the mailbox register structure
 * @return	Nothing
 */
__STATIC_INLINE void Chip_MBOX_DeInit(LPC_MBOX_T *pMBOX)
{
	Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_MAILBOX);
}

/**
 * @brief	Set data value in the mailbox based on the CPU ID
 * @param	pMBOX		: Pointer to the mailbox register structure
 * @param	cpu_id		: MAILBOX_CM0PLUS is M0+ or MAILBOX_CM4 is M4
 * @param	mboxData	: data to send in the mailbox
 * @return	Nothing
 * @note	Sets a data value to send via the MBOX to the other core.
 */
__STATIC_INLINE void Chip_MBOX_SetValue(LPC_MBOX_T *pMBOX, uint32_t cpu_id, uint32_t mboxData)
{
	pMBOX->BOX[cpu_id].IRQ = mboxData;
}

/**
 * @brief	Set data bits in the mailbox based on the CPU ID
 * @param	pMBOX		: Pointer to the mailbox register structure
 * @param	cpu_id		: MAILBOX_CM0PLUS is M0+ or MAILBOX_CM4 is M4
 * @param	mboxSetBits	: data bits to set in the mailbox
 * @return	Nothing
 * @note	Sets data bits to send via the MBOX to the other core, A value of 0 will
 * do nothing.  Only sets bits selected with a 1 in it's bit position.
 */
__STATIC_INLINE void Chip_MBOX_SetValueBits(LPC_MBOX_T *pMBOX, uint32_t cpu_id, uint32_t mboxSetBits)
{
	pMBOX->BOX[cpu_id].IRQSET = mboxSetBits;
}

/**
 * @brief	Clear data bits in the mailbox based on the CPU ID
 * @param	pMBOX		: Pointer to the mailbox register structure
 * @param	cpu_id		: MAILBOX_CM0PLUS is M0+ or MAILBOX_CM4 is M4
 * @param	mboxClrBits	: data bits to clear in the mailbox
 * @return	Nothing
 * @note	Clear data bits to send via the MBOX to the other core. A value of 0 will
 * do nothing. Only clears bits selected with a 1 in it's bit position.
 */
__STATIC_INLINE void Chip_MBOX_ClearValueBits(LPC_MBOX_T *pMBOX, uint32_t cpu_id, uint32_t mboxClrBits)
{
	pMBOX->BOX[cpu_id].IRQCLR = mboxClrBits;
}

/**
 * @brief	Get data in the mailbox based on the cpu_id
 * @param	pMBOX	: Pointer to the mailbox register structure
 * @param	cpu_id	: MAILBOX_CM0PLUS is M0+ or MAILBOX_CM4 is M4
 * @return	Current mailbox data
 */
__STATIC_INLINE uint32_t Chip_MBOX_GetValue(LPC_MBOX_T *pMBOX, uint32_t cpu_id)
{
	return pMBOX->BOX[cpu_id].IRQ;
}

/**
 * @brief	Get MUTEX state and lock mutex
 * @param	pMBOX	: Pointer to the mailbox register structure
 * @return	See note
 * @note	Returns '1' if the mutex was taken or '0' if another resources has the
 * mutex locked. Once a mutex is taken, it can be returned with the Chip_MBOX_SetMutex()
 * function.
 */
__STATIC_INLINE uint32_t Chip_MBOX_GetMutex(LPC_MBOX_T *pMBOX)
{
	return pMBOX->MUTEX;
}

/**
 * @brief	Set MUTEX state
 * @param	pMBOX	: Pointer to the mailbox register structure
 * @return	Nothing
 * @note	Sets mutex state to '1' and allows other resources to get the mutex
 */
__STATIC_INLINE void Chip_MBOX_SetMutex(LPC_MBOX_T *pMBOX)
{
	pMBOX->MUTEX = 1;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __MAILBOX_5410X_H_ */
