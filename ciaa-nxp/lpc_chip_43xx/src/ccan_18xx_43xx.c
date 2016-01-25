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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Configure the bit timing for CCAN bus */
STATIC void configTimming(LPC_CCAN_T *pCCAN,
						  uint32_t ClkDiv,
						  uint32_t BaudRatePrescaler,
						  uint8_t SynJumpWidth,
						  uint8_t Tseg1,
						  uint8_t Tseg2)
{
	/* Reset software */
	if (!(pCCAN->CNTL & CCAN_CTRL_INIT)) {
		pCCAN->CNTL |= CCAN_CTRL_INIT;
	}

	/*Set bus timing */
	pCCAN->CLKDIV = ClkDiv;			/* Divider for CAN VPB3 clock */
	pCCAN->CNTL |= CCAN_CTRL_CCE;		/* Start configuring bit timing */
	pCCAN->BT = (BaudRatePrescaler & 0x3F) | (SynJumpWidth & 0x03) << 6 | (Tseg1 & 0x0F) << 8 | (Tseg2 & 0x07) << 12;
	pCCAN->BRPE = BaudRatePrescaler >> 6;	/* Set Baud Rate Prescaler MSBs */
	pCCAN->CNTL &= ~CCAN_CTRL_CCE;		/* Stop configuring bit timing */

	/* Finish software initialization */
	pCCAN->CNTL &= ~CCAN_CTRL_INIT;
	while ( pCCAN->CNTL & CCAN_CTRL_INIT ) {}
}

/* Return 1->32; 0 if not find free msg */
STATIC uint8_t getFreeMsgObject(LPC_CCAN_T *pCCAN)
{
	uint32_t msg_valid;
	uint8_t i;
	msg_valid = Chip_CCAN_GetValidMsg(pCCAN);
	for (i = 0; i < CCAN_MSG_MAX_NUM; i++) {
		if (!((msg_valid >> i) & 1UL)) {
			return i + 1;
		}
	}
	return 0;	// No free object
}

STATIC void freeMsgObject(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum)
{
	Chip_CCAN_SetValidMsg(pCCAN, IFSel, msgNum, false);
}

/* Returns clock index for the peripheral block */
STATIC CHIP_CCU_CLK_T Chip_CCAN_GetClockIndex(LPC_CCAN_T *pCCAN)
{
	CHIP_CCU_CLK_T clkCCAN;

	if (pCCAN == LPC_C_CAN1) {
		clkCCAN = CLK_APB1_CAN1;
	}
	else {
		clkCCAN = CLK_APB3_CAN0;
	}

	return clkCCAN;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the CCAN peripheral, free all message object in RAM */
void Chip_CCAN_Init(LPC_CCAN_T *pCCAN)
{
	uint8_t i;

	Chip_Clock_EnableOpts(Chip_CCAN_GetClockIndex(pCCAN), true, false, 1);

	for (i = 1; i <= CCAN_MSG_MAX_NUM; i++) {
		freeMsgObject(pCCAN, CCAN_MSG_IF1, i);
	}
	Chip_CCAN_ClearStatus(pCCAN, (CCAN_STAT_RXOK | CCAN_STAT_TXOK));
}

/* De-initialize the CCAN peripheral */
void Chip_CCAN_DeInit(LPC_CCAN_T *pCCAN)
{
	Chip_Clock_Disable(Chip_CCAN_GetClockIndex(pCCAN));
}

/* Select bit rate for CCAN bus */
Status Chip_CCAN_SetBitRate(LPC_CCAN_T *pCCAN, uint32_t bitRate)
{
	uint32_t pClk, div, quanta, segs, seg1, seg2, clk_per_bit, can_sjw;
	pClk = Chip_Clock_GetRate(Chip_CCAN_GetClockIndex(pCCAN));
	clk_per_bit = pClk / bitRate;

	for (div = 0; div <= 15; div++) {
		for (quanta = 1; quanta <= 32; quanta++) {
			for (segs = 3; segs <= 17; segs++) {
				if (clk_per_bit == (segs * quanta * (div + 1))) {
					segs -= 3;
					seg1 = segs / 2;
					seg2 = segs - seg1;
					can_sjw = seg1 > 3 ? 3 : seg1;
					configTimming(pCCAN, div, quanta - 1, can_sjw, seg1, seg2);
					return SUCCESS;
				}
			}
		}
	}
	return ERROR;
}

/* Clear the status of CCAN bus */
void Chip_CCAN_ClearStatus(LPC_CCAN_T *pCCAN, uint32_t val)
{
	uint32_t tmp = Chip_CCAN_GetStatus(pCCAN);
	Chip_CCAN_SetStatus(pCCAN, tmp & (~val));
}

/* Set a message into the message object in message RAM */
void Chip_CCAN_SetMsgObject(LPC_CCAN_T *pCCAN,
							CCAN_MSG_IF_T IFSel,
							CCAN_TRANSFER_DIR_T dir,
							bool remoteFrame,
							uint8_t msgNum,
							const CCAN_MSG_OBJ_T *pMsgObj)
{
	uint16_t *pData;
	uint32_t msgCtrl = 0;

	if (pMsgObj == NULL) {
		return;
	}
	pData = (uint16_t *) (pMsgObj->data);

	msgCtrl |= CCAN_IF_MCTRL_UMSK | CCAN_IF_MCTRL_RMTEN(remoteFrame) | CCAN_IF_MCTRL_EOB |
			   (pMsgObj->dlc & CCAN_IF_MCTRL_DLC_MSK);
	if (dir == CCAN_TX_DIR) {
		msgCtrl |= CCAN_IF_MCTRL_TXIE;
		if (!remoteFrame) {
			msgCtrl |= CCAN_IF_MCTRL_TXRQ;
		}
	}
	else {
		msgCtrl |= CCAN_IF_MCTRL_RXIE;
	}

	pCCAN->IF[IFSel].MCTRL = msgCtrl;
	pCCAN->IF[IFSel].DA1 = *pData++;	/* Lower two bytes of message pointer */
	pCCAN->IF[IFSel].DA2 = *pData++;	/* Upper two bytes of message pointer */
	pCCAN->IF[IFSel].DB1 = *pData++;	/* Lower two bytes of message pointer */
	pCCAN->IF[IFSel].DB2 = *pData;	/* Upper two bytes of message pointer */

	/* Configure arbitration */
	if (!(pMsgObj->id & (0x1 << 30))) {					/* bit 30 is 0, standard frame */
		/* Mxtd: 0, Mdir: 1, Mask is 0x7FF */
		pCCAN->IF[IFSel].MSK2 = CCAN_IF_MASK2_MDIR(dir) | (CCAN_MSG_ID_STD_MASK << 2);
		pCCAN->IF[IFSel].MSK1 = 0x0000;

		/* MsgVal: 1, Mtd: 0, Dir: 1, ID = 0x200 */
		pCCAN->IF[IFSel].ARB2 = CCAN_IF_ARB2_MSGVAL | CCAN_IF_ARB2_DIR(dir) | (pMsgObj->id << 2);
		pCCAN->IF[IFSel].ARB1 = 0x0000;
	}
	else {										/* Extended frame */
		/* Mxtd: 1, Mdir: 1, Mask is 0x1FFFFFFF */
		pCCAN->IF[IFSel].MSK2 = CCAN_IF_MASK2_MXTD | CCAN_IF_MASK2_MDIR(dir) | (CCAN_MSG_ID_EXT_MASK >> 16);
		pCCAN->IF[IFSel].MSK1 = CCAN_MSG_ID_EXT_MASK & 0x0000FFFF;

		/* MsgVal: 1, Mtd: 1, Dir: 1, ID = 0x200000 */
		pCCAN->IF[IFSel].ARB2 = CCAN_IF_ARB2_MSGVAL | CCAN_IF_ARB2_XTD | CCAN_IF_ARB2_DIR(dir) | (pMsgObj->id >> 16);
		pCCAN->IF[IFSel].ARB1 = pMsgObj->id & 0x0000FFFF;
	}

	Chip_CCAN_TransferMsgObject(pCCAN, IFSel, CCAN_IF_CMDMSK_WR | CCAN_IF_CMDMSK_TRANSFER_ALL, msgNum);
}

/* Get a message object in message RAM into the message buffer */
void Chip_CCAN_GetMsgObject(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum, CCAN_MSG_OBJ_T *pMsgObj)
{
	uint32_t *pData;
	if (!pMsgObj) {
		return;
	}
	pData = (uint32_t *) pMsgObj->data;
	Chip_CCAN_TransferMsgObject(pCCAN,
								IFSel,
								CCAN_IF_CMDMSK_RD | CCAN_IF_CMDMSK_TRANSFER_ALL | CCAN_IF_CMDMSK_R_CLRINTPND,
								msgNum);

	if (pCCAN->IF[IFSel].MCTRL & CCAN_IF_MCTRL_NEWD) {
		pMsgObj->id = (pCCAN->IF[IFSel].ARB1) | (pCCAN->IF[IFSel].ARB2 << 16);
		pMsgObj->dlc = pCCAN->IF[IFSel].MCTRL & CCAN_IF_MCTRL_DLC_MSK;
		*pData++ = (pCCAN->IF[IFSel].DA2 << 16) | pCCAN->IF[IFSel].DA1;
		*pData = (pCCAN->IF[IFSel].DB2 << 16) | pCCAN->IF[IFSel].DB1;

		if (pMsgObj->id & (0x1 << 30)) {
			pMsgObj->id &= CCAN_MSG_ID_EXT_MASK;
		}
		else {
			pMsgObj->id >>= 18;
			pMsgObj->id &= CCAN_MSG_ID_STD_MASK;
		}
	}
}

/* Data transfer between IF registers and Message RAM */
void Chip_CCAN_TransferMsgObject(LPC_CCAN_T *pCCAN,
								 CCAN_MSG_IF_T IFSel,
								 uint32_t mask,
								 uint32_t msgNum) {
	msgNum &= 0x3F;
	pCCAN->IF[IFSel].CMDMSK = mask;
	pCCAN->IF[IFSel].CMDREQ = msgNum;
	while (pCCAN->IF[IFSel].CMDREQ & CCAN_IF_CMDREQ_BUSY ) {}
}

/* Enable/Disable the message object to valid */
void Chip_CCAN_SetValidMsg(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint8_t msgNum, bool valid)
{

	uint32_t temp;
	temp = pCCAN->IF[IFSel].ARB2;
	if (!valid) {
		pCCAN->IF[IFSel].ARB2 = (temp & (~CCAN_IF_ARB2_MSGVAL));
	}
	else {
		pCCAN->IF[IFSel].ARB2 = (temp | (CCAN_IF_ARB2_MSGVAL));
	}

	Chip_CCAN_TransferMsgObject(pCCAN, IFSel, CCAN_IF_CMDMSK_WR | CCAN_IF_CMDMSK_ARB, msgNum);
}

/* Send a message */
void Chip_CCAN_Send(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, bool remoteFrame, CCAN_MSG_OBJ_T *pMsgObj)
{
	uint8_t msgNum = getFreeMsgObject(pCCAN);
	if (!msgNum) {
		return;
	}
	Chip_CCAN_SetMsgObject(pCCAN, IFSel, CCAN_TX_DIR, remoteFrame, msgNum, pMsgObj);
	while (Chip_CCAN_GetTxRQST(pCCAN) >> (msgNum - 1)) {	// blocking , wait for sending completed
	}
	if (!remoteFrame) {
		freeMsgObject(pCCAN, IFSel, msgNum);
	}
}

/* Register a message ID for receiving */
void Chip_CCAN_AddReceiveID(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint32_t id)
{
	CCAN_MSG_OBJ_T temp;
	uint8_t msgNum = getFreeMsgObject(pCCAN);
	if (!msgNum) {
		return;
	}
	temp.id = id;
	Chip_CCAN_SetMsgObject(pCCAN, IFSel, CCAN_RX_DIR, false, msgNum, &temp);
}

/* Remove a registered message ID from receiving */
void Chip_CCAN_DeleteReceiveID(LPC_CCAN_T *pCCAN, CCAN_MSG_IF_T IFSel, uint32_t id)
{
	uint8_t i;
	CCAN_MSG_OBJ_T temp;
	for (i = 1; i <= CCAN_MSG_MAX_NUM; i++) {
		Chip_CCAN_GetMsgObject(pCCAN, IFSel, i, &temp);
		if (temp.id == id) {
			freeMsgObject(pCCAN, IFSel, i);
		}
	}
}

