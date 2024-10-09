/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "can.h"
#include "pyb_can.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_CAN

#if !MICROPY_HW_ENABLE_FDCAN

bool can_init(CAN_HandleTypeDef *can, int can_id, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart) {
    CAN_InitTypeDef *init = &can->Init;
    init->Mode = mode << 4; // shift-left so modes fit in a small-int
    init->Prescaler = prescaler;
    init->SJW = ((sjw - 1) & 3) << 24;
    init->BS1 = ((bs1 - 1) & 0xf) << 16;
    init->BS2 = ((bs2 - 1) & 7) << 20;
    init->TTCM = DISABLE;
    init->ABOM = auto_restart ? ENABLE : DISABLE;
    init->AWUM = DISABLE;
    init->NART = DISABLE;
    init->RFLM = DISABLE;
    init->TXFP = DISABLE;

    CAN_TypeDef *CANx = NULL;
    uint32_t sce_irq = 0;
    const machine_pin_obj_t *pins[2];

    switch (can_id) {
        #if defined(MICROPY_HW_CAN1_TX)
        case PYB_CAN_1:
            CANx = CAN1;
            sce_irq = CAN1_SCE_IRQn;
            pins[0] = MICROPY_HW_CAN1_TX;
            pins[1] = MICROPY_HW_CAN1_RX;
            __HAL_RCC_CAN1_CLK_ENABLE();
            break;
        #endif

        #if defined(MICROPY_HW_CAN2_TX)
        case PYB_CAN_2:
            CANx = CAN2;
            sce_irq = CAN2_SCE_IRQn;
            pins[0] = MICROPY_HW_CAN2_TX;
            pins[1] = MICROPY_HW_CAN2_RX;
            __HAL_RCC_CAN1_CLK_ENABLE(); // CAN2 is a "slave" and needs CAN1 enabled as well
            __HAL_RCC_CAN2_CLK_ENABLE();
            break;
        #endif

        #if defined(MICROPY_HW_CAN3_TX)
        case PYB_CAN_3:
            CANx = CAN3;
            sce_irq = CAN3_SCE_IRQn;
            pins[0] = MICROPY_HW_CAN3_TX;
            pins[1] = MICROPY_HW_CAN3_RX;
            __HAL_RCC_CAN3_CLK_ENABLE(); // CAN3 is a "master" and doesn't need CAN1 enabled as well
            break;
        #endif

        default:
            return false;
    }

    // init GPIO
    uint32_t pin_mode = MP_HAL_PIN_MODE_ALT;
    uint32_t pin_pull = MP_HAL_PIN_PULL_UP;
    for (int i = 0; i < 2; i++) {
        if (!mp_hal_pin_config_alt(pins[i], pin_mode, pin_pull, AF_FN_CAN, can_id)) {
            return false;
        }
    }

    // init CANx
    can->Instance = CANx;
    HAL_CAN_Init(can);

    __HAL_CAN_ENABLE_IT(can, CAN_IT_ERR | CAN_IT_BOF | CAN_IT_EPV | CAN_IT_EWG);

    NVIC_SetPriority(sce_irq, IRQ_PRI_CAN);
    HAL_NVIC_EnableIRQ(sce_irq);

    return true;
}

void can_deinit(CAN_HandleTypeDef *can) {
    HAL_CAN_DeInit(can);
    if (can->Instance == CAN1) {
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
        __HAL_RCC_CAN1_FORCE_RESET();
        __HAL_RCC_CAN1_RELEASE_RESET();
        __HAL_RCC_CAN1_CLK_DISABLE();
    #if defined(CAN2)
    } else if (can->Instance == CAN2) {
        HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
        HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
        __HAL_RCC_CAN2_FORCE_RESET();
        __HAL_RCC_CAN2_RELEASE_RESET();
        __HAL_RCC_CAN2_CLK_DISABLE();
    #endif
    #if defined(CAN3)
    } else if (can->Instance == CAN3) {
        HAL_NVIC_DisableIRQ(CAN3_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN3_RX1_IRQn);
        HAL_NVIC_DisableIRQ(CAN3_SCE_IRQn);
        __HAL_RCC_CAN3_FORCE_RESET();
        __HAL_RCC_CAN3_RELEASE_RESET();
        __HAL_RCC_CAN3_CLK_DISABLE();
    #endif
    }
}

void can_disable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    __HAL_CAN_DISABLE_IT(can, ((fifo == CAN_RX_FIFO0) ?
        (CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0) :
        (CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1)));
}

void can_enable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, bool enable_msg_received) {
    __HAL_CAN_ENABLE_IT(can, ((fifo == CAN_RX_FIFO0) ?
        ((enable_msg_received ? CAN_IT_FMP0 : 0) | CAN_IT_FF0 | CAN_IT_FOV0) :
        ((enable_msg_received ? CAN_IT_FMP1 : 0) | CAN_IT_FF1 | CAN_IT_FOV1)));
}

void can_clearfilter(CAN_HandleTypeDef *can, uint32_t filter_num, uint8_t bank) {
    CAN_FilterConfTypeDef filter;

    filter.FilterIdHigh = 0;
    filter.FilterIdLow = 0;
    filter.FilterMaskIdHigh = 0;
    filter.FilterMaskIdLow = 0;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterNumber = filter_num;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_16BIT;
    filter.FilterActivation = DISABLE;
    filter.BankNumber = bank;

    HAL_CAN_ConfigFilter(can, &filter);
}

int can_receive(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, CanRxMsgTypeDef *msg, uint8_t *data, uint32_t timeout_ms) {
    volatile uint32_t *rfr;
    if (fifo == CAN_RX_FIFO0) {
        rfr = &can->Instance->RF0R;
    } else {
        rfr = &can->Instance->RF1R;
    }

    // Wait for a message to become available, with timeout
    uint32_t start = HAL_GetTick();
    while ((*rfr & 3) == 0) {
        MICROPY_EVENT_POLL_HOOK
        if (HAL_GetTick() - start >= timeout_ms) {
            return -MP_ETIMEDOUT;
        }
    }

    // Read message data
    CAN_FIFOMailBox_TypeDef *box = &can->Instance->sFIFOMailBox[fifo];
    msg->IDE = box->RIR & 4;
    if (msg->IDE == CAN_ID_STD) {
        msg->StdId = box->RIR >> 21;
    } else {
        msg->ExtId = box->RIR >> 3;
    }
    msg->RTR = box->RIR & 2;
    msg->DLC = box->RDTR & 0xf;
    msg->FMI = box->RDTR >> 8 & 0xff;
    uint32_t rdlr = box->RDLR;
    data[0] = rdlr;
    data[1] = rdlr >> 8;
    data[2] = rdlr >> 16;
    data[3] = rdlr >> 24;
    uint32_t rdhr = box->RDHR;
    data[4] = rdhr;
    data[5] = rdhr >> 8;
    data[6] = rdhr >> 16;
    data[7] = rdhr >> 24;

    // Release (free) message from FIFO
    *rfr |= CAN_RF0R_RFOM0;

    return 0; // success
}

// Lightly modified version of HAL CAN_Transmit to handle Timeout=0 correctly
HAL_StatusTypeDef can_transmit(CAN_HandleTypeDef *hcan, CanTxMsgTypeDef *txmsg, uint8_t *data, uint32_t Timeout) {
    uint32_t transmitmailbox;
    uint32_t tickstart;
    uint32_t rqcpflag = 0;
    uint32_t txokflag = 0;

    hcan->pTxMsg = txmsg;
    (void)data; // Not needed here, caller has set it up as &tx_msg->Data

    // Check the parameters
    assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
    assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
    assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));

    // Select one empty transmit mailbox
    if ((hcan->Instance->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) {
        transmitmailbox = CAN_TXMAILBOX_0;
        rqcpflag = CAN_FLAG_RQCP0;
        txokflag = CAN_FLAG_TXOK0;
    } else if ((hcan->Instance->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) {
        transmitmailbox = CAN_TXMAILBOX_1;
        rqcpflag = CAN_FLAG_RQCP1;
        txokflag = CAN_FLAG_TXOK1;
    } else if ((hcan->Instance->TSR & CAN_TSR_TME2) == CAN_TSR_TME2) {
        transmitmailbox = CAN_TXMAILBOX_2;
        rqcpflag = CAN_FLAG_RQCP2;
        txokflag = CAN_FLAG_TXOK2;
    } else {
        transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
    }

    if (transmitmailbox != CAN_TXSTATUS_NOMAILBOX) {
        // Set up the Id
        hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
        if (hcan->pTxMsg->IDE == CAN_ID_STD) {
            assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));
            hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21) | \
                hcan->pTxMsg->RTR);
        } else {
            assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
            hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3) | \
                hcan->pTxMsg->IDE | \
                hcan->pTxMsg->RTR);
        }

        // Set up the DLC
        hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
        hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0;
        hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

        // Set up the data field
        hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3] << 24) |
            ((uint32_t)hcan->pTxMsg->Data[2] << 16) |
            ((uint32_t)hcan->pTxMsg->Data[1] << 8) |
            ((uint32_t)hcan->pTxMsg->Data[0]));
        hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7] << 24) |
            ((uint32_t)hcan->pTxMsg->Data[6] << 16) |
            ((uint32_t)hcan->pTxMsg->Data[5] << 8) |
            ((uint32_t)hcan->pTxMsg->Data[4]));
        // Request transmission
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;

        if (Timeout == 0) {
            return HAL_OK;
        }

        // Get tick
        tickstart = HAL_GetTick();
        // Check End of transmission flag
        while (!(__HAL_CAN_TRANSMIT_STATUS(hcan, transmitmailbox))) {
            // Check for the Timeout
            if (Timeout != HAL_MAX_DELAY) {
                if ((HAL_GetTick() - tickstart) > Timeout) {
                    // When the timeout expires, we try to abort the transmission of the packet
                    __HAL_CAN_CANCEL_TRANSMIT(hcan, transmitmailbox);
                    while (!__HAL_CAN_GET_FLAG(hcan, rqcpflag)) {
                    }
                    if (__HAL_CAN_GET_FLAG(hcan, txokflag)) {
                        // The abort attempt failed and the message was sent properly
                        return HAL_OK;
                    } else {
                        return HAL_TIMEOUT;
                    }
                }
            }
        }
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

// Workaround for the __HAL_CAN macros expecting a CAN_HandleTypeDef which we
// don't have in the ISR. Using this "fake" struct instead of CAN_HandleTypeDef
// so it's not possible to accidentally call an API that uses one of the other
// fields in the structure.
typedef struct {
    CAN_TypeDef *Instance;
} fake_handle_t;

static void can_rx_irq_handler(uint can_id, CAN_TypeDef *instance, can_rx_fifo_t fifo) {
    uint32_t full_flag, full_int, overrun_flag, overrun_int, pending_int;

    const fake_handle_t handle = {
        .Instance = instance,
    };

    if (fifo == CAN_RX_FIFO0) {
        full_flag = CAN_FLAG_FF0;
        full_int = CAN_IT_FF0;
        overrun_flag = CAN_FLAG_FOV0;
        overrun_int = CAN_IT_FOV0;
        pending_int = CAN_IT_FMP0;
    } else {
        full_flag = CAN_FLAG_FF1;
        full_int = CAN_IT_FF1;
        overrun_flag = CAN_FLAG_FOV1;
        overrun_int = CAN_IT_FOV1;
        pending_int = CAN_IT_FMP1;
    }

    bool full = __HAL_CAN_GET_FLAG(&handle, full_flag);
    bool overrun = __HAL_CAN_GET_FLAG(&handle, overrun_flag);

    // Note: receive interrupt bits are disabled below, and re-enabled by the
    // higher layer after calling can_receive()

    if (full) {
        __HAL_CAN_DISABLE_IT(&handle, full_int);
        __HAL_CAN_CLEAR_FLAG(&handle, full_flag);
        if (!overrun) {
            can_irq_handler(can_id, CAN_INT_FIFO_FULL, fifo);
        }
    }
    if (overrun) {
        __HAL_CAN_DISABLE_IT(&handle, overrun_int);
        __HAL_CAN_CLEAR_FLAG(&handle, overrun_flag);
        can_irq_handler(can_id, CAN_INT_FIFO_OVERFLOW, fifo);
    }

    if (!(full || overrun)) {
        // Process of elimination, if neither of the above
        // FIFO status flags are set then message pending interrupt is what fired.
        __HAL_CAN_DISABLE_IT(&handle, pending_int);
        can_irq_handler(can_id, CAN_INT_MESSAGE_RECEIVED, fifo);
    }
}

static void can_sce_irq_handler(uint can_id, CAN_TypeDef *instance) {
    instance->MSR = CAN_MSR_ERRI; // Write to clear ERRIE interrupt
    uint32_t esr = instance->ESR;
    if (esr & CAN_ESR_BOFF) {
        can_irq_handler(can_id, CAN_INT_ERR_BUS_OFF, 0);
    } else if (esr & CAN_ESR_EPVF) {
        can_irq_handler(can_id, CAN_INT_ERR_PASSIVE, 0);
    } else if (esr & CAN_ESR_EWGF) {
        can_irq_handler(can_id, CAN_INT_ERR_WARNING, 0);
    }
}

#if defined(MICROPY_HW_CAN1_TX)
void CAN1_RX0_IRQHandler(void) {
    IRQ_ENTER(CAN1_RX0_IRQn);
    can_rx_irq_handler(PYB_CAN_1, CAN1, CAN_RX_FIFO0);
    IRQ_EXIT(CAN1_RX0_IRQn);
}

void CAN1_RX1_IRQHandler(void) {
    IRQ_ENTER(CAN1_RX1_IRQn);
    can_rx_irq_handler(PYB_CAN_1, CAN1, CAN_RX_FIFO1);
    IRQ_EXIT(CAN1_RX1_IRQn);
}

void CAN1_SCE_IRQHandler(void) {
    IRQ_ENTER(CAN1_SCE_IRQn);
    can_sce_irq_handler(PYB_CAN_1, CAN1);
    IRQ_EXIT(CAN1_SCE_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN2_TX)
void CAN2_RX0_IRQHandler(void) {
    IRQ_ENTER(CAN2_RX0_IRQn);
    can_rx_irq_handler(PYB_CAN_2, CAN2, CAN_RX_FIFO0);
    IRQ_EXIT(CAN2_RX0_IRQn);
}

void CAN2_RX1_IRQHandler(void) {
    IRQ_ENTER(CAN2_RX1_IRQn);
    can_rx_irq_handler(PYB_CAN_2, CAN2, CAN_RX_FIFO1);
    IRQ_EXIT(CAN2_RX1_IRQn);
}

void CAN2_SCE_IRQHandler(void) {
    IRQ_ENTER(CAN2_SCE_IRQn);
    can_sce_irq_handler(PYB_CAN_2, CAN2);
    IRQ_EXIT(CAN2_SCE_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN3_TX)
void CAN3_RX0_IRQHandler(void) {
    IRQ_ENTER(CAN3_RX0_IRQn);
    can_rx_irq_handler(PYB_CAN_3, CAN3, CAN_RX_FIFO0);
    IRQ_EXIT(CAN3_RX0_IRQn);
}

void CAN3_RX1_IRQHandler(void) {
    IRQ_ENTER(CAN3_RX1_IRQn);
    can_rx_irq_handler(PYB_CAN_3, CAN3, CAN_RX_FIFO1);
    IRQ_EXIT(CAN3_RX1_IRQn);
}

void CAN3_SCE_IRQHandler(void) {
    IRQ_ENTER(CAN3_SCE_IRQn);
    can_sce_irq_handler(PYB_CAN_3, CAN3);
    IRQ_EXIT(CAN3_SCE_IRQn);
}
#endif

#endif // !MICROPY_HW_ENABLE_FDCAN

#endif // MICROPY_HW_ENABLE_CAN
