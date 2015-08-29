/*
 * @brief LPC18xx/43xx CCAN driver
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

#ifndef __CCAN_18XX_43XX_H_
#define __CCAN_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CCAN_18XX_43XX CHIP: LPC18xx/43xx CCAN driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief CCAN message interface register block structure
 */
typedef struct {	/*!< C_CAN message interface Structure       */
	__IO uint32_t CMDREQ;			/*!< Message interface command request  */
	__IO uint32_t CMDMSK;			/*!< Message interface command mask*/
	__IO uint32_t MSK1;				/*!< Message interface mask 1 */
	__IO uint32_t MSK2;				/*!< Message interface mask 2 */
	__IO uint32_t ARB1;				/*!< Message interface arbitration 1 */
	__IO uint32_t ARB2;				/*!< Message interface arbitration 2 */
	__IO uint32_t MCTRL;			/*!< Message interface message control */
	__IO uint32_t DA1;				/*!< Message interface data A1 */
	__IO uint32_t DA2;				/*!< Message interface data A2 */
	__IO uint32_t DB1;				/*!< Message interface data B1 */
	__IO uint32_t DB2;				/*!< Message interface data B2 */
	__I  uint32_t  RESERVED[13];
} CCAN_IF_T;

/**
 * @brief CCAN Controller Area Network register block structure
 */
typedef struct {						/*!< C_CAN Structure       */
	__IO uint32_t CNTL;					/*!< CAN control            */
	__IO uint32_t STAT;					/*!< Status register        */
	__I  uint32_t EC;					/*!< Error counter          */
	__IO uint32_t BT;					/*!< Bit timing register    */
	__I  uint32_t INT;					/*!< Interrupt register     */
	__IO uint32_t TEST;					/*!< Test register          */
	__IO uint32_t BRPE;					/*!< Baud rate prescaler extension register */
	__I  uint32_t  RESERVED0;
	CCAN_IF_T IF[2];
	__I  uint32_t  RESERVED2[8];
	__I  uint32_t TXREQ1;				/*!< Transmission request 1 */
	__I  uint32_t TXREQ2;				/*!< Transmission request 2 */
	__I  uint32_t  RESERVED3[6];
	__I  uint32_t ND1;					/*!< New data 1             */
	__I  uint32_t ND2;					/*!< New data 2             */
	__I  uint32_t  RESERVED4[6];
	__I  uint32_t IR1;					/*!< Interrupt pending 1    */
	__I  uint32_t IR2;					/*!< Interrupt pending 2    */
	__I  uint32_t  RESERVED5[6];
	__I  uint32_t MSGV1;				/*!< Message valid 1        */
	__I  uint32_t MSGV2;				/*!< Message valid 2        */
	__I  uint32_t  RESERVED6[6];
	__IO uint32_t CLKDIV;				/*!< CAN clock divider register */
} LPC_CCAN_T;

/* CCAN Control register bit definitions */
#define CCAN_CTRL_INIT      (1 << 0)	/*!< Initialization is started. */
#define CCAN_CTRL_IE        (1 << 1)	/*!< Module Interupt Enable. */
#define CCAN_CTRL_SIE       (1 << 2)	/*!< Status Change Interupt Enable. */
#define CCAN_CTRL_EIE       (1 << 3)	/*!< Error Interupt Enable. */
#define CCAN_CTRL_DAR       (1 << 5)	/*!< Automatic retransmission disabled. */
#define CCAN_CTRL_CCE       (1 << 6)	/*!< The CPU has write access to the CANBT register while the INIT bit is one.*/
#define CCAN_CTRL_TEST      (1 << 7)	/*!< Test mode. */

/* CCAN STAT register bit definitions */
#define CCAN_STAT_LEC_MASK  (0x07)		/* Mask for Last Error Code */
#define CCAN_STAT_TXOK      (1 << 3)	/* Transmitted a message successfully */
#define CCAN_STAT_RXOK      (1 << 4)	/* Received a message successfully */
#define CCAN_STAT_EPASS     (1 << 5)	/* The CAN controller is in the error passive state*/
#define CCAN_STAT_EWARN     (1 << 6)	/*At least one of the error counters in the EC has reached the error warning limit of 96.*/
#define CCAN_STAT_BOFF      (1 << 7)	/*The CAN controller is in busoff state.*/

/**
 * @brief Last Error Code definition
 */
typedef enum {
	CCAN_LEC_NO_ERROR,		/*!< No error */
	CCAN_LEC_STUFF_ERROR,	/*!< More than 5 equal bits in a sequence have occurred in a part of a received message where this is not allowed. */
	CCAN_LEC_FORM_ERROR,	/*!< A fixed format part of a received frame has the wrong format */
	CCAN_LEC_ACK_ERROR,		/*!< The message this CAN core transmitted was not acknowledged. */
	CCAN_LEC_BIT1_ERROR,	/*!< During the transmission of a message (with the exception of the arbitration field), the device wanted to send a HIGH/recessive level
							    (bit of logical value "1"), but the monitored bus value was LOW/dominant. */
	CCAN_LEC_BIT0_ERROR,	/*!< During the transmission of a message (or acknowledge bit, or active error flag, or overload flag), the device wanted to send a
							    LOW/dominant level (data or identifier bit logical value "0"), but the monitored Bus value was HIGH/recessive. During busoff recovery this
							    status is set each time a sequence of 11 HIGH/recessive bits has been monitored. This enables
							    the CPU to monitor the proceeding of the busoff recovery sequence (indicating the bus is not stuck at LOW/dominant or continuously disturbed). */
	CCAN_LEC_CRC_ERROR,		/*!< The CRC checksum was incorrect in the message received. */
} CCAN_LEC_T;

/* CCAN INT register bit definitions */
#define CCAN_INT_NO_PENDING       0			/*!< No interrupt pending */
#define CCAN_INT_STATUS           0x8000	/*!< Status interrupt*/
#define CCAN_INT_MSG_NUM(n)       (n)		/*!<Number of messages which caused interrupts */

/* CCAN TEST register bit definitions */
#define CCAN_TEST_BASIC_MODE      (1 << 2)	/*!<IF1 registers used as TX buffer, IF2 registers used as RX buffer. */
#define CCAN_TEST_SILENT_MODE     (1 << 3)	/*!<The module is in silent mode. */
#define CCAN_TEST_LOOPBACK_MODE   (1 << 4)	/*!<Loop back mode is enabled.*/
#define CCAN_TEST_TD_CONTROLLED   (0)		/*!< Level at the TD pin is controlled by the CAN controller.*/
#define CCAN_TEST_TD_MONITORED    (1 << 5)	/*!< The sample point can be monitored at the TD pin.*/
#define CCAN_TEST_TD_DOMINANT     (2 << 5)	/*!< TD pin is driven LOW/dominant.*/
#define CCAN_TEST_TD_RECESSIVE    (3 << 5)	/*!< TD pin is driven HIGH/recessive.*/
#define CCAN_TEST_RD_DOMINANT     (0)		/*!< The CAN bus is dominant (RD = 0).*/
#define CCAN_TEST_RD_RECESSIVE    (1 << 7)		/*!< The CAN bus is recessive (RD = 1).*/

#define CCAN_SEG1_DEFAULT_VAL 5
#define CCAN_SEG2_DEFAULT_VAL 4
#define CCAN_SJW_DEFAULT_VAL  0

/**
 * @brief CCAN Transfer direction definition
 */
typedef enum {
	CCAN_RX_DIR,
	CCAN_TX_DIR,
} CCAN_TRANSFER_DIR_T;

/**
 * @brief	Enable CCAN Interrupts
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	mask	: Interrupt mask, or-ed bit value of
 *						- CCAN_CTRL_IE <br>
 *						- CCAN_CTRL_SIE <br>
 *						- CCAN_CTRL_EIE <br>
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_EnableInt(LPC_CCAN_T *pCCAN, uint32_t mask)
{
	pCCAN->CNTL |= mask;
}

/**
 * @brief	Disable CCAN Interrupts
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	mask	: Interrupt mask, or-ed bit value of
 *						- CCAN_CTRL_IE <br>
 *						- CCAN_CTRL_SIE <br>
 *						- CCAN_CTRL_EIE <br>
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_DisableInt(LPC_CCAN_T *pCCAN, uint32_t mask)
{
	pCCAN->CNTL &= ~mask;
}

/**
 * @brief	Get the source ID of an interrupt
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @return	Interrupt source ID
 */
STATIC INLINE uint32_t Chip_CCAN_GetIntID(LPC_CCAN_T *pCCAN)
{
	return pCCAN->INT;
}

/**
 * @brief	Get the CCAN status register
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @return	CCAN status register (or-ed bit value of  CCAN_STAT_*)
 */
STATIC INLINE uint32_t Chip_CCAN_GetStatus(LPC_CCAN_T *pCCAN)
{
	return pCCAN->STAT;
}

/**
 * @brief	Set the CCAN status
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	val		: Value to be set for status register (or-ed bit value of  CCAN_STAT_*)
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_SetStatus(LPC_CCAN_T *pCCAN, uint32_t val)
{
	pCCAN->STAT = val & 0x1F;
}

/**
 * @brief	Clear the status of CCAN bus
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	val	: Status to be cleared (or-ed bit value of  CCAN_STAT_*)
 * @return	Nothing
 */
void Chip_CCAN_ClearStatus(LPC_CCAN_T *pCCAN, uint32_t val);

/**
 * @brief	Get the current value of the transmit/receive error counter
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	dir	: direction
 * @return	Current value of the transmit/receive error counter
 * @note	When @a dir is #CCAN_RX_DIR, then MSB (bit-7) indicates the
 * receiver error passive level, if the bit is High(1) then the reciever
 * counter has reached error passive level as specified in CAN2.0
 * specification; else if the bit is Low(0) it indicates that the
 * error counter is below the passive level. Bits from (bit6-0) has
 * the actual error count. When @a dir is #CCAN_TX_DIR, the complete
 * 8-bits indicates the number of tx errors.
 */
STATIC INLINE uint8_t Chip_CCAN_GetErrCounter(LPC_CCAN_T *pCCAN, CCAN_TRANSFER_DIR_T dir)
{
	return (dir == CCAN_TX_DIR) ? (pCCAN->EC & 0x0FF) : ((pCCAN->EC >> 8) & 0x0FF);
}

/**
 * @brief	Enable test mode in CCAN
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_EnableTestMode(LPC_CCAN_T *pCCAN)
{
	pCCAN->CNTL |= CCAN_CTRL_TEST;
}

/**
 * @brief	Enable test mode in CCAN
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_DisableTestMode(LPC_CCAN_T *pCCAN)
{
	pCCAN->CNTL &= ~CCAN_CTRL_TEST;
}

/**
 * @brief	Enable/Disable test mode in CCAN
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	cfg	: Test function, or-ed bit values of CCAN_TEST_*
 * @return	Nothing
 * @note	Test Mode must be enabled before using Chip_CCAN_EnableTestMode function.
 */
STATIC INLINE void Chip_CCAN_ConfigTestMode(LPC_CCAN_T *pCCAN, uint32_t cfg)
{
	pCCAN->TEST = cfg;
}

/**
 * @brief	Enable automatic retransmission
 * @param	pCCAN			: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_EnableAutoRetransmit(LPC_CCAN_T *pCCAN)
{
	pCCAN->CNTL &= ~CCAN_CTRL_DAR;
}

/**
 * @brief	Disable automatic retransmission
 * @param	pCCAN			: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_DisableAutoRetransmit(LPC_CCAN_T *pCCAN)
{
	pCCAN->CNTL |= CCAN_CTRL_DAR;
}

/**
 * @brief	Get the transmit repuest bit in all message objects
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @return	A 32 bits value, each bit corresponds to transmit request bit in message objects
 */
STATIC INLINE uint32_t Chip_CCAN_GetTxRQST(LPC_CCAN_T *pCCAN)
{
	return pCCAN->TXREQ1 | (pCCAN->TXREQ2 << 16);
}

/**
 * @brief	Initialize the CCAN peripheral, free all message object in RAM
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
void Chip_CCAN_Init(LPC_CCAN_T *pCCAN);

/**
 * @brief	De-initialize the CCAN peripheral
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @return	Nothing
 */
void Chip_CCAN_DeInit(LPC_CCAN_T *pCCAN);

/**
 * @brief	Select bit rate for CCAN bus
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	bitRate	: Bit rate to be set
 * @return	SUCCESS/ERROR
 */
Status Chip_CCAN_SetBitRate(LPC_CCAN_T *pCCAN, uint32_t bitRate);

/** Number of message objects in Message RAM */
#define CCAN_MSG_MAX_NUM                              32

/**
 * @brief CAN message object structure
 */
typedef struct {
	uint32_t    id;		/**< ID of message, if bit 30 is set then this is extended frame */
	uint32_t    dlc;	/**< Message data length */
	uint8_t     data[8];	/**< Message data */
} CCAN_MSG_OBJ_T;

typedef enum {
	CCAN_MSG_IF1 = 0,
	CCAN_MSG_IF2 = 1,
} CCAN_MSG_IF_T;

/* bit field of IF command request n register */
#define CCAN_IF_CMDREQ_MSG_NUM(n)  (n)			/* Message number (1->20) */
#define CCAN_IF_CMDREQ_BUSY          0x8000			/* 1 is writing is progress, cleared when RD/WR done */

/* bit field of IF command mask register */
#define CCAN_IF_CMDMSK_DATAB        (1 << 0)		/** 1 is transfer data byte 4-7 to message object, 0 is not */
#define CCAN_IF_CMDMSK_DATAA        (1 << 1)		/** 1 is transfer data byte 0-3 to message object, 0 is not */
#define CCAN_IF_CMDMSK_W_TXRQST     (1 << 2)		/** Request a transmission. Set the TXRQST bit IF1/2_MCTRL. */
#define CCAN_IF_CMDMSK_R_NEWDAT     (1 << 2)		/** Clear NEWDAT bit in the message object */
#define CCAN_IF_CMDMSK_R_CLRINTPND  (1 << 3)		/** Clear INTPND bit in the message object. */
#define CCAN_IF_CMDMSK_CTRL         (1 << 4)		/** 1 is transfer the CTRL bit to the message object, 0 is not */
#define CCAN_IF_CMDMSK_ARB          (1 << 5)		/** 1 is transfer the ARB bits to the message object, 0 is not */
#define CCAN_IF_CMDMSK_MASK         (1 << 6)		/** 1 is transfer the MASK bit to the message object, 0 is not */
#define CCAN_IF_CMDMSK_WR           (1 << 7)		/*  Tranfer direction: Write */
#define CCAN_IF_CMDMSK_RD           (0)				/*  Tranfer direction: Read */
#define CCAN_IF_CMDMSK_TRANSFER_ALL (CCAN_IF_CMDMSK_CTRL | CCAN_IF_CMDMSK_MASK | CCAN_IF_CMDMSK_ARB | \
									 CCAN_IF_CMDMSK_DATAB | CCAN_IF_CMDMSK_DATAA)

/* bit field of IF mask 2 register */
#define CCAN_IF_MASK2_MXTD          (1 << 15)				/* 1 is extended identifier bit is used in the RX filter unit, 0 is not */
#define CCAN_IF_MASK2_MDIR(n)       (((n) & 0x01) <<  14)	/* 1 is direction bit is used in the RX filter unit, 0 is not */

/* bit field of IF arbitration 2 register */
#define CCAN_IF_ARB2_DIR(n)         (((n) & 0x01) << 13)	/* 1: Dir = transmit, 0: Dir = receive */
#define CCAN_IF_ARB2_XTD            (1 << 14)		/* Extended identifier bit is used*/
#define CCAN_IF_ARB2_MSGVAL         (1 << 15)		/* Message valid bit, 1 is valid in the MO handler, 0 is ignored */

/* bit field of IF message control register */
#define CCAN_IF_MCTRL_DLC_MSK        0x000F			/* bit mask for DLC */
#define CCAN_IF_MCTRL_EOB           (1 << 7)		/* End of buffer, always write to 1 */
#define CCAN_IF_MCTRL_TXRQ          (1 << 8)		/* 1 is TxRqst enabled */
#define CCAN_IF_MCTRL_RMTEN(n)      (((n) & 1UL) << 9)	/* 1 is remote frame enabled */
#define CCAN_IF_MCTRL_RXIE          (1 << 10)		/* 1 is RX interrupt enabled */
#define CCAN_IF_MCTRL_TXIE          (1 << 11)		/* 1 is TX interrupt enabled */
#define CCAN_IF_MCTRL_UMSK          (1 << 12)		/* 1 is to use the mask for the receive filter mask. */
#define CCAN_IF_MCTRL_INTP          (1 << 13)		/* 1 indicates message object is an interrupt source */
#define CCAN_IF_MCTRL_MLST          (1 << 14)		/* 1 indicates a message loss. */
#define CCAN_IF_MCTRL_NEWD          (1 << 15)		/* 1 indicates new data is in the message buffer.  */

#define CCAN_MSG_ID_STD_MASK        0x07FF
#define CCAN_MSG_ID_EXT_MASK        0x1FFFFFFF

/**
 * @brief	Tranfer message object between IF registers and Message RAM
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	IFSel		: The Message interface to be used
 * @param	mask	: command mask (or-ed bit value of CCAN_IF_CMDMSK_*)
 * @param	msgNum		: The number of message object in message RAM to be get
 * @return	Nothing
 */
void Chip_CCAN_TransferMsgObject(LPC_CCAN_T *pCCAN,
								 CCAN_MSG_IF_T IFSel,
								 uint32_t mask,
								 uint32_t msgNum);

/**
 * @brief	Set a message into the message object in message RAM
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	IFSel		: The Message interface to be used
 * @param	dir	: transmit/receive
 * @param	remoteFrame: Enable/Disable passives transmit by using remote frame
 * @param	msgNum		: Message number
 * @param	pMsgObj		: Pointer of message to be set
 * @return	Nothing
 */
void Chip_CCAN_SetMsgObject (LPC_CCAN_T *pCCAN,
							 CCAN_MSG_IF_T IFSel,
							 CCAN_TRANSFER_DIR_T dir,
							 bool remoteFrame,
							 uint8_t msgNum,
							 const CCAN_MSG_OBJ_T *pMsgObj);

/**
 * @brief	Get a message object in message RAM into the message buffer
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	msgNum		: The number of message object in message RAM to be get
 * @param	pMsgObj		: Pointer of the message buffer
 * @return	Nothing
 */
void Chip_CCAN_GetMsgObject(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum, CCAN_MSG_OBJ_T *pMsgObj);

/**
 * @brief	Enable/Disable the message object to valid
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	msgNum	: Message number
 * @param	valid	: true: valid, false: invalide
 * @return	Nothing
 */
void Chip_CCAN_SetValidMsg(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum, bool valid);

/**
 * @brief	Check the message objects is valid or not
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @return	A 32 bits value, each bit corresponds to a message objects form 0 to 31 (1 is valid, 0 is invalid)
 */
STATIC INLINE uint32_t Chip_CCAN_GetValidMsg(LPC_CCAN_T *pCCAN)
{
	return pCCAN->MSGV1 | (pCCAN->MSGV2 << 16);
}

/**
 * @brief	Clear the pending message interrupt
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	msgNum	: Message number
 * @param	dir	: Select transmit or receive interrupt to be cleared
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_ClearMsgIntPend(LPC_CCAN_T *pCCAN,
											 CCAN_MSG_IF_T IFSel,
											 uint8_t msgNum,
											 CCAN_TRANSFER_DIR_T dir)
{
	Chip_CCAN_TransferMsgObject(pCCAN, IFSel, CCAN_IF_CMDMSK_RD | CCAN_IF_CMDMSK_R_CLRINTPND, msgNum);
}

/**
 * @brief	Clear new data flag bit in the message object
 * @param	pCCAN	: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	msgNum	: Message number
 * @return	Nothing
 */
STATIC INLINE void Chip_CCAN_ClearNewDataFlag(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum)
{
	Chip_CCAN_TransferMsgObject(pCCAN, IFSel, CCAN_IF_CMDMSK_RD | CCAN_IF_CMDMSK_R_NEWDAT, msgNum);
}

/**
 * @brief	Send a message
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	remoteFrame: Enable/Disable passives transmit by using remote frame
 * @param	pMsgObj		: Message to be transmitted
 * @return	Nothing
 */
void Chip_CCAN_Send (LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, bool remoteFrame, CCAN_MSG_OBJ_T *pMsgObj);

/**
 * @brief	Register a message ID for receiving
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	IFSel	: The Message interface to be used
 * @param	id		: Received message ID
 * @return	Nothing
 */
void Chip_CCAN_AddReceiveID(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint32_t id);

/**
 * @brief	Remove a registered message ID from receiving
 * @param	IFSel	: The Message interface to be used
 * @param	pCCAN		: The base of CCAN peripheral on the chip
 * @param	id		: Received message ID to be removed
 * @return	Nothing
 */
void Chip_CCAN_DeleteReceiveID(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint32_t id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CCAN_18XX_43XX_H_ */
