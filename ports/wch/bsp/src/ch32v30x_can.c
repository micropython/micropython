/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_can.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the CAN firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_can.h"
#include "ch32v30x_rcc.h"

/* CAN CTLR Register bits */
#define CTLR_DBF            ((uint32_t)0x00010000)

/* CAN Mailbox Transmit Request */
#define TMIDxR_TXRQ         ((uint32_t)0x00000001)

/* CAN FCTLR Register bits */
#define FCTLR_FINIT         ((uint32_t)0x00000001)

/* Time out for INAK bit */
#define INAK_TIMEOUT        ((uint32_t)0x0000FFFF)
/* Time out for SLAK bit */
#define SLAK_TIMEOUT        ((uint32_t)0x0000FFFF)

/* Flags in TSTATR register */
#define CAN_FLAGS_TSTATR    ((uint32_t)0x08000000)
/* Flags in RFIFO1 register */
#define CAN_FLAGS_RFIFO1    ((uint32_t)0x04000000)
/* Flags in RFIFO0 register */
#define CAN_FLAGS_RFIFO0    ((uint32_t)0x02000000)
/* Flags in STATR register */
#define CAN_FLAGS_STATR     ((uint32_t)0x01000000)
/* Flags in ERRSR register */
#define CAN_FLAGS_ERRSR     ((uint32_t)0x00F00000)

/* Mailboxes definition */
#define CAN_TXMAILBOX_0     ((uint8_t)0x00)
#define CAN_TXMAILBOX_1     ((uint8_t)0x01)
#define CAN_TXMAILBOX_2     ((uint8_t)0x02)

#define CAN_MODE_MASK       ((uint32_t)0x00000003)

static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit);

/*********************************************************************
 * @fn      CAN_DeInit
 *
 * @brief   Deinitializes the CAN peripheral registers to their default reset
 *        values.
 *
 * @param   CANx - where x can be 1 or 2 to select the CAN peripheral.
 *
 * @return  none
 */
void CAN_DeInit(CAN_TypeDef *CANx)
{
    if(CANx == CAN1)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
    }
    else
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
    }
}

/*********************************************************************
 * @fn      CAN_Init
 *
 * @brief   Initializes the CAN peripheral according to the specified
 *        parameters in the CAN_InitStruct.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_InitStruct - pointer to a CAN_InitTypeDef structure that
 *        contains the configuration information for the CAN peripheral.
 *
 * @return  InitStatus - CAN InitStatus state.
 *             CAN_InitStatus_Failed.
 *             CAN_InitStatus_Success.
 */
uint8_t CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct)
{
    uint8_t  InitStatus = CAN_InitStatus_Failed;
    uint32_t wait_ack = 0x00000000;

    CANx->CTLR &= (~(uint32_t)CAN_CTLR_SLEEP);
    CANx->CTLR |= CAN_CTLR_INRQ;

    while(((CANx->STATR & CAN_STATR_INAK) != CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
    {
        wait_ack++;
    }

    if((CANx->STATR & CAN_STATR_INAK) != CAN_STATR_INAK)
    {
        InitStatus = CAN_InitStatus_Failed;
    }
    else
    {
        if(CAN_InitStruct->CAN_TTCM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_TTCM;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_TTCM;
        }

        if(CAN_InitStruct->CAN_ABOM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_ABOM;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_ABOM;
        }

        if(CAN_InitStruct->CAN_AWUM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_AWUM;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_AWUM;
        }

        if(CAN_InitStruct->CAN_NART == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_NART;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_NART;
        }

        if(CAN_InitStruct->CAN_RFLM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_RFLM;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_RFLM;
        }

        if(CAN_InitStruct->CAN_TXFP == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_TXFP;
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_TXFP;
        }

        CANx->BTIMR = (uint32_t)((uint32_t)CAN_InitStruct->CAN_Mode << 30) |
                      ((uint32_t)CAN_InitStruct->CAN_SJW << 24) |
                      ((uint32_t)CAN_InitStruct->CAN_BS1 << 16) |
                      ((uint32_t)CAN_InitStruct->CAN_BS2 << 20) |
                      ((uint32_t)CAN_InitStruct->CAN_Prescaler - 1);
        CANx->CTLR &= ~(uint32_t)CAN_CTLR_INRQ;
        wait_ack = 0;

        while(((CANx->STATR & CAN_STATR_INAK) == CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
        {
            wait_ack++;
        }

        if((CANx->STATR & CAN_STATR_INAK) == CAN_STATR_INAK)
        {
            InitStatus = CAN_InitStatus_Failed;
        }
        else
        {
            InitStatus = CAN_InitStatus_Success;
        }
    }

    return InitStatus;
}

/*********************************************************************
 * @fn      CAN_FilterInit
 *
 * @brief   Initializes the CAN peripheral according to the specified
 *        parameters in the CAN_FilterInitStruct.
 *
 * @param   CAN_FilterInitStruct - pointer to a CAN_FilterInitTypeDef
 *        structure that contains the configuration information.
 *
 * @return  none
 */
void CAN_FilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct)
{
    uint32_t filter_number_bit_pos = 0;

    filter_number_bit_pos = ((uint32_t)1) << CAN_FilterInitStruct->CAN_FilterNumber;
    CAN1->FCTLR |= FCTLR_FINIT;
    CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;

    if(CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_16bit)
    {
        CAN1->FSCFGR &= ~(uint32_t)filter_number_bit_pos;

        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh);
    }

    if(CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_32bit)
    {
        CAN1->FSCFGR |= filter_number_bit_pos;

        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow);
    }

    if(CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdMask)
    {
        CAN1->FMCFGR &= ~(uint32_t)filter_number_bit_pos;
    }
    else
    {
        CAN1->FMCFGR |= (uint32_t)filter_number_bit_pos;
    }

    if(CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
    {
        CAN1->FAFIFOR &= ~(uint32_t)filter_number_bit_pos;
    }

    if(CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO1)
    {
        CAN1->FAFIFOR |= (uint32_t)filter_number_bit_pos;
    }

    if(CAN_FilterInitStruct->CAN_FilterActivation == ENABLE)
    {
        CAN1->FWR |= filter_number_bit_pos;
    }

    CAN1->FCTLR &= ~FCTLR_FINIT;
}

/*********************************************************************
 * @fn      CAN_StructInit
 *
 * @brief   Fills each CAN_InitStruct member with its default value.
 *
 * @param   CAN_InitStruct - pointer to a CAN_InitTypeDef structure which
 *        will be initialized.
 *
 * @return  none
 */
void CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct)
{
    CAN_InitStruct->CAN_TTCM = DISABLE;
    CAN_InitStruct->CAN_ABOM = DISABLE;
    CAN_InitStruct->CAN_AWUM = DISABLE;
    CAN_InitStruct->CAN_NART = DISABLE;
    CAN_InitStruct->CAN_RFLM = DISABLE;
    CAN_InitStruct->CAN_TXFP = DISABLE;
    CAN_InitStruct->CAN_Mode = CAN_Mode_Normal;
    CAN_InitStruct->CAN_SJW = CAN_SJW_1tq;
    CAN_InitStruct->CAN_BS1 = CAN_BS1_4tq;
    CAN_InitStruct->CAN_BS2 = CAN_BS2_3tq;
    CAN_InitStruct->CAN_Prescaler = 1;
}

/*********************************************************************
 * @fn      CAN_SlaveStartBank
 *
 * @brief   This function applies only to CH32 Connectivity line devices.
 *
 * @param   CAN_BankNumber - Select the start slave bank filter from 1..27.
 *
 * @return  none
 */
void CAN_SlaveStartBank(uint8_t CAN_BankNumber)
{
    CAN1->FCTLR |= FCTLR_FINIT;
    CAN1->FCTLR &= (uint32_t)0xFFFFC0F1;
    CAN1->FCTLR |= (uint32_t)(CAN_BankNumber) << 8;
    CAN1->FCTLR &= ~FCTLR_FINIT;
}

/*********************************************************************
 * @fn      CAN_DBGFreeze
 *
 * @brief   Enables or disables the DBG Freeze for CAN.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void CAN_DBGFreeze(CAN_TypeDef *CANx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        CANx->CTLR |= CTLR_DBF;
    }
    else
    {
        CANx->CTLR &= ~CTLR_DBF;
    }
}

/*********************************************************************
 * @fn      CAN_TTComModeCmd
 *
 * @brief   Enables or disabes the CAN Time TriggerOperation communication mode.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void CAN_TTComModeCmd(CAN_TypeDef *CANx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        CANx->CTLR |= CAN_CTLR_TTCM;

        CANx->sTxMailBox[0].TXMDTR |= ((uint32_t)CAN_TXMDT0R_TGT);
        CANx->sTxMailBox[1].TXMDTR |= ((uint32_t)CAN_TXMDT1R_TGT);
        CANx->sTxMailBox[2].TXMDTR |= ((uint32_t)CAN_TXMDT2R_TGT);
    }
    else
    {
        CANx->CTLR &= (uint32_t)(~(uint32_t)CAN_CTLR_TTCM);

        CANx->sTxMailBox[0].TXMDTR &= ((uint32_t)~CAN_TXMDT0R_TGT);
        CANx->sTxMailBox[1].TXMDTR &= ((uint32_t)~CAN_TXMDT1R_TGT);
        CANx->sTxMailBox[2].TXMDTR &= ((uint32_t)~CAN_TXMDT2R_TGT);
    }
}

/*********************************************************************
 * @fn      CAN_Transmit
 *
 * @brief   Initiates the transmission of a message.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          TxMessage - pointer to a structure which contains CAN Id, CAN
 *        DLC and CAN data.
 *
 * @return  transmit_mailbox - The number of the mailbox that is used for
 *        transmission or CAN_TxStatus_NoMailBox if there is no empty mailbox.
 */
uint8_t CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage)
{
    uint8_t transmit_mailbox = 0;

    if((CANx->TSTATR & CAN_TSTATR_TME0) == CAN_TSTATR_TME0)
    {
        transmit_mailbox = 0;
    }
    else if((CANx->TSTATR & CAN_TSTATR_TME1) == CAN_TSTATR_TME1)
    {
        transmit_mailbox = 1;
    }
    else if((CANx->TSTATR & CAN_TSTATR_TME2) == CAN_TSTATR_TME2)
    {
        transmit_mailbox = 2;
    }
    else
    {
        transmit_mailbox = CAN_TxStatus_NoMailBox;
    }

    if(transmit_mailbox != CAN_TxStatus_NoMailBox)
    {
        CANx->sTxMailBox[transmit_mailbox].TXMIR &= TMIDxR_TXRQ;
        if(TxMessage->IDE == CAN_Id_Standard)
        {
            CANx->sTxMailBox[transmit_mailbox].TXMIR |= ((TxMessage->StdId << 21) |
                                                         TxMessage->RTR);
        }
        else
        {
            CANx->sTxMailBox[transmit_mailbox].TXMIR |= ((TxMessage->ExtId << 3) |
                                                         TxMessage->IDE |
                                                         TxMessage->RTR);
        }

        TxMessage->DLC &= (uint8_t)0x0000000F;
        CANx->sTxMailBox[transmit_mailbox].TXMDTR &= (uint32_t)0xFFFFFFF0;
        CANx->sTxMailBox[transmit_mailbox].TXMDTR |= TxMessage->DLC;

        CANx->sTxMailBox[transmit_mailbox].TXMDLR = (((uint32_t)TxMessage->Data[3] << 24) |
                                                     ((uint32_t)TxMessage->Data[2] << 16) |
                                                     ((uint32_t)TxMessage->Data[1] << 8) |
                                                     ((uint32_t)TxMessage->Data[0]));
        CANx->sTxMailBox[transmit_mailbox].TXMDHR = (((uint32_t)TxMessage->Data[7] << 24) |
                                                     ((uint32_t)TxMessage->Data[6] << 16) |
                                                     ((uint32_t)TxMessage->Data[5] << 8) |
                                                     ((uint32_t)TxMessage->Data[4]));
        CANx->sTxMailBox[transmit_mailbox].TXMIR |= TMIDxR_TXRQ;
    }

    return transmit_mailbox;
}

/*********************************************************************
 * @fn      CAN_TransmitStatus
 *
 * @brief   Checks the transmission of a message.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          TransmitMailbox - the number of the mailbox that is used for
 *        transmission.
 *
 * @return  state -
 *            CAN_TxStatus_Ok.
 *            CAN_TxStatus_Failed.
 */
uint8_t CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox)
{
    uint32_t state = 0;

    switch(TransmitMailbox)
    {
        case(CAN_TXMAILBOX_0):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP0 | CAN_TSTATR_TXOK0 | CAN_TSTATR_TME0);
            break;

        case(CAN_TXMAILBOX_1):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP1 | CAN_TSTATR_TXOK1 | CAN_TSTATR_TME1);
            break;

        case(CAN_TXMAILBOX_2):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP2 | CAN_TSTATR_TXOK2 | CAN_TSTATR_TME2);
            break;

        default:
            state = CAN_TxStatus_Failed;
            break;
    }

    switch(state)
    {
        case(0x0):
            state = CAN_TxStatus_Pending;
            break;

        case(CAN_TSTATR_RQCP0 | CAN_TSTATR_TME0):
            state = CAN_TxStatus_Failed;
            break;

        case(CAN_TSTATR_RQCP1 | CAN_TSTATR_TME1):
            state = CAN_TxStatus_Failed;
            break;

        case(CAN_TSTATR_RQCP2 | CAN_TSTATR_TME2):
            state = CAN_TxStatus_Failed;
            break;

        case(CAN_TSTATR_RQCP0 | CAN_TSTATR_TXOK0 | CAN_TSTATR_TME0):
            state = CAN_TxStatus_Ok;
            break;

        case(CAN_TSTATR_RQCP1 | CAN_TSTATR_TXOK1 | CAN_TSTATR_TME1):
            state = CAN_TxStatus_Ok;
            break;

        case(CAN_TSTATR_RQCP2 | CAN_TSTATR_TXOK2 | CAN_TSTATR_TME2):
            state = CAN_TxStatus_Ok;
            break;

        default:
            state = CAN_TxStatus_Failed;
            break;
    }

    return (uint8_t)state;
}

/*********************************************************************
 * @fn      CAN_CancelTransmit
 *
 * @brief   Cancels a transmit request.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          Mailbox -  Mailbox number.
 *            CAN_TXMAILBOX_0.
 *            CAN_TXMAILBOX_1.
 *            CAN_TXMAILBOX_2.
 *
 * @return  none
 */
void CAN_CancelTransmit(CAN_TypeDef *CANx, uint8_t Mailbox)
{
    switch(Mailbox)
    {
        case(CAN_TXMAILBOX_0):
            CANx->TSTATR |= CAN_TSTATR_ABRQ0;
            break;

        case(CAN_TXMAILBOX_1):
            CANx->TSTATR |= CAN_TSTATR_ABRQ1;
            break;

        case(CAN_TXMAILBOX_2):
            CANx->TSTATR |= CAN_TSTATR_ABRQ2;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      CAN_Receive
 *
 * @brief   Receives a message.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          FIFONumber - Receive FIFO number.
 *            CAN_FIFO0.
 *            CAN_FIFO1.
 *          RxMessage -  pointer to a structure receive message which contains
 *        CAN Id, CAN DLC, CAN datas and FMI number.
 *
 * @return  none
 */
void CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage)
{
    RxMessage->IDE = (uint8_t)0x04 & CANx->sFIFOMailBox[FIFONumber].RXMIR;

    if(RxMessage->IDE == CAN_Id_Standard)
    {
        RxMessage->StdId = (uint32_t)0x000007FF & (CANx->sFIFOMailBox[FIFONumber].RXMIR >> 21);
    }
    else
    {
        RxMessage->ExtId = (uint32_t)0x1FFFFFFF & (CANx->sFIFOMailBox[FIFONumber].RXMIR >> 3);
    }

    RxMessage->RTR = (uint8_t)0x02 & CANx->sFIFOMailBox[FIFONumber].RXMIR;
    RxMessage->DLC = (uint8_t)0x0F & CANx->sFIFOMailBox[FIFONumber].RXMDTR;
    RxMessage->FMI = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDTR >> 8);
    RxMessage->Data[0] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RXMDLR;
    RxMessage->Data[1] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 8);
    RxMessage->Data[2] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 16);
    RxMessage->Data[3] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 24);
    RxMessage->Data[4] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RXMDHR;
    RxMessage->Data[5] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 8);
    RxMessage->Data[6] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 16);
    RxMessage->Data[7] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 24);

    if(FIFONumber == CAN_FIFO0)
    {
        CANx->RFIFO0 |= CAN_RFIFO0_RFOM0;
    }
    else
    {
        CANx->RFIFO1 |= CAN_RFIFO1_RFOM1;
    }
}

/*********************************************************************
 * @fn      CAN_FIFORelease
 *
 * @brief   Releases the specified FIFO.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          FIFONumber - Receive FIFO number.
 *            CAN_FIFO0.
 *            CAN_FIFO1.
 *
 * @return  none
 */
void CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber)
{
    if(FIFONumber == CAN_FIFO0)
    {
        CANx->RFIFO0 |= CAN_RFIFO0_RFOM0;
    }
    else
    {
        CANx->RFIFO1 |= CAN_RFIFO1_RFOM1;
    }
}

/*********************************************************************
 * @fn      CAN_MessagePending
 *
 * @brief   Returns the number of pending messages.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          FIFONumber - Receive FIFO number.
 *            CAN_FIFO0.
 *            CAN_FIFO1.
 *
 * @return  message_pending: which is the number of pending message.
 */
uint8_t CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber)
{
    uint8_t message_pending = 0;

    if(FIFONumber == CAN_FIFO0)
    {
        message_pending = (uint8_t)(CANx->RFIFO0 & (uint32_t)0x03);
    }
    else if(FIFONumber == CAN_FIFO1)
    {
        message_pending = (uint8_t)(CANx->RFIFO1 & (uint32_t)0x03);
    }
    else
    {
        message_pending = 0;
    }

    return message_pending;
}

/*********************************************************************
 * @fn      CAN_OperatingModeRequest
 *
 * @brief   Select the CAN Operation mode.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_OperatingMode - CAN Operating Mode.
 *            CAN_OperatingMode_Initialization.
 *            CAN_OperatingMode_Normal.
 *            CAN_OperatingMode_Sleep.
 *
 * @return  status -
 *          CAN_ModeStatus_Failed - CAN failed entering the specific mode.
 *          CAN_ModeStatus_Success - CAN Succeed entering the specific mode.
 */
uint8_t CAN_OperatingModeRequest(CAN_TypeDef *CANx, uint8_t CAN_OperatingMode)
{
    uint8_t  status = CAN_ModeStatus_Failed;
    uint32_t timeout = INAK_TIMEOUT;

    if(CAN_OperatingMode == CAN_OperatingMode_Initialization)
    {
        CANx->CTLR = (uint32_t)((CANx->CTLR & (uint32_t)(~(uint32_t)CAN_CTLR_SLEEP)) | CAN_CTLR_INRQ);

        while(((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_INAK) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_INAK)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else if(CAN_OperatingMode == CAN_OperatingMode_Normal)
    {
        CANx->CTLR &= (uint32_t)(~(CAN_CTLR_SLEEP | CAN_CTLR_INRQ));

        while(((CANx->STATR & CAN_MODE_MASK) != 0) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != 0)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else if(CAN_OperatingMode == CAN_OperatingMode_Sleep)
    {
        CANx->CTLR = (uint32_t)((CANx->CTLR & (uint32_t)(~(uint32_t)CAN_CTLR_INRQ)) | CAN_CTLR_SLEEP);

        while(((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_SLAK) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_SLAK)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else
    {
        status = CAN_ModeStatus_Failed;
    }

    return (uint8_t)status;
}

/*********************************************************************
 * @fn      CAN_Sleep
 *
 * @brief   Enters the low power mode.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *
 * @return  sleepstatus -
 *            CAN_Sleep_Ok.
 *            CAN_Sleep_Failed.
 */
uint8_t CAN_Sleep(CAN_TypeDef *CANx)
{
    uint8_t sleepstatus = CAN_Sleep_Failed;

    CANx->CTLR = (((CANx->CTLR) & (uint32_t)(~(uint32_t)CAN_CTLR_INRQ)) | CAN_CTLR_SLEEP);

    if((CANx->STATR & (CAN_STATR_SLAK | CAN_STATR_INAK)) == CAN_STATR_SLAK)
    {
        sleepstatus = CAN_Sleep_Ok;
    }

    return (uint8_t)sleepstatus;
}

/*********************************************************************
 * @fn      CAN_WakeUp
 *
 * @brief   Wakes the CAN up.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *
 * @return  wakeupstatus -
 *            CAN_WakeUp_Ok.
 *            CAN_WakeUp_Failed.
 */
uint8_t CAN_WakeUp(CAN_TypeDef *CANx)
{
    uint32_t wait_slak = SLAK_TIMEOUT;
    uint8_t  wakeupstatus = CAN_WakeUp_Failed;

    CANx->CTLR &= ~(uint32_t)CAN_CTLR_SLEEP;

    while(((CANx->STATR & CAN_STATR_SLAK) == CAN_STATR_SLAK) && (wait_slak != 0x00))
    {
        wait_slak--;
    }
    if((CANx->STATR & CAN_STATR_SLAK) != CAN_STATR_SLAK)
    {
        wakeupstatus = CAN_WakeUp_Ok;
    }

    return (uint8_t)wakeupstatus;
}

/*********************************************************************
 * @fn      CAN_GetLastErrorCode
 *
 * @brief   Returns the CANx's last error code (LEC).
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *
 * @return  errorcode - specifies the Error code.
 *            CAN_ErrorCode_NoErr - No Error.
 *            CAN_ErrorCode_StuffErr - Stuff Error.
 *            CAN_ErrorCode_FormErr - Form Error.
 *            CAN_ErrorCode_ACKErr - Acknowledgment Error.
 *            CAN_ErrorCode_BitRecessiveErr - Bit Recessive Error.
 *            CAN_ErrorCode_BitDominantErr - Bit Dominant Error.
 *            CAN_ErrorCode_CRCErr - CRC Error.
 *            CAN_ErrorCode_SoftwareSetErr - Software Set Error.
 */
uint8_t CAN_GetLastErrorCode(CAN_TypeDef *CANx)
{
    uint8_t errorcode = 0;

    errorcode = (((uint8_t)CANx->ERRSR) & (uint8_t)CAN_ERRSR_LEC);

    return errorcode;
}

/*********************************************************************
 * @fn      CAN_GetReceiveErrorCounter
 *
 * @brief   Returns the CANx Receive Error Counter (REC).
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *
 * @return  counter - CAN Receive Error Counter.
 */
uint8_t CAN_GetReceiveErrorCounter(CAN_TypeDef *CANx)
{
    uint8_t counter = 0;

    counter = (uint8_t)((CANx->ERRSR & CAN_ERRSR_REC) >> 24);

    return counter;
}

/*********************************************************************
 * @fn      CAN_GetLSBTransmitErrorCounter
 *
 * @brief   Returns the LSB of the 9-bit CANx Transmit Error Counter(TEC).
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *
 * @return  counter - LSB of the 9-bit CAN Transmit Error Counter.
 */
uint8_t CAN_GetLSBTransmitErrorCounter(CAN_TypeDef *CANx)
{
    uint8_t counter = 0;

    counter = (uint8_t)((CANx->ERRSR & CAN_ERRSR_TEC) >> 16);

    return counter;
}

/*********************************************************************
 * @fn      CAN_ITConfig
 *
 * @brief   Enables or disables the specified CANx interrupts.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_IT - specifies the CAN interrupt sources to be enabled or disabled.
 *            CAN_IT_TME.
 *            CAN_IT_FMP0.
 *            CAN_IT_FF0.
 *            CAN_IT_FOV0.
 *            CAN_IT_FMP1.
 *            CAN_IT_FF1.
 *            CAN_IT_FOV1.
 *            CAN_IT_EWG.
 *            CAN_IT_EPV.
 *            CAN_IT_LEC.
 *            CAN_IT_ERR.
 *            CAN_IT_WKU.
 *            CAN_IT_SLK.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  counter - LSB of the 9-bit CAN Transmit Error Counter.
 */
void CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        CANx->INTENR |= CAN_IT;
    }
    else
    {
        CANx->INTENR &= ~CAN_IT;
    }
}

/*********************************************************************
 * @fn      CAN_GetFlagStatus
 *
 * @brief   Checks whether the specified CAN flag is set or not.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_FLAG - specifies the flag to check.
 *            CAN_FLAG_EWG.
 *            CAN_FLAG_EPV.
 *            CAN_FLAG_BOF.
 *            CAN_FLAG_RQCP0.
 *            CAN_FLAG_RQCP1.
 *            CAN_FLAG_RQCP2.
 *            CAN_FLAG_FMP1.
 *            CAN_FLAG_FF1.
 *            CAN_FLAG_FOV1.
 *            CAN_FLAG_FMP0.
 *            CAN_FLAG_FF0.
 *            CAN_FLAG_FOV0.
 *            CAN_FLAG_WKU.
 *            CAN_FLAG_SLAK.
 *            CAN_FLAG_LEC.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  FlagStatus - SET or RESET.
 */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef *CANx, uint32_t CAN_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((CAN_FLAG & CAN_FLAGS_ERRSR) != (uint32_t)RESET)
    {
        if((CANx->ERRSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_STATR) != (uint32_t)RESET)
    {
        if((CANx->STATR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_TSTATR) != (uint32_t)RESET)
    {
        if((CANx->TSTATR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_RFIFO0) != (uint32_t)RESET)
    {
        if((CANx->RFIFO0 & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else
    {
        if((uint32_t)(CANx->RFIFO1 & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      CAN_ClearFlag
 *
 * @brief   Clears the CAN's pending flags.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_FLAG - specifies the flag to clear.
 *            CAN_FLAG_RQCP0.
 *            CAN_FLAG_RQCP1.
 *            CAN_FLAG_RQCP2.
 *            CAN_FLAG_FF1.
 *            CAN_FLAG_FOV1.
 *            CAN_FLAG_FF0.
 *            CAN_FLAG_FOV0.
 *            CAN_FLAG_WKU.
 *            CAN_FLAG_SLAK.
 *            CAN_FLAG_LEC.
 *
 * @return  none
 */
void CAN_ClearFlag(CAN_TypeDef *CANx, uint32_t CAN_FLAG)
{
    uint32_t flagtmp = 0;

    if(CAN_FLAG == CAN_FLAG_LEC)
    {
        CANx->ERRSR = (uint32_t)RESET;
    }
    else
    {
        flagtmp = CAN_FLAG & 0x000FFFFF;

        if((CAN_FLAG & CAN_FLAGS_RFIFO0) != (uint32_t)RESET)
        {
            CANx->RFIFO0 = (uint32_t)(flagtmp);
        }
        else if((CAN_FLAG & CAN_FLAGS_RFIFO1) != (uint32_t)RESET)
        {
            CANx->RFIFO1 = (uint32_t)(flagtmp);
        }
        else if((CAN_FLAG & CAN_FLAGS_TSTATR) != (uint32_t)RESET)
        {
            CANx->TSTATR = (uint32_t)(flagtmp);
        }
        else
        {
            CANx->STATR = (uint32_t)(flagtmp);
        }
    }
}

/*********************************************************************
 * @fn      CAN_GetITStatus
 *
 * @brief   Checks whether the specified CANx interrupt has occurred or not.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_IT -  specifies the CAN interrupt source to check.
 *            CAN_IT_TME.
 *            CAN_IT_FMP0.
 *            CAN_IT_FF0.
 *            CAN_IT_FOV0.
 *            CAN_IT_FMP1.
 *            CAN_IT_FF1.
 *            CAN_IT_FOV1.
 *            CAN_IT_WKU.
 *            CAN_IT_SLK.
 *            CAN_IT_EWG.
 *            CAN_IT_EPV.
 *            CAN_IT_BOF.
 *            CAN_IT_LEC.
 *            CAN_IT_ERR.
 *
 * @return  ITStatus - SET or RESET.
 */
ITStatus CAN_GetITStatus(CAN_TypeDef *CANx, uint32_t CAN_IT)
{
    ITStatus itstatus = RESET;

    if((CANx->INTENR & CAN_IT) != RESET)
    {
        switch(CAN_IT)
        {
            case CAN_IT_TME:
                itstatus = CheckITStatus(CANx->TSTATR, CAN_TSTATR_RQCP0 | CAN_TSTATR_RQCP1 | CAN_TSTATR_RQCP2);
                break;

            case CAN_IT_FMP0:
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FMP0);
                break;

            case CAN_IT_FF0:
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FULL0);
                break;

            case CAN_IT_FOV0:
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FOVR0);
                break;

            case CAN_IT_FMP1:
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FMP1);
                break;

            case CAN_IT_FF1:
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FULL1);
                break;

            case CAN_IT_FOV1:
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FOVR1);
                break;

            case CAN_IT_WKU:
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_WKUI);
                break;

            case CAN_IT_SLK:
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_SLAKI);
                break;

            case CAN_IT_EWG:
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_EWGF);
                break;

            case CAN_IT_EPV:
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_EPVF);
                break;

            case CAN_IT_BOF:
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_BOFF);
                break;

            case CAN_IT_LEC:
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_LEC);
                break;

            case CAN_IT_ERR:
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_ERRI);
                break;

            default:
                itstatus = RESET;
                break;
        }
    }
    else
    {
        itstatus = RESET;
    }

    return itstatus;
}

/*********************************************************************
 * @fn      CAN_ClearITPendingBit
 *
 * @brief   Clears the CANx's interrupt pending bits.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          CAN_IT - specifies the interrupt pending bit to clear.
 *            CAN_IT_TME.
 *            CAN_IT_FF0.
 *            CAN_IT_FOV0.
 *            CAN_IT_FF1.
 *            CAN_IT_FOV1.
 *            CAN_IT_WKU.
 *            CAN_IT_SLK.
 *            CAN_IT_EWG.
 *            CAN_IT_EPV.
 *            CAN_IT_BOF.
 *            CAN_IT_LEC.
 *            CAN_IT_ERR.
 *
 * @return  none
 */
void CAN_ClearITPendingBit(CAN_TypeDef *CANx, uint32_t CAN_IT)
{
    switch(CAN_IT)
    {
        case CAN_IT_TME:
            CANx->TSTATR = CAN_TSTATR_RQCP0 | CAN_TSTATR_RQCP1 | CAN_TSTATR_RQCP2;
            break;

        case CAN_IT_FF0:
            CANx->RFIFO0 = CAN_RFIFO0_FULL0;
            break;

        case CAN_IT_FOV0:
            CANx->RFIFO0 = CAN_RFIFO0_FOVR0;
            break;

        case CAN_IT_FF1:
            CANx->RFIFO1 = CAN_RFIFO1_FULL1;
            break;

        case CAN_IT_FOV1:
            CANx->RFIFO1 = CAN_RFIFO1_FOVR1;
            break;

        case CAN_IT_WKU:
            CANx->STATR = CAN_STATR_WKUI;
            break;

        case CAN_IT_SLK:
            CANx->STATR = CAN_STATR_SLAKI;
            break;

        case CAN_IT_EWG:
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_EPV:
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_BOF:
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_LEC:
            CANx->ERRSR = RESET;
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_ERR:
            CANx->ERRSR = RESET;
            CANx->STATR = CAN_STATR_ERRI;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      CheckITStatus
 *
 * @brief   Checks whether the CAN interrupt has occurred or not.
 *
 * @param   CAN_Reg - specifies the CAN interrupt register to check
 *          It_Bit - specifies the interrupt source bit to check.
 *
 * @return  ITStatus - SET or RESET.
 */
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit)
{
    ITStatus pendingbitstatus = RESET;

    if((CAN_Reg & It_Bit) != (uint32_t)RESET)
    {
        pendingbitstatus = SET;
    }
    else
    {
        pendingbitstatus = RESET;
    }

    return pendingbitstatus;
}
