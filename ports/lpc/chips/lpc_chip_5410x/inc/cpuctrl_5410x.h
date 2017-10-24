/*
 * @brief LPC5410X CPU multi-core support driver
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

#ifndef __CPUCTRL_5410X_H_
#define __CPUCTRL_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CPUCTRL_5410X CHIP: LPC5410X CPU multi-core support driver
 * @ingroup CHIP_5410X_DRIVERS
 * This driver helps with determine which MCU core the software is running,
 * whether the MCU core is in master or slave mode, and provides functions
 * for master and slave core control.<br>
 *
 * The functions for the driver are provided as part of the
 * @ref POWER_LIBRARY_5410X library. For more information on using the
 * LPC5410x LPCopen package with multi-core, see @ref CHIP_5410X_MULTICORE<br>.
 * @{
 */

/**
 * @brief	Determine which MCU this code is running on
 * @return  true if executing on the CM4, or false if executing on the CM0+
 */
__STATIC_INLINE bool Chip_CPU_IsM4Core(void) {
	/* M4 core is designated by values 0xC24 on bits 15..4 */
	if (((SCB->CPUID >> 4) & 0xFFF) == 0xC24) {
		return true;
	}

	return false;
}

/* Core selection */
typedef enum {
	CORESELECT_M0PLUS = 0,
	CORESELECT_M4
} CORESELECT_T;

/**
 * @brief	Select master core and system power control ownership
 * @return	Nothing
 * @note	This function can be used to select the master core and which
 * core can powerdown the system. The master core can be re-selected on
 * either the current master or slave core. Power control ownership is used
 * to select which core can place the system in DEEP SLEEP, POWERDOWN, and
 * DEEP POWERDOWN modes. (See @ref Chip_POWER_EnterPowerMode). Note both
 * the master and slave cores can used SLEEP mode, but only the master core
 * can use the other modes.
 */
void Chip_CPU_SelectMasterCore(CORESELECT_T master, CORESELECT_T ownerPower);

/**
 * @brief	Determine if this core is a slave or master
 * @return  true if this MCU is operating as the master, or false if operating as a slave
 */
bool Chip_CPU_IsMasterCore(void);

/**
 * @brief	Setup M0+ boot and reset M0+ core
 * @param	coentry		: Pointer to boot entry point for M0+ core
 * @param	costackptr	: Pointer to where stack should be located for M0+ core
 * @return  Nothing
 * @note	Will setup boot stack and entry point, enable M0+ clock and then
 * reset M0+ core.
 */
void Chip_CPU_CM0Boot(uint32_t *coentry, uint32_t *costackptr);

/**
 * @brief	Setup M4 boot and reset M4 core
 * @param	coentry		: Pointer to boot entry point for M4 core
 * @param	costackptr	: Pointer to where stack should be located for M4 core
 * @return  Nothing
 * @note	Will setup boot stack and entry point, enable M4 clock and then
 * reset M0+ core.
 */
void Chip_CPU_CM4Boot(uint32_t *coentry, uint32_t *costackptr);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CPUCTRL_5410X_H_ */
