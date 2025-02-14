/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_CAN_H
#define MICROPY_INCLUDED_STM32_CAN_H

#include "py/mphal.h"

#if MICROPY_HW_ENABLE_CAN

// This API provides a higher abstraction layer over the two STM32
// CAN controller APIs provided by the ST HAL (one for their classic
// CAN controller, one for their FD CAN controller.)

// There are two implementations, can.c and fdcan.c - only one is included in
// the build for a given board.

#define PYB_CAN_1 (1)
#define PYB_CAN_2 (2)
#define PYB_CAN_3 (3)

#define MASK16 (0)
#define LIST16 (1)
#define MASK32 (2)
#define LIST32 (3)

#if MICROPY_HW_ENABLE_FDCAN
#define CAN_TypeDef                 FDCAN_GlobalTypeDef
#define CAN_HandleTypeDef           FDCAN_HandleTypeDef
#define CanTxMsgTypeDef             FDCAN_TxHeaderTypeDef
#define CanRxMsgTypeDef             FDCAN_RxHeaderTypeDef
#endif

enum {
    CAN_STATE_STOPPED,
    CAN_STATE_ERROR_ACTIVE,
    CAN_STATE_ERROR_WARNING,
    CAN_STATE_ERROR_PASSIVE,
    CAN_STATE_BUS_OFF,
};

typedef enum _rx_state_t {
    RX_STATE_FIFO_EMPTY = 0,
    RX_STATE_MESSAGE_PENDING,
    RX_STATE_FIFO_FULL,
    RX_STATE_FIFO_OVERFLOW,
} rx_state_t;

typedef enum {
    CAN_INT_MESSAGE_RECEIVED,
    CAN_INT_FIFO_FULL,
    CAN_INT_FIFO_OVERFLOW, // These first 3 correspond to pyb.CAN.rxcallback args

    CAN_INT_ERR_BUS_OFF,
    CAN_INT_ERR_PASSIVE,
    CAN_INT_ERR_WARNING,
} can_int_t;

// RX FIFO numbering
//
// Note: For traditional CAN peripheral, the values of CAN_FIFO0 and CAN_FIFO1 are the same
// as this enum. FDCAN macros FDCAN_RX_FIFO0 and FDCAN_RX_FIFO1 are hardware offsets and not the same.
typedef enum {
    CAN_RX_FIFO0,
    CAN_RX_FIFO1,
} can_rx_fifo_t;

bool can_init(CAN_HandleTypeDef *can, int can_id, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart);
void can_deinit(CAN_HandleTypeDef *can);

void can_clearfilter(CAN_HandleTypeDef *can, uint32_t filter_num, uint8_t bank);
int can_receive(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, CanRxMsgTypeDef *msg, uint8_t *data, uint32_t timeout_ms);
HAL_StatusTypeDef can_transmit(CAN_HandleTypeDef *hcan, CanTxMsgTypeDef *txmsg, uint8_t *data, uint32_t Timeout);

// Disable all CAN receive interrupts for a FIFO
void can_disable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo);

// Enable CAN receive interrupts for a FIFO.
//
// Interrupt for CAN_INT_MESSAGE_RECEIVED is only enabled if enable_msg_received is set.
void can_enable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, bool enable_msg_received);

// Implemented in pyb_can.c, called from lower layer
extern void can_irq_handler(uint can_id,  can_int_t interrupt, can_rx_fifo_t fifo);

#if MICROPY_HW_ENABLE_FDCAN

static inline unsigned can_rx_pending(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    return HAL_FDCAN_GetRxFifoFillLevel(can, fifo == CAN_RX_FIFO0 ? FDCAN_RX_FIFO0 : FDCAN_RX_FIFO1);
}

#else

static inline unsigned can_rx_pending(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    return __HAL_CAN_MSG_PENDING(can, fifo == CAN_RX_FIFO0 ? CAN_FIFO0 : CAN_FIFO1);
}

#endif // MICROPY_HW_ENABLE_FDCAN

#endif // MICROPY_HW_ENABLE_CAN

#endif // MICROPY_INCLUDED_STM32_CAN_H
