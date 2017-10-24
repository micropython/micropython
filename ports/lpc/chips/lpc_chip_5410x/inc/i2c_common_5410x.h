/*
 * @brief LPC5410x I2C driver
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

#ifndef __I2C_COMMON_5410X_H_
#define __I2C_COMMON_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2C_5410X CHIP: LPC5410x I2C driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief I2C register block structure
 */
typedef struct {					/* I2C0 Structure         */
	__IO uint32_t CFG;				/*!< I2C Configuration Register common for Master, Slave and Monitor */
	__IO uint32_t STAT;				/*!< I2C Status Register common for Master, Slave and Monitor */
	__IO uint32_t INTENSET;			/*!< I2C Interrupt Enable Set Register common for Master, Slave and Monitor */
	__O  uint32_t INTENCLR;			/*!< I2C Interrupt Enable Clear Register common for Master, Slave and Monitor */
	__IO uint32_t TIMEOUT;			/*!< I2C Timeout value Register */
	__IO uint32_t CLKDIV;			/*!< I2C Clock Divider Register */
	__IO uint32_t INTSTAT;			/*!< I2C Interrupt Status Register */
	__I  uint32_t RESERVED0;
	__IO uint32_t MSTCTL;			/*!< I2C Master Control Register */
	__IO uint32_t MSTTIME;			/*!< I2C Master Time Register for SCL */
	__IO uint32_t MSTDAT;			/*!< I2C Master Data Register */
	__I  uint32_t RESERVED1[5];
	__IO uint32_t SLVCTL;			/*!< I2C Slave Control Register */
	__IO uint32_t SLVDAT;			/*!< I2C Slave Data Register */
	__IO uint32_t SLVADR[4];		/*!< I2C Slave Address Registers */
	__IO uint32_t SLVQUAL0;			/*!< I2C Slave Address Qualifier 0 Register */
	__I  uint32_t RESERVED2[9];
	__IO uint32_t MONRXDAT;			/*!< I2C Monitor Data Register */
} LPC_I2C_T;

/*
 * @brief I2C Configuration register Bit definition
 */
#define I2C_CFG_MSTEN             (1 << 0)			/*!< Master Enable/Disable Bit */
#define I2C_CFG_SLVEN             (1 << 1)			/*!< Slave Enable/Disable Bit */
#define I2C_CFG_MONEN             (1 << 2)			/*!< Monitor Enable/Disable Bit */
#define I2C_CFG_TIMEOUTEN         (1 << 3)			/*!< Timeout Enable/Disable Bit */
#define I2C_CFG_MONCLKSTR         (1 << 4)			/*!< Monitor Clock Stretching Bit */
#define I2C_CFG_MASK              ((uint32_t) 0x1F)	/*!< Configuration Register Mask */

/*
 * @brief I2C Status register Bit definition
 */
#define I2C_STAT_MSTPENDING       (1 << 0)		/*!< Master Pending Status Bit */
#define I2C_STAT_MSTSTATE         (0x7 << 1)	/*!< Master State Code */
#define I2C_STAT_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Bit */
#define I2C_STAT_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Bit */
#define I2C_STAT_SLVPENDING       (1 << 8)		/*!< Slave Pending Status Bit */
#define I2C_STAT_SLVSTATE         (0x3 << 9)	/*!< Slave State Code */
#define I2C_STAT_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Bit */
#define I2C_STAT_SLVIDX           (0x3 << 12)	/*!< Slave Address Index */
#define I2C_STAT_SLVSEL           (1 << 14)		/*!< Slave Selected Bit */
#define I2C_STAT_SLVDESEL         (1 << 15)		/*!< Slave Deselect Bit */
#define I2C_STAT_MONRDY           (1 << 16)		/*!< Monitor Ready Bit */
#define I2C_STAT_MONOV            (1 << 17)		/*!< Monitor Overflow Flag */
#define I2C_STAT_MONACTIVE        (1 << 18)		/*!< Monitor Active Flag */
#define I2C_STAT_MONIDLE          (1 << 19)		/*!< Monitor Idle Flag */
#define I2C_STAT_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Flag */
#define I2C_STAT_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Flag */

#define I2C_STAT_MSTCODE_IDLE       (0)			/*!< Master Idle State Code */
#define I2C_STAT_MSTCODE_RXREADY    (1)			/*!< Master Receive Ready State Code */
#define I2C_STAT_MSTCODE_TXREADY    (2)			/*!< Master Transmit Ready State Code */
#define I2C_STAT_MSTCODE_NACKADR    (3)			/*!< Master NACK by slave on address State Code */
#define I2C_STAT_MSTCODE_NACKDAT    (4)			/*!< Master NACK by slave on data State Code */

#define I2C_STAT_SLVCODE_ADDR         (0)		/*!< Master Idle State Code */
#define I2C_STAT_SLVCODE_RX           (1)		/*!< Received data is available Code */
#define I2C_STAT_SLVCODE_TX           (2)		/*!< Data can be transmitted Code */

/*
 * @brief I2C Interrupt Enable Set register Bit definition
 */
#define I2C_INTENSET_MSTPENDING       (1 << 0)		/*!< Master Pending Interrupt Enable Bit */
#define I2C_INTENSET_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Interrupt Enable Bit */
#define I2C_INTENSET_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Interrupt Enable Bit */
#define I2C_INTENSET_SLVPENDING       (1 << 8)		/*!< Slave Pending Interrupt Enable Bit */
#define I2C_INTENSET_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Interrupt Enable Bit */
#define I2C_INTENSET_SLVDESEL         (1 << 15)		/*!< Slave Deselect Interrupt Enable Bit */
#define I2C_INTENSET_MONRDY           (1 << 16)		/*!< Monitor Ready Interrupt Enable Bit */
#define I2C_INTENSET_MONOV            (1 << 17)		/*!< Monitor Overflow Interrupt Enable Bit */
#define I2C_INTENSET_MONIDLE          (1 << 19)		/*!< Monitor Idle Interrupt Enable Bit */
#define I2C_INTENSET_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Enable Bit */
#define I2C_INTENSET_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Enable Bit */

/*
 * @brief I2C Interrupt Enable Clear register Bit definition
 */
#define I2C_INTENCLR_MSTPENDING       (1 << 0)		/*!< Master Pending Interrupt Clear Bit */
#define I2C_INTENCLR_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Interrupt Clear Bit */
#define I2C_INTENCLR_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Interrupt Clear Bit */
#define I2C_INTENCLR_SLVPENDING       (1 << 8)		/*!< Slave Pending Interrupt Clear Bit */
#define I2C_INTENCLR_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Interrupt Clear Bit */
#define I2C_INTENCLR_SLVDESEL         (1 << 15)		/*!< Slave Deselect Interrupt Clear Bit */
#define I2C_INTENCLR_MONRDY           (1 << 16)		/*!< Monitor Ready Interrupt Clear Bit */
#define I2C_INTENCLR_MONOV            (1 << 17)		/*!< Monitor Overflow Interrupt Clear Bit */
#define I2C_INTENCLR_MONIDLE          (1 << 19)		/*!< Monitor Idle Interrupt Clear Bit */
#define I2C_INTENCLR_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Clear Bit */
#define I2C_INTENCLR_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Clear Bit */

/*
 * @brief I2C TimeOut Value Macro
 */
#define I2C_TIMEOUT_VAL(n)              (((uint32_t) ((n) - 1) & 0xFFF0) | 0x000F)		/*!< Macro for Timeout value register */

/*
 * @brief I2C Interrupt Status register Bit definition
 */
#define I2C_INTSTAT_MSTPENDING      (1 << 0)		/*!< Master Pending Interrupt Status Bit */
#define I2C_INTSTAT_MSTRARBLOSS     (1 << 4)		/*!< Master Arbitration Loss Interrupt Status Bit */
#define I2C_INTSTAT_MSTSTSTPERR     (1 << 6)		/*!< Master Start Stop Error Interrupt Status Bit */
#define I2C_INTSTAT_SLVPENDING      (1 << 8)		/*!< Slave Pending Interrupt Status Bit */
#define I2C_INTSTAT_SLVNOTSTR       (1 << 11)		/*!< Slave not stretching Clock Interrupt Status Bit */
#define I2C_INTSTAT_SLVDESEL        (1 << 15)		/*!< Slave Deselect Interrupt Status Bit */
#define I2C_INTSTAT_MONRDY          (1 << 16)		/*!< Monitor Ready Interrupt Status Bit */
#define I2C_INTSTAT_MONOV           (1 << 17)		/*!< Monitor Overflow Interrupt Status Bit */
#define I2C_INTSTAT_MONIDLE         (1 << 19)		/*!< Monitor Idle Interrupt Status Bit */
#define I2C_INTSTAT_EVENTTIMEOUT    (1 << 24)		/*!< Event Timeout Interrupt Status Bit */
#define I2C_INTSTAT_SCLTIMEOUT      (1 << 25)		/*!< SCL Timeout Interrupt Status Bit */

/*
 * @brief I2C Master Control register Bit definition
 */
#define I2C_MSTCTL_MSTCONTINUE  (1 << 0)		/*!< Master Continue Bit */
#define I2C_MSTCTL_MSTSTART     (1 << 1)		/*!< Master Start Control Bit */
#define I2C_MSTCTL_MSTSTOP      (1 << 2)		/*!< Master Stop Control Bit */
#define I2C_MSTCTL_MSTDMA       (1 << 3)		/*!< Master DMA Enable Bit */

/*
 * @brief I2C Master Time Register Field definition
 */
#define I2C_MSTTIME_MSTSCLLOW   (0x07 << 0)		/*!< Master SCL Low Time field */
#define I2C_MSTTIME_MSTSCLHIGH  (0x07 << 4)		/*!< Master SCL High Time field */

/*
 * @brief I2C Master Data Mask
 */
#define I2C_MSTDAT_DATAMASK         ((uint32_t) 0x00FF << 0)	/*!< Master data mask */

/*
 * @brief I2C Slave Control register Bit definition
 */
#define I2C_SLVCTL_SLVCONTINUE    (1 << 0)		/*!< Slave Continue Bit */
#define I2C_SLVCTL_SLVNACK        (1 << 1)		/*!< Slave NACK Bit */
#define I2C_SLVCTL_SLVDMA         (1 << 3)		/*!< Slave DMA Enable Bit */

/*
 * @brief I2C Slave Data Mask
 */
#define I2C_SLVDAT_DATAMASK         ((uint32_t) 0x00FF << 0)	/*!< Slave data mask */

/*
 * @brief I2C Slave Address register Bit definition
 */
#define I2C_SLVADR_SADISABLE      (1 << 0)		/*!< Slave Address n Disable Bit */
#define I2C_SLVADR_SLVADR         (0x7F << 1)	/*!< Slave Address field */
#define I2C_SLVADR_MASK           ((uint32_t) 0x00FF)	/*!< Slave Address Mask */

/*
 * @brief I2C Slave Address Qualifier 0 Register Bit definition
 */
#define I2C_SLVQUAL_QUALMODE0     (1 << 0)		/*!< Slave Qualifier Mode Enable Bit */
#define I2C_SLVQUAL_SLVQUAL0      (0x7F << 1)	/*!< Slave Qualifier Address for Address 0 */

/*
 * @brief I2C Monitor Data Register Bit definition
 */
#define I2C_MONRXDAT_DATA         (0xFF << 0)		/*!< Monitor Function Receive Data Field */
#define I2C_MONRXDAT_MONSTART     (1 << 8)			/*!< Monitor Received Start Bit */
#define I2C_MONRXDAT_MONRESTART   (1 << 9)			/*!< Monitor Received Repeated Start Bit */
#define I2C_MONRXDAT_MONNACK      (1 << 10)			/*!< Monitor Received Nack Bit */

/*
 * @brief I2C Configuration register Bit definition
 */
#define I2C_CFG_MSTEN             (1 << 0)	/*!< Master Enable/Disable Bit */
#define I2C_CFG_SLVEN             (1 << 1)	/*!< Slave Enable/Disable Bit */
#define I2C_CFG_MONEN             (1 << 2)	/*!< Monitor Enable/Disable Bit */
#define I2C_CFG_TIMEOUTEN         (1 << 3)	/*!< Timeout Enable/Disable Bit */
#define I2C_CFG_MONCLKSTR         (1 << 4)	/*!< Monitor Clock Stretching Bit */
#define I2C_CFG_MASK              ((uint32_t) 0x1F)	/*!< Configuration Register Mask */

/*
 * @brief I2C Status register Bit definition
 */
#define I2C_STAT_MSTPENDING       (1 << 0)		/*!< Master Pending Status Bit */
#define I2C_STAT_MSTSTATE         (0x7 << 1)	/*!< Master State Code */
#define I2C_STAT_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Bit */
#define I2C_STAT_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Bit */
#define I2C_STAT_SLVPENDING       (1 << 8)		/*!< Slave Pending Status Bit */
#define I2C_STAT_SLVSTATE         (0x3 << 9)	/*!< Slave State Code */
#define I2C_STAT_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Bit */
#define I2C_STAT_SLVIDX           (0x3 << 12)	/*!< Slave Address Index */
#define I2C_STAT_SLVSEL           (1 << 14)		/*!< Slave Selected Bit */
#define I2C_STAT_SLVDESEL         (1 << 15)		/*!< Slave Deselect Bit */
#define I2C_STAT_MONRDY           (1 << 16)		/*!< Monitor Ready Bit */
#define I2C_STAT_MONOV            (1 << 17)		/*!< Monitor Overflow Flag */
#define I2C_STAT_MONACTIVE        (1 << 18)		/*!< Monitor Active Flag */
#define I2C_STAT_MONIDLE          (1 << 19)		/*!< Monitor Idle Flag */
#define I2C_STAT_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Flag */
#define I2C_STAT_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Flag */

#define I2C_STAT_MSTCODE_IDLE       (0)			/*!< Master Idle State Code */
#define I2C_STAT_MSTCODE_RXREADY    (1)			/*!< Master Receive Ready State Code */
#define I2C_STAT_MSTCODE_TXREADY    (2)			/*!< Master Transmit Ready State Code */
#define I2C_STAT_MSTCODE_NACKADR    (3)			/*!< Master NACK by slave on address State Code */
#define I2C_STAT_MSTCODE_NACKDAT    (4)			/*!< Master NACK by slave on data State Code */

#define I2C_STAT_SLVCODE_ADDR         (0)		/*!< Master Idle State Code */
#define I2C_STAT_SLVCODE_RX           (1)		/*!< Received data is available Code */
#define I2C_STAT_SLVCODE_TX           (2)		/*!< Data can be transmitted Code */

/*
 * @brief I2C Interrupt Enable Set register Bit definition
 */
#define I2C_INTENSET_MSTPENDING       (1 << 0)		/*!< Master Pending Interrupt Enable Bit */
#define I2C_INTENSET_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Interrupt Enable Bit */
#define I2C_INTENSET_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Interrupt Enable Bit */
#define I2C_INTENSET_SLVPENDING       (1 << 8)		/*!< Slave Pending Interrupt Enable Bit */
#define I2C_INTENSET_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Interrupt Enable Bit */
#define I2C_INTENSET_SLVDESEL         (1 << 15)		/*!< Slave Deselect Interrupt Enable Bit */
#define I2C_INTENSET_MONRDY           (1 << 16)		/*!< Monitor Ready Interrupt Enable Bit */
#define I2C_INTENSET_MONOV            (1 << 17)		/*!< Monitor Overflow Interrupt Enable Bit */
#define I2C_INTENSET_MONIDLE          (1 << 19)		/*!< Monitor Idle Interrupt Enable Bit */
#define I2C_INTENSET_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Enable Bit */
#define I2C_INTENSET_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Enable Bit */

/*
 * @brief I2C Interrupt Enable Clear register Bit definition
 */
#define I2C_INTENCLR_MSTPENDING       (1 << 0)		/*!< Master Pending Interrupt Clear Bit */
#define I2C_INTENCLR_MSTRARBLOSS      (1 << 4)		/*!< Master Arbitration Loss Interrupt Clear Bit */
#define I2C_INTENCLR_MSTSTSTPERR      (1 << 6)		/*!< Master Start Stop Error Interrupt Clear Bit */
#define I2C_INTENCLR_SLVPENDING       (1 << 8)		/*!< Slave Pending Interrupt Clear Bit */
#define I2C_INTENCLR_SLVNOTSTR        (1 << 11)		/*!< Slave not stretching Clock Interrupt Clear Bit */
#define I2C_INTENCLR_SLVDESEL         (1 << 15)		/*!< Slave Deselect Interrupt Clear Bit */
#define I2C_INTENCLR_MONRDY           (1 << 16)		/*!< Monitor Ready Interrupt Clear Bit */
#define I2C_INTENCLR_MONOV            (1 << 17)		/*!< Monitor Overflow Interrupt Clear Bit */
#define I2C_INTENCLR_MONIDLE          (1 << 19)		/*!< Monitor Idle Interrupt Clear Bit */
#define I2C_INTENCLR_EVENTTIMEOUT     (1 << 24)		/*!< Event Timeout Interrupt Clear Bit */
#define I2C_INTENCLR_SCLTIMEOUT       (1 << 25)		/*!< SCL Timeout Interrupt Clear Bit */

/*
 * @brief I2C TimeOut Value Macro
 */
#define I2C_TIMEOUT_VAL(n)              (((uint32_t) ((n) - 1) & 0xFFF0) | 0x000F)		/*!< Macro for Timeout value register */

/*
 * @brief I2C Interrupt Status register Bit definition
 */
#define I2C_INTSTAT_MSTPENDING      (1 << 0)		/*!< Master Pending Interrupt Status Bit */
#define I2C_INTSTAT_MSTRARBLOSS     (1 << 4)		/*!< Master Arbitration Loss Interrupt Status Bit */
#define I2C_INTSTAT_MSTSTSTPERR     (1 << 6)		/*!< Master Start Stop Error Interrupt Status Bit */
#define I2C_INTSTAT_SLVPENDING      (1 << 8)		/*!< Slave Pending Interrupt Status Bit */
#define I2C_INTSTAT_SLVNOTSTR       (1 << 11)		/*!< Slave not stretching Clock Interrupt Status Bit */
#define I2C_INTSTAT_SLVDESEL        (1 << 15)		/*!< Slave Deselect Interrupt Status Bit */
#define I2C_INTSTAT_MONRDY          (1 << 16)		/*!< Monitor Ready Interrupt Status Bit */
#define I2C_INTSTAT_MONOV           (1 << 17)		/*!< Monitor Overflow Interrupt Status Bit */
#define I2C_INTSTAT_MONIDLE         (1 << 19)		/*!< Monitor Idle Interrupt Status Bit */
#define I2C_INTSTAT_EVENTTIMEOUT    (1 << 24)		/*!< Event Timeout Interrupt Status Bit */
#define I2C_INTSTAT_SCLTIMEOUT      (1 << 25)		/*!< SCL Timeout Interrupt Status Bit */

/*
 * @brief I2C Master Control register Bit definition
 */
#define I2C_MSTCTL_MSTCONTINUE  (1 << 0)		/*!< Master Continue Bit */
#define I2C_MSTCTL_MSTSTART     (1 << 1)		/*!< Master Start Control Bit */
#define I2C_MSTCTL_MSTSTOP      (1 << 2)		/*!< Master Stop Control Bit */
#define I2C_MSTCTL_MSTDMA       (1 << 3)		/*!< Master DMA Enable Bit */

/*
 * @brief I2C Master Time Register Field definition
 */
#define I2C_MSTTIME_MSTSCLLOW   (0x07 << 0)		/*!< Master SCL Low Time field */
#define I2C_MSTTIME_MSTSCLHIGH  (0x07 << 4)		/*!< Master SCL High Time field */

/*
 * @brief I2C Master Data Mask
 */
#define I2C_MSTDAT_DATAMASK         ((uint32_t) 0x00FF << 0)	/*!< Master data mask */

/*
 * @brief I2C Slave Control register Bit definition
 */
#define I2C_SLVCTL_SLVCONTINUE    (1 << 0)		/*!< Slave Continue Bit */
#define I2C_SLVCTL_SLVNACK        (1 << 1)		/*!< Slave NACK Bit */
#define I2C_SLVCTL_SLVDMA         (1 << 3)		/*!< Slave DMA Enable Bit */

/*
 * @brief I2C Slave Data Mask
 */
#define I2C_SLVDAT_DATAMASK         ((uint32_t) 0x00FF << 0)	/*!< Slave data mask */

/*
 * @brief I2C Slave Address register Bit definition
 */
#define I2C_SLVADR_SADISABLE      (1 << 0)		/*!< Slave Address n Disable Bit */
#define I2C_SLVADR_SLVADR         (0x7F << 1)	/*!< Slave Address field */
#define I2C_SLVADR_MASK           ((uint32_t) 0x00FF)	/*!< Slave Address Mask */

/*
 * @brief I2C Slave Address Qualifier 0 Register Bit definition
 */
#define I2C_SLVQUAL_QUALMODE0     (1 << 0)		/*!< Slave Qualifier Mode Enable Bit */
#define I2C_SLVQUAL_SLVQUAL0      (0x7F << 1)	/*!< Slave Qualifier Address for Address 0 */

/*
 * @brief I2C Monitor Data Register Bit definition
 */
#define I2C_MONRXDAT_DATA         (0xFF << 0)		/*!< Monitor Function Receive Data Field */
#define I2C_MONRXDAT_MONSTART     (1 << 8)			/*!< Monitor Received Start Bit */
#define I2C_MONRXDAT_MONRESTART   (1 << 9)			/*!< Monitor Received Repeated Start Bit */
#define I2C_MONRXDAT_MONNACK      (1 << 10)			/*!< Monitor Received Nack Bit */

/**
 * @brief	Initialize I2C Interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function enables the I2C clock for both the master and
 * slave interfaces if the I2C channel.

 */
__STATIC_INLINE void Chip_I2C_Init(LPC_I2C_T *pI2C)
{
	/* Compiler must be able to optimize out non applicable cases */
	switch ((uint32_t) pI2C) {
	case LPC_I2C2_BASE:
		/* Enable clock to I2C peripheral and reset */
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_I2C2);
		Chip_SYSCON_PeriphReset(RESET_I2C2);
		return;

	case LPC_I2C1_BASE:
		/* Enable clock to I2C peripheral and reset */
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_I2C1);
		Chip_SYSCON_PeriphReset(RESET_I2C1);
		return;

	default:
		/* Enable clock to I2C peripheral and reset */
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_I2C0);
		Chip_SYSCON_PeriphReset(RESET_I2C0);
		return;
	}
}

/**
 * @brief	Shutdown I2C Interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function disables the I2C clock for both the master and
 * slave interfaces if the I2C channel.
 */
__STATIC_INLINE void Chip_I2C_DeInit(LPC_I2C_T *pI2C)
{
	/* Compiler must be able to optimize out non applicable cases */
	switch ((uint32_t) pI2C) {
	case LPC_I2C2_BASE:
		Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_I2C2);
		return;

	case LPC_I2C1_BASE:
		Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_I2C1);
		return;

	default:
		Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_I2C0);
		return;
	}
}

/**
 * @brief	Sets register bit values with mask
 * @param	pReg32	: Pointer to 32-bit register to set bits for
 * @param	mask	: Zero write mask, will mask these bits are write back as 0
 * @param	bits	: Bits to set
 * @return	Nothing
 * @note	This function is needed for read-modify-write operations where the register
 * being changed has read bits that are undefined, but must be written back as 0.
 */
static INLINE void Chip_I2C_SetRegMask(volatile uint32_t *pReg32, uint32_t mask, uint32_t bits)
{
	*pReg32 = (*pReg32 & mask) | bits;
}

/**
 * @brief	Clears register bit values with mask
 * @param	pReg32	: Pointer to 32-bit register to clear bits for
 * @param	mask	: Zero write mask, will mask these bits are write back as 0
 * @param	bits	: Bits to clear
 * @return	Nothing
 * @note	This function is needed for read-modify-write operations where the register
 * being changed has read bits that are undefined, but must be written back as 0.
 */
static INLINE void Chip_I2C_ClearRegMask(volatile uint32_t *pReg32, uint32_t mask, uint32_t bits)
{
	*pReg32 = (*pReg32 & mask) & ~bits;
}

/**
 * @brief	Sets I2C Clock Divider registers
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	clkdiv	: Clock Divider value for I2C, value is between (1 - 65536)
 * @return	Nothing
 * @note	The clock to I2C block is determined by the following formula (I2C_PCLK
 *          is the frequency of the system clock): <br>
 *              I2C Clock Frequency = (I2C_PCLK)/clkdiv;<br>
 * This divider must be setup for both the master and slave modes of the
 * controller. When used in master mode, this divider is used with the MSTTIME
 * register to generate the master I2C clock. Since the MSTTIME register has a
 * limited range of divide values, this value should be selected to give an I2C
 * clock rate that is about 2x to 18x greater than the desired master clock rate.
 */
__STATIC_INLINE void Chip_I2C_SetClockDiv(LPC_I2C_T *pI2C, uint32_t clkdiv)
{
	if ((clkdiv >= 1) && (clkdiv <= 65536)) {
		pI2C->CLKDIV = clkdiv - 1;
	}
	else {
		pI2C->CLKDIV = 0;
	}
}

/**
 * @brief	Get I2C Clock Divider registers
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Clock Divider value
 * @note	Return the divider value for the I2C block
 *          It is the CLKDIV register value + 1
 */
static INLINE uint32_t Chip_I2C_GetClockDiv(LPC_I2C_T *pI2C)
{
	return (pI2C->CLKDIV & 0xFFFF) + 1;
}

/**
 * @brief	Enable I2C Interrupts
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	intEn	: ORed Value of I2C_INTENSET_* values to enable
 * @return	Nothing
 */
static INLINE void Chip_I2C_EnableInt(LPC_I2C_T *pI2C, uint32_t intEn)
{
	pI2C->INTENSET = intEn;
}

/**
 * @brief	Disable I2C Interrupts
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	intClr	: ORed Value of I2C_INTENSET_* values to disable
 * @return	Nothing
 */
static INLINE void Chip_I2C_DisableInt(LPC_I2C_T *pI2C, uint32_t intClr)
{
	pI2C->INTENCLR = intClr;
}

/**
 * @brief	Disable I2C Interrupts
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	intClr	: ORed Value of I2C_INTENSET_* values to disable
 * @return	Nothing
 * @note	It is recommended to use the Chip_I2C_DisableInt() function
 * instead of this function.
 */
static INLINE void Chip_I2C_ClearInt(LPC_I2C_T *pI2C, uint32_t intClr)
{
	Chip_I2C_DisableInt(pI2C, intClr);
}

/**
 * @brief	Returns pending I2C Interrupts
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	All pending interrupts, mask with I2C_INTENSET_* to determine specific interrupts
 */
static INLINE uint32_t Chip_I2C_GetPendingInt(LPC_I2C_T *pI2C)
{
	return pI2C->INTSTAT;
}

/**
 * @}
 */

 #ifdef __cplusplus
}
#endif

#endif /* __I2C_COMMON_5410X_H_ */
