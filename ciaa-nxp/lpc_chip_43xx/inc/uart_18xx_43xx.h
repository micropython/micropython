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

#ifndef __UART_18XX_43XX_H_
#define __UART_18XX_43XX_H_

#include "ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup UART_18XX_43XX CHIP: LPC18xx/43xx UART driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief USART register block structure
 */
typedef struct {					/*!< USARTn Structure       */

	union {
		__IO uint32_t  DLL;			/*!< Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
		__O  uint32_t  THR;			/*!< Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0). */
		__I  uint32_t  RBR;			/*!< Receiver Buffer Register. Contains the next received character to be read (DLAB = 0). */
	};

	union {
		__IO uint32_t IER;			/*!< Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0). */
		__IO uint32_t DLM;			/*!< Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
	};

	union {
		__O  uint32_t FCR;			/*!< FIFO Control Register. Controls UART FIFO usage and modes. */
		__I  uint32_t IIR;			/*!< Interrupt ID Register. Identifies which interrupt(s) are pending. */
	};

	__IO uint32_t LCR;				/*!< Line Control Register. Contains controls for frame formatting and break generation. */
	__IO uint32_t MCR;				/*!< Modem Control Register. Only present on USART ports with full modem support. */
	__I  uint32_t LSR;				/*!< Line Status Register. Contains flags for transmit and receive status, including line errors. */
	__I  uint32_t MSR;				/*!< Modem Status Register. Only present on USART ports with full modem support. */
	__IO uint32_t SCR;				/*!< Scratch Pad Register. Eight-bit temporary storage for software. */
	__IO uint32_t ACR;				/*!< Auto-baud Control Register. Contains controls for the auto-baud feature. */
	__IO uint32_t ICR;				/*!< IrDA control register (not all UARTS) */
	__IO uint32_t FDR;				/*!< Fractional Divider Register. Generates a clock input for the baud rate divider. */
	__IO uint32_t OSR;				/*!< Oversampling Register. Controls the degree of oversampling during each bit time. Only on some UARTS. */
	__IO uint32_t TER1;				/*!< Transmit Enable Register. Turns off USART transmitter for use with software flow control. */
	uint32_t  RESERVED0[3];
    __IO uint32_t HDEN;				/*!< Half-duplex enable Register- only on some UARTs */
	__I  uint32_t RESERVED1[1];
	__IO uint32_t SCICTRL;			/*!< Smart card interface control register- only on some UARTs */

	__IO uint32_t RS485CTRL;		/*!< RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes. */
	__IO uint32_t RS485ADRMATCH;	/*!< RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode. */
	__IO uint32_t RS485DLY;			/*!< RS-485/EIA-485 direction control delay. */

	union {
		__IO uint32_t SYNCCTRL;		/*!< Synchronous mode control register. Only on USARTs. */
		__I  uint32_t FIFOLVL;		/*!< FIFO Level register. Provides the current fill levels of the transmit and receive FIFOs. */
	};

	__IO uint32_t TER2;				/*!< Transmit Enable Register. Only on LPC177X_8X UART4 and LPC18XX/43XX USART0/2/3. */
} LPC_USART_T;


/**
 * @brief Macro defines for UART Receive Buffer register
 */
#define UART_RBR_MASKBIT    (0xFF)		        /*!< UART Received Buffer mask bit (8 bits) */

/**
 * @brief Macro defines for UART Divisor Latch LSB register
 */
#define UART_LOAD_DLL(div)  ((div) & 0xFF)		/*!< Macro for loading LSB of divisor */
#define UART_DLL_MASKBIT    (0xFF)	            /*!< Divisor latch LSB bit mask */

/**
 * @brief Macro defines for UART Divisor Latch MSB register
 */
#define UART_LOAD_DLM(div)  (((div) >> 8) & 0xFF)	/*!< Macro for loading MSB of divisors */
#define UART_DLM_MASKBIT    (0xFF)		            /*!< Divisor latch MSB bit mask */

/**
 * @brief Macro defines for UART Interrupt Enable Register
 */
#define UART_IER_RBRINT      (1 << 0)	/*!< RBR Interrupt enable */
#define UART_IER_THREINT     (1 << 1)	/*!< THR Interrupt enable */
#define UART_IER_RLSINT      (1 << 2)	/*!< RX line status interrupt enable */
#define UART_IER_MSINT       (1 << 3)	/*!< Modem status interrupt enable - valid for 11xx, 17xx/40xx UART1, 18xx/43xx UART1  only */
#define UART_IER_CTSINT      (1 << 7)	/*!< CTS signal transition interrupt enable - valid for 17xx/40xx UART1, 18xx/43xx UART1 only */
#define UART_IER_ABEOINT     (1 << 8)	/*!< Enables the end of auto-baud interrupt */
#define UART_IER_ABTOINT     (1 << 9)	/*!< Enables the auto-baud time-out interrupt */
#define UART_IER_BITMASK     (0x307)	/*!< UART interrupt enable register bit mask  - valid for 13xx, 17xx/40xx UART0/2/3, 18xx/43xx UART0/2/3 only*/
#define UART1_IER_BITMASK    (0x30F)	/*!< UART1 interrupt enable register bit mask - valid for 11xx only */
#define UART2_IER_BITMASK    (0x38F)	/*!< UART2 interrupt enable register bit mask - valid for 17xx/40xx UART1, 18xx/43xx UART1 only */

/**
 * @brief Macro defines for UART Interrupt Identification Register
 */
#define UART_IIR_INTSTAT_PEND   (1 << 0)	/*!< Interrupt pending status - Active low */
#define UART_IIR_FIFO_EN        (3 << 6)	/*!< These bits are equivalent to FCR[0] */
#define UART_IIR_ABEO_INT       (1 << 8)	/*!< End of auto-baud interrupt */
#define UART_IIR_ABTO_INT       (1 << 9)	/*!< Auto-baud time-out interrupt */
#define UART_IIR_BITMASK        (0x3CF)		/*!< UART interrupt identification register bit mask */

/* Interrupt ID bit definitions */
#define UART_IIR_INTID_MASK     (7 << 1)	/*!< Interrupt identification: Interrupt ID mask */
#define UART_IIR_INTID_RLS      (3 << 1)	/*!< Interrupt identification: Receive line interrupt */
#define UART_IIR_INTID_RDA      (2 << 1)	/*!< Interrupt identification: Receive data available interrupt */
#define UART_IIR_INTID_CTI      (6 << 1)	/*!< Interrupt identification: Character time-out indicator interrupt */
#define UART_IIR_INTID_THRE     (1 << 1)	/*!< Interrupt identification: THRE interrupt */
#define UART_IIR_INTID_MODEM    (0 << 1)	/*!< Interrupt identification: Modem interrupt */

/**
 * @brief Macro defines for UART FIFO Control Register
 */
#define UART_FCR_FIFO_EN        (1 << 0)	/*!< UART FIFO enable */
#define UART_FCR_RX_RS          (1 << 1)	/*!< UART RX FIFO reset */
#define UART_FCR_TX_RS          (1 << 2)	/*!< UART TX FIFO reset */
#define UART_FCR_DMAMODE_SEL    (1 << 3)	/*!< UART DMA mode selection - valid for 17xx/40xx, 18xx/43xx only */
#define UART_FCR_BITMASK        (0xCF)		/*!< UART FIFO control bit mask */

#define UART_TX_FIFO_SIZE       (16)

/* FIFO trigger level bit definitions */
#define UART_FCR_TRG_LEV0       (0)			/*!< UART FIFO trigger level 0: 1 character */
#define UART_FCR_TRG_LEV1       (1 << 6)	/*!< UART FIFO trigger level 1: 4 character */
#define UART_FCR_TRG_LEV2       (2 << 6)	/*!< UART FIFO trigger level 2: 8 character */
#define UART_FCR_TRG_LEV3       (3 << 6)	/*!< UART FIFO trigger level 3: 14 character */

/**
 * @brief Macro defines for UART Line Control Register
 */
/* UART word length select bit definitions */
#define UART_LCR_WLEN_MASK      (3 << 0)		/*!< UART word length select bit mask */
#define UART_LCR_WLEN5          (0 << 0)		/*!< UART word length select: 5 bit data mode */
#define UART_LCR_WLEN6          (1 << 0)		/*!< UART word length select: 6 bit data mode */
#define UART_LCR_WLEN7          (2 << 0)		/*!< UART word length select: 7 bit data mode */
#define UART_LCR_WLEN8          (3 << 0)		/*!< UART word length select: 8 bit data mode */

/* UART Stop bit select bit definitions */
#define UART_LCR_SBS_MASK       (1 << 2)		/*!< UART stop bit select: bit mask */
#define UART_LCR_SBS_1BIT       (0 << 2)		/*!< UART stop bit select: 1 stop bit */
#define UART_LCR_SBS_2BIT       (1 << 2)		/*!< UART stop bit select: 2 stop bits (in 5 bit data mode, 1.5 stop bits) */

/* UART Parity enable bit definitions */
#define UART_LCR_PARITY_EN      (1 << 3)		/*!< UART Parity Enable */
#define UART_LCR_PARITY_DIS     (0 << 3)		/*!< UART Parity Disable */
#define UART_LCR_PARITY_ODD     (0 << 4)		/*!< UART Parity select: Odd parity */
#define UART_LCR_PARITY_EVEN    (1 << 4)		/*!< UART Parity select: Even parity */
#define UART_LCR_PARITY_F_1     (2 << 4)		/*!< UART Parity select: Forced 1 stick parity */
#define UART_LCR_PARITY_F_0     (3 << 4)		/*!< UART Parity select: Forced 0 stick parity */
#define UART_LCR_BREAK_EN       (1 << 6)		/*!< UART Break transmission enable */
#define UART_LCR_DLAB_EN        (1 << 7)		/*!< UART Divisor Latches Access bit enable */
#define UART_LCR_BITMASK        (0xFF)			/*!< UART line control bit mask */

/**
 * @brief Macro defines for UART Modem Control Register
 */
#define UART_MCR_DTR_CTRL       (1 << 0)		/*!< Source for modem output pin DTR */
#define UART_MCR_RTS_CTRL       (1 << 1)		/*!< Source for modem output pin RTS */
#define UART_MCR_LOOPB_EN       (1 << 4)		/*!< Loop back mode select */
#define UART_MCR_AUTO_RTS_EN    (1 << 6)		/*!< Enable Auto RTS flow-control */
#define UART_MCR_AUTO_CTS_EN    (1 << 7)		/*!< Enable Auto CTS flow-control */
#define UART_MCR_BITMASK        (0xD3)			/*!< UART bit mask value */

/**
 * @brief Macro defines for UART Line Status Register
 */
#define UART_LSR_RDR        (1 << 0)	/*!< Line status: Receive data ready */
#define UART_LSR_OE         (1 << 1)	/*!< Line status: Overrun error */
#define UART_LSR_PE         (1 << 2)	/*!< Line status: Parity error */
#define UART_LSR_FE         (1 << 3)	/*!< Line status: Framing error */
#define UART_LSR_BI         (1 << 4)	/*!< Line status: Break interrupt */
#define UART_LSR_THRE       (1 << 5)	/*!< Line status: Transmit holding register empty */
#define UART_LSR_TEMT       (1 << 6)	/*!< Line status: Transmitter empty */
#define UART_LSR_RXFE       (1 << 7)	/*!< Line status: Error in RX FIFO */
#define UART_LSR_TXFE       (1 << 8)	/*!< Line status: Error in RX FIFO */
#define UART_LSR_BITMASK    (0xFF)		/*!< UART Line status bit mask */
#define UART1_LSR_BITMASK   (0x1FF)		/*!< UART1 Line status bit mask - valid for 11xx, 18xx/43xx UART0/2/3 only */

/**
 * @brief Macro defines for UART Modem Status Register
 */
#define UART_MSR_DELTA_CTS      (1 << 0)	/*!< Modem status: State change of input CTS */
#define UART_MSR_DELTA_DSR      (1 << 1)	/*!< Modem status: State change of input DSR */
#define UART_MSR_LO2HI_RI       (1 << 2)	/*!< Modem status: Low to high transition of input RI */
#define UART_MSR_DELTA_DCD      (1 << 3)	/*!< Modem status: State change of input DCD */
#define UART_MSR_CTS            (1 << 4)	/*!< Modem status: Clear To Send State */
#define UART_MSR_DSR            (1 << 5)	/*!< Modem status: Data Set Ready State */
#define UART_MSR_RI             (1 << 6)	/*!< Modem status: Ring Indicator State */
#define UART_MSR_DCD            (1 << 7)	/*!< Modem status: Data Carrier Detect State */
#define UART_MSR_BITMASK        (0xFF)		/*!< Modem status: MSR register bit-mask value */

/**
 * @brief Macro defines for UART Auto baudrate control register
 */
#define UART_ACR_START              (1 << 0)	/*!< UART Auto-baud start */
#define UART_ACR_MODE               (1 << 1)	/*!< UART Auto baudrate Mode 1 */
#define UART_ACR_AUTO_RESTART       (1 << 2)	/*!< UART Auto baudrate restart */
#define UART_ACR_ABEOINT_CLR        (1 << 8)	/*!< UART End of auto-baud interrupt clear */
#define UART_ACR_ABTOINT_CLR        (1 << 9)	/*!< UART Auto-baud time-out interrupt clear */
#define UART_ACR_BITMASK            (0x307)		/*!< UART Auto Baudrate register bit mask */

/**
 * Autobaud modes
 */
#define UART_ACR_MODE0              (0)	/*!< Auto baudrate Mode 0 */
#define UART_ACR_MODE1              (1)	/*!< Auto baudrate Mode 1 */

/**
 * @brief Macro defines for UART RS485 Control register
 */
#define UART_RS485CTRL_NMM_EN       (1 << 0)	/*!< RS-485/EIA-485 Normal Multi-drop Mode (NMM) is disabled */
#define UART_RS485CTRL_RX_DIS       (1 << 1)	/*!< The receiver is disabled */
#define UART_RS485CTRL_AADEN        (1 << 2)	/*!< Auto Address Detect (AAD) is enabled */
#define UART_RS485CTRL_SEL_DTR      (1 << 3)	/*!< If direction control is enabled (bit DCTRL = 1), pin DTR is
												        used for direction control */
#define UART_RS485CTRL_DCTRL_EN     (1 << 4)	/*!< Enable Auto Direction Control */
#define UART_RS485CTRL_OINV_1       (1 << 5)	/*!< This bit reverses the polarity of the direction
												       control signal on the RTS (or DTR) pin. The direction control pin
												       will be driven to logic "1" when the transmitter has data to be sent */
#define UART_RS485CTRL_BITMASK      (0x3F)		/*!< RS485 control bit-mask value */

/**
 * @brief Macro defines for UART IrDA Control Register - valid for 11xx, 17xx/40xx UART0/2/3, 18xx/43xx UART3 only
 */
#define UART_ICR_IRDAEN         (1 << 0)			/*!< IrDA mode enable */
#define UART_ICR_IRDAINV        (1 << 1)			/*!< IrDA serial input inverted */
#define UART_ICR_FIXPULSE_EN    (1 << 2)			/*!< IrDA fixed pulse width mode */
#define UART_ICR_PULSEDIV(n)    ((n & 0x07) << 3)	/*!< PulseDiv - Configures the pulse when FixPulseEn = 1 */
#define UART_ICR_BITMASK        (0x3F)				/*!< UART IRDA bit mask */

/**
 * @brief Macro defines for UART half duplex register - ????
 */
#define UART_HDEN_HDEN          ((1 << 0))			/*!< enable half-duplex mode*/

/**
 * @brief Macro defines for UART Smart card interface Control Register - valid for 11xx, 18xx/43xx UART0/2/3 only
 */
#define UART_SCICTRL_SCIEN        (1 << 0)			/*!< enable asynchronous half-duplex smart card interface*/
#define UART_SCICTRL_NACKDIS      (1 << 1)			/*!< NACK response is inhibited*/
#define UART_SCICTRL_PROTSEL_T1   (1 << 2)			/*!< ISO7816-3 protocol T1 is selected*/
#define UART_SCICTRL_TXRETRY(n)   ((n & 0x07) << 5)	/*!< number of retransmission*/
#define UART_SCICTRL_GUARDTIME(n) ((n & 0xFF) << 8)	/*!< Extra guard time*/

/**
 * @brief Macro defines for UART Fractional Divider Register
 */
#define UART_FDR_DIVADDVAL(n)   (n & 0x0F)			/*!< Baud-rate generation pre-scaler divisor */
#define UART_FDR_MULVAL(n)      ((n << 4) & 0xF0)	/*!< Baud-rate pre-scaler multiplier value */
#define UART_FDR_BITMASK        (0xFF)				/*!< UART Fractional Divider register bit mask */

/**
 * @brief Macro defines for UART Tx Enable Register
 */
#define UART_TER1_TXEN      (1 << 7)		/*!< Transmit enable bit  - valid for 11xx, 13xx, 17xx/40xx only */
#define UART_TER2_TXEN      (1 << 0)		/*!< Transmit enable bit  - valid for 18xx/43xx only */

/**
 * @brief Macro defines for UART Synchronous Control Register - 11xx, 18xx/43xx UART0/2/3 only
 */
#define UART_SYNCCTRL_SYNC             (1 << 0)			/*!< enable synchronous mode*/
#define UART_SYNCCTRL_CSRC_MASTER      (1 << 1)  		/*!< synchronous master mode*/
#define UART_SYNCCTRL_FES              (1 << 2)			/*!< sample on falling edge*/
#define UART_SYNCCTRL_TSBYPASS         (1 << 3)			/*!< to be defined*/
#define UART_SYNCCTRL_CSCEN            (1 << 4)			/*!< Continuous running clock enable (master mode only)*/
#define UART_SYNCCTRL_STARTSTOPDISABLE (1 << 5)	        /*!< Do not send start/stop bit*/
#define UART_SYNCCTRL_CCCLR            (1 << 6)			/*!< stop continuous clock*/

/**
 * @brief	Enable transmission on UART TxD pin
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return Nothing
 */
STATIC INLINE void Chip_UART_TXEnable(LPC_USART_T *pUART)
{
    pUART->TER2 = UART_TER2_TXEN;
}

/**
 * @brief	Disable transmission on UART TxD pin
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return Nothing
 */
STATIC INLINE void Chip_UART_TXDisable(LPC_USART_T *pUART)
{
    pUART->TER2 = 0;
}

/**
 * @brief	Transmit a single data byte through the UART peripheral
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	data	: Byte to transmit
 * @return	Nothing
 * @note	This function attempts to place a byte into the UART transmit
 *			FIFO or transmit hold register regard regardless of UART state
 */
STATIC INLINE void Chip_UART_SendByte(LPC_USART_T *pUART, uint8_t data)
{
	pUART->THR = (uint32_t) data;
}

/**
 * @brief	Read a single byte data from the UART peripheral
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	A single byte of data read
 * @note	This function reads a byte from the UART receive FIFO or
 *			receive hold register regard regardless of UART state. The
 *			FIFO status should be read first prior to using this function
 */
STATIC INLINE uint8_t Chip_UART_ReadByte(LPC_USART_T *pUART)
{
	return (uint8_t) (pUART->RBR & UART_RBR_MASKBIT);
}

/**
 * @brief	Enable UART interrupts
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	intMask	: OR'ed Interrupts to enable in the Interrupt Enable Register (IER)
 * @return	Nothing
 * @note	Use an OR'ed value of UART_IER_* definitions with this function
 *			to enable specific UART interrupts. The Divisor Latch Access Bit
 *			(DLAB) in LCR must be cleared in order to access the IER register.
 *			This function doesn't alter the DLAB state
 */
STATIC INLINE void Chip_UART_IntEnable(LPC_USART_T *pUART, uint32_t intMask)
{
	pUART->IER |= intMask;
}

/**
 * @brief	Disable UART interrupts
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	intMask	: OR'ed Interrupts to disable in the Interrupt Enable Register (IER)
 * @return	Nothing
 * @note	Use an OR'ed value of UART_IER_* definitions with this function
 *			to disable specific UART interrupts. The Divisor Latch Access Bit
 *			(DLAB) in LCR must be cleared in order to access the IER register.
 *			This function doesn't alter the DLAB state
 */
STATIC INLINE void Chip_UART_IntDisable(LPC_USART_T *pUART, uint32_t intMask)
{
	pUART->IER &= ~intMask;
}

/**
 * @brief	Returns UART interrupts that are enabled
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Returns the enabled UART interrupts
 * @note	Use an OR'ed value of UART_IER_* definitions with this function
 *			to determine which interrupts are enabled. You can check
 *			for multiple enabled bits if needed.
 */
STATIC INLINE uint32_t Chip_UART_GetIntsEnabled(LPC_USART_T *pUART)
{
	return pUART->IER;
}

/**
 * @brief	Read the Interrupt Identification Register (IIR)
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Current pending interrupt status per the IIR register
 */
STATIC INLINE uint32_t Chip_UART_ReadIntIDReg(LPC_USART_T *pUART)
{
	return pUART->IIR;
}

/**
 * @brief	Setup the UART FIFOs
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	fcr		: FIFO control register setup OR'ed flags
 * @return	Nothing
 * @note	Use OR'ed value of UART_FCR_* definitions with this function
 *			to select specific options. For example, to enable the FIFOs
 *			with a RX trip level of 8 characters, use something like
 *			(UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2)
 */
STATIC INLINE void Chip_UART_SetupFIFOS(LPC_USART_T *pUART, uint32_t fcr)
{
	pUART->FCR = fcr;
}

/**
 * @brief	Configure data width, parity and stop bits
 * @param	pUART	: Pointer to selected pUART peripheral
 * @param	config	: UART configuration, OR'ed values of UART_LCR_* defines
 * @return	Nothing
 * @note	Select OR'ed config options for the UART from the UART_LCR_*
 *			definitions. For example, a configuration of 8 data bits, 1
 *			stop bit, and even (enabled) parity would be
 *			(UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN)
 */
STATIC INLINE void Chip_UART_ConfigData(LPC_USART_T *pUART, uint32_t config)
{
	pUART->LCR = config;
}

/**
 * @brief	Enable access to Divisor Latches
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Nothing
 */
STATIC INLINE void Chip_UART_EnableDivisorAccess(LPC_USART_T *pUART)
{
	pUART->LCR |= UART_LCR_DLAB_EN;
}

/**
 * @brief	Disable access to Divisor Latches
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Nothing
 */
STATIC INLINE void Chip_UART_DisableDivisorAccess(LPC_USART_T *pUART)
{
	pUART->LCR &= ~UART_LCR_DLAB_EN;
}

/**
 * @brief	Set LSB and MSB divisor latch registers
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	dll		: Divisor Latch LSB value
 * @param	dlm		: Divisor Latch MSB value
 * @return	Nothing
 * @note	The Divisor Latch Access Bit (DLAB) in LCR must be set in
 *			order to access the USART Divisor Latches. This function
 *			doesn't alter the DLAB state.
 */
STATIC INLINE void Chip_UART_SetDivisorLatches(LPC_USART_T *pUART, uint8_t dll, uint8_t dlm)
{
	pUART->DLL = (uint32_t) dll;
	pUART->DLM = (uint32_t) dlm;
}


/**
 * @brief	Return modem control register/status
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Modem control register (status)
 * @note	Mask bits of the returned status value with UART_MCR_*
 *			definitions for specific statuses.
 */
STATIC INLINE uint32_t Chip_UART_ReadModemControl(LPC_USART_T *pUART)
{
	return pUART->MCR;
}

/**
 * @brief	Set modem control register/status
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	mcr		: Modem control register flags to set
 * @return	Nothing
 * @note	Use an Or'ed value of UART_MCR_* definitions with this
 *			call to set specific options.
 */
STATIC INLINE void Chip_UART_SetModemControl(LPC_USART_T *pUART, uint32_t mcr)
{
	pUART->MCR |= mcr;
}

/**
 * @brief	Clear modem control register/status
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	mcr		: Modem control register flags to clear
 * @return	Nothing
 * @note	Use an Or'ed value of UART_MCR_* definitions with this
 *			call to clear specific options.
 */
STATIC INLINE void Chip_UART_ClearModemControl(LPC_USART_T *pUART, uint32_t mcr)
{
	pUART->MCR &= ~mcr;
}

/**
 * @brief	Return Line Status register/status (LSR)
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Line Status register (status)
 * @note	Mask bits of the returned status value with UART_LSR_*
 *			definitions for specific statuses.
 */
STATIC INLINE uint32_t Chip_UART_ReadLineStatus(LPC_USART_T *pUART)
{
	return pUART->LSR;
}

/**
 * @brief	Return Modem Status register/status (MSR)
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Modem Status register (status)
 * @note	Mask bits of the returned status value with UART_MSR_*
 *			definitions for specific statuses.
 */
STATIC INLINE uint32_t Chip_UART_ReadModemStatus(LPC_USART_T *pUART)
{
	return pUART->MSR;
}

/**
 * @brief	Write a byte to the scratchpad register
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	data	: Byte value to write
 * @return	Nothing
 */
STATIC INLINE void Chip_UART_SetScratch(LPC_USART_T *pUART, uint8_t data)
{
	pUART->SCR = (uint32_t) data;
}

/**
 * @brief	Returns current byte value in the scratchpad register
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Byte value read from scratchpad register
 */
STATIC INLINE uint8_t Chip_UART_ReadScratch(LPC_USART_T *pUART)
{
	return (uint8_t) (pUART->SCR & 0xFF);
}

/**
 * @brief	Set autobaud register options
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	acr		: Or'ed values to set for ACR register
 * @return	Nothing
 * @note	Use an Or'ed value of UART_ACR_* definitions with this
 *			call to set specific options.
 */
STATIC INLINE void Chip_UART_SetAutoBaudReg(LPC_USART_T *pUART, uint32_t acr)
{
	pUART->ACR |= acr;
}

/**
 * @brief	Clear autobaud register options
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	acr		: Or'ed values to clear for ACR register
 * @return	Nothing
 * @note	Use an Or'ed value of UART_ACR_* definitions with this
 *			call to clear specific options.
 */
STATIC INLINE void Chip_UART_ClearAutoBaudReg(LPC_USART_T *pUART, uint32_t acr)
{
	pUART->ACR &= ~acr;
}

/**
 * @brief	Set RS485 control register options
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	ctrl	: Or'ed values to set for RS485 control register
 * @return	Nothing
 * @note	Use an Or'ed value of UART_RS485CTRL_* definitions with this
 *			call to set specific options.
 */
STATIC INLINE void Chip_UART_SetRS485Flags(LPC_USART_T *pUART, uint32_t ctrl)
{
	pUART->RS485CTRL |= ctrl;
}

/**
 * @brief	Clear RS485 control register options
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	ctrl	: Or'ed values to clear for RS485 control register
 * @return	Nothing
 * @note	Use an Or'ed value of UART_RS485CTRL_* definitions with this
 *			call to clear specific options.
 */
STATIC INLINE void Chip_UART_ClearRS485Flags(LPC_USART_T *pUART, uint32_t ctrl)
{
	pUART->RS485CTRL &= ~ctrl;
}

/**
 * @brief	Set RS485 address match value
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	addr	: Address match value for RS-485/EIA-485 mode
 * @return	Nothing
 */
STATIC INLINE void Chip_UART_SetRS485Addr(LPC_USART_T *pUART, uint8_t addr)
{
	pUART->RS485ADRMATCH = (uint32_t) addr;
}

/**
 * @brief	Read RS485 address match value
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	Address match value for RS-485/EIA-485 mode
 */
STATIC INLINE uint8_t Chip_UART_GetRS485Addr(LPC_USART_T *pUART)
{
	return (uint8_t) (pUART->RS485ADRMATCH & 0xFF);
}

/**
 * @brief	Set RS485 direction control (RTS or DTR) delay value
 * @param	pUART	: Pointer to selected UART peripheral
 * @param	dly		: direction control (RTS or DTR) delay value
 * @return	Nothing
 * @note	This delay time is in periods of the baud clock. Any delay
 *			time from 0 to 255 bit times may be programmed.
 */
STATIC INLINE void Chip_UART_SetRS485Delay(LPC_USART_T *pUART, uint8_t dly)
{
	pUART->RS485DLY = (uint32_t) dly;
}

/**
 * @brief	Read RS485 direction control (RTS or DTR) delay value
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	direction control (RTS or DTR) delay value
 * @note	This delay time is in periods of the baud clock. Any delay
 *			time from 0 to 255 bit times may be programmed.
 */
STATIC INLINE uint8_t Chip_UART_GetRS485Delay(LPC_USART_T *pUART)
{
	return (uint8_t) (pUART->RS485DLY & 0xFF);
}

/**
 * @brief	Initializes the pUART peripheral
 * @param	pUART		: Pointer to selected pUART peripheral
 * @return	Nothing
 */
void Chip_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	De-initializes the pUART peripheral.
 * @param	pUART		: Pointer to selected pUART peripheral
 * @return	Nothing
 */
void Chip_UART_DeInit(LPC_USART_T *pUART);


/**
 * @brief	Check whether if UART is busy or not
 * @param	pUART	: Pointer to selected pUART peripheral
 * @return	RESET if UART is not busy, otherwise return SET
 */
FlagStatus Chip_UART_CheckBusy(LPC_USART_T *pUART);

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
 * @brief	Sets best dividers to get a target bit rate (without fractional divider)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	baudrate	: Target baud rate (baud rate = bit rate)
 * @return	The actual baud rate, or 0 if no rate can be found
 */
uint32_t Chip_UART_SetBaud(LPC_USART_T *pUART, uint32_t baudrate);

/**
 * @brief	Sets best dividers to get a target bit rate (with fractional divider)
 * @param	pUART		: Pointer to selected UART peripheral
 * @param	baud		: Target baud rate (baud rate = bit rate)
 * @return	The actual baud rate, or 0 if no rate can be found
 * @note	The maximum bit rate possible is (clk / 16), the next possible bit
 * 			rate is (clk / 32), the next possible bit rate is (clk / 48), no
 * 			rates in-between any of the above three maximum rates could be set
 * 			using this API. Fractional dividers can only be used for rates
 * 			lower than (clk / 48) where @a clk is the base clock of the UART.
 */
uint32_t Chip_UART_SetBaudFDR(LPC_USART_T *pUART, uint32_t baud);

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
 * @param	bytes	: Number of bytes to move
 * @return	The number of bytes placed into the ring buffer
 * @note	Will move the data into the TX ring buffer and start the
 *			transfer. If the number of bytes returned is less than the
 *			number of bytes to send, the ring buffer is considered full.
 */
uint32_t Chip_UART_SendRB(LPC_USART_T *pUART, RINGBUFF_T *pRB, const void *data, int bytes);

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
 * @brief	Returns the Auto Baud status
 * @param	pUART	: Pointer to selected UART peripheral
 * @return	RESET if autobaud not completed, SET if autobaud completed
 */
FlagStatus Chip_UART_GetABEOStatus(LPC_USART_T *pUART);

/**
 * @brief	Start/stop autobaud operation
 * @param	pUART	    : Pointer to selected UART peripheral
 * @param	mode	    : Autobaud mode (UART_ACR_MODE0 or UART_ACR_MODE1)
 * @param	autorestart	: Enable autorestart (true to enable or false to disable)
 * @param	NewState	: ENABLE to start autobaud operation, DISABLE to
 *                          stop autobaud operation
 * @return	Nothing
 */
void Chip_UART_ABCmd(LPC_USART_T *pUART, uint32_t mode, bool autorestart,
        FunctionalState NewState);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __UART_18XX_43XX_H_ */
