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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "py/objtuple.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/binary.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "can.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_CAN && MICROPY_HW_ENABLE_FDCAN

#define FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U) /* Remote Transmission Request */
#define FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U) /* Error State Indicator       */
#define FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU) /* Timestamp                   */
#define FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U) /* Data Length Code            */
#define FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U) /* Bit Rate Switch             */
#define FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U) /* FD Format                   */
#define FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U) /* Event FIFO Control          */
#define FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U) /* Message Marker              */
#define FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U) /* Filter Index                */
#define FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U) /* Accepted Non-matching Frame */
#define FDCAN_ELEMENT_MASK_ET    ((uint32_t)0x00C00000U) /* Event type                  */

void can_init0(void) {
    for (uint i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all)); i++) {
        MP_STATE_PORT(pyb_can_obj_all)[i] = NULL;
    }
}

void can_deinit_all(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all)); i++) {
        pyb_can_obj_t *can_obj = MP_STATE_PORT(pyb_can_obj_all)[i];
        if (can_obj != NULL) {
            can_deinit(can_obj);
        }
    }
}

// assumes Init parameters have been set up correctly
bool can_init(pyb_can_obj_t *can_obj) {
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
    uint32_t mode = MP_HAL_PIN_MODE_ALT;
    uint32_t pull = MP_HAL_PIN_PULL_UP;
    for (int i = 0; i < 2; i++) {
        if (!mp_hal_pin_config_alt(pins[i], mode, pull, AF_FN_CAN, can_obj->can_id)) {
            return false;
        }
    }

    // init CANx
    can_obj->can.Instance = CANx;

    HAL_FDCAN_Init(&can_obj->can);

    // Disable acceptance of non-matching frames (enabled by default)
    HAL_FDCAN_ConfigGlobalFilter(&can_obj->can,
            FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);

    // The configuration registers are locked after CAN is started.
    HAL_FDCAN_Start(&can_obj->can);

    // Reset all filters.
    for (int f=0; f<64; f++) {
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
    if(fifo == FDCAN_RX_FIFO0) {
        rxf = &can->Instance->RXF0S;
        rxa = &can->Instance->RXF0A;
    } else {
        rxf = &can->Instance->RXF1S;
        rxa = &can->Instance->RXF1A;
    }

    // Wait for a message to become available, with timeout
    uint32_t start = HAL_GetTick();
    while ((*rxf & 7) == 0) {
        MICROPY_EVENT_POLL_HOOK
        if (HAL_GetTick() - start >= timeout_ms) {
            return -MP_ETIMEDOUT;
        }
    }

    // Read message data
    uint32_t index = 0;
    uint32_t *address;

    /* Calculate Rx FIFO 0 element address */
    index = ((can->Instance->RXF0S & FDCAN_RXF0S_F0GI) >> 8);
    address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * can->Init.RxFifo0ElmtSize * 4));
    hdr->IdType = *address & FDCAN_ELEMENT_MASK_XTD;
    if(hdr->IdType == FDCAN_STANDARD_ID) {
      hdr->Identifier = ((*address & FDCAN_ELEMENT_MASK_STDID) >> 18);
    } else {
      hdr->Identifier = (*address & FDCAN_ELEMENT_MASK_EXTID);
    }
    hdr->RxFrameType = (*address & FDCAN_ELEMENT_MASK_RTR);
    hdr->ErrorStateIndicator = (*address++ & FDCAN_ELEMENT_MASK_ESI);
    hdr->RxTimestamp = (*address & FDCAN_ELEMENT_MASK_TS);
    hdr->DataLength =  ((*address & FDCAN_ELEMENT_MASK_DLC)>>16);
    hdr->BitRateSwitch = (*address & FDCAN_ELEMENT_MASK_BRS);
    hdr->FDFormat = (*address & FDCAN_ELEMENT_MASK_FDF);
    hdr->FilterIndex = ((*address & FDCAN_ELEMENT_MASK_FIDX) >> 24);
    hdr->IsFilterMatchingFrame = ((*address++ & FDCAN_ELEMENT_MASK_ANMF) >> 31);
    // Copy data.
    uint8_t *pdata = (uint8_t *)address;
    for(uint32_t i=0; i<8; i++) { // TODO use DLCtoBytes[hdr->DataLength] for length > 8
      *data++ = *pdata++;
    }

    // Release (free) message from FIFO
    *rxa = index;

    return 0; // success
}

void can_rx_irq_handler(uint can_id, uint fifo_id) {
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

#endif // MICROPY_HW_ENABLE_FDCAN
