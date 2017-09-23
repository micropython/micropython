/*
 * @brief LPC5410X SPI master driver
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

#ifndef __SPIM_5410X_H_
#define __SPIM_5410X_H_

#include "spi_common_5410x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SPI_MASTER_5410X CHIP: LPC5410X SPI master driver
 * @ingroup SPI_COMMON_5410X
 * @{
 */

/**
 * @brief	Get SPI master bit rate
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	The actual SPI clock bit rate
 */
uint32_t Chip_SPIM_GetClockRate(LPC_SPI_T *pSPI);

/**
 * @brief	Set SPI master bit rate
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	rate	: Desired clock bit rate for the SPI interface
 * @return	The actual SPI clock bit rate
 * @note	This function will set the SPI clock divider to get closest
 * to the desired rate as possible.
 */
uint32_t Chip_SPIM_SetClockRate(LPC_SPI_T *pSPI, uint32_t rate);

/**
 * @brief SPI Delay Configure Struct
 */
typedef struct {
	uint8_t PreDelay;					/** Pre-delay value in SPI clocks, 0 - 15 */
	uint8_t PostDelay;					/** Post-delay value in SPI clocks, 0 - 15 */
	uint8_t FrameDelay;					/** Delay value between frames of a transfer in SPI clocks, 0 - 15 */
	uint8_t TransferDelay;				/** Delay value between transfers in SPI clocks, 1 - 16 */
} SPIM_DELAY_CONFIG_T;

/**
 * @brief	Config SPI Delay parameters
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	pConfig	: SPI Delay Configure Struct
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPIM_DelayConfig(LPC_SPI_T *pSPI, SPIM_DELAY_CONFIG_T *pConfig)
{
	pSPI->DLY = (SPI_DLY_PRE_DELAY(pConfig->PreDelay) |
				 SPI_DLY_POST_DELAY(pConfig->PostDelay) |
				 SPI_DLY_FRAME_DELAY(pConfig->FrameDelay) |
				 SPI_DLY_TRANSFER_DELAY(pConfig->TransferDelay - 1));
}

/**
 * @brief	Forces an end of transfer for the current master transfer
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note	Use this function to perform an immediate end of trasnfer for the
 * current master operation. If the master is currently transferring data started
 * with the Chip_SPIM_Xfer function, this terminates the transfer after the
 * current byte completes and completes the transfer.
 */
__STATIC_INLINE void Chip_SPIM_ForceEndOfTransfer(LPC_SPI_T *pSPI)
{
	pSPI->STAT = SPI_STAT_EOT;
}

/**
 * @brief	Assert a SPI select
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	sselNum	: SPI select to assert, 0 - 3
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPIM_AssertSSEL(LPC_SPI_T *pSPI, uint8_t sselNum)
{
	pSPI->TXCTRL = pSPI->TXCTRL & (SPI_TXDATCTL_CTRLMASK & ~SPI_TXDATCTL_DEASSERTNUM_SSEL(sselNum));
}

/**
 * @brief	Deassert a SPI select
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	sselNum	: SPI select to deassert, 0 - 3
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPIM_DeAssertSSEL(LPC_SPI_T *pSPI, uint8_t sselNum)
{
	pSPI->TXCTRL = (pSPI->TXCTRL | SPI_TXDATCTL_DEASSERTNUM_SSEL(sselNum)) & SPI_TXDATCTL_CTRLMASK;
}

/**
 * @brief	Enable loopback mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 * @note	Serial input is taken from the serial output (MOSI or MISO) rather
 * than the serial input pin.
 */
__STATIC_INLINE void Chip_SPIM_EnableLoopBack(LPC_SPI_T *pSPI)
{
	Chip_SPI_SetCFGRegBits(pSPI, SPI_CFG_LBM_EN);
}

/**
 * @brief	Disable loopback mode
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SPIM_DisableLoopBack(LPC_SPI_T *pSPI)
{
	Chip_SPI_ClearCFGRegBits(pSPI, SPI_CFG_LBM_EN);
}

/**
 * @brief	Event handler ID passed during a call-back event
 */
typedef enum {
	SPIM_EVT_SSELASSERT,			/**< Event identifier for Slave Assert */
	SPIM_EVT_SSELDEASSERT,			/**< Event identifier for Slave deassert */
	SPIM_EVT_TXDONE,				/**< Event identifier for TX complete */
	SPIM_EVT_RXDONE,				/**< Event identifier for RX complete */
} SPIM_EVENT_T;

/** @brief SPI Master XFER states */
#define SPIM_XFER_STATE_IDLE         0	/**< SPI XFER is IDLE */
#define SPIM_XFER_STATE_BUSY         1	/**< SPI XFER is busy transfering data */
#define SPIM_XFER_STATE_DONE         2	/**< SPI XFER is complete */

/** @brief	SPI master xfer options */
#define SPIM_XFER_OPTION_EOT         (1 << 4)			/**< SPI SLAVE Select will be deasserted when xfer is done */
#define SPIM_XFER_OPTION_EOF         (1 << 5)			/**< Insert a frame delay */
#define SPIM_XFER_OPTION_SIZE(x)     ((((x) - 1) & 0xF) << 8)	/**< Number of bits in transfer data */

/** Slave transfer data context */
typedef struct SPIM_XFER {
	int (*cbFunc)(SPIM_EVENT_T event, struct SPIM_XFER *xfer);	/**< Callback function for event handling; NULL when no call-back required
																   @a event will be one of #SPIS_EVENT_T and @a xfer is pointer to xfer structure */

	void *txBuff;					/**< TX buffer pointer; Must be a uint16_t pointer when transfer
									    size is 16 bits and uint8_t pointer when transfer size is 8 bits (can be NULL only when *txCount is 0) */
	void *rxBuff;					/**< RX buffer pointer; Must be uint16_t pointer when transfer size is 16 bits or
									    must be uint8_t pointer when transfer is 8-bits (can be NULL only when *txCount is 0) */
	int32_t txCount;				/**< Pointer to an int32_t memory (never initialize to NULL) that has the Size of the txBuff in items (not bytes), not modified by driver */
	int32_t rxCount;				/**< Number of items (not bytes) to send in rxBuff buffer (Never initialize to NULL), not modified by driver */
	int32_t txDoneCount;			/**< Total items (not bytes) transmitted (initialize to 0), modified by driver [In case of underflow txDoneCount will be greater than *txCount] */
	int32_t rxDoneCount;			/**< Total items (not bytes) received (initialize to 0), modified by driver [In case of over flow rxDoneCount will be greater than *rxCount] */
	uint8_t sselNum;				/**< Slave number assigned to this transfer, 0 - 3, modified by driver */
	uint8_t state;					/**< Initialize to #SPIS_XFER_STATE_IDLE; driver sets to #SPIS_XFER_STATE_BUSY or #SPIS_XFER_STATE_DONE */
	uint16_t options;				/**< SPI Transfer options (or'd values of #SPIM_XFER_OPTION_EOT, #SPIM_XFER_OPTION_EOF, #SPIM_XFER_OPTION_SIZE) */
} SPIM_XFER_T;

/**
 * @brief	SPI master transfer state change handler
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIM_XFER_T structure see notes below
 * @return	Nothing
 * @note	See @ref SPIM_XFER_T for more information on this function. When using
 * this function, the SPI master interrupts should be enabled and setup in the SPI
 * interrupt handler to call this function when they fire. This function is meant
 * to be called from the interrupt handler.
 */
void Chip_SPIM_XferHandler(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer);

/**
 * @brief	Start non-blocking SPI master transfer
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIM_XFER_T structure see notes below
 * @return	Nothing
 * @note	This function starts a non-blocking SPI master transfer with the
 * parameters setup in the passed @ref SPIM_XFER_T structure. Once the transfer is
 * started, the interrupt handler must call Chip_SPIM_XferHandler to keep the
 * transfer going and fed with data. This function should only be called when
 * the master is idle.<br>
 *
 * This function must be called with the options and sselNum fields correctly
 * setup. Initial data buffers and the callback pointer must also be setup. No
 * sanity checks are performed on the passed data.<br>
 *
 * Example call:<br>
 * SPIM_XFER_T mxfer;
 * mxfer.pCB = (&)masterCallbacks;
 * mxfer.sselNum = 2; // Use chip select 2
 * mxfer.options = SPI_TXCTL_FLEN(8); // 8 data bits, supports 1 - 16 bits
 * mxfer.options |= SPI_TXCTL_EOT | SPI_TXCTL_EOF; // Apply frame and transfer delays to master transfer
 * mxfer.options |= SPI_TXCTL_RXIGNORE; // Ignore RX data, will toss receive data regardless of pRXData8 or pRXData16 buffer
 * mxfer.pTXData8 = SendBuffer;
 * mxfer.txCount = 16; // Number of bytes to send before SPIMasterXferSend callback is called
 * mxfer.pRXData8 = RecvBuffer; // Will not receive data if pRXData8/pRXData16 is NULL or SPI_TXCTL_RXIGNORE option is set
 * mxfer.rxCount = 16; // Number of bytes to receive before SPIMasterXferRecv callback is called
 * Chip_SPIM_Xfer(LPC_SPI0, &mxfer); // Start transfer
 *
 * Note that the transfer, once started, needs to be constantly fed by the callbacks.
 * The txCount and rxCount field only indicate the buffer size before the callbacks are called.
 * To terminate the transfer, the SPIMasterXferSend callback must set the terminate field.
 */
void Chip_SPIM_Xfer(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer);

/**
 * @brief	Perform blocking SPI master transfer
 * @param	pSPI	: The base of SPI peripheral on the chip
 * @param	xfer	: Pointer to a SPIM_XFER_T structure see notes below
 * @return	Nothing
 * @note	This function starts a blocking SPI master transfer with the
 * parameters setup in the passed @ref SPIM_XFER_T structure. Once the transfer is
 * started, the callbacks in Chip_SPIM_XferHandler may be called to keep the
 * transfer going and fed with data. SPI interrupts must be disabled prior to
 * calling this function. It is not recommended to use this function.<br>
 */
void Chip_SPIM_XferBlocking(LPC_SPI_T *pSPI, SPIM_XFER_T *xfer);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SPIM_5410X_H_ */
