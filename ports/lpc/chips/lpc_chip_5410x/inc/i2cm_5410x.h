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

#ifndef __I2CM_5410X_H_
#define __I2CM_5410X_H_

#include "i2c_common_5410x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2CM_5410X CHIP: LPC5410X I2C master-only driver
 * @ingroup I2C_5410X
 * This driver only works in master mode. To describe the I2C transactions
 * following symbols are used in driver documentation.
 *
 * Key to symbols
 * ==============
 * S     (1 bit) : Start bit
 * P     (1 bit) : Stop bit
 * Rd/Wr (1 bit) : Read/Write bit. Rd equals 1, Wr equals 0.
 * A, NA (1 bit) : Acknowledge and Not-Acknowledge bit.
 * Addr  (7 bits): I2C 7 bit address. Note that this can be expanded as usual to
 *                 get a 10 bit I2C address.
 * Data  (8 bits): A plain data byte. Sometimes, I write DataLow, DataHigh
 *                 for 16 bit data.
 * [..]: Data sent by I2C device, as opposed to data sent by the host adapter.
 * @{
 */

/** I2CM_5410X_STATUS_TYPES I2C master transfer status types
 */

#define I2CM_STATUS_OK              0x00		/*!< Requested Request was executed successfully. */
#define I2CM_STATUS_ERROR           0x01		/*!< Unknown error condition. */
#define I2CM_STATUS_NAK_ADR         0x02		/*!< No acknowledgement received from slave during address phase. */
#define I2CM_STATUS_BUS_ERROR       0x03		/*!< I2C bus error */
#define I2CM_STATUS_NAK_DAT           0x04		/*!< No acknowledgement received from slave during address phase. */
#define I2CM_STATUS_ARBLOST         0x05		/*!< Arbitration lost. */
#define I2CM_STATUS_BUSY            0xFF		/*!< I2C transmistter is busy. */

/**
 * @brief Master transfer data structure definitions
 */
typedef struct {
	const uint8_t *txBuff;		/*!< Pointer to array of bytes to be transmitted */
	uint8_t *rxBuff;			/*!< Pointer memory where bytes received from I2C be stored */
	volatile uint16_t txSz;		/*!< Number of bytes in transmit array,
								         if 0 only receive transfer will be carried on */
	volatile uint16_t rxSz;		/*!< Number of bytes to received,
								         if 0 only transmission we be carried on */
	volatile uint16_t status;	/*!< Status of the current I2C transfer */
	uint8_t slaveAddr;			/*!< 7-bit I2C Slave address */
} I2CM_XFER_T;

/**
 * @brief	Sets HIGH and LOW duty cycle registers
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	sclH	: Number of I2C_PCLK cycles for the SCL HIGH time value between (2 - 9).
 * @param	sclL	: Number of I2C_PCLK cycles for the SCL LOW time value between (2 - 9).
 * @return	Nothing
 * @note	The I2C clock divider should be set to the appropriate value before calling this function
 *				The I2C baud is determined by the following formula: <br>
 *        I2C_bitFrequency = (I2C_PCLK)/(I2C_CLKDIV * (sclH + sclL)) <br>
 *				where I2C_PCLK is the frequency of the System clock and I2C_CLKDIV is I2C clock divider
 */
void Chip_I2CM_SetDutyCycle(LPC_I2C_T *pI2C, uint16_t sclH, uint16_t sclL);

/**
 * @brief	Set up bus speed for LPC_I2C controller
 * @param	pI2C		: Pointer to selected I2C peripheral
 * @param	busSpeed	: I2C bus clock rate
 * @return	Nothing
 * @note	Per I2C specification the busSpeed should be
 *          @li 100000 for Standard mode
 *          @li 400000 for Fast mode
 *          @li 1000000 for Fast mode plus
 *          IOCON registers corresponding to I2C pads should be updated
 *          according to the bus mode.
 */
void Chip_I2CM_SetBusSpeed(LPC_I2C_T *pI2C, uint32_t busSpeed);

/**
 * @brief	Enable I2C Master interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note
 */
static INLINE void Chip_I2CM_Enable(LPC_I2C_T *pI2C)
{
	Chip_I2C_SetRegMask(&pI2C->CFG, I2C_CFG_MASK, I2C_CFG_MSTEN);
}

/**
 * @brief	Disable I2C Master interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note
 */
static INLINE void Chip_I2CM_Disable(LPC_I2C_T *pI2C)
{
	Chip_I2C_ClearRegMask(&pI2C->CFG, I2C_CFG_MASK, I2C_CFG_MSTEN);
}

/**
 * @brief	Get I2C Status
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	I2C Status register value
 * @note	This function returns the value of the status register.
 */
static INLINE uint32_t Chip_I2CM_GetStatus(LPC_I2C_T *pI2C)
{
	return pI2C->STAT;
}

/**
 * @brief	Clear I2C status bits (master)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param clrStatus : Status bit to clear, ORed Value of I2C_STAT_MSTRARBLOSS and I2C_STAT_MSTSTSTPERR
 * @return	Nothing
 * @note	This function clears selected status flags.
 */
static INLINE void Chip_I2CM_ClearStatus(LPC_I2C_T *pI2C, uint32_t clrStatus)
{
	/* Clear Master Arbitration Loss and Start, Stop Error */
	pI2C->STAT = clrStatus & (I2C_STAT_MSTRARBLOSS | I2C_STAT_MSTSTSTPERR);
}

/**
 * @brief	Check if I2C Master is pending
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns TRUE if the Master is pending else returns FALSE
 * @note
 */
static INLINE bool Chip_I2CM_IsMasterPending(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_MSTPENDING) != 0;
}

/**
 * @brief	Get current state of the I2C Master
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Master State Code, a value in the range of 0 - 4
 * @note	After the Master is pending this state code tells the reason
 *        for Master pending.
 */
static INLINE uint32_t Chip_I2CM_GetMasterState(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_MSTSTATE) >> 1;
}

/**
 * @brief	Transmit START or Repeat-START signal on I2C bus
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to transmit START condition when
 *        the bus becomes free. This should be called only when master is pending.
 *				The function writes a complete value to Master Control register, ORing is not advised.
 */
static INLINE void Chip_I2CM_SendStart(LPC_I2C_T *pI2C)
{
	pI2C->MSTCTL = I2C_MSTCTL_MSTSTART;
}

/**
 * @brief	Transmit STOP signal on I2C bus
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to transmit STOP condition.
 *				This should be called only when master is pending. The function writes a
 *				complete value to Master Control register, ORing is not advised.
 */
static INLINE void Chip_I2CM_SendStop(LPC_I2C_T *pI2C)
{
	pI2C->MSTCTL = I2C_MSTCTL_MSTSTOP;
}

/**
 * @brief	Master Continue transfer operation
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the master controller to continue transmission.
 *				This should be called only when master is pending. The function writes a
 *				complete value to Master Control register, ORing is not advised.
 */
static INLINE void Chip_I2CM_MasterContinue(LPC_I2C_T *pI2C)
{
	pI2C->MSTCTL = I2C_MSTCTL_MSTCONTINUE;
}

/**
 * @brief	Transmit a single data byte through the I2C peripheral (master)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	data	: Byte to transmit
 * @return	Nothing
 * @note	This function attempts to place a byte into the I2C Master
 *			Data Register
 *
 */
static INLINE void Chip_I2CM_WriteByte(LPC_I2C_T *pI2C, uint8_t data)
{
	pI2C->MSTDAT = (uint32_t) data;
}

/**
 * @brief	Read a single byte data from the I2C peripheral (master)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	A single byte of data read
 * @note	This function reads a byte from the I2C receive hold register
 *			regardless of I2C state.
 */
static INLINE uint8_t Chip_I2CM_ReadByte(LPC_I2C_T *pI2C)
{
	return (uint8_t) (pI2C->MSTDAT & I2C_MSTDAT_DATAMASK);
}

/**
 * @brief	Transfer state change handler
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfer	: Pointer to a I2CM_XFER_T structure see notes below
 * @return Returns non-zero value on completion of transfer. The @a status
 *         member of @a xfer structure contains the current status of the
 *         transfer at the end of the call.
 * @note
 * The parameter @a xfer should be same as the one passed to Chip_I2CM_Xfer()
 * routine. This function should be called from the I2C interrupt handler
 * only when a master interrupt occurs.
 */
uint32_t Chip_I2CM_XferHandler(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

/**
 * @brief	Transmit and Receive data in master mode
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfer	: Pointer to a I2CM_XFER_T structure see notes below
 * @return	Nothing
 * @note
 * The parameter @a xfer should have its member @a slaveAddr initialized
 * to the 7-Bit slave address to which the master will do the xfer, Bit0
 * to bit6 should have the address and Bit8 is ignored. During the transfer
 * no code (like event handler) must change the content of the memory
 * pointed to by @a xfer. The member of @a xfer, @a txBuff and @a txSz be
 * initialized to the memory from which the I2C must pick the data to be
 * transfered to slave and the number of bytes to send respectively, similarly
 * @a rxBuff and @a rxSz must have pointer to memroy where data received
 * from slave be stored and the number of data to get from slave respectilvely.
 * Following types of transfers are possible:
 * - Write-only transfer: When @a rxSz member of @a xfer is set to 0.
 *
 *          S Addr Wr [A] txBuff0 [A] txBuff1 [A] ... txBuffN [A] P
 *
 *      - If I2CM_XFER_OPTION_IGNORE_NACK is set in @a options memeber
 *
 *          S Addr Wr [A] txBuff0 [A or NA] ... txBuffN [A or NA] P
 *
 * - Read-only transfer: When @a txSz member of @a xfer is set to 0.
 *
 *          S Addr Rd [A] [rxBuff0] A [rxBuff1] A ... [rxBuffN] NA P
 *
 *      - If I2CM_XFER_OPTION_LAST_RX_ACK is set in @a options memeber
 *
 *          S Addr Rd [A] [rxBuff0] A [rxBuff1] A ... [rxBuffN] A P
 *
 * - Read-Write transfer: When @a rxSz and @ txSz members of @a xfer are non-zero.
 *
 *          S Addr Wr [A] txBuff0 [A] txBuff1 [A] ... txBuffN [A]
 *              S Addr Rd [A] [rxBuff0] A [rxBuff1] A ... [rxBuffN] NA P
 *
 */
void Chip_I2CM_Xfer(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

/**
 * @brief	Transmit and Receive data in master mode
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfer	: Pointer to a I2CM_XFER_T structure see notes below
 * @return Returns non-zero value on successful completion of transfer.
 * @note
 * This function operates same as Chip_I2CM_Xfer(), but is a blocking call.
 */
uint32_t Chip_I2CM_XferBlocking(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

/**
 * @}
 */

 #ifdef __cplusplus
}
#endif

#endif /* __I2CM_5410X_H_ */
