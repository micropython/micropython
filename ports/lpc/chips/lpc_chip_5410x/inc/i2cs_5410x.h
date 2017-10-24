/*
 * @brief LPC5410X I2C slave driver
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

#ifndef __I2CS_5410X_H_
#define __I2CS_5410X_H_

#include "i2c_common_5410x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup I2CS_5410X CHIP: LPC5410X I2C slave-only driver
 * @ingroup I2C_5410X
 * This driver only works in slave mode.
 * @{
 */

/** @brief I2C slave service start callback
 * This callback is called from the I2C slave handler when an I2C slave address is
 * received and needs servicing. It's used to indicate the start of a slave transfer
 * that will happen on the slave bus.
 */
typedef void (*I2CSlaveXferStart)(uint8_t addr);

/** @brief I2C slave send data callback
 * This callback is called from the I2C slave handler when an I2C slave address needs
 * data to send.<br>
 * If you want to NAK the master, return I2C_SLVCTL_SLVNACK to the caller.
 * Return I2C_SLVCTL_SLVCONTINUE or 0 to the caller for normal non-DMA data transfer.
 * If you've setup a DMA descriptor for the transfer, return I2C_SLVCTL_SLVDMA to the caller.<br>
 */
typedef uint8_t (*I2CSlaveXferSend)(uint8_t *data);

/** @brief I2C slave receive data callback
 * This callback is called from the I2C slave handler when an I2C slave address has
 * receive data.<br>
 * If you want to NAK the master, return I2C_SLVCTL_SLVNACK to the caller.
 * Return I2C_SLVCTL_SLVCONTINUE or 0 to the caller for normal non-DMA data transfer.
 * If you've setup a DMA descriptor for the transfer, return I2C_SLVCTL_SLVDMA to the caller.<br>
 */
typedef uint8_t (*I2CSlaveXferRecv)(uint8_t data);

/** @brief I2C slave service done callback
 * This callback is called from the I2C slave handler when an I2C slave transfer is
 * completed. It's used to indicate the end of a slave transfer.
 */
typedef void (*I2CSlaveXferDone)(void);

/**
 * Slave transfer are performed using 4 callbacks. These 3 callbacks handle most I2C
 * slave transfer cases. When the slave is setup and a slave interrupt is receive
 * and processed with the Chip_I2CS_XferHandler() function in the I2C interrupt handler,
 * one of these 4 callbacks is called. The callbacks can be used for unsized transfers
 * from the master.
 *
 * When an address is received, the SlaveXferAddr() callback is called with the
 * received address. Only addresses enabled in the slave controller will be handled.
 * The slave controller can support up to 4 slave addresses.
 *
 * If the master is going to perform a read operation, the SlaveXferSend() callback
 * is called. Place the data byte to send in *data and return a value of 0 or
 * I2C_SLVCTL_SLVCONTINUE to the caller, or return a value o I2C_SLVCTL_SLVNACK to
 * NACK the master. If you are using DMA and have setup a DMA descriptor in the
 * callback, return I2C_SLVCTL_SLVDMA.<br>
 *
 * If the master performs a write operation, the SlaveXferRecv() callback is called
 * with the received data. Return a value of 0 or I2C_SLVCTL_SLVCONTINUE to the caller
 * to continue data transfer. Return I2C_SLVCTL_SLVNACK to NACk the master. If you
 * are using DMA and have setup a DMA descriptor in the callback, return
 * I2C_SLVCTL_SLVDMA.<br>
 *
 * Once the transfer completes, the SlaveXferDone() callback will be called.<br>
 */
typedef struct {
	I2CSlaveXferStart slaveStart;	/*!< Called when an matching I2C slave address is received */
	I2CSlaveXferSend slaveSend;		/*!< Called when a byte is needed to send to master */
	I2CSlaveXferRecv slaveRecv;		/*!< Called when a byte is received from master */
	I2CSlaveXferDone slaveDone;		/*!< Called when a slave transfer is complete */
} I2CS_XFER_T;

/**
 * @brief	Enable I2C slave interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	Do not call this function until the slave interface is fully configured.
 */
__STATIC_INLINE void Chip_I2CS_Enable(LPC_I2C_T *pI2C)
{
	Chip_I2C_SetRegMask(&pI2C->CFG, I2C_CFG_MASK, I2C_CFG_SLVEN);
}

/**
 * @brief	Disable I2C slave interface
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 */
__STATIC_INLINE void Chip_I2CS_Disable(LPC_I2C_T *pI2C)
{
	Chip_I2C_ClearRegMask(&pI2C->CFG, I2C_CFG_MASK, I2C_CFG_SLVEN);
}

/**
 * @brief	Get I2C Status
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	I2C Status register value
 * @note	This function returns the value of the status register.
 */
__STATIC_INLINE uint32_t Chip_I2CS_GetStatus(LPC_I2C_T *pI2C)
{
	return pI2C->STAT;
}

/**
 * @brief	Clear I2C status bits (slave)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param clrStatus : Status bit to clear, must be I2C_STAT_SLVDESEL
 * @return	Nothing
 * @note	This function clears selected status flags.
 */
__STATIC_INLINE void Chip_I2CS_ClearStatus(LPC_I2C_T *pI2C, uint32_t clrStatus)
{
	pI2C->STAT = clrStatus & I2C_STAT_SLVDESEL;
}

/**
 * @brief	Check if I2C slave is pending
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns TRUE if the slave is pending else returns FALSE
 * @note
 */
__STATIC_INLINE bool Chip_I2CS_IsSlavePending(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_SLVPENDING) != 0;
}

/**
 * @brief	Check if I2C slave is selected
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns TRUE if the slave is is selected, otherwise FALSE
 * @note
 */
__STATIC_INLINE bool Chip_I2CS_IsSlaveSelected(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_SLVSEL) != 0;
}

/**
 * @brief	Check if I2C slave is deselected
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Returns TRUE if the slave is is deselected, otherwise FALSE
 * @note
 */
__STATIC_INLINE bool Chip_I2CS_IsSlaveDeSelected(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_SLVDESEL) != 0;
}

/**
 * @brief	Get current state of the I2C slave
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	slave State Code, a value of type I2C_STAT_SLVCODE_*
 * @note	After the slave is pending this state code tells the reason
 *        for slave pending.
 */
__STATIC_INLINE uint32_t Chip_I2CS_GetSlaveState(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_SLVSTATE) >> 9;
}

/**
 * @brief	Returns the current slave address match index
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	slave match index, 0 - 3
 */
__STATIC_INLINE uint32_t Chip_I2CS_GetSlaveMatchIndex(LPC_I2C_T *pI2C)
{
	return (pI2C->STAT & I2C_STAT_SLVIDX) >> 12;
}

/**
 * @brief	Slave Continue transfer operation (ACK)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the slave controller to continue transmission.
 *				This should be called only when slave is pending. The function writes a
 *				complete value to slave Control register, ORing is not advised.
 */
__STATIC_INLINE void Chip_I2CS_SlaveContinue(LPC_I2C_T *pI2C)
{
	pI2C->SLVCTL = I2C_SLVCTL_SLVCONTINUE;
}

/**
 * @brief	Slave NACK operation
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function sets the slave controller to NAK the master.
 */
__STATIC_INLINE void Chip_I2CS_SlaveNACK(LPC_I2C_T *pI2C)
{
	pI2C->SLVCTL = I2C_SLVCTL_SLVNACK;
}

/**
 * @brief	Enable slave DMA operation
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function enables DMA mode for the slave controller. In DMA
 * mode, the 'continue' and 'NACK' operations aren't used and the I2C slave
 * controller will automatically NACK any bytes beyond the available DMA
 * buffer size.
 */
__STATIC_INLINE void Chip_I2CS_SlaveEnableDMA(LPC_I2C_T *pI2C)
{
	pI2C->SLVCTL = I2C_SLVCTL_SLVDMA;
}

/**
 * @brief	Disable slave DMA operation
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	Nothing
 * @note	This function disables DMA mode for the slave controller.
 */
__STATIC_INLINE void Chip_I2CS_SlaveDisableDMA(LPC_I2C_T *pI2C)
{
	pI2C->SLVCTL = 0;
}

/**
 * @brief	Transmit a single data byte through the I2C peripheral (slave)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	data	: Byte to transmit
 * @return	Nothing
 * @note	This function attempts to place a byte into the I2C slave
 *			Data Register
 *
 */
__STATIC_INLINE void Chip_I2CS_WriteByte(LPC_I2C_T *pI2C, uint8_t data)
{
	pI2C->SLVDAT = (uint32_t) data;
}

/**
 * @brief	Read a single byte data from the I2C peripheral (slave)
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @return	A single byte of data read
 * @note	This function reads a byte from the I2C receive hold register
 *			regardless of I2C state.
 */
__STATIC_INLINE uint8_t Chip_I2CS_ReadByte(LPC_I2C_T *pI2C)
{
	return (uint8_t) (pI2C->SLVDAT & I2C_SLVDAT_DATAMASK);
}

/**
 * @brief	Set a I2C slave address for slave operation
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	slvNum	: Possible slave address number, between 0 - 3
 * @param	slvAddr	: Slave Address for the index (7-bits, bit 7 = 0)
 * @return	Nothing
 * @note	Setting a slave address also enables the slave address. Do
 * not 'pre-shift' the slave address.
 */
__STATIC_INLINE void Chip_I2CS_SetSlaveAddr(LPC_I2C_T *pI2C, uint8_t slvNum, uint8_t slvAddr)
{
	pI2C->SLVADR[slvNum] = (uint32_t) (slvAddr << 1);
}

/**
 * @brief	Return a I2C programmed slave address
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	slvNum	: Possible slave address number, between 0 - 3
 * @return	Nothing
 */
__STATIC_INLINE uint8_t Chip_I2CS_GetSlaveAddr(LPC_I2C_T *pI2C, uint8_t slvNum)
{
	return (pI2C->SLVADR[slvNum] >> 1) & 0x7F;
}

/**
 * @brief	Enable a I2C address
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	slvNum	: Possible slave address number, between 0 - 3
 * @return	Nothing
 */
__STATIC_INLINE void Chip_I2CS_EnableSlaveAddr(LPC_I2C_T *pI2C, uint8_t slvNum)
{
	pI2C->SLVADR[slvNum] = (pI2C->SLVADR[slvNum] & I2C_SLVADR_MASK) & ~I2C_SLVADR_SADISABLE;
}

/**
 * @brief	Disable a I2C address
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	slvNum	: Possible slave address number, between 0 - 3
 * @return	Nothing
 */
__STATIC_INLINE void Chip_I2CS_DisableSlaveAddr(LPC_I2C_T *pI2C, uint8_t slvNum)
{
	pI2C->SLVADR[slvNum] = (pI2C->SLVADR[slvNum] & I2C_SLVADR_MASK) | I2C_SLVADR_SADISABLE;
}

/**
 * @brief	Setup slave qialifier address
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	extend	: true to extend I2C slave detect address 0 range, or false to match to corresponding bits
 * @param	slvAddr	: Slave address qualifier, see SLVQUAL0 register in User Manual
 * @return	Nothing
 * @note	Do not 'pre-shift' the slave address.
 */
__STATIC_INLINE void Chip_I2CS_SetSlaveQual0(LPC_I2C_T *pI2C, bool extend, uint8_t slvAddr)
{
	slvAddr = slvAddr << 1;
	if (extend) {
		slvAddr |= I2C_SLVQUAL_QUALMODE0;
	}

	pI2C->SLVQUAL0 = slvAddr;
}

/**
 * @brief	Slave transfer state change handler
 * @param	pI2C	: Pointer to selected I2C peripheral
 * @param	xfers	: Pointer to a I2CS_MULTI_XFER_T structure see notes below
 * @return	Returns non-zero value on completion of transfer or NAK
 * @note	See @ref I2CS_XFER_T for more information on this function. When using
 * this function, the I2C_INTENSET_SLVPENDING and I2C_INTENSET_SLVDESEL interrupts
 * should be enabled and setup in the I2C interrupt handler to call this function
 * when they fire.
 */
uint32_t Chip_I2CS_XferHandler(LPC_I2C_T *pI2C, const I2CS_XFER_T *xfers);

/**
 * @}
 */

 #ifdef __cplusplus
}
#endif

#endif /* __I2CS_5410X_H_ */
