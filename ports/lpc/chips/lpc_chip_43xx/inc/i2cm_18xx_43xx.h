/*
 * @brief LPC18xx/43xx I2C driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

#ifndef __I2CM_18XX_43XX_H_
#define __I2CM_18XX_43XX_H_

#include "i2c_common_18xx_43xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2CM_18XX_43XX CHIP: LPC18xx/43xx I2C master-only driver
 * @ingroup I2C_18XX_43XX
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

/** I2CM_18XX_43XX_OPTIONS_TYPES I2C master transfer options
 * @{
 */

/** Ignore NACK during data transfer. By default transfer is aborted. */
#define I2CM_XFER_OPTION_IGNORE_NACK     0x01
/** ACK last byte received. By default we NACK last byte we receive per I2C spec. */
#define I2CM_XFER_OPTION_LAST_RX_ACK     0x02

/**
 * @}
 */

/** I2CM_18XX_43XX_STATUS_TYPES I2C master transfer status types
 * @{
 */

#define I2CM_STATUS_OK              0x00		/*!< Requested Request was executed successfully. */
#define I2CM_STATUS_ERROR           0x01		/*!< Unknown error condition. */
#define I2CM_STATUS_NAK             0x02		/*!< No acknowledgement received from slave. */
#define I2CM_STATUS_BUS_ERROR       0x03		/*!< I2C bus error */
#define I2CM_STATUS_SLAVE_NAK       0x04		/*!< No device responded for given slave address during SLA+W or SLA+R */
#define I2CM_STATUS_ARBLOST         0x05		/*!< Arbitration lost. */
#define I2CM_STATUS_BUSY            0xFF		/*!< I2C transmitter is busy. */

/**
 * @}
 */

/**
 * @brief Master transfer data structure definitions
 */
typedef struct {
	uint8_t slaveAddr;		/*!< 7-bit I2C Slave address */
	uint8_t options;		/*!< Options for transfer*/
	uint16_t status;		/*!< Status of the current I2C transfer */
	uint16_t txSz;			/*!< Number of bytes in transmit array,
							   if 0 only receive transfer will be carried on */
	uint16_t rxSz;			/*!< Number of bytes to received,
							   if 0 only transmission we be carried on */
	const uint8_t *txBuff;	/*!< Pointer to array of bytes to be transmitted */
	uint8_t *rxBuff;		/*!< Pointer memory where bytes received from I2C be stored */
} I2CM_XFER_T;

/**
 * @brief	Initialize I2C Interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function enables the I2C clock.
 */
void Chip_I2CM_Init(LPC_I2C_T *pI2C);

/**
 * @brief	Shutdown I2C Interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function disables the I2C clock.
 */
void Chip_I2CM_DeInit(LPC_I2C_T *pI2C);

/**
 * @brief	Sets HIGH and LOW duty cycle registers
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	sclH	: Number of I2C_PCLK cycles for the SCL HIGH time.
 * @param	sclL	: Number of I2C_PCLK cycles for the SCL LOW time.
 * @return	Nothing
 * @note	The frequency is determined by the following formula (I2C_PCLK
 *          is the frequency of the peripheral I2C clock): <br>
 *              I2C_bitFrequency = (I2C_PCLK)/(sclH + sclL);
 */
static INLINE void Chip_I2CM_SetDutyCycle(LPC_I2C_T *pI2C, uint16_t sclH, uint16_t sclL)
{
	pI2C->SCLH = (uint32_t) sclH;
	pI2C->SCLL = (uint32_t) sclL;
}

/**
 * @brief	Set up bus speed for LPC_I2C controller
 * @param	pI2C	: Pointer to selected I2C peripheral
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
 * @brief	Transmit START or Repeat-START signal on I2C bus
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to transmit START condition when
 *          the bus becomes free.
 */
static INLINE void Chip_I2CM_SendStart(LPC_I2C_T *pI2C)
{
	pI2C->CONSET = I2C_CON_I2EN | I2C_CON_STA;
}

/**
 * @brief	Reset I2C controller state
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function clears all control/status flags.
 */
static INLINE void Chip_I2CM_ResetControl(LPC_I2C_T *pI2C)
{
	/* Reset STA, AA and SI. Stop flag should not be cleared as it is a reserved bit */
	pI2C->CONCLR = I2C_CON_SI | I2C_CON_STA | I2C_CON_AA;

}

/**
 * @brief	Transmit a single data byte through the I2C peripheral
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	data	: Byte to transmit
 * @return	Nothing
 * @note	This function attempts to place a byte into the UART transmit
 *			FIFO or transmit hold register regard regardless of UART state
 *
 */
static INLINE void Chip_I2CM_WriteByte(LPC_I2C_T *pI2C, uint8_t data)
{
	pI2C->DAT = (uint32_t) data;
}

/**
 * @brief	Read a single byte data from the I2C peripheral
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	A single byte of data read
 * @note	This function reads a byte from the I2C receive hold register
 *			regardless of I2C state. The I2C status should be read first prior
 *			to using this function.
 */
static INLINE uint8_t Chip_I2CM_ReadByte(LPC_I2C_T *pI2C)
{
	return (uint8_t) (pI2C->DAT & I2C_I2DAT_BITMASK);
}

/**
 * @brief	Generate NACK after receiving next byte
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to NACK after receiving next
 *          byte from slave transmitter. Used before receiving last byte.
 */
static INLINE void Chip_I2CM_NackNextByte(LPC_I2C_T *pI2C)
{
	pI2C->CONCLR = I2C_CON_AA;
}

/**
 * @brief	Transmit STOP signal on I2C bus
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to transmit STOP condition.
 */
static INLINE void Chip_I2CM_SendStop(LPC_I2C_T *pI2C)
{
	pI2C->CONSET = I2C_CON_STO;
}

/**
 * @brief	Force start I2C transmit
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function forces I2C state machine to start transmitting.
 *			If an uncontrolled source generates a superfluous START or masks
 *          a STOP condition, then the I2C-bus stays busy indefinitely. If
 *          the STA flag is set and bus access is not obtained within a
 *          reasonable amount of time, then a forced access to the I2C-bus is
 *          possible. This is achieved by setting the STO flag while the STA
 *          flag is still set. No STOP condition is transmitted.
 */
static INLINE void Chip_I2CM_ForceStart(LPC_I2C_T *pI2C)
{
	/* check if we are pending on start */
	if (pI2C->CONSET & I2C_CON_STA) {
		pI2C->CONSET = I2C_CON_STO;
	}
	else {
		Chip_I2CM_SendStart(pI2C);
	}
}

/**
 * @brief	Transmit STOP+START signal on I2C bus
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the controller to transmit STOP condition
 *          followed by a START condition.
 */
static INLINE void Chip_I2CM_SendStartAfterStop(LPC_I2C_T *pI2C)
{
	pI2C->CONSET = I2C_CON_STO | I2C_CON_STA;
}

/**
 * @brief	Check if I2C controller state changed
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns 0 if state didn't change
 * @note
 */
static INLINE uint32_t Chip_I2CM_StateChanged(LPC_I2C_T *pI2C)
{
	return pI2C->CONSET & I2C_CON_SI;
}

/**
 * @brief	Clear state change interrupt flag
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note
 */
static INLINE void Chip_I2CM_ClearSI(LPC_I2C_T *pI2C)
{
	/* Stop flag should not be cleared as it is a reserved bit */
	pI2C->CONCLR = I2C_CON_SI | I2C_CON_STA;
}

/**
 * @brief	Check if I2C bus is free per our controller
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns 0 if busy else a non-zero value.
 * @note	I2C controller clears STO bit when it sees STOP
 *          condition after a START condition on the bus.
 */
static INLINE uint32_t Chip_I2CM_BusFree(LPC_I2C_T *pI2C)
{
	return !(pI2C->CONSET & I2C_CON_STO);
}

/**
 * @brief	Get current state of the I2C controller
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns 0 if busy else a non-zero value.
 * @note	I2C controller clears STO bit when it sees STOP
 *          condition after a START condition on the bus.
 */
static INLINE uint32_t Chip_I2CM_GetCurState(LPC_I2C_T *pI2C)
{
	return pI2C->STAT & I2C_STAT_CODE_BITMASK;
}

/**
 * @brief	Disable I2C interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note
 */
static INLINE void Chip_I2CM_Disable(LPC_I2C_T *pI2C)
{
	pI2C->CONCLR = I2C_CON_I2EN;
}

/**
 * @brief	Transfer state change handler handler
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfer	: Pointer to a I2CM_XFER_T structure see notes below
 * @return Returns non-zero value on completion of transfer. The @a status
 *         member of @a xfer structure contains the current status of the
 *         transfer at the end of the call.
 * @note
 * The parameter @a xfer should be same as the one passed to Chip_I2CM_Xfer()
 * routine.
 */
uint32_t Chip_I2CM_XferHandler(LPC_I2C_T *pI2C, I2CM_XFER_T *xfer);

/**
 * @brief	Transmit and Receive data in master mode
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfer	: Pointer to a I2CM_XFER_T structure see notes below
 * @return Nothing.
 * @note
 * The parameter @a xfer should have its member @a slaveAddr initialized
 * to the 7-Bit slave address to which the master will do the xfer, Bit0
 * to bit6 should have the address and Bit8 is ignored. During the transfer
 * no code (like event handler) must change the content of the memory
 * pointed to by @a xfer. The member of @a xfer, @a txBuff and @a txSz be
 * initialized to the memory from which the I2C must pick the data to be
 * transferred to slave and the number of bytes to send respectively, similarly
 * @a rxBuff and @a rxSz must have pointer to memory where data received
 * from slave be stored and the number of data to get from slave respectively.
 * Following types of transfers are possible:
 * - Write-only transfer: When @a rxSz member of @a xfer is set to 0.
 *
 *          S Addr Wr [A] txBuff0 [A] txBuff1 [A] ... txBuffN [A] P
 *
 *      - If I2CM_XFER_OPTION_IGNORE_NACK is set in @a options member
 *
 *          S Addr Wr [A] txBuff0 [A or NA] ... txBuffN [A or NA] P
 *
 * - Read-only transfer: When @a txSz member of @a xfer is set to 0.
 *
 *          S Addr Rd [A] [rxBuff0] A [rxBuff1] A ... [rxBuffN] NA P
 *
 *      - If I2CM_XFER_OPTION_LAST_RX_ACK is set in @a options member
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
 * @brief	Write given buffer of data to I2C interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	buff	: Pointer to buffer to be transmitted
 * @param	len     : Length of the buffer
 * @return	Returns number of bytes written.
 * @note	This function is a blocking call. The function generates
 *          START/repeat-START condition on bus and starts transmitting
 *          data until transfer finishes or a NACK is received. No
 *          STOP condition is transmitted on the bus.
 *
 *          S Data0 [A] Data1 [A] ... DataN [A]
 */
uint32_t Chip_I2CM_Write(LPC_I2C_T *pI2C, const uint8_t *buff, uint32_t len);

/**
 * @brief	Read data from I2C slave to given buffer
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	buff	:   Pointer to buffer for data received from I2C slave
 * @param	len     : Length of the buffer
 * @return	Returns number of bytes read.
 * @note	This function is a blocking call. The function generates
 *          START/repeat-START condition on bus and starts reading
 *          data until requested number of bytes are read. No
 *          STOP condition is transmitted on the bus.
 *
 *          S [Data0] A [Data1] A ... [DataN] A
 */
uint32_t Chip_I2CM_Read(LPC_I2C_T *pI2C, uint8_t *buff, uint32_t len);

/**
 * @}
 */

 #ifdef __cplusplus
}
#endif

#endif /* __I2C_18XX_43XX_H_ */






