/*
 * @brief LPC18xx/43xx UART chip driver
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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Autobaud status flag */
STATIC volatile FlagStatus ABsyncSts = RESET;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
 /* UART Peripheral clocks */
static const CHIP_CCU_CLK_T UART_PClock[] = {CLK_MX_UART0, CLK_MX_UART1, CLK_MX_UART2, CLK_MX_UART3};

/* UART Bus clocks */
static const CHIP_CCU_CLK_T UART_BClock[] = {CLK_APB0_UART0, CLK_APB0_UART1, CLK_APB2_UART2, CLK_APB2_UART3};

/* Returns clock index for the peripheral block */
static int Chip_UART_GetIndex(LPC_USART_T *pUART)
{
	uint32_t base = (uint32_t) pUART;
	switch(base) {
		case LPC_USART0_BASE:
			return 0;
		case LPC_UART1_BASE:
			return 1;
		case LPC_USART2_BASE:
			return 2;
		case LPC_USART3_BASE:
			return 3;
		default:
			return 0; /* Should never come here */
	}
}

/* UART Autobaud command interrupt handler */
STATIC void Chip_UART_ABIntHandler(LPC_USART_T *pUART)
{
	/* Handle End Of Autobaud interrupt */
	if((Chip_UART_ReadIntIDReg(pUART) & UART_IIR_ABEO_INT) != 0) {
        Chip_UART_SetAutoBaudReg(pUART, UART_ACR_ABEOINT_CLR);
		Chip_UART_IntDisable(pUART, UART_IER_ABEOINT);
	    if (ABsyncSts == RESET) {
	        ABsyncSts = SET;
        }
	}

    /* Handle Autobaud Timeout interrupt */
	if((Chip_UART_ReadIntIDReg(pUART) & UART_IIR_ABTO_INT) != 0) {
        Chip_UART_SetAutoBaudReg(pUART, UART_ACR_ABTOINT_CLR);
		Chip_UART_IntDisable(pUART, UART_IER_ABTOINT);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initializes the pUART peripheral */
void Chip_UART_Init(LPC_USART_T *pUART)
{
    volatile uint32_t tmp;

	/* Enable UART clocking. UART base clock(s) must already be enabled */
	Chip_Clock_EnableOpts(UART_PClock[Chip_UART_GetIndex(pUART)], true, true, 1);

	/* Enable FIFOs by default, reset them */
	Chip_UART_SetupFIFOS(pUART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));

    /* Disable Tx */
    Chip_UART_TXDisable(pUART);

    /* Disable interrupts */
	pUART->IER = 0;
	/* Set LCR to default state */
	pUART->LCR = 0;
	/* Set ACR to default state */
	pUART->ACR = 0;
    /* Set RS485 control to default state */
	pUART->RS485CTRL = 0;
	/* Set RS485 delay timer to default state */
	pUART->RS485DLY = 0;
	/* Set RS485 addr match to default state */
	pUART->RS485ADRMATCH = 0;

    /* Clear MCR */
    if (pUART == LPC_UART1) {
		/* Set Modem Control to default state */
		pUART->MCR = 0;
		/*Dummy Reading to Clear Status */
		tmp = pUART->MSR;
	}

	/* Default 8N1, with DLAB disabled */
	Chip_UART_ConfigData(pUART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));

	/* Disable fractional divider */
	pUART->FDR = 0x10;
}

/* De-initializes the pUART peripheral */
void Chip_UART_DeInit(LPC_USART_T *pUART)
{
    /* Disable Tx */
    Chip_UART_TXDisable(pUART);

    /* Disable clock */
	Chip_Clock_Disable(UART_PClock[Chip_UART_GetIndex(pUART)]);
}

/* Transmit a byte array through the UART peripheral (non-blocking) */
int Chip_UART_Send(LPC_USART_T *pUART, const void *data, int numBytes)
{
	int sent = 0;
	uint8_t *p8 = (uint8_t *) data;

	/* Send until the transmit FIFO is full or out of bytes */
	while ((sent < numBytes) &&
		   ((Chip_UART_ReadLineStatus(pUART) & UART_LSR_THRE) != 0)) {
		Chip_UART_SendByte(pUART, *p8);
		p8++;
		sent++;
	}

	return sent;
}

/* Check whether if UART is busy or not */
FlagStatus Chip_UART_CheckBusy(LPC_USART_T *pUART)
{
	if (pUART->LSR & UART_LSR_TEMT) {
		return RESET;
	}
	else {
		return SET;
	}
}

/* Transmit a byte array through the UART peripheral (blocking) */
int Chip_UART_SendBlocking(LPC_USART_T *pUART, const void *data, int numBytes)
{
	int pass, sent = 0;
	uint8_t *p8 = (uint8_t *) data;

	while (numBytes > 0) {
		pass = Chip_UART_Send(pUART, p8, numBytes);
		numBytes -= pass;
		sent += pass;
		p8 += pass;
	}

	return sent;
}

/* Read data through the UART peripheral (non-blocking) */
int Chip_UART_Read(LPC_USART_T *pUART, void *data, int numBytes)
{
	int readBytes = 0;
	uint8_t *p8 = (uint8_t *) data;

	/* Send until the transmit FIFO is full or out of bytes */
	while ((readBytes < numBytes) &&
		   ((Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR) != 0)) {
		*p8 = Chip_UART_ReadByte(pUART);
		p8++;
		readBytes++;
	}

	return readBytes;
}

/* Read data through the UART peripheral (blocking) */
int Chip_UART_ReadBlocking(LPC_USART_T *pUART, void *data, int numBytes)
{
	int pass, readBytes = 0;
	uint8_t *p8 = (uint8_t *) data;

	while (readBytes < numBytes) {
		pass = Chip_UART_Read(pUART, p8, numBytes);
		numBytes -= pass;
		readBytes += pass;
		p8 += pass;
	}

	return readBytes;
}

/* Determines and sets best dividers to get a target bit rate */
uint32_t Chip_UART_SetBaud(LPC_USART_T *pUART, uint32_t baudrate)
{
	uint32_t div, divh, divl, clkin;

	/* Determine UART clock in rate without FDR */
	clkin = Chip_Clock_GetRate(UART_BClock[Chip_UART_GetIndex(pUART)]);
	div = clkin / (baudrate * 16);

	/* High and low halves of the divider */
	divh = div / 256;
	divl = div - (divh * 256);

	Chip_UART_EnableDivisorAccess(pUART);
	Chip_UART_SetDivisorLatches(pUART, divl, divh);
	Chip_UART_DisableDivisorAccess(pUART);

	/* Fractional FDR alreadt setup for 1 in UART init */

	return clkin / div;
}

/* UART receive-only interrupt handler for ring buffers */
void Chip_UART_RXIntHandlerRB(LPC_USART_T *pUART, RINGBUFF_T *pRB)
{
	/* New data will be ignored if data not popped in time */
	while (Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR) {
		uint8_t ch = Chip_UART_ReadByte(pUART);
		RingBuffer_Insert(pRB, &ch);
	}
}

/* UART transmit-only interrupt handler for ring buffers */
void Chip_UART_TXIntHandlerRB(LPC_USART_T *pUART, RINGBUFF_T *pRB)
{
	uint8_t ch;

	/* Fill FIFO until full or until TX ring buffer is empty */
	while ((Chip_UART_ReadLineStatus(pUART) & UART_LSR_THRE) != 0 &&
		   RingBuffer_Pop(pRB, &ch)) {
		Chip_UART_SendByte(pUART, ch);
	}

	/* Turn off interrupt if the ring buffer is empty */
	if (RingBuffer_IsEmpty(pRB)) {
		/* Shut down transmit */
		Chip_UART_IntDisable(pUART, UART_IER_THREINT);
	}
}

/* Populate a transmit ring buffer and start UART transmit */
uint32_t Chip_UART_SendRB(LPC_USART_T *pUART, RINGBUFF_T *pRB, const void *data, int bytes)
{
	uint32_t ret;
	uint8_t *p8 = (uint8_t *) data;

	/* Don't let UART transmit ring buffer change in the UART IRQ handler */
	Chip_UART_IntDisable(pUART, UART_IER_THREINT);

	/* Move as much data as possible into transmit ring buffer */
	ret = RingBuffer_InsertMult(pRB, p8, bytes);
	Chip_UART_TXIntHandlerRB(pUART, pRB);

	/* Add additional data to transmit ring buffer if possible */
	ret += RingBuffer_InsertMult(pRB, (p8 + ret), (bytes - ret));

	/* Enable UART transmit interrupt */
	Chip_UART_IntEnable(pUART, UART_IER_THREINT);

	return ret;
}

/* Copy data from a receive ring buffer */
int Chip_UART_ReadRB(LPC_USART_T *pUART, RINGBUFF_T *pRB, void *data, int bytes)
{
	(void) pUART;

	return RingBuffer_PopMult(pRB, (uint8_t *) data, bytes);
}

/* UART receive/transmit interrupt handler for ring buffers */
void Chip_UART_IRQRBHandler(LPC_USART_T *pUART, RINGBUFF_T *pRXRB, RINGBUFF_T *pTXRB)
{
	/* Handle transmit interrupt if enabled */
	if (pUART->IER & UART_IER_THREINT) {
		Chip_UART_TXIntHandlerRB(pUART, pTXRB);

		/* Disable transmit interrupt if the ring buffer is empty */
		if (RingBuffer_IsEmpty(pTXRB)) {
			Chip_UART_IntDisable(pUART, UART_IER_THREINT);
		}
	}

	/* Handle receive interrupt */
	Chip_UART_RXIntHandlerRB(pUART, pRXRB);

    /* Handle Autobaud interrupts */
    Chip_UART_ABIntHandler(pUART);
}

/* Determines and sets best dividers to get a target baud rate */
uint32_t Chip_UART_SetBaudFDR(LPC_USART_T *pUART, uint32_t baud)
{
	uint32_t sdiv = 0, sm = 1, sd = 0;
	uint32_t pclk, m, d;
	uint32_t odiff = -1UL; /* old best diff */

	/* Get base clock for the corresponding UART */
	pclk = Chip_Clock_GetRate(UART_BClock[Chip_UART_GetIndex(pUART)]);

	/* Loop through all possible fractional divider values */
	for (m = 1; odiff && m < 16; m++) {
		for (d = 0; d < m; d++) {
			uint32_t diff, div;
			uint64_t dval = (((uint64_t) pclk << 28) * m) / (baud * (m + d));

			/* Lower 32-bit of dval has diff */
			diff = (uint32_t) dval;
			/* Upper 32-bit of dval has div */
			div = (uint32_t) (dval >> 32);

			/* Closer to next div */
			if ((int)diff < 0) {
				diff = -diff;
				div ++;
			}

			/* Check if new value is worse than old or out of range */
			if (odiff < diff || !div || (div >> 16) || (div < 3 && d)) {
				continue;
			}

			/* Store the new better values */
			sdiv = div;
			sd = d;
			sm = m;
			odiff = diff;

			/* On perfect match, break loop */
			if(!diff) {
				break;
			}
		}
	}

	/* Return 0 if a vaild divisor is not possible */
	if (!sdiv) {
		return 0;
	}

	/* Update UART registers */
	Chip_UART_EnableDivisorAccess(pUART);
	Chip_UART_SetDivisorLatches(pUART, UART_LOAD_DLL(sdiv), UART_LOAD_DLM(sdiv));
	Chip_UART_DisableDivisorAccess(pUART);

	/* Set best fractional divider */
	pUART->FDR = (UART_FDR_MULVAL(sm) | UART_FDR_DIVADDVAL(sd));

	/* Return actual baud rate */
	return (pclk >> 4) * sm / (sdiv * (sm + sd));
}

/* UART interrupt service routine */
FlagStatus Chip_UART_GetABEOStatus(LPC_USART_T *pUART)
{
	(void) pUART;
	return ABsyncSts;
}

/* Start/Stop Auto Baudrate activity */
void Chip_UART_ABCmd(LPC_USART_T *pUART, uint32_t mode, bool autorestart, FunctionalState NewState)
{
    uint32_t tmp = 0;

	if (NewState == ENABLE) {
		/* Clear DLL and DLM value */
		pUART->LCR |= UART_LCR_DLAB_EN;
		pUART->DLL = 0;
		pUART->DLM = 0;
		pUART->LCR &= ~UART_LCR_DLAB_EN;

		/* FDR value must be reset to default value */
		pUART->FDR = 0x10;

		if (mode == UART_ACR_MODE1) {
			tmp = UART_ACR_START | UART_ACR_MODE;
		}
		else {
			tmp = UART_ACR_START;
		}

		if (autorestart == true) {
			tmp |= UART_ACR_AUTO_RESTART;
		}
		pUART->ACR = tmp;
	}
	else {
		pUART->ACR = 0;
	}
}

