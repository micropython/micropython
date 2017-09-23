/*
 * @brief LPC5410X SPI common functions and definitions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2015
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

#ifndef __SPI_COMMON_5410X_H_
#define __SPI_COMMON_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SPI_COMMON_5410X CHIP: LPC5410X SPI driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief SPI register block structure
 */
typedef struct {					/*!< SPI Structure */
	__IO uint32_t  CFG;				/*!< SPI Configuration register */
	__IO uint32_t  DLY;				/*!< SPI Delay register */
	__IO uint32_t  STAT;			/*!< SPI Status register */
	__IO uint32_t  INTENSET;		/*!< SPI Interrupt Enable Set register */
	__O uint32_t  INTENCLR;		/*!< SPI Interrupt Enable Clear register */
	__I  uint32_t  RXDAT;			/*!< SPI Receive Data register */
	__IO uint32_t  TXDATCTL;		/*!< SPI Transmit Data with Control register */
	__O  uint32_t  TXDAT;			/*!< SPI Transmit Data register */
	__IO uint32_t  TXCTRL;			/*!< SPI Transmit Control register */
	__IO uint32_t  DIV;				/*!< SPI clock Divider register */
	__I  uint32_t  INTSTAT;			/*!< SPI Interrupt Status register */
} LPC_SPI_T;

/**
 * Macro defines for SPI Configuration register
 */
#define SPI_CFG_BITMASK         (0xFBD)						/** SPI register bit mask */
#define SPI_CFG_SPI_EN          (1 << 0)					/** SPI Slave Mode Select */
#define SPI_CFG_SLAVE_EN        (0 << 0)					/** SPI Master Mode Select */
#define SPI_CFG_MASTER_EN       (1 << 2)					/** SPI MSB First mode enable */
#define SPI_CFG_MSB_FIRST_EN    (0 << 3)					/** SPI LSB First mode enable */
#define SPI_CFG_LSB_FIRST_EN    (1 << 3)					/** SPI Clock Phase Select */
#define SPI_CFG_CPHA_FIRST      (0 << 4)					/** Capture data on the first edge, Change data on the following edge */
#define SPI_CFG_CPHA_SECOND     (1 << 4)					/** SPI Clock Polarity Select */
#define SPI_CFG_CPOL_LO         (0 << 5)					/** The rest state of the clock (between frames) is low. */
#define SPI_CFG_CPOL_HI         (1 << 5)					/** The rest state of the clock (between frames) is high. */
#define SPI_CFG_LBM_EN          (1 << 7)					/** SPI control 1 loopback mode enable */
#define SPI_CFG_SPOL_LO         (0 << 8)					/** SPI SSEL0 Polarity Select */
#define SPI_CFG_SPOL_HI         (1 << 8)					/** SSEL0 is active High */
#define SPI_CFG_SPOLNUM_HI(n)   (1 << ((n) + 8))			/** SSELN is active High, selects 0 - 3 */

/**
 * Macro defines for SPI Delay register
 */
#define  SPI_DLY_BITMASK            (0xFFFF)				/** SPI DLY Register Mask */
#define  SPI_DLY_PRE_DELAY(n)       (((n) & 0x0F) << 0)		/** Time in SPI clocks between SSEL assertion and the beginning of a data frame */
#define  SPI_DLY_POST_DELAY(n)      (((n) & 0x0F) << 4)		/** Time in SPI clocks between the end of a data frame and SSEL deassertion. */
#define  SPI_DLY_FRAME_DELAY(n)     (((n) & 0x0F) << 8)		/** Minimum time in SPI clocks between adjacent data frames. */
#define  SPI_DLY_TRANSFER_DELAY(n)  (((n) & 0x0F) << 12)	/** Minimum time in SPI clocks that the SSEL is deasserted between transfers. */

/**
 * Macro defines for SPI Status register
 */
#define SPI_STAT_BITMASK            (0x1FF)					/** SPI STAT Register BitMask */
#define SPI_STAT_RXRDY              (1 << 0)				/** Receiver Ready Flag */
#define SPI_STAT_TXRDY              (1 << 1)				/** Transmitter Ready Flag */
#define SPI_STAT_RXOV               (1 << 2)				/** Receiver Overrun interrupt flag */
#define SPI_STAT_TXUR               (1 << 3)				/** Transmitter Underrun interrupt flag (In Slave Mode only) */
#define SPI_STAT_SSA                (1 << 4)				/** Slave Select Assert */
#define SPI_STAT_SSD                (1 << 5)				/** Slave Select Deassert */
#define SPI_STAT_STALLED            (1 << 6)				/** Stalled status flag */
#define SPI_STAT_EOT                (1 << 7)				/** End Transfer flag */
#define SPI_STAT_MSTIDLE            (1 << 8)				/** Idle status flag */

/**
 * Macro defines for SPI Interrupt Enable read and Set register
 */
#define SPI_INTENSET_BITMASK        (0x3F)					/** SPI INTENSET Register BitMask */
#define SPI_INTENSET_RXDYEN         (1 << 0)				/** Enable Interrupt when receiver data is available */
#define SPI_INTENSET_TXDYEN         (1 << 1)				/** Enable Interrupt when the transmitter holding register is available. */
#define SPI_INTENSET_RXOVEN         (1 << 2)				/**  Enable Interrupt when a receiver overrun occurs */
#define SPI_INTENSET_TXUREN         (1 << 3)				/**  Enable Interrupt when a transmitter underrun occurs (In Slave Mode Only)*/
#define SPI_INTENSET_SSAEN          (1 << 4)				/**  Enable Interrupt when the Slave Select is asserted.*/
#define SPI_INTENSET_SSDEN          (1 << 5)				/**  Enable Interrupt when the Slave Select is deasserted..*/

/**
 * Macro defines for SPI Interrupt Enable Clear register
 */
#define SPI_INTENCLR_BITMASK        (0x3F)					/** SPI INTENCLR Register BitMask */
#define SPI_INTENCLR_RXDYEN         (1 << 0)				/** Disable Interrupt when receiver data is available */
#define SPI_INTENCLR_TXDYEN         (1 << 1)				/** Disable Interrupt when the transmitter holding register is available. */
#define SPI_INTENCLR_RXOVEN         (1 << 2)				/** Disable Interrupt when a receiver overrun occurs */
#define SPI_INTENCLR_TXUREN         (1 << 3)				/** Disable Interrupt when a transmitter underrun occurs (In Slave Mode Only) */
#define SPI_INTENCLR_SSAEN          (1 << 4)				/** Disable Interrupt when the Slave Select is asserted. */
#define SPI_INTENCLR_SSDEN          (1 << 5)				/** Disable Interrupt when the Slave Select is deasserted.. */

/**
 * Macro defines for SPI Receiver Data register
 */
#define SPI_RXDAT_BITMASK           (0x1FFFFF)				/** SPI RXDAT Register BitMask */
#define SPI_RXDAT_DATA(n)           ((n) & 0xFFFF)			/** Receiver Data  */
#define SPI_RXDAT_RXSSELN_ACTIVE    (0 << 16)				/** The state of SSEL pin is active */
#define SPI_RXDAT_RXSSELN_INACTIVE  ((1 << 16)				/** The state of SSEL pin is inactive */
#define SPI_RXDAT_RXSSELNUM_INACTIVE(n) (1 << ((n) + 16))	/** The state of SSELN pin is inactive */
#define SPI_RXDAT_SOT               (1 << 20)				/** Start of Transfer flag  */

/**
 * Macro defines for SPI Transmitter Data and Control register
 */
#define SPI_TXDATCTL_BITMASK        (0xF7FFFFF)				/** SPI TXDATCTL Register BitMask */
#define SPI_TXDATCTL_DATA(n)        ((n) & 0xFFFF)			/** SPI Transmit Data */
#define SPI_TXDATCTL_CTRLMASK       (0xF7F0000)				/** SPI TXDATCTL Register BitMask for control bits only */
#define SPI_TXDATCTL_ASSERT_SSEL    (0 << 16)				/** Assert SSEL0 pin */
#define SPI_TXDATCTL_DEASSERT_SSEL  (1 << 16)				/** Deassert SSEL0 pin */
#define SPI_TXDATCTL_DEASSERTNUM_SSEL(n)    (1 << ((n) + 16))	/** Deassert SSELN pin */
#define SPI_TXDATCTL_DEASSERT_ALL   (0xF << 16)				/** Deassert all SSEL pins */
#define SPI_TXDATCTL_EOT            (1 << 20)				/** End of Transfer flag (TRANSFER_DELAY is applied after sending the current frame) */
#define SPI_TXDATCTL_EOF            (1 << 21)				/** End of Frame flag (FRAME_DELAY is applied after sending the current part) */
#define SPI_TXDATCTL_RXIGNORE       (1 << 22)				/** Receive Ignore Flag */
#define SPI_TXDATCTL_FLEN(n)        (((n) & 0x0F) << 24)	/** Frame length - 1 */

/**
 * Macro defines for SPI Transmitter Data Register
 */
#define SPI_TXDAT_DATA(n)           ((n) & 0xFFFF)			/** SPI Transmit Data */

/**
 * Macro defines for SPI Transmitter Control register
 */
#define SPI_TXCTL_BITMASK           (0xF7F0000)				/** SPI TXDATCTL Register BitMask */
#define SPI_TXCTL_ASSERT_SSEL       (0 << 16)				/** Assert SSEL0 pin */
#define SPI_TXCTL_DEASSERT_SSEL     (1 << 16)				/** Deassert SSEL0 pin */
#define SPI_TXCTL_DEASSERTNUM_SSEL(n)   (1 << ((n) + 16))	/** Deassert SSELN pin */
#define SPI_TXDATCTL_DEASSERT_ALL   (0xF << 16)				/** Deassert all SSEL pins */
#define SPI_TXCTL_EOT               (1 << 20)				/** End of Transfer flag (TRANSFER_DELAY is applied after sending the current frame) */
#define SPI_TXCTL_EOF               (1 << 21)				/** End of Frame flag (FRAME_DELAY is applied after sending the current part) */
#define SPI_TXCTL_RXIGNORE          (1 << 22)				/** Receive Ignore Flag */
#define SPI_TXCTL_FLEN(n)           ((((n) - 1) & 0x0F) << 24)	/** Frame length, 0 - 16 */
#define SPI_TXCTL_FLENMASK          (0xF << 24)				/** Frame length mask */

/**
 * Macro defines for SPI Divider register
 */
#define SPI_DIV_VAL(n)          ((n) & 0xFFFF)				/** Rate divider value mask (In Master Mode only)*/

/**
 * Macro defines for SPI Interrupt Status register
 */
#define SPI_INTSTAT_BITMASK         (0x3F)					/** SPI INTSTAT Register Bitmask */
#define SPI_INTSTAT_RXRDY           (1 << 0)				/** Receiver Ready Flag */
#define SPI_INTSTAT_TXRDY           (1 << 1)				/** Transmitter Ready Flag */
#define SPI_INTSTAT_RXOV            (1 << 2)				/** Receiver Overrun interrupt flag */
#define SPI_INTSTAT_TXUR            (1 << 3)				/** Transmitter Underrun interrupt flag (In Slave Mode only) */
#define SPI_INTSTAT_SSA             (1 << 4)				/** Slave Select Assert */
#define SPI_INTSTAT_SSD             (1 << 5)				/** Slave Select Deassert */

/** @brief SPI Clock Mode*/
typedef enum {
	ROM_SPI_CLOCK_CPHA0_CPOL0 = 0,						/**< CPHA = 0, CPOL = 0 */
	ROM_SPI_CLOCK_MODE0 = ROM_SPI_CLOCK_CPHA0_CPOL0,	/**< Alias for CPHA = 0, CPOL = 0 */
	ROM_SPI_CLOCK_CPHA1_CPOL0 = 1,						/**< CPHA = 0, CPOL = 1 */
	ROM_SPI_CLOCK_MODE1 = ROM_SPI_CLOCK_CPHA1_CPOL0,	/**< Alias for CPHA = 0, CPOL = 1 */
	ROM_SPI_CLOCK_CPHA0_CPOL1 = 2,						/**< CPHA = 1, CPOL = 0 */
	ROM_SPI_CLOCK_MODE2 = ROM_SPI_CLOCK_CPHA0_CPOL1,	/**< Alias for CPHA = 1, CPOL = 0 */
	ROM_SPI_CLOCK_CPHA1_CPOL1 = 3,						/**< CPHA = 1, CPOL = 1 */
	ROM_SPI_CLOCK_MODE3 = ROM_SPI_CLOCK_CPHA1_CPOL1,	/**< Alias for CPHA = 1, CPOL = 1 */
} ROM_SPI_CLOCK_MODE_T;

/**
 * Macro defines for SPI Configuration register
 */
#define SPI_CFG_BITMASK         (0xFBD)						/** SPI register bit mask */
#define SPI_CFG_SPI_EN          (1 << 0)					/** SPI Slave Mode Select */
#define SPI_CFG_SLAVE_EN        (0 << 0)					/** SPI Master Mode Select */
#define SPI_CFG_MASTER_EN       (1 << 2)					/** SPI MSB First mode enable */
#define SPI_CFG_MSB_FIRST_EN    (0 << 3)					/** SPI LSB First mode enable */
#define SPI_CFG_LSB_FIRST_EN    (1 << 3)					/** SPI Clock Phase Select */
#define SPI_CFG_CPHA_FIRST      (0 << 4)					/** Capture data on the first edge, Change data on the following edge */
#define SPI_CFG_CPHA_SECOND     (1 << 4)					/** SPI Clock Polarity Select */
#define SPI_CFG_CPOL_LO         (0 << 5)					/** The rest state of the clock (between frames) is low. */
#define SPI_CFG_CPOL_HI         (1 << 5)					/** The rest state of the clock (between frames) is high. */
#define SPI_CFG_LBM_EN          (1 << 7)					/** SPI control 1 loopback mode enable */
#define SPI_CFG_SPOL_LO         (0 << 8)					/** SPI SSEL0 Polarity Select */
#define SPI_CFG_SPOL_HI         (1 << 8)					/** SSEL0 is active High */
#define SPI_CFG_SPOLNUM_HI(n)   (1 << ((n) + 8))			/** SSELN is active High, selects 0 - 3 */

/**
 * Macro defines for SPI Delay register
 */
#define  SPI_DLY_BITMASK            (0xFFFF)				/** SPI DLY Register Mask */
#define  SPI_DLY_PRE_DELAY(n)       (((n) & 0x0F) << 0)		/** Time in SPI clocks between SSEL assertion and the beginning of a data frame */
#define  SPI_DLY_POST_DELAY(n)      (((n) & 0x0F) << 4)		/** Time in SPI clocks between the end of a data frame and SSEL deassertion. */
#define  SPI_DLY_FRAME_DELAY(n)     (((n) & 0x0F) << 8)		/** Minimum time in SPI clocks between adjacent data frames. */
#define  SPI_DLY_TRANSFER_DELAY(n)  (((n) & 0x0F) << 12)	/** Minimum time in SPI clocks that the SSEL is deasserted between transfers. */

/**
 * Macro defines for SPI Status register
 */
#define SPI_STAT_BITMASK            (0x1FF)					/** SPI STAT Register BitMask */
#define SPI_STAT_RXRDY              (1 << 0)				/** Receiver Ready Flag */
#define SPI_STAT_TXRDY              (1 << 1)				/** Transmitter Ready Flag */
#define SPI_STAT_RXOV               (1 << 2)				/** Receiver Overrun interrupt flag */
#define SPI_STAT_TXUR               (1 << 3)				/** Transmitter Underrun interrupt flag (In Slave Mode only) */
#define SPI_STAT_SSA                (1 << 4)				/** Slave Select Assert */
#define SPI_STAT_SSD                (1 << 5)				/** Slave Select Deassert */
#define SPI_STAT_STALLED            (1 << 6)				/** Stalled status flag */
#define SPI_STAT_EOT                (1 << 7)				/** End Transfer flag */
#define SPI_STAT_MSTIDLE            (1 << 8)				/** Idle status flag */

/**
 * Macro defines for SPI Interrupt Enable read and Set register
 */
#define SPI_INTENSET_BITMASK        (0x3F)					/** SPI INTENSET Register BitMask */
#define SPI_INTENSET_RXDYEN         (1 << 0)				/** Enable Interrupt when receiver data is available */
#define SPI_INTENSET_TXDYEN         (1 << 1)				/** Enable Interrupt when the transmitter holding register is available. */
#define SPI_INTENSET_RXOVEN         (1 << 2)				/**  Enable Interrupt when a receiver overrun occurs */
#define SPI_INTENSET_TXUREN         (1 << 3)				/**  Enable Interrupt when a transmitter underrun occurs (In Slave Mode Only)*/
#define SPI_INTENSET_SSAEN          (1 << 4)				/**  Enable Interrupt when the Slave Select is asserted.*/
#define SPI_INTENSET_SSDEN          (1 << 5)				/**  Enable Interrupt when the Slave Select is deasserted..*/

/**
 * Macro defines for SPI Interrupt Enable Clear register
 */
#define SPI_INTENCLR_BITMASK        (0x3F)					/** SPI INTENCLR Register BitMask */
#define SPI_INTENCLR_RXDYEN         (1 << 0)				/** Disable Interrupt when receiver data is available */
#define SPI_INTENCLR_TXDYEN         (1 << 1)				/** Disable Interrupt when the transmitter holding register is available. */
#define SPI_INTENCLR_RXOVEN         (1 << 2)				/** Disable Interrupt when a receiver overrun occurs */
#define SPI_INTENCLR_TXUREN         (1 << 3)				/** Disable Interrupt when a transmitter underrun occurs (In Slave Mode Only) */
#define SPI_INTENCLR_SSAEN          (1 << 4)				/** Disable Interrupt when the Slave Select is asserted. */
#define SPI_INTENCLR_SSDEN          (1 << 5)				/** Disable Interrupt when the Slave Select is deasserted.. */

/**
 * Macro defines for SPI Receiver Data register
 */
#define SPI_RXDAT_BITMASK           (0x1FFFFF)				/** SPI RXDAT Register BitMask */
#define SPI_RXDAT_DATA(n)           ((n) & 0xFFFF)			/** Receiver Data  */
#define SPI_RXDAT_RXSSELN_ACTIVE    (0 << 16)				/** The state of SSEL pin is active */
#define SPI_RXDAT_RXSSELN_INACTIVE  ((1 << 16)				/** The state of SSEL pin is inactive */
#define SPI_RXDAT_RXSSELNUM_INACTIVE(n) (1 << ((n) + 16))	/** The state of SSELN pin is inactive */
#define SPI_RXDAT_SOT               (1 << 20)				/** Start of Transfer flag  */

/**
 * Macro defines for SPI Transmitter Data and Control register
 */
#define SPI_TXDATCTL_BITMASK        (0xF7FFFFF)				/** SPI TXDATCTL Register BitMask */
#define SPI_TXDATCTL_DATA(n)        ((n) & 0xFFFF)			/** SPI Transmit Data */
#define SPI_TXDATCTL_CTRLMASK       (0xF7F0000)				/** SPI TXDATCTL Register BitMask for control bits only */
#define SPI_TXDATCTL_ASSERT_SSEL    (0 << 16)				/** Assert SSEL0 pin */
#define SPI_TXDATCTL_DEASSERT_SSEL  (1 << 16)				/** Deassert SSEL0 pin */
#define SPI_TXDATCTL_DEASSERTNUM_SSEL(n)    (1 << ((n) + 16))	/** Deassert SSELN pin */
#define SPI_TXDATCTL_DEASSERT_ALL   (0xF << 16)				/** Deassert all SSEL pins */
#define SPI_TXDATCTL_EOT            (1 << 20)				/** End of Transfer flag (TRANSFER_DELAY is applied after sending the current frame) */
#define SPI_TXDATCTL_EOF            (1 << 21)				/** End of Frame flag (FRAME_DELAY is applied after sending the current part) */
#define SPI_TXDATCTL_RXIGNORE       (1 << 22)				/** Receive Ignore Flag */
#define SPI_TXDATCTL_FLEN(n)        (((n) & 0x0F) << 24)	/** Frame length - 1 */

/**
 * Macro defines for SPI Transmitter Data Register
 */
#define SPI_TXDAT_DATA(n)           ((n) & 0xFFFF)			/** SPI Transmit Data */

/**
 * Macro defines for SPI Transmitter Control register
 */
#define SPI_TXCTL_BITMASK           (0xF7F0000)				/** SPI TXDATCTL Register BitMask */
#define SPI_TXCTL_ASSERT_SSEL       (0 << 16)				/** Assert SSEL0 pin */
#define SPI_TXCTL_DEASSERT_SSEL     (1 << 16)				/** Deassert SSEL0 pin */
#define SPI_TXCTL_DEASSERTNUM_SSEL(n)   (1 << ((n) + 16))	/** Deassert SSELN pin */
#define SPI_TXDATCTL_DEASSERT_ALL   (0xF << 16)				/** Deassert all SSEL pins */
#define SPI_TXCTL_EOT               (1 << 20)				/** End of Transfer flag (TRANSFER_DELAY is applied after sending the current frame) */
#define SPI_TXCTL_EOF               (1 << 21)				/** End of Frame flag (FRAME_DELAY is applied after sending the current part) */
#define SPI_TXCTL_RXIGNORE          (1 << 22)				/** Receive Ignore Flag */
#define SPI_TXCTL_FLEN(n)           ((((n) - 1) & 0x0F) << 24)	/** Frame length, 0 - 16 */
#define SPI_TXCTL_FLENMASK          (0xF << 24)				/** Frame length mask */

/**
 * Macro defines for SPI Divider register
 */
#define SPI_DIV_VAL(n)          ((n) & 0xFFFF)				/** Rate divider value mask (In Master Mode only)*/

/**
 * Macro defines for SPI Interrupt Status register
 */
#define SPI_INTSTAT_BITMASK         (0x3F)					/** SPI INTSTAT Register Bitmask */
#define SPI_INTSTAT_RXRDY           (1 << 0)				/** Receiver Ready Flag */
#define SPI_INTSTAT_TXRDY           (1 << 1)				/** Transmitter Ready Flag */
#define SPI_INTSTAT_RXOV            (1 << 2)				/** Receiver Overrun interrupt flag */
#define SPI_INTSTAT_TXUR            (1 << 3)				/** Transmitter Underrun interrupt flag (In Slave Mode only) */
#define SPI_INTSTAT_SSA             (1 << 4)				/** Slave Select Assert */
#define SPI_INTSTAT_SSD             (1 << 5)				/** Slave Select Deassert */

/**
 * @brief	Set SPI CFG register values
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	bits	: CFG register bits to set, amd OR'ed value of SPI_CFG_* definitions
 * @return	Nothing
 * @note	This function safely sets only the selected bits in the SPI CFG register.
 * It can be used to enable multiple bits at once.
 */
__STATIC_INLINE void Chip_SPI_SetCFGRegBits(LPC_SPI_T *pSPI, uint32_t bits)
{
	/* Mask off bits that are write as 0, read as undefined */
	pSPI->CFG = (pSPI->CFG | bits) & SPI_CFG_BITMASK;
}

/**
 * @brief	Clear SPI CFG register values
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	bits	: CFG register bits to clear, amd OR'ed value of SPI_CFG_* definitions
 * @return	Nothing
 * @note	This function safely clears only the selected bits in the SPI CFG register.
 * It can be used to disable multiple bits at once.
 */
__STATIC_INLINE void Chip_SPI_ClearCFGRegBits(LPC_SPI_T *pSPI, uint32_t bits)
{
	/* Mask off bits that are write as 0, read as undefined */
	pSPI->CFG = pSPI->CFG & (SPI_CFG_BITMASK & ~bits);
}

/**
 * @brief	Enable SPI peripheral
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_Enable(LPC_SPI_T *pSPI)
{
	Chip_SPI_SetCFGRegBits(pSPI, SPI_CFG_SPI_EN);
}

/**
 * @brief	Disable SPI peripheral
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_Disable(LPC_SPI_T *pSPI)
{
	Chip_SPI_ClearCFGRegBits(pSPI, SPI_CFG_SPI_EN);
}

/**
 * @brief   Initialize the SPI
 * @param	pSPI	: The base SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_Init(LPC_SPI_T *pSPI)
{
	if (pSPI == LPC_SPI1) {
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_SPI1);
		Chip_SYSCON_PeriphReset(RESET_SPI1);
	}
	else {
		Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_SPI0);
		Chip_SYSCON_PeriphReset(RESET_SPI0);
	}
}

/**
 * @brief	Disable SPI operation
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note	The SPI controller is disabled.
 */
__STATIC_INLINE void Chip_SPI_DeInit(LPC_SPI_T *pSPI)
{
	Chip_SPI_Disable(pSPI);
	if (pSPI == LPC_SPI1) {
		Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_SPI1);
	}
	else {
		Chip_Clock_DisablePeriphClock(SYSCON_CLOCK_SPI0);
	}
}

/**
 * @brief	Enable SPI master mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note SPI slave mode will be disabled with this call. All SPI SSEL
 * lines will also be deasserted.
 */
__STATIC_INLINE void Chip_SPI_EnableMasterMode(LPC_SPI_T *pSPI)
{
	Chip_SPI_SetCFGRegBits(pSPI, SPI_CFG_MASTER_EN);

	/* Deassert all chip selects, only in master mode */
	pSPI->TXCTRL = SPI_TXDATCTL_DEASSERT_ALL;
}

/**
 * @brief	Enable SPI slave mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note SPI master mode will be disabled with this call.
 */
__STATIC_INLINE void Chip_SPI_EnableSlaveMode(LPC_SPI_T *pSPI)
{
	Chip_SPI_ClearCFGRegBits(pSPI, SPI_CFG_MASTER_EN);
}

/**
 * @brief	Enable LSB First transfers
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_EnableLSBFirst(LPC_SPI_T *pSPI)
{
	Chip_SPI_SetCFGRegBits(pSPI, SPI_CFG_LSB_FIRST_EN);
}

/**
 * @brief	Enable MSB First transfers
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_EnableMSBFirst(LPC_SPI_T *pSPI)
{
	Chip_SPI_ClearCFGRegBits(pSPI, SPI_CFG_LSB_FIRST_EN);
}

/** @brief SPI Clock Mode*/
typedef enum IP_SPI_CLOCK_MODE {
	SPI_CLOCK_CPHA0_CPOL0 = SPI_CFG_CPOL_LO | SPI_CFG_CPHA_FIRST,		/**< CPHA = 0, CPOL = 0 */
	SPI_CLOCK_MODE0 = SPI_CLOCK_CPHA0_CPOL0,							/**< Alias for CPHA = 0, CPOL = 0 */
	SPI_CLOCK_CPHA1_CPOL0 = SPI_CFG_CPOL_LO | SPI_CFG_CPHA_SECOND,		/**< CPHA = 0, CPOL = 1 */
	SPI_CLOCK_MODE1 = SPI_CLOCK_CPHA1_CPOL0,							/**< Alias for CPHA = 0, CPOL = 1 */
	SPI_CLOCK_CPHA0_CPOL1 = SPI_CFG_CPOL_HI | SPI_CFG_CPHA_FIRST,		/**< CPHA = 1, CPOL = 0 */
	SPI_CLOCK_MODE2 = SPI_CLOCK_CPHA0_CPOL1,							/**< Alias for CPHA = 1, CPOL = 0 */
	SPI_CLOCK_CPHA1_CPOL1 = SPI_CFG_CPOL_HI | SPI_CFG_CPHA_SECOND,		/**< CPHA = 1, CPOL = 1 */
	SPI_CLOCK_MODE3 = SPI_CLOCK_CPHA1_CPOL1,							/**< Alias for CPHA = 1, CPOL = 1 */
} SPI_CLOCK_MODE_T;

/**
 * @brief	Set SPI mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	mode	: SPI mode to set the SPI interface to
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_SetSPIMode(LPC_SPI_T *pSPI, SPI_CLOCK_MODE_T mode)
{
	Chip_SPI_ClearCFGRegBits(pSPI, (SPI_CFG_CPOL_HI | SPI_CFG_CPHA_SECOND));
	Chip_SPI_SetCFGRegBits(pSPI, (uint32_t) mode);
}

/**
 * @brief	Set polarity on the SPI chip select high
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	csNum	: Chip select number, 0 - 3
 * @return	Nothing
 * @note	SPI chip select polarity is active high.
 */
__STATIC_INLINE void Chip_SPI_SetCSPolHigh(LPC_SPI_T *pSPI, uint8_t csNum)
{
	Chip_SPI_SetCFGRegBits(pSPI, SPI_CFG_SPOLNUM_HI(csNum));
}

/**
 * @brief	Set polarity on the SPI chip select low
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	csNum	: Chip select number, 0 - 3
 * @return	Nothing
 * @note	SPI chip select polarity is active low.
 */
__STATIC_INLINE void Chip_SPI_SetCSPolLow(LPC_SPI_T *pSPI, uint8_t csNum)
{
	Chip_SPI_ClearCFGRegBits(pSPI, SPI_CFG_SPOLNUM_HI(csNum));
}

/** SPI configuration structure used for setting up master/slave mode, LSB or
 * MSB first, and SPI mode in a single function call. */
typedef struct {
	uint32_t master             : 8;	/* Set to non-0 value to use master mode, 0 for slave */
	uint32_t lsbFirst           : 8;	/* Set to non-0 value to send LSB first, 0 for MSB first */
	SPI_CLOCK_MODE_T mode       : 8;	/* Mode selection */
	uint32_t reserved           : 8;	/* Reserved, for alignment only */
} SPI_CFGSETUP_T;

/**
 * @brief	Setup SPI configuration
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	pCFG	: Pointer to SPI configuration structure
 * @return	Nothing
 */
void Chip_SPI_ConfigureSPI(LPC_SPI_T *pSPI, SPI_CFGSETUP_T *pCFG);

/**
 * @brief	Get the current status of SPI controller
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	SPI Status (Or-ed bit value of SPI_STAT_*)
 * @note	Mask the return value with a value of type SPI_STAT_* to determine
 * if that status is active.
 */
__STATIC_INLINE uint32_t Chip_SPI_GetStatus(LPC_SPI_T *pSPI)
{
	return pSPI->STAT;
}

/**
 * @brief	Clear SPI status
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	Flag	: Clear Flag (Or-ed bit value of SPI_STAT_*)
 * @return	Nothing
 * @note	Only SPI_STAT_RXOV, SPI_STAT_TXUR, SPI_STAT_SSA, and
 * SPI_STAT_SSD statuses can be cleared.
 */
__STATIC_INLINE void Chip_SPI_ClearStatus(LPC_SPI_T *pSPI, uint32_t Flag)
{
	pSPI->STAT = Flag;
}

/**
 * @brief	Enable a SPI interrupt
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	intMask	: Or'ed value of SPI_INTENSET_* values to enable
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_EnableInts(LPC_SPI_T *pSPI, uint32_t intMask)
{
	pSPI->INTENSET = intMask;
}

/**
 * @brief	Disable a SPI interrupt
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	intMask	: Or'ed value of SPI_INTENCLR_* values to disable
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_DisableInts(LPC_SPI_T *pSPI, uint32_t intMask)
{
	pSPI->INTENCLR = intMask;
}

/**
 * @brief	Return enabled SPI interrupts
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	An Or'ed value of SPI_INTENSET_* values
 * @note	Mask the return value with a SPI_INTENSET_* value to determine
 * if the interrupt is enabled.
 */
__STATIC_INLINE uint32_t Chip_SPI_GetEnabledInts(LPC_SPI_T *pSPI)
{
	return pSPI->INTENSET;
}

/**
 * @brief	Return pending SPI interrupts
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	An Or'ed value of SPI_INTSTAT_* values
 * @note	Mask the return value with a SPI_INTSTAT_* value to determine
 * if the interrupt is pending.
 */
__STATIC_INLINE uint32_t Chip_SPI_GetPendingInts(LPC_SPI_T *pSPI)
{
	return pSPI->INTSTAT;
}

/**
 * @brief	Flush FIFOs
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_FlushFifos(LPC_SPI_T *pSPI)
{
	Chip_SPI_Disable(pSPI);
	Chip_SPI_Enable(pSPI);
}

/**
 * @brief	Read raw data from receive FIFO with status bits
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Current value in receive data FIFO plus status bits
 */
__STATIC_INLINE uint32_t Chip_SPI_ReadRawRXFifo(LPC_SPI_T *pSPI)
{
	return pSPI->RXDAT;
}

/**
 * @brief	Read data from receive FIFO masking off status bits
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Current value in receive data FIFO
 * @note	The return value is masked with 0xFFFF to not exceed 16-bits. All
 * other status bits are thrown away. This register should only be read if it
 * has data in it. This function is useful for systems that don't need SPI
 * select (SSEL) monitoring.
 */
__STATIC_INLINE uint32_t Chip_SPI_ReadRXData(LPC_SPI_T *pSPI)
{
	return pSPI->RXDAT & 0xFFFF;
}

/**
 * @brief	Write data to transmit FIFO
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	data	: Data to write
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_WriteTXData(LPC_SPI_T *pSPI, uint16_t data)
{
	pSPI->TXDAT = (uint32_t) data;
}

/**
 * @brief	Set SPI TXCTRL register control options
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	bits	: TXCTRL register bits to set, amd OR'ed value of SPI_TXDATCTL_* definitions
 * @return	Nothing
 * @note	This function safely sets only the selected bits in the SPI TXCTRL register.
 * It can be used to enable multiple bits at once.
 */
__STATIC_INLINE void Chip_SPI_SetTXCTRLRegBits(LPC_SPI_T *pSPI, uint32_t bits)
{
	pSPI->TXCTRL = (pSPI->TXCTRL | bits) & SPI_TXDATCTL_CTRLMASK;
}

/**
 * @brief	Clear SPI TXCTRL register control options
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	bits	: TXCTRL register bits to clear, amd OR'ed value of SPI_TXDATCTL_* definitions
 * @return	Nothing
 * @note	This function safely clears only the selected bits in the SPI TXCTRL register.
 * It can be used to disable multiple bits at once.
 */
__STATIC_INLINE void Chip_SPI_ClearTXCTRLRegBits(LPC_SPI_T *pSPI, uint32_t bits)
{
	pSPI->TXCTRL = pSPI->TXCTRL & (SPI_TXDATCTL_CTRLMASK & ~bits);
}

/**
 * @brief	Set TX control options (safe)
 * @param	pSPI		: The base of SPI peripheral on the chip
 * @param	ctrlBits	: Or'ed control bits to set
 * @return	Nothing
 * @note	Selectable control states include SPI_TXCTL_DEASSERTNUM_SSEL(0/1/2/3),
 * SPI_TXCTL_EOT, SPI_TXCTL_EOF, SPI_TXCTL_RXIGNORE, and SPI_TXCTL_FLEN(bits).
 */
__STATIC_INLINE void Chip_SPI_SetTXCtl(LPC_SPI_T *pSPI, uint32_t ctrlBits)
{
	Chip_SPI_SetTXCTRLRegBits(pSPI, ctrlBits);
}

/**
 * @brief	Clear TX control options (safe)
 * @param	pSPI		: The base of SPI peripheral on the chip
 * @param	ctrlBits	: Or'ed control bits to clear
 * @return	Nothing
 * @note	Selectable control states include SPI_TXCTL_DEASSERTNUM_SSEL(0/1/2/3),
 * SPI_TXCTL_EOT, SPI_TXCTL_EOF, SPI_TXCTL_RXIGNORE, and SPI_TXCTL_FLEN(bits).
 */
__STATIC_INLINE void Chip_SPI_ClearTXCtl(LPC_SPI_T *pSPI, uint32_t ctrlBits)
{
	Chip_SPI_ClearTXCTRLRegBits(pSPI, ctrlBits);
}

/**
 * @brief	Set TX data transfer size in bits
 * @param	pSPI		: The base of SPI peripheral on the chip
 * @param	ctrlBits	: Number of bits to transmit and receive, must be 1 to 16
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPI_SetXferSize(LPC_SPI_T *pSPI, uint32_t ctrlBits)
{
	Chip_SPI_ClearTXCTRLRegBits(pSPI, SPI_TXCTL_FLENMASK);
	Chip_SPI_SetTXCTRLRegBits(pSPI, SPI_TXCTL_FLEN(ctrlBits));
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_COMMON_5410X_H_ */
