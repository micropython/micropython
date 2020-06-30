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
#include "irq.h"

#if MICROPY_HW_ENABLE_CAN && MICROPY_HW_ENABLE_FDCAN

#define FDCAN_ELEMENT_MASK_STDID (0x1ffc0000) // Standard Identifier
#define FDCAN_ELEMENT_MASK_EXTID (0x1fffffff) // Extended Identifier
#define FDCAN_ELEMENT_MASK_RTR   (0x20000000) // Remote Transmission Request
#define FDCAN_ELEMENT_MASK_XTD   (0x40000000) // Extended Identifier
#define FDCAN_ELEMENT_MASK_ESI   (0x80000000) // Error State Indicator
#define FDCAN_ELEMENT_MASK_TS    (0x0000ffff) // Timestamp
#define FDCAN_ELEMENT_MASK_DLC   (0x000f0000) // Data Length Code
#define FDCAN_ELEMENT_MASK_BRS   (0x00100000) // Bit Rate Switch
#define FDCAN_ELEMENT_MASK_FDF   (0x00200000) // FD Format
#define FDCAN_ELEMENT_MASK_FIDX  (0x7f000000) // Filter Index
#define FDCAN_ELEMENT_MASK_ANMF  (0x80000000) // Accepted Non-matching Frame

bool can_init(pyb_can_obj_t *can_obj, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart) {
    (void)auto_restart;

    FDCAN_InitTypeDef *init = &can_obj->can.Init;
    init->FrameFormat = FDCAN_FRAME_CLASSIC;
    init->Mode = mode;

    init->NominalPrescaler = prescaler; // tq = NominalPrescaler x (1/fdcan_ker_ck)
    init->NominalSyncJumpWidth = sjw;
    init->NominalTimeSeg1 = bs1; // NominalTimeSeg1 = Propagation_segment + Phase_segment_1
    init->NominalTimeSeg2 = bs2;

    init->AutoRetransmission = ENABLE;
    init->TransmitPause = DISABLE;
    init->ProtocolException = ENABLE;

    // The Message RAM is shared between CAN1 and CAN2. Setting the offset to half
    // the Message RAM for the second CAN and using half the resources for each CAN.
    if (can_obj->can_id == PYB_CAN_1) {
        init->MessageRAMOffset = 0;
    } else {
        init->MessageRAMOffset = 2560 / 2;
    }

    init->StdFiltersNbr = 64; // 128 / 2
    init->ExtFiltersNbr = 0; // Not used

    init->TxEventsNbr = 16;  // 32 / 2
    init->RxBuffersNbr = 32; // 64 / 2
    init->TxBuffersNbr = 16; // 32 / 2

    init->RxFifo0ElmtsNbr = 64; // 128 / 2
    init->RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;

    init->RxFifo1ElmtsNbr = 64; // 128 / 2
    init->RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;

    init->TxFifoQueueElmtsNbr = 16; // Tx fifo elements
    init->TxElmtSize = FDCAN_DATA_BYTES_8;
    init->TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

    FDCAN_GlobalTypeDef *CANx = NULL;
    const pin_obj_t *pins[2];

    switch (can_obj->can_id) {
        #if defined(MICROPY_HW_CAN1_TX)
        case PYB_CAN_1:
            CANx = FDCAN1;
            pins[0] = MICROPY_HW_CAN1_TX;
            pins[1] = MICROPY_HW_CAN1_RX;
            break;
        #endif

        #if defined(MICROPY_HW_CAN2_TX)
        case PYB_CAN_2:
            CANx = FDCAN2;
            pins[0] = MICROPY_HW_CAN2_TX;
            pins[1] = MICROPY_HW_CAN2_RX;
            break;
        #endif

        default:
            return false;
    }

    // Enable FDCAN clock
    __HAL_RCC_FDCAN_CLK_ENABLE();

    // init GPIO
    uint32_t pin_mode = MP_HAL_PIN_MODE_ALT;
    uint32_t pin_pull = MP_HAL_PIN_PULL_UP;
    for (int i = 0; i < 2; ++i) {
        if (!mp_hal_pin_config_alt(pins[i], pin_mode, pin_pull, AF_FN_CAN, can_obj->can_id)) {
            return false;
        }
    }

    // init CANx
    can_obj->can.Instance = CANx;
    HAL_FDCAN_Init(&can_obj->can);

    // Disable acceptance of non-matching frames (enabled by default)
    HAL_FDCAN_ConfigGlobalFilter(&can_obj->can, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);

    // The configuration registers are locked after CAN is started.
    HAL_FDCAN_Start(&can_obj->can);

    // Reset all filters
    for (int f = 0; f < 64; ++f) {
        can_clearfilter(can_obj, f, 0);
    }

    can_obj->is_enabled = true;
    can_obj->num_error_warning = 0;
    can_obj->num_error_passive = 0;
    can_obj->num_bus_off = 0;

    switch (can_obj->can_id) {
        case PYB_CAN_1:
            NVIC_SetPriority(FDCAN1_IT0_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
            NVIC_SetPriority(FDCAN1_IT1_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
            break;
        case PYB_CAN_2:
            NVIC_SetPriority(FDCAN2_IT0_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
            NVIC_SetPriority(FDCAN2_IT1_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
            break;
        default:
            return false;
    }

    __HAL_FDCAN_ENABLE_IT(&can_obj->can, FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE);
    __HAL_FDCAN_ENABLE_IT(&can_obj->can, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
    __HAL_FDCAN_ENABLE_IT(&can_obj->can, FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
    __HAL_FDCAN_ENABLE_IT(&can_obj->can, FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO1_FULL);

    return true;
}

void can_deinit(pyb_can_obj_t *self) {
    self->is_enabled = false;
    HAL_FDCAN_DeInit(&self->can);
    if (self->can.Instance == FDCAN1) {
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
        // TODO check if FDCAN2 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #if defined(MICROPY_HW_CAN2_TX)
    } else if (self->can.Instance == FDCAN2) {
        HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
        // TODO check if FDCAN2 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #endif
    }
}

void can_clearfilter(pyb_can_obj_t *self, uint32_t f, uint8_t bank) {
    if (self && self->can.Instance) {
        FDCAN_FilterTypeDef filter = {0};
        filter.IdType = FDCAN_STANDARD_ID;
        filter.FilterIndex = f;
        filter.FilterConfig = FDCAN_FILTER_DISABLE;
        HAL_FDCAN_ConfigFilter(&self->can, &filter);
    }
}

int can_receive(FDCAN_HandleTypeDef *can, int fifo, FDCAN_RxHeaderTypeDef *hdr, uint8_t *data, uint32_t timeout_ms) {
    volatile uint32_t *rxf, *rxa;
    uint32_t fl;

    if (fifo == FDCAN_RX_FIFO0) {
        rxf = &can->Instance->RXF0S;
        rxa = &can->Instance->RXF0A;
        fl = FDCAN_RXF0S_F0FL;
    } else {
        rxf = &can->Instance->RXF1S;
        rxa = &can->Instance->RXF1A;
        fl = FDCAN_RXF1S_F1FL;
    }

    // Wait for a message to become available, with timeout
    uint32_t start = HAL_GetTick();
    while ((*rxf & fl) == 0) {
        if (timeout_ms != HAL_MAX_DELAY) {
            if (HAL_GetTick() - start >= timeout_ms) {
                return -MP_ETIMEDOUT;
            }
        }
        MICROPY_EVENT_POLL_HOOK
    }

    // Get pointer to incoming message
    uint32_t index, *address;
    if (fifo == FDCAN_RX_FIFO0) {
        index = (*rxf & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
        address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * can->Init.RxFifo0ElmtSize * 4));
    } else {
        index = (*rxf & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
        address = (uint32_t *)(can->msgRam.RxFIFO1SA + (index * can->Init.RxFifo1ElmtSize * 4));
    }

    // Parse header of message
    hdr->IdType = *address & FDCAN_ELEMENT_MASK_XTD;
    if (hdr->IdType == FDCAN_STANDARD_ID) {
        hdr->Identifier = (*address & FDCAN_ELEMENT_MASK_STDID) >> 18;
    } else {
        hdr->Identifier = *address & FDCAN_ELEMENT_MASK_EXTID;
    }
    hdr->RxFrameType = *address & FDCAN_ELEMENT_MASK_RTR;
    hdr->ErrorStateIndicator = *address++ & FDCAN_ELEMENT_MASK_ESI;
    hdr->RxTimestamp = *address & FDCAN_ELEMENT_MASK_TS;
    hdr->DataLength = (*address & FDCAN_ELEMENT_MASK_DLC) >> 16;
    hdr->BitRateSwitch = *address & FDCAN_ELEMENT_MASK_BRS;
    hdr->FDFormat = *address & FDCAN_ELEMENT_MASK_FDF;
    hdr->FilterIndex = (*address & FDCAN_ELEMENT_MASK_FIDX) >> 24;
    hdr->IsFilterMatchingFrame = (*address++ & FDCAN_ELEMENT_MASK_ANMF) >> 31;

    // Copy data
    uint8_t *pdata = (uint8_t *)address;
    for (uint32_t i = 0; i < 8; ++i) { // TODO use DLCtoBytes[hdr->DataLength] for length > 8
        *data++ = *pdata++;
    }

    // Release (free) message from FIFO
    *rxa = index;

    return 0; // success
}

STATIC void can_rx_irq_handler(uint can_id, uint fifo_id) {
    mp_obj_t callback;
    pyb_can_obj_t *self;
    mp_obj_t irq_reason = MP_OBJ_NEW_SMALL_INT(0);
    byte *state;

    self = MP_STATE_PORT(pyb_can_obj_all)[can_id - 1];

    if (fifo_id == FDCAN_RX_FIFO0) {
        callback = self->rxcallback0;
        state = &self->rx_state0;
    } else {
        callback = self->rxcallback1;
        state = &self->rx_state1;
    }

    switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            __HAL_FDCAN_DISABLE_IT(&self->can,  (fifo_id == FDCAN_RX_FIFO0) ?
                FDCAN_IT_RX_FIFO0_NEW_MESSAGE : FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
            irq_reason = MP_OBJ_NEW_SMALL_INT(0);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_MESSAGE_PENDING:
            __HAL_FDCAN_DISABLE_IT(&self->can, (fifo_id == FDCAN_RX_FIFO0) ? FDCAN_IT_RX_FIFO0_FULL : FDCAN_IT_RX_FIFO1_FULL);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, (fifo_id == FDCAN_RX_FIFO0) ? FDCAN_FLAG_RX_FIFO0_FULL : FDCAN_FLAG_RX_FIFO1_FULL);
            irq_reason = MP_OBJ_NEW_SMALL_INT(1);
            *state = RX_STATE_FIFO_FULL;
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_FDCAN_DISABLE_IT(&self->can, (fifo_id == FDCAN_RX_FIFO0) ?
                FDCAN_IT_RX_FIFO0_MESSAGE_LOST : FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, (fifo_id == FDCAN_RX_FIFO0) ?
                FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST : FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST);
            irq_reason = MP_OBJ_NEW_SMALL_INT(2);
            *state = RX_STATE_FIFO_OVERFLOW;
            break;
        case RX_STATE_FIFO_OVERFLOW:
            // This should never happen
            break;
    }

    pyb_can_handle_callback(self, fifo_id, callback, irq_reason);
}

#if defined(MICROPY_HW_CAN1_TX)
void FDCAN1_IT0_IRQHandler(void) {
    IRQ_ENTER(FDCAN1_IT0_IRQn);
    can_rx_irq_handler(PYB_CAN_1, FDCAN_RX_FIFO0);
    IRQ_EXIT(FDCAN1_IT0_IRQn);
}

void FDCAN1_IT1_IRQHandler(void) {
    IRQ_ENTER(FDCAN1_IT1_IRQn);
    can_rx_irq_handler(PYB_CAN_1, FDCAN_RX_FIFO1);
    IRQ_EXIT(FDCAN1_IT1_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN2_TX)
void FDCAN2_IT0_IRQHandler(void) {
    IRQ_ENTER(FDCAN2_IT0_IRQn);
    can_rx_irq_handler(PYB_CAN_2, FDCAN_RX_FIFO0);
    IRQ_EXIT(FDCAN2_IT0_IRQn);
}

void FDCAN2_IT1_IRQHandler(void) {
    IRQ_ENTER(FDCAN2_IT1_IRQn);
    can_rx_irq_handler(PYB_CAN_2, FDCAN_RX_FIFO1);
    IRQ_EXIT(FDCAN2_IT1_IRQn);
}
#endif

#endif // MICROPY_HW_ENABLE_CAN && MICROPY_HW_ENABLE_FDCAN
