/*
 * @brief LPC5410X SPI slave driver
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

#ifndef __SPIS_5410X_H_
#define __SPIS_5410X_H_

#include "spi_common_5410x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SPI_SLAVE_5410X CHIP: LPC5410X SPI slave driver
 * @ingroup SPI_COMMON_5410X
 * @{
 */

/**
 * @brief	Event handler ID passed during a call-back event
 */
typedef enum {
	SPIS_EVT_SSELASSERT,			/**< Event identifier for Slave Assert */
	SPIS_EVT_SSELDEASSERT,			/**< Event identifier for Slave deassert */
	SPIS_EVT_TXDONE,				/**< Event identifier for TX complete */
	SPIS_EVT_RXDONE,				/**< Event identifier for RX complete */
} SPIS_EVENT_T;

/** @brief SPI XFER states */
#define SPIS_XFER_STATE_IDLE         0	/**< SPI XFER is IDLE */
#define SPIS_XFER_STATE_BUSY         1	/**< SPI XFER is busy transfering data */
#define SPIS_XFER_STATE_DONE         2	/**< SPI XFER is complete */

/** Slave transfer data context */
typedef struct SPIS_XFER {
	int (*cbFunc)(SPIS_EVENT_T event, struct SPIS_XFER *xfer);	/**< Callback function for event handling; NULL when no call-back required
																   @a event will be one of #SPIS_EVENT_T and @a xfer is pointer to xfer structure [Return value ignored] */

	void *txBuff;					/**< TX buffer pointer; Must be a uint16_t pointer when transfer
									    size is 16 bits and uint8_t pointer when transfer size is 8 bits */
	void *rxBuff;					/**< RX buffer pointer; Must be uint16_t pointer when transfer size is 16 bits or
									    must be uint8_t pointer when transfer is 8-bits */
	int32_t txCount;				/**< Pointer to an int32_t memory (never initialize to NULL) that has the Size of the txBuff in items (not bytes), not modified by driver */
	int32_t rxCount;				/**< Number of items (not bytes) to send in rxBuff buffer (Never initialize to NULL), not modified by driver */
	int32_t txDoneCount;			/**< Total items (not bytes) transmitted (initialize to 0), modified by driver [In case of underflow txDoneCount will be greater than *txCount] */
	int32_t rxDoneCount;			/**< Total items (not bytes) received (initialize to 0), modified by driver [In case of over flow rxDoneCount will be greater than *rxCount] */
	uint8_t sselNum;				/**< Slave number assigned to this transfer, 0 - 3, modified by driver */
	uint8_t state;					/**< Initialize to #SPIS_XFER_STATE_IDLE; driver sets to #SPIS_XFER_STATE_BUSY or #SPIS_XFER_STATE_DONE */
	uint16_t reserved0;				/**< Reserved field */
} SPIS_XFER_T;

/**
 * @brief	SPI slave transfer state change handler
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIS_XFER_T structure see notes below
 * @return	returns 0 on success, or SPI_STAT_RXOV and/or SPI_STAT_TXUR on an error
 * @note	See @ref SPIS_XFER_T for more information on this function. When using
 * this function, the SPI slave interrupts should be enabled and setup in the SPI
 * interrupt handler to call this function when they fire. This function is meant
 * to be called from the interrupt handler. The @ref SPIS_XFER_T data does not need
 * to be setup prior to the call and should be setup by the callbacks instead.<br>
 *
 * The callbacks are handled in the interrupt handler. If you are getting overflow
 * or underflow errors, you might need to lower the speed of the master clock or
 * extend the master's select assetion time.<br>
 */
uint32_t Chip_SPIS_XferHandler(LPC_SPI_T *pSPI, SPIS_XFER_T *xfer);

/**
 * @brief	Pre-buffers slave transmit data
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIS_XFER_T structure see notes below
 * @return	Nothing
 * @note Pre-buffering allows the slave to prime the transmit FIFO with data prior to
 * the master starting a transfer. If data is not pre-buffered, the initial slave
 * transmit data will always be 0x0 with a slave transmit underflow status.
 * Pre-buffering is best used when only a single slave select is used by an
 * application.
 */
__STATIC_INLINE void Chip_SPIS_PreBuffSlave(LPC_SPI_T *pSPI, SPIS_XFER_T *xfer)
{
	Chip_SPIS_XferHandler(pSPI, xfer);
}

/**
 * @brief	SPI slave transfer blocking function
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIS_XFER_T structure
 * @return	returns 0 on success, or SPI_STAT_RXOV and/or SPI_STAT_TXUR on an error
 * @note	This function performs a blocking transfer on the SPI slave interface.
 * It is not recommended to use this function. Once this function is called, it
 * will block forever until a slave transfer consisting of a slave SSEL assertion,
 * and de-assertion occur. The callbacks are still used for slave data buffer
 * management. SPI interrupts must be disabled prior to calling this function.
 */
uint32_t Chip_SPIS_XferBlocking(LPC_SPI_T *pSPI, SPIS_XFER_T *xfer);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SPIS_5410X_H_ */
