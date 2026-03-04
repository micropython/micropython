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
// Interface compatibility for the classic CAN controller HAL
#define CAN_TypeDef                 FDCAN_GlobalTypeDef
#define CAN_HandleTypeDef           FDCAN_HandleTypeDef
#define CanTxMsgTypeDef             FDCAN_TxHeaderTypeDef
#define CanRxMsgTypeDef             FDCAN_RxHeaderTypeDef

#define CAN_MODE_NORMAL             FDCAN_MODE_NORMAL
#define CAN_MODE_LOOPBACK           FDCAN_MODE_EXTERNAL_LOOPBACK
#define CAN_MODE_SILENT             FDCAN_MODE_BUS_MONITORING
#define CAN_MODE_SILENT_LOOPBACK    FDCAN_MODE_INTERNAL_LOOPBACK

// FDCAN peripheral has independent indexes for standard id vs extended id filters
#if defined(STM32G4)
#define CAN_HW_MAX_STD_FILTER 28
#define CAN_HW_MAX_EXT_FILTER 8
#elif defined(STM32H7)
// The RAM filtering section is configured for 64 x 1 word elements for 11-bit standard
// identifiers, and 31 x 2 words elements for 29-bit extended identifiers.
// The total number of words reserved for the filtering per FDCAN instance is 126 words.
#define CAN_HW_MAX_STD_FILTER 64
#define CAN_HW_MAX_EXT_FILTER 31
#endif

#else

// CAN1 & CAN2 have 28 filters which can be arbitrarily split, but machine.CAN
// implementation hard-codes to 14/14. CAN3 has 14 independent filters.
#define CAN_HW_MAX_FILTER 14

#endif

typedef enum {
    CAN_STATE_STOPPED,
    CAN_STATE_ERROR_ACTIVE,
    CAN_STATE_ERROR_WARNING,
    CAN_STATE_ERROR_PASSIVE,
    CAN_STATE_BUS_OFF,
} can_state_t;

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

    CAN_INT_TX_COMPLETE,
} can_int_t;

typedef enum {
    CAN_TX_FIFO,
    CAN_TX_QUEUE,
} can_tx_mode_t;

// Counter data as used by pyb.CAN.info() and machine.CAN.get_counters()
typedef struct {
    unsigned tec;
    unsigned rec;
    unsigned tx_pending;
    unsigned rx_fifo0_pending;
    unsigned rx_fifo1_pending;
} can_counters_t;

#if defined(STM32H7)
#define CAN_TX_QUEUE_LEN 16
#else
// FDCAN STM32G4, bxCAN
#define CAN_TX_QUEUE_LEN 3
#endif

// RX FIFO numbering
//
// Note: For traditional CAN peripheral, the values of CAN_FIFO0 and CAN_FIFO1 are the same
// as this enum. FDCAN macros FDCAN_RX_FIFO0 and FDCAN_RX_FIFO1 are hardware offsets and not the same.
typedef enum {
    CAN_RX_FIFO0,
    CAN_RX_FIFO1,
} can_rx_fifo_t;

bool can_init(CAN_HandleTypeDef *can, int can_id, can_tx_mode_t tx_mode, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart);
void can_deinit(CAN_HandleTypeDef *can);

uint32_t can_get_source_freq(void);

// Filtering is slightly different for FDCAN vs CAN
#if MICROPY_HW_ENABLE_FDCAN
void can_clearfilter(CAN_HandleTypeDef *can, uint32_t filter_num, bool ext_id);
#else
void can_clearfilter(CAN_HandleTypeDef *can, uint32_t filter_num, uint8_t bank);
#endif
int can_receive(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, CanRxMsgTypeDef *msg, uint8_t *data, uint32_t timeout_ms);

// Transmit a CAN frame (callee to choose the transmit slot). Used by pyb.CAN only, does not enable TX interrupt
// On FDCAN this function invalidates the 'txmsg' structure if successful.
HAL_StatusTypeDef can_transmit(CAN_HandleTypeDef *hcan, CanTxMsgTypeDef *txmsg, uint8_t *data, uint32_t Timeout);

// Tell the controller to copy a CAN frame copied to 'index' and start transmitting
// On FDCAN this function invalidates the 'txmsg' structure if successful.
HAL_StatusTypeDef can_transmit_buf_index(CAN_HandleTypeDef *hcan, int index, CanTxMsgTypeDef *txmsg, const uint8_t *data);

// Cancel the pending transmission in the specified buffer index. Returns after buffer stops transmitting.
// Result is true if buffer was transmitting, false if not transmitting (or finished transmitting before cancellation)
bool can_cancel_transmit(CAN_HandleTypeDef *hcan, int index);

// Get the lowest index of a buffer in FAILED or SUCCEEDED state, or -1 if none exists
// Calling this function also re-enables the TX done IRQ for this peripheral
int can_get_transmit_finished(CAN_HandleTypeDef *hcan, bool *is_success);

// Disable all CAN receive interrupts for a FIFO
void can_disable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo);

// Enable CAN receive interrupts for a FIFO.
//
// Interrupt for CAN_INT_MESSAGE_RECEIVED is only enabled if enable_msg_received is set.
void can_enable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, bool enable_msg_received);

// Disable all pending TX interrupts (ahead of restart or deinit). Will re-enable n next transmit
void can_disable_tx_interrupts(CAN_HandleTypeDef *can);

can_state_t can_get_state(CAN_HandleTypeDef *can);

void can_get_counters(CAN_HandleTypeDef *can, can_counters_t *counters);

// Restart controller (clears error states). Caller expected to check controller initialised already.
void can_restart(CAN_HandleTypeDef *can);

// Implemented in pyb_can.c, called from lower layer
extern void pyb_can_irq_handler(uint can_id,  can_int_t interrupt, can_rx_fifo_t fifo);

// Implemented in machine_can.c, called from lower layer
extern void machine_can_irq_handler(uint can_id,  can_int_t interrupt);

#if MICROPY_HW_ENABLE_FDCAN

static inline unsigned can_is_rx_pending(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    return HAL_FDCAN_GetRxFifoFillLevel(can, fifo == CAN_RX_FIFO0 ? FDCAN_RX_FIFO0 : FDCAN_RX_FIFO1);
}

#else

static inline unsigned can_is_rx_pending(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    return __HAL_CAN_MSG_PENDING(can, fifo == CAN_RX_FIFO0 ? CAN_FIFO0 : CAN_FIFO1);
}

#endif // MICROPY_HW_ENABLE_FDCAN

#endif // MICROPY_HW_ENABLE_CAN

#endif // MICROPY_INCLUDED_STM32_CAN_H
