/*
 * @brief LPC5410X UART driver
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

#ifndef __UART_5410X_H_
#define __UART_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ring_buffer.h"

/** @defgroup UART_5410X CHIP: LPC5410X UART Driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/* UART Status Register bits */
#define UART_RXRDY           (1 << 0)	/* Receive data ready */
#define UART_RXIDLE          (1 << 1)	/* Receiver Idle */
#define UART_TXRDY           (1 << 2)	/* Transmitter ready */
#define UART_TXIDLE          (1 << 3)	/* Transmitter Idle */
#define UART_RXDERR          (0xF100)	/* overrun err, frame err, parity err, RxNoise err */
#define UART_TXDERR          (0x0200)	/* underrun err */
#define UART_START           (0x1000)

/* UART Interrupt register bits */
#define UART_INT_RXRDY          (1 << 0)
#define UART_INT_TXRDY          (1 << 2)
#define UART_INT_TXIDLE         (1 << 3)
#define UART_INT_CTS            (1 << 5)
#define UART_INT_TXDIS          (1 << 6)
#define UART_INT_OVR            (1 << 8)
#define UART_INT_BREAK          (1 << 11)
#define UART_INT_START          (1 << 12)
#define UART_INT_FRMERR         (1 << 13)
#define UART_INT_PARERR         (1 << 14)
#define UART_INT_RXNOISE        (1 << 15)
#define UART_INT_ABAUDERR       (1 << 16)

/* Configuration register bits */
#define UARTEN      1

#define UART_CTL_TXDIS          (1UL << 6)
#define UART_CTL_TXBRKEN        (1UL << 1)
#define UART_CTL_AUTOBAUD       (1UL << 16)
#define UART_CFG_RES            (2UL | (1UL << 10) | (1UL << 13) | (1UL << 17) | (0xFFUL << 24))
#define UART_CFG_ENABLE          1
#define UART_PAR_MASK           (3 << 4)
#define UART_DATA_MASK          (3 << 2)
#define UART_CTL_RES            (1UL | (7UL << 3) | (1UL << 7) | (0x3FUL << 10) | (0x7FFFUL << 17))
#define UART_IDLE_MASK          (1 << 3)
#define UART_STAT_CTS           (1 << 4)
#define UART_STAT_BREAK         (1 << 10)
#define UART_STAT_RXIDLE        (1 << 1)

/*******************
 * EXPORTED MACROS  *
 ********************/
#define     ECHO_EN             1
#define     ECHO_DIS            0

/*********************
 * EXPORTED TYPEDEFS  *
 **********************/

typedef struct {		/* UART registers Structure          */
	__IO uint32_t CFG;				/*!< Offset: 0x000 Configuration register  */
	__IO uint32_t CTL;				/*!< Offset: 0x004 Control register */
	__IO uint32_t STAT;				/*!< Offset: 0x008 Status register */
	__IO uint32_t INTENSET;			/*!< Offset: 0x00C Interrupt Enable Read and Set register */
	__O  uint32_t INTENCLR;			/*!< Offset: 0x010 Interrupt Enable Clear register */
	__I  uint32_t RXDAT;		/*!< Offset: 0x014 Receiver Data register */
	__I  uint32_t RXDATSTAT;	/*!< Offset: 0x018 Rx Data with status */
	__O  uint32_t TXDAT;			/*!< Offset: 0x01C Transmitter Data Register */
	__IO uint32_t BRG;				/*!< Offset: 0x020 Baud Rate Generator register */
	__I  uint32_t INTSTAT;	/*!< Offset: 0x024 Interrupt Status register */
	__IO uint32_t OSR;				/*!< Offset: 0x028 Oversampling register */
	__IO uint32_t ADR;				/*!< Offset: 0x02C Address register (for automatic address matching) */
} LPC_USART_T;

/**
 * @brief UART CFG register definitions
 */
// #define UART_CFG_ENABLE         (0x01 << 0)
#define UART_CFG_DATALEN_7      (0x00 << 2)		/*!< UART 7 bit length mode */
#define UART_CFG_DATALEN_8      (0x01 << 2)		/*!< UART 8 bit length mode */
#define UART_CFG_DATALEN_9      (0x02 << 2)		/*!< UART 9 bit length mode */
#define UART_CFG_PARITY_NONE    (0x00 << 4)		/*!< No parity */
#define UART_CFG_PARITY_EVEN    (0x02 << 4)		/*!< Even parity */
#define UART_CFG_PARITY_ODD     (0x03 << 4)		/*!< Odd parity */
#define UART_CFG_STOPLEN_1      (0x00 << 6)		/*!< UART One Stop Bit Select */
#define UART_CFG_STOPLEN_2      (0x01 << 6)		/*!< UART Two Stop Bits Select */
#define UART_CFG_MODE32K        (0x01 << 7)		/*!< UART 32K MODE */
#define UART_CFG_LINMODE        (0x01 << 8)		/*!< UART LIN MODE */
#define UART_CFG_CTSEN          (0x01 << 9)		/*!< CTS enable bit */
#define UART_CFG_SYNCEN         (0x01 << 11)	/*!< Synchronous mode enable bit */
#define UART_CFG_CLKPOL         (0x01 << 12)	/*!< Un_RXD rising edge sample enable bit */
#define UART_CFG_SYNCMST        (0x01 << 14)	/*!< Select master mode (synchronous mode) enable bit */
#define UART_CFG_LOOP           (0x01 << 15)	/*!< Loopback mode enable bit */

/**
 * @brief UART CTRL register definitions
 */
#define UART_CTRL_TXBRKEN       (0x01 << 1)		/*!< Continuous break enable bit */
#define UART_CTRL_ADDRDET       (0x01 << 2)		/*!< Address detect mode enable bit */
#define UART_CTRL_TXDIS         (0x01 << 6)		/*!< Transmit disable bit */
#define UART_CTRL_CC            (0x01 << 8)		/*!< Continuous Clock mode enable bit */
#define UART_CTRL_CLRCC         (0x01 << 9)		/*!< Clear Continuous Clock bit */
#define UART_CTRL_AUTOBAUD      (0x01 << 16)	/*!< Auto baud bit */

/**
 * @brief UART STAT register definitions
 */
#define UART_STAT_RXRDY         (0x01 << 0)			/*!< Receiver ready */
// #define UART_STAT_RXIDLE        (0x01 << 1)			/*!< Receiver idle */
#define UART_STAT_TXRDY         (0x01 << 2)			/*!< Transmitter ready for data */
#define UART_STAT_TXIDLE        (0x01 << 3)			/*!< Transmitter idle */
// #define UART_STAT_CTS           (0x01 << 4)			/*!< Status of CTS signal */
#define UART_STAT_DELTACTS      (0x01 << 5)			/*!< Change in CTS state */
#define UART_STAT_TXDISINT      (0x01 << 6)			/*!< Transmitter disabled */
#define UART_STAT_OVERRUNINT    (0x01 << 8)			/*!< Overrun Error interrupt flag. */
#define UART_STAT_RXBRK         (0x01 << 10)		/*!< Received break */
#define UART_STAT_DELTARXBRK    (0x01 << 11)		/*!< Change in receive break detection */
#define UART_STAT_START         (0x01 << 12)		/*!< Start detected */
#define UART_STAT_FRM_ERRINT    (0x01 << 13)		/*!< Framing Error interrupt flag */
#define UART_STAT_PAR_ERRINT    (0x01 << 14)		/*!< Parity Error interrupt flag */
#define UART_STAT_RXNOISEINT    (0x01 << 15)		/*!< Received Noise interrupt flag */
#define UART_STAT_ABERR         (0x01 << 16)		/*!< Auto baud error flag */

/**
 * @brief UART INTENSET/INTENCLR register definitions
 */
#define UART_INTEN_RXRDY        (0x01 << 0)			/*!< Receive Ready interrupt */
#define UART_INTEN_TXRDY        (0x01 << 2)			/*!< Transmit Ready interrupt */
#define UART_INTEN_DELTACTS     (0x01 << 5)			/*!< Change in CTS state interrupt */
#define UART_INTEN_TXDIS        (0x01 << 6)			/*!< Transmitter disable interrupt */
#define UART_INTEN_OVERRUN      (0x01 << 8)			/*!< Overrun error interrupt */
#define UART_INTEN_DELTARXBRK   (0x01 << 11)		/*!< Change in receiver break detection interrupt */
#define UART_INTEN_START        (0x01 << 12)		/*!< Start detect interrupt */
#define UART_INTEN_FRAMERR      (0x01 << 13)		/*!< Frame error interrupt */
#define UART_INTEN_PARITYERR    (0x01 << 14)		/*!< Parity error interrupt */
#define UART_INTEN_RXNOISE      (0x01 << 15)		/*!< Received noise interrupt */

/**
 * @brief	UART Baud rate calculation structure
 * @note
 * Use oversampling (@a ovr) value other than 16, only if the difference
 * between the actual baud and desired baud has an unacceptable error percentage.
 * Smaller @a ovr values can cause the sampling position within the data-bit
 * less accurate an may potentially cause more noise errors or incorrect data
 * set ovr to < 10 only when there is no other higher values suitable. Note that
 * the UART OSR and BRG are -1 encoded i.e., when writing to register BRG @a div must
 * be (div - 1) and when writing to OSR @a ovr must be (ovr - 1)
 */
typedef struct {
	uint32_t clk;	/*!< IN: Base clock to fractional divider; OUT: "Base clock rate for UART" */
	uint32_t baud;	/*!< IN: Required baud rate; OUT: Actual baud rate */
	uint8_t ovr;	/*!< IN: Number of desired over samples [0-auto detect or values 5 to 16]; OUT: Auto detected over samples [unchanged if IN is not 0] */
	uint8_t mul;	/*!< IN: 0 - calculate MUL, 1 - do't calculate (@a clk) has UART base clock; OUT: MUL value to be set in FRG register */
	uint16_t div;	/*!< OUT: Integer divider to divide the "Base clock rate for UART" */
} UART_BAUD_T;

/**
 * @brief	Calculate baudrate parameters for a UART
 * @param	pUART		: Pointer to selected UARTx peripheral (Ignored)
 * @param	pBaud		: Pointer to baud structure #UART_BAUD_T
 * @return	LPC_OK(0) on success; non-zero on failure
 * @note	Values returned by pBaud is valid only if the return value is LPC_OK.
 */
uint32_t Chip_UART_CalcBaud(LPC_USART_T *pUART, UART_BAUD_T *pBaud);

/**
 * @brief	Enable the UART
 * @param	pUART		: Pointer to selected UARTx peripheral
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UART_Enable(LPC_USART_T *pUART)
{
	pUART->CFG |= UART_CFG_ENABLE;
}

/**
 * @brief	Disable the UART
 * @param	pUART	: Pointer to selected UARTx peripheral
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UART_Disable(LPC_USART_T *pUART)
{
	pUART->CFG &= ~UART_CFG_ENABLE;
}

/**
 * @brief	Enable transmission on UART TxD pin
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return Nothing
 */
__STATIC_INLINE void Chip_UART_TXEnable(LPC_USART_T *pUART)
{
	pUART->CTL &= ~UART_CTRL_TXDIS;
}

/**
 * @brief	Disable transmission on UART TxD pin
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return Nothing
 */
__STATIC_INLINE void Chip_UART_TXDisable(LPC_USART_T *pUART)
{
	pUART->CTL |= UART_CTRL_TXDIS;
}

/**
 * @brief	Set auto baud
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return true if auto baud succeeds, false if fails
 */
__STATIC_INLINE uint32_t Chip_UART_AutoBaud(LPC_USART_T *pUART)
{
	while ( (pUART->STAT & UART_STAT_RXIDLE) != UART_STAT_RXIDLE ) {}
	pUART->CTL |= UART_CTRL_AUTOBAUD;
	while ( pUART->CTL & UART_CTRL_AUTOBAUD ) {
		if ( pUART->STAT & UART_STAT_ABERR ) {
			pUART->STAT = UART_STAT_ABERR;
			return false;
		}
	}
	return true;
}

/**
 * @brief	Transmit a single data byte through the UART peripheral
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	data	: Byte to transmit
 * @return	Nothing
 * @note	This function attempts to place a byte into the UART transmit
 *			holding register regard regardless of UART state.
 */
__STATIC_INLINE void Chip_UART_SendByte(LPC_USART_T *pUART, uint8_t data)
{
	pUART->TXDAT = (uint32_t) data;
}

/**
 * @brief	Read a single byte data from the UART peripheral
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	A single byte of data read
 * @note	This function reads a byte from the UART receive FIFO or
 *			receive hold register regard regardless of UART state. The
 *			FIFO status should be read first prior to using this function
 */
__STATIC_INLINE uint32_t Chip_UART_ReadByte(LPC_USART_T *pUART)
{
	/* Strip off undefined reserved bits, keep 9 lower bits */
	return (uint32_t) (pUART->RXDAT & 0x000001FF);
}

/**
 * @brief	Sets the UART baudrate divider values (BRG and OSR)
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	div		: Baud rate clock divider value
 * @param	ovr		: Over sampling value to be used
 * @return	Nothing
 */
__STATIC_INLINE void Chip_UART_Div(LPC_USART_T *pUART, uint32_t div, uint32_t ovr)
{
	if (div) {
		pUART->BRG = div - 1;
	}
	if (ovr) {
		pUART->OSR = ovr - 1;
	}
}

/**
 * @brief	Enable UART interrupts
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	intMask	: OR'ed Interrupts to enable
 * @return	Nothing
 * @note	Use an OR'ed value of UART_INTEN_* definitions with this function
 *			to enable specific UART interrupts.
 */
__STATIC_INLINE void Chip_UART_IntEnable(LPC_USART_T *pUART, uint32_t intMask)
{
	pUART->INTENSET = intMask;
}

/**
 * @brief	Disable UART interrupts
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	intMask	: OR'ed Interrupts to disable
 * @return	Nothing
 * @note	Use an OR'ed value of UART_INTEN_* definitions with this function
 *			to disable specific UART interrupts.
 */
__STATIC_INLINE void Chip_UART_IntDisable(LPC_USART_T *pUART, uint32_t intMask)
{
	pUART->INTENCLR = intMask;
}

/**
 * @brief	Returns UART interrupts that are enabled
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Returns the enabled UART interrupts
 * @note	Use an OR'ed value of UART_INTEN_* definitions with this function
 *			to determine which interrupts are enabled. You can check
 *			for multiple enabled bits if needed.
 */
__STATIC_INLINE uint32_t Chip_UART_GetIntsEnabled(LPC_USART_T *pUART)
{
	return pUART->INTENSET;
}

/**
 * @brief	Get UART interrupt status
 * @param	pUART	: The base of UART peripheral on the chip
 * @return	The Interrupt status register of UART
 * @note	Multiple interrupts may be pending. Mask the return value
 *			with one or more UART_INTEN_* definitions to determine
 *			pending interrupts.
 */
__STATIC_INLINE uint32_t Chip_UART_GetIntStatus(LPC_USART_T *pUART)
{
	return pUART->INTSTAT;
}

/**
 * @brief	Configure data width, parity and stop bits
 * @param	pUART	: Pointer to selected pUART peripheral
 * @param	config	: UART configuration, OR'ed values of select UART_CFG_* defines
 * @return	Nothing
 * @note	Select OR'ed config options for the UART from the UART_CFG_PARITY_*,
 *			UART_CFG_STOPLEN_*, and UART_CFG_DATALEN_* definitions. For example,
 *			a configuration of 8 data bits, 1 stop bit, and even (enabled) parity would be
 *			(UART_CFG_DATALEN_8 | UART_CFG_STOPLEN_1 | UART_CFG_PARITY_EVEN). Will not
 *			alter other bits in the CFG register.
 */
__STATIC_INLINE void Chip_UART_ConfigData(LPC_USART_T *pUART, uint32_t config)
{
	pUART->CFG = config;
}

/**
 * @brief	Get the UART status register
 * @param	pUART	: Pointer to selected UARTx peripheral
 * @return	UART status register
 * @note	Multiple statuses may be pending. Mask the return value
 *			with one or more UART_STAT_* definitions to determine
 *			statuses.
 */
__STATIC_INLINE uint32_t Chip_UART_GetStatus(LPC_USART_T *pUART)
{
	return pUART->STAT;
}

/**
 * @brief	Clear the UART status register
 * @param	pUART	: Pointer to selected UARTx peripheral
 * @param	stsMask	: OR'ed statuses to disable
 * @return	Nothing
 * @note	Multiple interrupts may be pending. Mask the return value
 *			with one or more UART_INTEN_* definitions to determine
 *			pending interrupts.
 */
__STATIC_INLINE void Chip_UART_ClearStatus(LPC_USART_T *pUART, uint32_t stsMask)
{
	pUART->STAT = stsMask;
}

/**
 * @brief	Initialize the UART peripheral
 * @param	pUART	: The base of UART peripheral on the chip
 * @return	Nothing
 */
void Chip_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Deinitialize the UART peripheral
 * @param	pUART	: The base of UART peripheral on the chip
 * @return	Nothing
 */
void Chip_UART_DeInit(LPC_USART_T *pUART);

/**
 * @brief	Transmit a byte array through the UART peripheral (non-blocking)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	data		: Pointer to bytes to transmit
 * @param	numBytes	: Number of bytes to transmit
 * @return	The actual number of bytes placed into the FIFO
 * @note	This function places data into the transmit FIFO until either
 *			all the data is in the FIFO or the FIFO is full. This function
 *			will not block in the FIFO is full. The actual number of bytes
 *			placed into the FIFO is returned. This function ignores errors.
 */
int Chip_UART_Send(LPC_USART_T *pUART, const void *data, int numBytes);

/**
 * @brief	Read data through the UART peripheral (non-blocking)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	data		: Pointer to bytes array to fill
 * @param	numBytes	: Size of the passed data array
 * @return	The actual number of bytes read
 * @note	This function reads data from the receive FIFO until either
 *			all the data has been read or the passed buffer is completely full.
 *			This function will not block. This function ignores errors.
 */
int Chip_UART_Read(LPC_USART_T *pUART, void *data, int numBytes);

/**
 * @brief	Set baud rate for UART
 * @param	pUART	: The base of UART peripheral on the chip
 * @param	baudrate: Baud rate to be set
 * @return	Nothing
 * @note
 * Setting the baud rate of one UART will affect the baud rate of other
 * UART's as the Fractional divider is shared between all the UART.
 */
void Chip_UART_SetBaud(LPC_USART_T *pUART, uint32_t baudrate);

/**
 * @brief	Transmit a byte array through the UART peripheral (blocking)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	data		: Pointer to data to transmit
 * @param	numBytes	: Number of bytes to transmit
 * @return	The number of bytes transmitted
 * @note	This function will send or place all bytes into the transmit
 *			FIFO. This function will block until the last bytes are in the FIFO.
 */
int Chip_UART_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes);

/**
 * @brief	Read data through the UART peripheral (blocking)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	data		: Pointer to data array to fill
 * @param	numBytes	: Size of the passed data array
 * @return	The size of the dat array
 * @note	This function reads data from the receive FIFO until the passed
 *			buffer is completely full. The function will block until full.
 *			This function ignores errors.
 */
int Chip_UART_ReadBlocking(LPC_USART_T *pUART, void *data, int numBytes);

/**
 * @brief	UART receive-only interrupt handler for ring buffers
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	pRB		: Pointer to ring buffer structure to use
 * @return	Nothing
 * @note	If ring buffer support is desired for the receive side
 *			of data transfer, the UART interrupt should call this
 *			function for a receive based interrupt status.
 */
void Chip_UART_RXIntHandlerRB(LPC_USART_T *pUART, RINGBUFF_T *pRB);

/**
 * @brief	UART transmit-only interrupt handler for ring buffers
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	pRB		: Pointer to ring buffer structure to use
 * @return	Nothing
 * @note	If ring buffer support is desired for the transmit side
 *			of data transfer, the UART interrupt should call this
 *			function for a transmit based interrupt status.
 */
void Chip_UART_TXIntHandlerRB(LPC_USART_T *pUART, RINGBUFF_T *pRB);

/**
 * @brief	Populate a transmit ring buffer and start UART transmit
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	pRB		: Pointer to ring buffer structure to use
 * @param	data	: Pointer to buffer to move to ring buffer
 * @param	count	: Number of bytes to move
 * @return	The number of bytes placed into the ring buffer
 * @note	Will move the data into the TX ring buffer and start the
 *			transfer. If the number of bytes returned is less than the
 *			number of bytes to send, the ring buffer is considered full.
 */
uint32_t Chip_UART_SendRB(LPC_USART_T *pUART, RINGBUFF_T *pRB, const void *data, int count);

/**
 * @brief	Copy data from a receive ring buffer
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	pRB		: Pointer to ring buffer structure to use
 * @param	data	: Pointer to buffer to fill from ring buffer
 * @param	bytes	: Size of the passed buffer in bytes
 * @return	The number of bytes placed into the ring buffer
 * @note	Will move the data from the RX ring buffer up to the
 *			the maximum passed buffer size. Returns 0 if there is
 *			no data in the ring buffer.
 */
int Chip_UART_ReadRB(LPC_USART_T *pUART, RINGBUFF_T *pRB, void *data, int bytes);

/**
 * @brief	UART receive/transmit interrupt handler for ring buffers
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	pRXRB	: Pointer to transmit ring buffer
 * @param	pTXRB	: Pointer to receive ring buffer
 * @return	Nothing
 * @note	This provides a basic implementation of the UART IRQ
 *			handler for support of a ring buffer implementation for
 *			transmit and receive.
 */
void Chip_UART_IRQRBHandler(LPC_USART_T *pUART, RINGBUFF_T *pRXRB, RINGBUFF_T *pTXRB);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __UART_5410X_H_ */
