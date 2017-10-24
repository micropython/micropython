/*
 * @brief LPC5410X System FIFO chip driver
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

#ifndef __FIFO_5410X_H_
#define __FIFO_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup FIFO_5410X CHIP: LPC5410X System FIFO chip driver
 * @ingroup CHIP_5410X_DRIVERS
 * This driver provides basic functionality for the system FIFO
 * and can be used to increase the amount of FIFO space available
 * to the UART and SPI peripherals. If using the system FIFO with the
 * UART or SPI drivers, the standard UART and SPI transfer handlers
 * cannot be used and buffer/stream management and status checking
 * must occur in the user application.
 * @{
 */

/** Maximum USART peripherals */
#define LPC_FIFO_USART_MAX      (4)

/** Maximum SPI peripherals */
#define LPC_FIFO_SPI_MAX        (2)

/**
 * @brief LPC5410X System FIFO USART register block structure
 */
typedef struct {
	__IO uint32_t CFG;			/*!< USART configuration Register */
	__IO uint32_t STAT;			/*!< USART status Register */
	__IO uint32_t INTSTAT;		/*!< USART interrupt status Register */
	__IO uint32_t CTLSET;		/*!< USART control read and set Register */
	__IO uint32_t CTLCLR;		/*!< USART control clear Register */
	__IO uint32_t RXDAT;		/*!< USART received data Register */
	__IO uint32_t RXDATSTAT;	/*!< USART received data with status Register */
	__IO uint32_t TXDAT;		/*!< USART transmit data Register */
	__I uint32_t  RESERVED[0x38];
} LPC_FIFO_USART_T;

/**
 * @brief LPC5410X System FIFO SPI register block structure
 */
typedef struct {
	__IO uint32_t CFG;			/*!< SPI configuration Register */
	__IO uint32_t STAT;			/*!< SPI status Register */
	__IO uint32_t INTSTAT;		/*!< SPI interrupt status Register */
	__IO uint32_t CTLSET;		/*!< SPI control read and set Register */
	__IO uint32_t CTLCLR;		/*!< SPI control clear Register */
	__I  uint32_t RXDAT;		/*!< SPI received data Register */
	union {
		__O uint32_t TXDATSPI;	/*!< SPI transmit data and control Register */
		struct {
			__O uint16_t TXDATSPI_DATA;	/*!< SPI transmit data Register */
			__O uint16_t TXDATSPI_CTRL;	/*!< SPI transmit control Register */
		};

	};

	__I  uint32_t RESERVED[0x39];
} LPC_FIFO_SPI_T;

/**
 * @brief LPC5410X System FIFO common register block structure
 */
typedef struct {
	__I  uint32_t reserved0[0x40];
	__IO uint32_t FIFOCTLUSART;			/*!< USART FIFO global control Register */
	__O  uint32_t FIFOUPDATEUSART;		/*!< USART FIFO global update Register */
	__I  uint32_t reserved1[0x2];
	__IO uint32_t FIFOCFGUSART[LPC_FIFO_USART_MAX];	/*!< USART FIFO configuration Registers */
	__I  uint32_t reserved2[0x38];
	__IO uint32_t FIFOCTLSPI;			/*!< SPI FIFO global control Register */
	__O  uint32_t FIFOUPDATESPI;		/*!< SPI FIFO global update Register */
	__I  uint32_t reserved3[0x2];
	__IO uint32_t FIFOCFGSPI[LPC_FIFO_SPI_MAX];		/*!< SPI FIFO configuration Registers */
	__I  uint32_t reserved4[0x3A];
	__I  uint32_t reserved5[((0x1000 - 0x300) / sizeof(uint32_t))];
} LPC_FIFO_CMN_T;

/**
 * @brief LPC5410X Complete system FIFO register block structure
 */
typedef struct {
	LPC_FIFO_CMN_T      common;
	LPC_FIFO_USART_T    usart[LPC_FIFO_USART_MAX];
	__I uint32_t        reserved0[((0x2000 - 0x1400) / sizeof(uint32_t))];
	LPC_FIFO_SPI_T      spi[LPC_FIFO_SPI_MAX];
} LPC_FIFO_T;

/** @defgroup FIFO_CMN_5410X CHIP: Common FIFO functions
 * These functions are for both the USART and SPI configuration and
 * status.
 * @{
 */

/**
 * @brief	Initializes the system FIFO
 * @brief	pFIFO	: Pointer to system FIFO registers
 * @return	Nothing
 */
void Chip_FIFO_Init(LPC_FIFO_T *pFIFO);

/**
 * @brief	Deinitializes the system FIFO
 * @brief	pFIFO	: Pointer to system FIFO registers
 * @return	Nothing
 */
void Chip_FIFO_Deinit(LPC_FIFO_T *pFIFO);

/** USART/SPI peripheral identifier */
typedef enum {FIFO_USART, FIFO_SPI} LPC_FIFO_PERIPHID_T;

/** USART/SPI FIFO direction identifier */
typedef enum {FIFO_RX, FIFO_TX} LPC_FIFO_DIR_T;

/**
 * @brief	Get the FIFO space available for the USART/SPI direction
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	periphId	: Peripheral identifer
 * @brief	dir			: FIFO direction
 * @return	Amount of FIFO space available for the peripheral
 */
uint32_t Chip_FIFO_GetFifoSpace(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir);

/** USART and SPI FIFO common statuses */
#define LPC_FIFO_STAT_RXPAUSED      (1 << 1)		/*!< Receive FIFOs paused status */
#define LPC_FIFO_STAT_RXEMPTY       (1 << 2)		/*!< Receive FIFOs empty status */
#define LPC_FIFO_STAT_TXPAUSED      (1 << 9)		/*!< Transmit FIFOs paused status */
#define LPC_FIFO_STAT_TXEMPTY       (1 << 10)		/*!< Transmit FIFOs empty status */

/**
 * @brief	Get periperhal FIFO status
 * @brief	pFIFO	: Pointer to system FIFO registers
 * @return	A bitfield of status values, mask with LPC_FIFO_STAT_* values
 * @note	Mask with one or more LPC_FIFO_STAT_* definitions to get the
 * status of the peripherals.
 */
__STATIC_INLINE uint32_t Chip_FIFO_GetFifoStatus(LPC_FIFO_T *pFIFO)
{
	return pFIFO->common.FIFOCTLUSART;
}

/**
 * @brief	Pause a peripheral FIFO
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	periphId	: Peripheral identifer
 * @brief	dir			: FIFO direction
 * @return	Nothing
 */
void Chip_FIFO_PauseFifo(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir);

/**
 * @brief	Unpause a peripheral FIFO
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	periphId	: Peripheral identifer
 * @brief	dir			: FIFO direction
 * @return	Nothing
 */
void Chip_FIFO_UnpauseFifo(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_DIR_T dir);

/** Stucture for setting USART or SPI FIFO sizes */
typedef struct {
	uint16_t    fifoRXSize[4];	/*!< FIFO RX size, 0-3 for USARTS 0-3, or 0-1 for SPIS 0-1 */
	uint16_t    fifoTXSize[4];	/*!< FIFO TX size, 0-3 for USARTS 0-3, or 0-1 for SPIS 0-1 */
} LPC_FIFO_CFGSIZE_T;

/**
 * @brief	Configure a peripheral's FIFO sizes
 * @brief	pFIFO	: Pointer to system FIFO registers
 * @brief	periphId	: Peripheral identifer
 * @brief	pSizes	: Pointer to s structure filled out with the peripherla FIFO sizes
 * @return	Nothing
 * @note	This function configures all the FIFOs for a supported peripheral
 * in a single call. Use 0 to disable the FIFO. This function will pause the FIFO
 * and leave it paused after configuration, call Chip_FIFO_UnpauseFifo() after
 * calling this function.
 */
void Chip_FIFO_ConfigFifoSize(LPC_FIFO_T *pFIFO, LPC_FIFO_PERIPHID_T periphId, LPC_FIFO_CFGSIZE_T *pSizes);

/** Stucture for setting USART and SPI FIFO configuration */
typedef struct {
	uint32_t    noTimeoutContWrite  : 1;	/*!< Timeout Continue On Write, set to 0 to reset timeout on each TX FIFO data, or 1 for accumulated timeout */
	uint32_t    noTimeoutContEmpty  : 1;	/*!< Timeout Continue On Empty., set to 0 to reset timeout on each RX FIFO data, or 1 for accumulated timeout */
	uint32_t    timeoutBase         : 4;	/*!< Specifies the least significant timer bit to compare to TimeoutValue. See User Manual */
	uint32_t    timeoutValue        : 4;	/*!< Specifies the maximum time value for timeout at the timer position identified by TimeoutBase. See User Manual */
	uint32_t    rxThreshold         : 8;	/*!< Receive FIFO Threshold, number of data to receive prior to interrupt */
	uint32_t    txThreshold         : 8;	/*!< Transmit FIFO Threshold, number of free TX data entries available prior to interrupt */
} LPC_FIFO_CFG_T;

/** USART and SPI FIFO statuses */
#define LPC_PERIPFIFO_STAT_RXTH         (1 << 0)		/*!< When 1, the receive FIFO threshold has been reached */
#define LPC_PERIPFIFO_STAT_TXTH         (1 << 1)		/*!< When 1, the transmit FIFO threshold has been reached */
#define LPC_PERIPFIFO_STATCLR_RXTIMEOUT (1 << 4)		/*!< When 1, the receive FIFO has timed out, based on the timeout configuration in the CFG register */
#define LPC_PERIPFIFO_STATCLR_BUSERR    (1 << 7)		/*!< Bus Error. When 1, a bus error has occurred while processing data for the peripheral. The bus error flag can be cleared by writing a 1 to this bit. */
#define LPC_PERIPFIFO_STAT_RXEMPTY      (1 << 8)		/*!< Receive FIFO Empty. When 1, the receive FIFO is currently empty. */
#define LPC_PERIPFIFO_STAT_TXEMPTY      (1 << 9)		/*!< Transmit FIFO Empty. When 1, the transmit FIFO is currently empty. */

/** USART interrupt enable/disable bits */
#define LPC_PERIPFIFO_INT_RXTH          (1 << 0)		/*!< Receive FIFO Threshold Interrupt Enable */
#define LPC_PERIPFIFO_INT_TXTH          (1 << 1)		/*!< Transmit FIFO Threshold Interrupt Enable */
#define LPC_PERIPFIFO_INT_RXTIMEOUT     (1 << 4)		/*!< Receive FIFO Timeout Interrupt Enable */
#define LPC_PERIPFIFO_INT_RXFLUSH       (1 << 8)		/*!< Receive FIFO flush */
#define LPC_PERIPFIFO_INT_TXFLUSH       (1 << 9)		/*!< Transmit FIFO flush */

/**
 * @}
 */

/** @defgroup FIFO_USART_5410X CHIP: USART FIFO functions
 * These functions are for both the USART configuration, control, and status.
 * @{
 */

/**
 * @brief	Configure the USART system FIFO
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	pUSARTCfg	: Pointer to USART configuration
 * @return	Nothing
 */
void Chip_FIFOUSART_Configure(LPC_FIFO_T *pFIFO, int usartIndex, LPC_FIFO_CFG_T *pUSARTCfg);

/**
 * @brief	Get USART FIFO statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @return	USART system FIFO statuses (mask with LPC_PERIPFIFO_STAT* values)
 */
__STATIC_INLINE uint32_t Chip_FIFOUSART_GetStatus(LPC_FIFO_T *pFIFO, int usartIndex)
{
	return pFIFO->usart[usartIndex].STAT;
}

/**
 * @brief	Get USART RX FIFO count
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @return	Returns how many entries may be read from the receive FIFO. 0 = FIFO empty.
 */
__STATIC_INLINE uint32_t Chip_FIFOUSART_GetRxCount(LPC_FIFO_T *pFIFO, int usartIndex)
{
	return (pFIFO->usart[usartIndex].STAT >> 16) & 0xFF;
}

/**
 * @brief	Get USART TC FIFO count
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @return	Returns how many entries may be written to the transmit FIFO. 0 = FIFO full.
 */
__STATIC_INLINE uint32_t Chip_FIFOUSART_GetTxCount(LPC_FIFO_T *pFIFO, int usartIndex)
{
	return (pFIFO->usart[usartIndex].STAT >> 24) & 0xFF;
}

/**
 * @brief	Clear USART FIFO statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	clearMask	: Mask of latched bits to cleared, Or'ed values of LPC_PERIPFIFO_STATCLR*
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOUSART_ClearStatus(LPC_FIFO_T *pFIFO, int usartIndex, uint32_t clearMask)
{
	pFIFO->usart[usartIndex].STAT = clearMask;
}

/**
 * @brief	Get USART FIFO pending interrupt statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @return	USART system FIFO pending interrupt statuses (mask with LPC_PERIPFIFO_STAT* values)
 */
__STATIC_INLINE uint32_t Chip_FIFOUSART_GetIntStatus(LPC_FIFO_T *pFIFO, int usartIndex)
{
	return pFIFO->usart[usartIndex].INTSTAT;
}

/**
 * @brief	Enable USART system FIFO interrupts
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	intMask		: Interrupts to enable (LPC_PERIPFIFO_INT_RXTH, LPC_PERIPFIFO_INT_TXTH, or LPC_PERIPFIFO_INT_RXTIMEOUT)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOUSART_EnableInts(LPC_FIFO_T *pFIFO, int usartIndex, uint32_t intMask)
{
	pFIFO->usart[usartIndex].CTLSET = intMask;
}

/**
 * @brief	Disable USART system FIFO interrupts
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	intMask		: Interrupts to disable (LPC_PERIPFIFO_INT_RXTH, LPC_PERIPFIFO_INT_TXTH, or LPC_PERIPFIFO_INT_RXTIMEOUT)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOUSART_DisableInts(LPC_FIFO_T *pFIFO, int usartIndex, uint32_t intMask)
{
	pFIFO->usart[usartIndex].CTLCLR = intMask;
}

/**
 * @brief	Flush TX and/or RX USART system FIFOs
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	flushMask	: FIFOS to flush (Or'ed LPC_PERIPFIFO_INT_RXFLUSH and/or LPC_PERIPFIFO_INT_TXFLUSH)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOUSART_FlushFIFO(LPC_FIFO_T *pFIFO, int usartIndex, uint32_t flushMask)
{
	flushMask = flushMask & (LPC_PERIPFIFO_INT_RXFLUSH | LPC_PERIPFIFO_INT_TXFLUSH);

	pFIFO->usart[usartIndex].CTLSET = flushMask;
	pFIFO->usart[usartIndex].CTLCLR = flushMask;
}

/**
 * @brief	Write data to a USART system FIFO (non-blocking)
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	sz8			: Set to true for 8-bit or less data, or false for >8-bit
 * @brief	buff		: Pointer to data in buffer to write
 * @brief	numData		: Maximum number of data values to write
 * @return	The number of data values written to the USART system FIFO
 */
int Chip_FIFOUSART_WriteTX(LPC_FIFO_T *pFIFO, int usartIndex, bool sz8, void *buff, int numData);

/**
 * @brief	Read data from a USART system FIFO (non-blocking)
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	sz8			: Set to true for 8-bit or less data, or false for >8-bit
 * @brief	buff		: Pointer to data buffer to read into
 * @brief	numData		: Maximum number of data values to read
 * @return	The number of data values read from the USART system FIFO
 */
int Chip_FIFOUSART_ReadRX(LPC_FIFO_T *pFIFO, int usartIndex, bool sz8, void *buff, int numData);

/** USART FIFO read FIFO statuses */
#define LPC_USARTRXFIFO_STAT_FRAMERR    (1 << 13)		/*!< Framing Error status flag */
#define LPC_USARTRXFIFO_STAT_PARITYERR  (1 << 14)		/*!< Parity Error status flag */
#define LPC_USARTRXFIFO_STAT_RXNOISE    (1 << 15)		/*!< Received Noise flag */

/**
 * @brief	Read data from a USART system FIFO with status (non-blocking)
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	usartIndex	: USART system FIFO index, 0 - 3
 * @brief	buff		: Pointer to data buffer to read into
 * @brief	numData		: Maximum number of data values to read
 * @return	The number of data values with status read from the USART system FIFO. Mask
 * the upper bits of each word in the buffer with the LPC_USARTRXFIFO_STAT_* flags to
 * determine individual data status.
 */
int Chip_FIFOUSART_ReadRXStatus(LPC_FIFO_T *pFIFO, int usartIndex, uint16_t *buff, int numData);

/**
 * @}
 */

/** @defgroup FIFO_SPI_5410X CHIP: SPI FIFO functions
 * These functions are for both the SPI configuration, control, and status.
 * @{
 */

/**
 * @brief	Configure the SPI system FIFO
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @brief	pUSARTCfg	: Pointer to SPI configuration
 * @return	Nothing
 */
void Chip_FIFOSPI_Configure(LPC_FIFO_T *pFIFO, int spiIndex, LPC_FIFO_CFG_T *pSPICfg);

/**
 * @brief	Get SPI FIFO statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @return	SPI system FIFO statuses (mask with LPC_PERIPFIFO_STAT* values)
 */
__STATIC_INLINE uint32_t Chip_FIFOSPI_GetStatus(LPC_FIFO_T *pFIFO, int spiIndex)
{
	return pFIFO->spi[spiIndex].STAT;
}

/**
 * @brief	Get SPI RX FIFO count
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @return	Returns how many entries may be read from the receive FIFO. 0 = FIFO empty.
 */
__STATIC_INLINE uint32_t Chip_FIFOSPI_GetRxCount(LPC_FIFO_T *pFIFO, int spiIndex)
{
	return (pFIFO->spi[spiIndex].STAT >> 16) & 0xFF;
}

/**
 * @brief	Get SPI TX FIFO count
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @return	Returns how many entries may be written to the transmit FIFO. 0 = FIFO full.
 */
__STATIC_INLINE uint32_t Chip_FIFOSPI_GetTxCount(LPC_FIFO_T *pFIFO, int spiIndex)
{
	return (pFIFO->spi[spiIndex].STAT >> 24) & 0xFF;
}

/**
 * @brief	Clear SPI FIFO statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @brief	clearMask	: Mask of latched bits to cleared, Or'ed values of LPC_PERIPFIFO_STATCLR*
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOSPI_ClearStatus(LPC_FIFO_T *pFIFO, int spiIndex, uint32_t clearMask)
{
	pFIFO->spi[spiIndex].STAT = clearMask;
}

/**
 * @brief	Get SPI FIFO pending interrupt statuses
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @return	SPI system FIFO pending interrupt statuses (mask with LPC_PERIPFIFO_STAT* values)
 */
__STATIC_INLINE uint32_t Chip_FIFOSPI_GetIntStatus(LPC_FIFO_T *pFIFO, int spiIndex)
{
	return pFIFO->spi[spiIndex].INTSTAT;
}

/**
 * @brief	Enable SPI system FIFO interrupts
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @brief	intMask		: Interrupts to enable (LPC_PERIPFIFO_INT_RXTH, LPC_PERIPFIFO_INT_TXTH, or LPC_PERIPFIFO_INT_RXTIMEOUT)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOSPI_EnableInts(LPC_FIFO_T *pFIFO, int spiIndex, uint32_t intMask)
{
	pFIFO->spi[spiIndex].CTLSET = intMask;
}

/**
 * @brief	Disable SPI system FIFO interrupts
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @brief	intMask		: Interrupts to disable (LPC_PERIPFIFO_INT_RXTH, LPC_PERIPFIFO_INT_TXTH, or LPC_PERIPFIFO_INT_RXTIMEOUT)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOSPI_DisableInts(LPC_FIFO_T *pFIFO, int spiIndex, uint32_t intMask)
{
	pFIFO->spi[spiIndex].CTLCLR = intMask;
}

/**
 * @brief	Flush TX and/or RX SPI system FIFOs
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	spiIndex	: SPI system FIFO index, 0 - 1
 * @brief	flushMask	: FIFOS to flush (Or'ed LPC_PERIPFIFO_INT_RXFLUSH and/or LPC_PERIPFIFO_INT_TXFLUSH)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_FIFOSPI_FlushFIFO(LPC_FIFO_T *pFIFO, int spiIndex, uint32_t flushMask)
{
	flushMask = flushMask & (LPC_PERIPFIFO_INT_RXFLUSH | LPC_PERIPFIFO_INT_TXFLUSH);

	pFIFO->spi[spiIndex].CTLSET = flushMask;
	pFIFO->spi[spiIndex].CTLCLR = flushMask;
}

/** SPI transfer flags */
#define LPC_SPIFIFO_FLAG_EOF            (1 << 21)		/*!< Add a delay between frames */
#define LPC_SPIFIFO_FLAG_RXIGNORE       (1 << 22)		/*!< Ignore RX data */

/** SPI transfer error statuses */
#define LPC_SPIFIFO_STAT_BUSY           (0x0)		/*!< SPI transfer busy/in progress */
#define LPC_SPIFIFO_STAT_BADPARAM       (0x1)		/*!< SPI paramaters for transfer are invalid */
#define LPC_SPIFIFO_STAT_TXUNDERRUN     (0x2)		/*!< Slave mode only, transmit FIFO underrun */
#define LPC_SPIFIFO_STAT_RXOVERRUN      (0x3)		/*!< Slave mode only, receive FIFO overrun */
#define LPC_SPIFIFO_STAT_COMPLETE       (0xF)		/*!< SPI transfer completed successfully */

#if 0	/* Sorry, not yet support */
/** Stucture for SPI control */
typedef struct {
	uint32_t    start   : 1;		/*!< Indicates transfer start, 0 = transfer resume, 1 = transfer start (automatically set by Chip_FIFOSPI_StartTransfer()) */
	uint32_t    end     : 1;		/*!< Transfer wil end once buffers are empty */
	uint32_t    sz8     : 1;		/*!< Specifies the in and out buffer sizes, 0 = 16-bit, 1 = 8-bit */
	uint32_t    sselNum : 2;		/*!< SPI chip select number, 0 - 3 */
	void        *inBuff;			/*!< SPI transfer in data buffer pointer */
	uint32_t    inIndex;			/*!< SPI transfer in buffer index */
	void        *outBuff;			/*!< SPI transfer out data buffer pointer */
	uint32_t    outIndex;			/*!< SPI transfer out buffer index */
	uint32_t    numData;			/*!< Size of data both the receive and transfer buffers */
	int         spiIndex;			/*!< SPI system FIFO index, 0 - 1 */
} LPC_FIFO_SPICTL_T;

/**
 * @brief	Start a SPI data transfer (non-blocking)
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	pSetupData	: Pointer to SPI transfer setup structure
 * @return	Nothing
 * @note	Simply calls Chip_FIFOSPI_Transfer() with pSetupData->start = 1.
 */
void Chip_FIFOSPI_StartTransfer(LPC_FIFO_T *pFIFO, LPC_FIFO_SPICTL_T *pSetupData);

/**
 * @brief	Feed a SPI data transfer (non-blocking)
 * @brief	pFIFO		: Pointer to system FIFO registers
 * @brief	pSetupData	: Pointer to SPI transfer setup structure
 * @return	Nothing
 * @note	Continues SPI transfer usng the system FIFO.
 */
void Chip_FIFOSPI_Transfer(LPC_FIFO_T *pFIFO, LPC_FIFO_SPICTL_T *pSetupData);

#endif

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __FIFO_5410X_H_ */
