/*
 * @brief LPC18xx/43xx Reset Generator Unit driver
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

#ifndef __RGU_18XX_43XX_H_
#define __RGU_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup RGU_18XX_43XX CHIP: LPC18xx/43xx Reset Generator Unit (RGU) driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief RGU reset enumerations
 */
typedef enum CHIP_RGU_RST {
	RGU_CORE_RST,
	RGU_PERIPH_RST,
	RGU_MASTER_RST,
	RGU_WWDT_RST = 4,
	RGU_CREG_RST,
	RGU_BUS_RST = 8,
	RGU_SCU_RST,
	RGU_M0SUB_RST = 12,
	RGU_M3_RST,
	RGU_LCD_RST = 16,
	RGU_USB0_RST,
	RGU_USB1_RST,
	RGU_DMA_RST,
	RGU_SDIO_RST,
	RGU_EMC_RST,
	RGU_ETHERNET_RST,
	RGU_FLASHA_RST = 25,
	RGU_EEPROM_RST = 27,
	RGU_GPIO_RST,
	RGU_FLASHB_RST,
	RGU_TIMER0_RST = 32,
	RGU_TIMER1_RST,
	RGU_TIMER2_RST,
	RGU_TIMER3_RST,
	RGU_RITIMER_RST,
	RGU_SCT_RST,
	RGU_MOTOCONPWM_RST,
	RGU_QEI_RST,
	RGU_ADC0_RST,
	RGU_ADC1_RST,
	RGU_DAC_RST,
	RGU_UART0_RST = 44,
	RGU_UART1_RST,
	RGU_UART2_RST,
	RGU_UART3_RST,
	RGU_I2C0_RST,
	RGU_I2C1_RST,
	RGU_SSP0_RST,
	RGU_SSP1_RST,
	RGU_I2S_RST,
	RGU_SPIFI_RST,
	RGU_CAN1_RST,
	RGU_CAN0_RST,
#ifdef CHIP_LPC43XX
	RGU_M0APP_RST,
	RGU_SGPIO_RST,
	RGU_SPI_RST,
	RGU_ADCHS_RST = 60,
#endif
	RGU_LAST_RST = 63,
} CHIP_RGU_RST_T;

/**
 * @brief RGU register structure
 */
typedef struct {							/*!< RGU Structure          */
	__I  uint32_t  RESERVED0[64];
	__O  uint32_t  RESET_CTRL[2];			/*!< Reset control register 0,1 */
	__I  uint32_t  RESERVED1[2];
	__IO uint32_t  RESET_STATUS[4];			/*!< Reset status register 0 to 3 */
	__I  uint32_t  RESERVED2[12];
	__I  uint32_t  RESET_ACTIVE_STATUS[2];	/*!< Reset active status register 0, 1 */
	__I  uint32_t  RESERVED3[170];
	__IO uint32_t  RESET_EXT_STAT[RGU_LAST_RST + 1];/*!< Reset external status registers */
} LPC_RGU_T;

/**
 * @brief	Trigger a peripheral reset for the selected peripheral
 * @param	ResetNumber	: Peripheral reset number to trigger
 * @return	Nothing
 */
STATIC INLINE void Chip_RGU_TriggerReset(CHIP_RGU_RST_T ResetNumber)
{
	LPC_RGU->RESET_CTRL[ResetNumber >> 5] = 1 << (ResetNumber & 31);
	/* Reset will auto clear after 1 clock cycle */
}

/**
 * @brief	Checks the reset status of a peripheral
 * @param	ResetNumber	: Peripheral reset number to trigger
 * @return	true if the periperal is still being reset
 */
STATIC INLINE bool Chip_RGU_InReset(CHIP_RGU_RST_T ResetNumber)
{
	return !(LPC_RGU->RESET_ACTIVE_STATUS[ResetNumber >> 5] & (1 << (ResetNumber & 31)));
}

/**
 * @brief	Clears reset for the selected peripheral
 * @param	ResetNumber	: Peripheral reset number to trigger (RGU_M0SUB_RST or RGU_M0APP_RST)
 * @return	Nothing
 * @note
 * Almost all peripherals will auto clear the reset bit. Only a few peripherals
 * like the Cortex M0 Core in LPC43xx will not auto clear the reset and require
 * this function to clear the reset bit. This function clears all reset bits in
 * a reset register.
 */
STATIC INLINE void Chip_RGU_ClearReset(CHIP_RGU_RST_T ResetNumber)
{
	LPC_RGU->RESET_CTRL[ResetNumber >> 5] = 0;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RGU_18XX_43XX_H_ */
