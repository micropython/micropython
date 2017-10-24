/*
 * @brief LPC43xx SPI driver
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

#ifndef __SPI_43XX_H_
#define __SPI_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SPI_43XX CHIP: LPC43xx SPI driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * This module is present in LPC43xx MCUs only.
 * @{
 */
#if defined(CHIP_LPC43XX)

/**
 * @brief SPI register block structure
 */
typedef struct {					/*!< SPI Structure          */
	__IO uint32_t  CR;				/*!< SPI Control Register. This register controls the operation of the SPI. */
	__I  uint32_t  SR;				/*!< SPI Status Register. This register shows the status of the SPI. */
	__IO uint32_t  DR;				/*!< SPI Data Register. This bi-directional register provides the transmit and receive data for the SPI. Transmit data is provided to the SPI0 by writing to this register. Data received by the SPI0 can be read from this register. */
	__IO uint32_t  CCR;				/*!< SPI Clock Counter Register. This register controls the frequency of a master's SCK0. */
	__I  uint32_t  RESERVED0[3];
	__IO uint32_t  INT;				/*!< SPI Interrupt Flag. This register contains the interrupt flag for the SPI interface. */
} LPC_SPI_T;

/*
 * Macro defines for SPI Control register
 */
/* SPI CFG Register BitMask */
#define SPI_CR_BITMASK       ((uint32_t) 0xFFC)
/** Enable of controlling the number of bits per transfer  */
#define SPI_CR_BIT_EN         ((uint32_t) (1 << 2))
/** Mask of field of bit controlling */
#define SPI_CR_BITS_MASK      ((uint32_t) 0xF00)
/** Set the number of bits per a transfer */
#define SPI_CR_BITS(n)        ((uint32_t) ((n << 8) & 0xF00))	/* n is in range 8-16 */
/** SPI Clock Phase Select*/
#define SPI_CR_CPHA_FIRST     ((uint32_t) (0))	/*Capture data on the first edge, Change data on the following edge*/
#define SPI_CR_CPHA_SECOND    ((uint32_t) (1 << 3))	/*Change data on the first edge, Capture data on the following edge*/
/** SPI Clock Polarity Select*/
#define SPI_CR_CPOL_LO        ((uint32_t) (0))	/* The rest state of the clock (between frames) is low.*/
#define SPI_CR_CPOL_HI        ((uint32_t) (1 << 4))	/* The rest state of the clock (between frames) is high.*/
/** SPI Slave Mode Select */
#define SPI_CR_SLAVE_EN       ((uint32_t) 0)
/** SPI Master Mode Select */
#define SPI_CR_MASTER_EN      ((uint32_t) (1 << 5))
/** SPI MSB First mode enable */
#define SPI_CR_MSB_FIRST_EN   ((uint32_t) 0)	/*Data will be transmitted and received in standard order (MSB first).*/
/** SPI LSB First mode enable */
#define SPI_CR_LSB_FIRST_EN   ((uint32_t) (1 << 6))	/*Data will be transmitted and received in reverse order (LSB first).*/
/** SPI interrupt enable */
#define SPI_CR_INT_EN         ((uint32_t) (1 << 7))

/*
 * Macro defines for SPI Status register
 */
/** SPI STAT Register BitMask */
#define SPI_SR_BITMASK        ((uint32_t) 0xF8)
/** Slave abort Flag */
#define SPI_SR_ABRT           ((uint32_t) (1 << 3))	/* When 1, this bit indicates that a slave abort has occurred. */
/* Mode fault Flag */
#define SPI_SR_MODF           ((uint32_t) (1 << 4))	/* when 1, this bit indicates that a Mode fault error has occurred. */
/** Read overrun flag*/
#define SPI_SR_ROVR           ((uint32_t) (1 << 5))	/* When 1, this bit indicates that a read overrun has occurred. */
/** Write collision flag. */
#define SPI_SR_WCOL           ((uint32_t) (1 << 6))	/* When 1, this bit indicates that a write collision has occurred.. */
/** SPI transfer complete flag. */
#define SPI_SR_SPIF           ((uint32_t) (1 << 7))		/* When 1, this bit indicates when a SPI data transfer is complete.. */
/** SPI error flag */
#define SPI_SR_ERROR          (SPI_SR_ABRT | SPI_SR_MODF | SPI_SR_ROVR | SPI_SR_WCOL)
/*
 * Macro defines for SPI Test Control Register register
 */
/*Enable SPI Test Mode */
#define SPI_TCR_TEST(n)       ((uint32_t) ((n & 0x3F) << 1))

/*
 * Macro defines for SPI Interrupt register
 */
/** SPI interrupt flag */
#define SPI_INT_SPIF          ((uint32_t) (1 << 0))

/**
 * Macro defines for SPI Data register
 */
/** Receiver Data  */
#define SPI_DR_DATA(n)        ((uint32_t) ((n) & 0xFFFF))

/** @brief SPI Mode*/
typedef enum {
	SPI_MODE_MASTER = SPI_CR_MASTER_EN,			/* Master Mode */
	SPI_MODE_SLAVE = SPI_CR_SLAVE_EN,			/* Slave Mode */
} SPI_MODE_T;

/** @brief SPI Clock Mode*/
typedef enum {
	SPI_CLOCK_CPHA0_CPOL0 = SPI_CR_CPOL_LO | SPI_CR_CPHA_FIRST,		/**< CPHA = 0, CPOL = 0 */
	SPI_CLOCK_CPHA0_CPOL1 = SPI_CR_CPOL_HI | SPI_CR_CPHA_FIRST,		/**< CPHA = 0, CPOL = 1 */
	SPI_CLOCK_CPHA1_CPOL0 = SPI_CR_CPOL_LO | SPI_CR_CPHA_SECOND,	/**< CPHA = 1, CPOL = 0 */
	SPI_CLOCK_CPHA1_CPOL1 = SPI_CR_CPOL_HI | SPI_CR_CPHA_SECOND,	/**< CPHA = 1, CPOL = 1 */
	SPI_CLOCK_MODE0 = SPI_CLOCK_CPHA0_CPOL0,/**< alias */
	SPI_CLOCK_MODE1 = SPI_CLOCK_CPHA1_CPOL0,/**< alias */
	SPI_CLOCK_MODE2 = SPI_CLOCK_CPHA0_CPOL1,/**< alias */
	SPI_CLOCK_MODE3 = SPI_CLOCK_CPHA1_CPOL1,/**< alias */
} SPI_CLOCK_MODE_T;

/** @brief SPI Data Order Mode*/
typedef enum {
	SPI_DATA_MSB_FIRST = SPI_CR_MSB_FIRST_EN,			/* Standard Order */
	SPI_DATA_LSB_FIRST = SPI_CR_LSB_FIRST_EN,			/* Reverse Order */
} SPI_DATA_ORDER_T;

/*
 * @brief Number of bits per frame
 */
typedef enum {
	SPI_BITS_8 = SPI_CR_BITS(8),		/**< 8 bits/frame */
	SPI_BITS_9 = SPI_CR_BITS(9),		/**< 9 bits/frame */
	SPI_BITS_10 = SPI_CR_BITS(10),		/**< 10 bits/frame */
	SPI_BITS_11 = SPI_CR_BITS(11),		/**< 11 bits/frame */
	SPI_BITS_12 = SPI_CR_BITS(12),		/**< 12 bits/frame */
	SPI_BITS_13 = SPI_CR_BITS(13),		/**< 13 bits/frame */
	SPI_BITS_14 = SPI_CR_BITS(14),		/**< 14 bits/frame */
	SPI_BITS_15 = SPI_CR_BITS(15),		/**< 15 bits/frame */
	SPI_BITS_16 = SPI_CR_BITS(16),		/**< 16 bits/frame */
} SPI_BITS_T;

/** SPI callback function type*/
typedef void (*SPI_CALLBACK_T)(void);
/*
 * @brief SPI config format
 */
typedef struct {
	SPI_BITS_T bits;						/*!< bits/frame */
	SPI_CLOCK_MODE_T clockMode;	/*!< Format config: clock phase/polarity */
	SPI_DATA_ORDER_T dataOrder;	/*!< Data order (MSB first/LSB first) */
} SPI_CONFIG_FORMAT_T;

/*
 * @brief SPI data setup structure
 */
typedef struct {
	uint8_t      *pTxData;					/*!< Pointer to transmit data */
	uint8_t      *pRxData;					/*!< Pointer to receive data */
	uint32_t  cnt;							/*!< Transfer counter */
	uint32_t  length;						/*!< Length of transfer data */
	SPI_CALLBACK_T    fnBefFrame;				/*!< Function to call before sending frame */
	SPI_CALLBACK_T    fnAftFrame;				/*!< Function to call after sending frame */
	SPI_CALLBACK_T    fnBefTransfer;			/*!< Function to call before starting a transfer */
	SPI_CALLBACK_T    fnAftTransfer;			/*!< Function to call after a transfer complete */
} SPI_DATA_SETUP_T;

/**
 * @brief	Get the current status of SPI controller
 * @return	SPI controller status (Or-ed value of SPI_SR_*)
 */
STATIC INLINE uint32_t Chip_SPI_GetStatus(LPC_SPI_T *pSPI)
{
	return pSPI->SR;
}

/**
 * @brief	Send SPI 16-bit data
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	data	: Transmit Data
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_SendFrame(LPC_SPI_T *pSPI, uint16_t data)
{
	pSPI->DR = SPI_DR_DATA(data);
}

/**
 * @brief	Get received SPI data
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	receive data
 */
STATIC INLINE uint16_t Chip_SPI_ReceiveFrame(LPC_SPI_T *pSPI)
{
	return SPI_DR_DATA(pSPI->DR);
}

/**
 * @brief	Set up output clocks per bit for SPI bus
 * @param	pSPI		: The base of SPI peripheral on the chip
 * @param	counter	: the number of SPI peripheral clock cycles that make up an SPI clock
 * @return	 Nothing
 * @note	The counter must be an even number greater than or equal to 8. <br>
 *		The SPI SCK rate = PCLK_SPI / counter.
 */
STATIC INLINE void Chip_SPI_SetClockCounter(LPC_SPI_T *pSPI, uint32_t counter)
{
	pSPI->CCR = counter;
}

/**
 * @brief   Set up the SPI frame format
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	format			: Pointer to Frame format structure
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_SetFormat(LPC_SPI_T *pSPI, SPI_CONFIG_FORMAT_T *format)
{
	pSPI->CR = (pSPI->CR & (~0xF1C)) | SPI_CR_BIT_EN | format->bits | format->clockMode | format->dataOrder;
}

/**
 * @brief	Get the number of bits transferred in each frame
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	 the number of bits transferred in each frame
 */
STATIC INLINE SPI_BITS_T Chip_SPI_GetDataSize(LPC_SPI_T *pSPI)
{
	return (pSPI->CR & SPI_CR_BIT_EN) ? ((SPI_BITS_T) (pSPI->CR & SPI_CR_BITS_MASK)) : SPI_BITS_8;
}

/**
 * @brief	Get the current CPHA & CPOL setting
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	CPHA & CPOL setting
 */
STATIC INLINE SPI_CLOCK_MODE_T Chip_SPI_GetClockMode(LPC_SPI_T *pSPI)
{
	return (SPI_CLOCK_MODE_T) (pSPI->CR & (3 << 3));
}

/**
 * @brief	Set the SPI working as master or slave mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	 Operating mode
 */
STATIC INLINE SPI_MODE_T Chip_SPI_GetMode(LPC_SPI_T *pSPI)
{
	return (SPI_MODE_T) (pSPI->CR & (1 << 5));
}

/**
 * @brief   Set the SPI operating modes, master or slave
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	mode		: master mode/slave mode
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_SetMode(LPC_SPI_T *pSPI, SPI_MODE_T mode)
{
	pSPI->CR = (pSPI->CR & (~(1 << 5))) | mode;
}

/**
 * @brief   Set the clock frequency for SPI interface
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	bitRate		: The SPI bit rate
 * @return	Nothing
 */
void Chip_SPI_SetBitRate(LPC_SPI_T *pSPI, uint32_t bitRate);

/**
 * @brief   Enable SPI interrupt
 * @param	pSPI			: The base SPI peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_Int_Enable(LPC_SPI_T *pSPI)
{
	pSPI->CR |= SPI_CR_INT_EN;
}

/**
 * @brief   Disable SPI interrupt
 * @param	pSPI			: The base SPI peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_Int_Disable(LPC_SPI_T *pSPI)
{
	pSPI->CR &= ~SPI_CR_INT_EN;
}

/**
 * @brief	Get the interrupt status
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	SPI interrupt Status (Or-ed bit value of SPI_INT_*)
 */
STATIC INLINE uint32_t Chip_SPI_Int_GetStatus(LPC_SPI_T *pSPI)
{
	return pSPI->INT;
}

/**
 * @brief	Clear the interrupt status
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	mask	: SPI interrupt mask (Or-ed bit value of SPI_INT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_SPI_Int_ClearStatus(LPC_SPI_T *pSPI, uint32_t mask)
{
	pSPI->INT = mask;
}

/**
 * @brief   Initialize the SPI
 * @param	pSPI			: The base SPI peripheral on the chip
 * @return	Nothing
 */
void Chip_SPI_Init(LPC_SPI_T *pSPI);

/**
 * @brief	Deinitialise the SPI
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note	The SPI controller is disabled
 */
void Chip_SPI_DeInit(LPC_SPI_T *pSPI);

/**
 * @brief   Clean all data in RX FIFO of SPI
 * @param	pSPI			: The base SPI peripheral on the chip
 * @return	Nothing
 */
void Chip_SPI_Int_FlushData(LPC_SPI_T *pSPI);

/**
 * @brief   SPI Interrupt Read/Write with 8-bit frame width
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	xf_setup		: Pointer to a SPI_DATA_SETUP_T structure that contains specified
 *                          information about transmit/receive data	configuration
 * @return	SUCCESS or ERROR
 */
Status Chip_SPI_Int_RWFrames8Bits(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *xf_setup);

/**
 * @brief   SPI Interrupt Read/Write with 16-bit frame width
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	xf_setup		: Pointer to a SPI_DATA_SETUP_T structure that contains specified
 *                          information about transmit/receive data	configuration
 * @return	SUCCESS or ERROR
 */
Status Chip_SPI_Int_RWFrames16Bits(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *xf_setup);

/**
 * @brief   SPI Polling Read/Write in blocking mode
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	pXfSetup		: Pointer to a SPI_DATA_SETUP_T structure that contains specified
 *                          information about transmit/receive data	configuration
 * @return	Actual data length has been transferred
 * @note
 * This function can be used in both master and slave mode. It starts with writing phase and after that,
 * a reading phase is generated to read any data available in RX_FIFO. All needed information is prepared
 * through xf_setup param.
 */
uint32_t Chip_SPI_RWFrames_Blocking(LPC_SPI_T *pSPI, SPI_DATA_SETUP_T *pXfSetup);

/**
 * @brief   SPI Polling Write in blocking mode
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	buffer			: Buffer address
 * @param	buffer_len		: Buffer length
 * @return	Actual data length has been transferred
 * @note
 * This function can be used in both master and slave mode. First, a writing operation will send
 * the needed data. After that, a dummy reading operation is generated to clear data buffer
 */
uint32_t Chip_SPI_WriteFrames_Blocking(LPC_SPI_T *pSPI, uint8_t *buffer, uint32_t buffer_len);

/**
 * @brief   SPI Polling Read in blocking mode
 * @param	pSPI			: The base SPI peripheral on the chip
 * @param	buffer			: Buffer address
 * @param	buffer_len		: The length of buffer
 * @return	Actual data length has been transferred
 * @note
 * This function can be used in both master and slave mode. First, a dummy writing operation is generated
 * to clear data buffer. After that, a reading operation will receive the needed data
 */
uint32_t Chip_SPI_ReadFrames_Blocking(LPC_SPI_T *pSPI, uint8_t *buffer, uint32_t buffer_len);

#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_43XX_H_ */






