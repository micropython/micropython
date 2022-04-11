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

#define FDCAN_RX_FIFO0_MASK (FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST | FDCAN_FLAG_RX_FIFO0_FULL | FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE)
#define FDCAN_RX_FIFO1_MASK (FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST | FDCAN_FLAG_RX_FIFO1_FULL | FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE)
#define FDCAN_ERROR_STATUS_MASK (FDCAN_FLAG_ERROR_PASSIVE | FDCAN_FLAG_ERROR_WARNING | FDCAN_FLAG_BUS_OFF)

#if defined(STM32H7)
// adaptations for H7 to G4 naming convention in HAL
#define FDCAN_IT_GROUP_RX_FIFO0         (FDCAN_ILS_RF0NL | FDCAN_ILS_RF0FL | FDCAN_ILS_RF0LL)
#define FDCAN_IT_GROUP_BIT_LINE_ERROR   (FDCAN_ILS_EPE | FDCAN_ILS_ELOE)
#define FDCAN_IT_GROUP_PROTOCOL_ERROR   (FDCAN_ILS_ARAE | FDCAN_ILS_PEDE | FDCAN_ILS_PEAE | FDCAN_ILS_WDIE | FDCAN_ILS_BOE | FDCAN_ILS_EWE)
#define FDCAN_IT_GROUP_RX_FIFO1         (FDCAN_ILS_RF1NL | FDCAN_ILS_RF1FL | FDCAN_ILS_RF1LL)
#endif

// The dedicated Message RAM should be 2560 words, but the way it's defined in stm32h7xx_hal_fdcan.c
// as (SRAMCAN_BASE + FDCAN_MESSAGE_RAM_SIZE - 0x4U) limits the usable number of words to 2559 words.
#define FDCAN_MESSAGE_RAM_SIZE  (2560 - 1)

// also defined in <PROC>_hal_fdcan.c, but not able to declare extern and reach the variable
const uint8_t DLCtoBytes[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

bool can_init(pyb_can_obj_t *can_obj, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart) {
    (void)auto_restart;

    FDCAN_InitTypeDef *init = &can_obj->can.Init;
    // Configure FDCAN with FD frame and BRS support.
    init->FrameFormat = FDCAN_FRAME_FD_BRS;
    init->Mode = mode;

    init->NominalPrescaler = prescaler; // tq = NominalPrescaler x (1/fdcan_ker_ck)
    init->NominalSyncJumpWidth = sjw;
    init->NominalTimeSeg1 = bs1; // NominalTimeSeg1 = Propagation_segment + Phase_segment_1
    init->NominalTimeSeg2 = bs2;

    init->AutoRetransmission = ENABLE;
    init->TransmitPause = DISABLE;
    init->ProtocolException = ENABLE;

    #if defined(STM32G4)
    init->ClockDivider = FDCAN_CLOCK_DIV1;
    init->DataPrescaler = 1;
    init->DataSyncJumpWidth = 1;
    init->DataTimeSeg1 = 1;
    init->DataTimeSeg2 = 1;
    init->StdFiltersNbr = 28; // /2  ? if FDCAN2 is used !!?
    init->ExtFiltersNbr = 0; // Not used
    init->TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    #elif defined(STM32H7)
    // The dedicated FDCAN RAM is 2560 32-bit words and shared between the FDCAN instances.
    // To support 2 FDCAN instances simultaneously, the Message RAM is divided in half by
    // setting the second FDCAN memory offset to half the RAM size. With this configuration,
    // the maximum words per FDCAN instance is 1280 32-bit words.
    if (can_obj->can_id == PYB_CAN_1) {
        init->MessageRAMOffset = 0;
    } else {
        init->MessageRAMOffset = FDCAN_MESSAGE_RAM_SIZE / 2;
    }
    // An element stored in the Message RAM contains an identifier, DLC, control bits, the
    // data field and the specific transmission or reception bits field for control.
    // The following code configures the different Message RAM sections per FDCAN instance.

    // The RAM filtering section is configured for 64 x 1 word elements for 11-bit standard
    // identifiers, and 31 x 2 words elements for 29-bit extended identifiers.
    // The total number of words reserved for the filtering per FDCAN instance is 126 words.
    init->StdFiltersNbr = 64;
    init->ExtFiltersNbr = 31;

    // The Tx event FIFO is used to store the message ID and the timestamp of successfully
    // transmitted elements. The Tx event FIFO can store a maximum of 32 (2 words) elements.
    // NOTE: Events are stored in Tx event FIFO only if tx_msg.TxEventFifoControl is enabled.
    init->TxEventsNbr = 0;

    // Transmission section is configured in FIFO mode operation, with no dedicated Tx buffers.
    // The Tx FIFO can store a maximum of 32 elements (or 576 words), each element is 18 words
    // long (to support a maximum of 64 bytes data field):
    //  2 words header + 16 words data field (to support up to 64 bytes of data).
    // The total number of words reserved for the Tx FIFO per FDCAN instance is 288 words.
    init->TxBuffersNbr = 0;
    init->TxFifoQueueElmtsNbr = 16;
    init->TxElmtSize = FDCAN_DATA_BYTES_64;
    init->TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

    // Reception section is configured to use Rx FIFO 0 and Rx FIFO1, with no dedicated Rx buffers.
    // Each Rx FIFO can store a maximum of 64 elements (1152 words), each element is 18 words
    // long (to support a maximum of 64 bytes data field):
    //  2 words header + 16 words data field (to support up to 64 bytes of data).
    // The total number of words reserved for the Rx FIFOs per FDCAN instance is 864 words.
    init->RxBuffersNbr = 0;
    init->RxFifo0ElmtsNbr = 24;
    init->RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
    init->RxFifo1ElmtsNbr = 24;
    init->RxFifo1ElmtSize = FDCAN_DATA_BYTES_64;
    #endif

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
    // catch bad configuration errors.
    if (HAL_FDCAN_Init(&can_obj->can) != HAL_OK) {
        return false;
    }

    // Disable acceptance of non-matching frames (enabled by default)
    HAL_FDCAN_ConfigGlobalFilter(&can_obj->can, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);

    // The configuration registers are locked after CAN is started.
    HAL_FDCAN_Start(&can_obj->can);

    // Reset all filters
    for (int f = 0; f < init->StdFiltersNbr; ++f) {
        can_clearfilter(can_obj, f, false);
    }

    for (int f = 0; f < init->ExtFiltersNbr; ++f) {
        can_clearfilter(can_obj, f, true);
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
        #if defined(MICROPY_HW_CAN2_TX)
        case PYB_CAN_2:
            NVIC_SetPriority(FDCAN2_IT0_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
            NVIC_SetPriority(FDCAN2_IT1_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
            break;
        #endif
        default:
            return false;
    }
    // FDCAN IT 0
    HAL_FDCAN_ConfigInterruptLines(&can_obj->can, FDCAN_IT_GROUP_RX_FIFO0 | FDCAN_IT_GROUP_BIT_LINE_ERROR | FDCAN_IT_GROUP_PROTOCOL_ERROR, FDCAN_INTERRUPT_LINE0);
    // FDCAN IT 1
    HAL_FDCAN_ConfigInterruptLines(&can_obj->can, FDCAN_IT_GROUP_RX_FIFO1, FDCAN_INTERRUPT_LINE1);

    uint32_t ActiveITs = FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE;
    ActiveITs |= FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE;
    ActiveITs |= FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_MESSAGE_LOST;
    ActiveITs |= FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO1_FULL;
    HAL_FDCAN_ActivateNotification(&can_obj->can, ActiveITs, 0);
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

void can_clearfilter(pyb_can_obj_t *self, uint32_t f, uint8_t extid) {
    if (self && self->can.Instance) {
        FDCAN_FilterTypeDef filter = {0};
        if (extid == 1) {
            filter.IdType = FDCAN_EXTENDED_ID;
        } else {
            filter.IdType = FDCAN_STANDARD_ID;
        }
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
        #if defined(STM32G4)
        address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * (18U * 4U)));  // SRAMCAN_RF0_SIZE bytes, size not configurable
        #else
        address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * can->Init.RxFifo0ElmtSize * 4));
        #endif
    } else {
        index = (*rxf & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
        #if defined(STM32G4)
        // ToDo: test FIFO1, FIFO 0 is ok
        address = (uint32_t *)(can->msgRam.RxFIFO1SA + (index * (18U * 4U)));  // SRAMCAN_RF1_SIZE bytes, size not configurable
        #else
        address = (uint32_t *)(can->msgRam.RxFIFO1SA + (index * can->Init.RxFifo1ElmtSize * 4));
        #endif
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
    // Convert DLC to Bytes.
    hdr->DataLength = DLCtoBytes[hdr->DataLength];

    // Copy data
    uint8_t *pdata = (uint8_t *)address;
    for (uint32_t i = 0; i < hdr->DataLength; ++i) {
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

    CAN_TypeDef *can = self->can.Instance;

    uint32_t RxFifo0ITs;
    uint32_t RxFifo1ITs;
    // uint32_t Errors;
    uint32_t ErrorStatusITs;
    uint32_t Psr;

    RxFifo0ITs = can->IR & FDCAN_RX_FIFO0_MASK;
    RxFifo0ITs &= can->IE;
    RxFifo1ITs = can->IR & FDCAN_RX_FIFO1_MASK;
    RxFifo1ITs &= can->IE;
    // Errors = (&self->can)->Instance->IR & FDCAN_ERROR_MASK;
    // Errors &= (&self->can)->Instance->IE;
    ErrorStatusITs = can->IR & FDCAN_ERROR_STATUS_MASK;
    ErrorStatusITs &= can->IE;
    Psr = can->PSR;

    if (fifo_id == FDCAN_RX_FIFO0) {
        callback = self->rxcallback0;
        state = &self->rx_state0;
        if (RxFifo0ITs & FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE);
            irq_reason = MP_OBJ_NEW_SMALL_INT(0);
            *state = RX_STATE_MESSAGE_PENDING;

        }
        if (RxFifo0ITs & FDCAN_FLAG_RX_FIFO0_FULL) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO0_FULL);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO0_FULL);
            irq_reason = MP_OBJ_NEW_SMALL_INT(1);
            *state = RX_STATE_FIFO_FULL;

        }
        if (RxFifo0ITs & FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO0_MESSAGE_LOST);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST);
            irq_reason = MP_OBJ_NEW_SMALL_INT(2);
            *state = RX_STATE_FIFO_OVERFLOW;
        }

    } else {
        callback = self->rxcallback1;
        state = &self->rx_state1;
        if (RxFifo1ITs & FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE);
            irq_reason = MP_OBJ_NEW_SMALL_INT(0);
            *state = RX_STATE_MESSAGE_PENDING;

        }
        if (RxFifo1ITs & FDCAN_FLAG_RX_FIFO1_FULL) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO1_FULL);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO1_FULL);
            irq_reason = MP_OBJ_NEW_SMALL_INT(1);
            *state = RX_STATE_FIFO_FULL;

        }
        if (RxFifo1ITs & FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST) {
            __HAL_FDCAN_DISABLE_IT(&self->can, FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
            __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST);
            irq_reason = MP_OBJ_NEW_SMALL_INT(2);
            *state = RX_STATE_FIFO_OVERFLOW;
        }
    }

    if (ErrorStatusITs & FDCAN_FLAG_ERROR_WARNING) {
        __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_ERROR_WARNING);
        if (Psr & FDCAN_PSR_EW) {
            irq_reason = MP_OBJ_NEW_SMALL_INT(3);
            // mp_printf(MICROPY_ERROR_PRINTER, "clear warning %08x\n", (can->IR & FDCAN_ERROR_STATUS_MASK));
        }
    }
    if (ErrorStatusITs & FDCAN_FLAG_ERROR_PASSIVE) {
        __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_ERROR_PASSIVE);
        if (Psr & FDCAN_PSR_EP) {
            irq_reason = MP_OBJ_NEW_SMALL_INT(4);
            // mp_printf(MICROPY_ERROR_PRINTER, "clear passive %08x\n", (can->IR & FDCAN_ERROR_STATUS_MASK));
        }
    }
    if (ErrorStatusITs & FDCAN_FLAG_BUS_OFF) {
        __HAL_FDCAN_CLEAR_FLAG(&self->can, FDCAN_FLAG_BUS_OFF);
        if (Psr & FDCAN_PSR_BO) {
            irq_reason = MP_OBJ_NEW_SMALL_INT(5);
            // mp_printf(MICROPY_ERROR_PRINTER, "bus off %08x\n", (can->IR & FDCAN_ERROR_STATUS_MASK));
        }
    }

    pyb_can_handle_callback(self, fifo_id, callback, irq_reason);
    // mp_printf(MICROPY_ERROR_PRINTER, "Ints: %08x, %08x, %08x\n", RxFifo0ITs, RxFifo1ITs, ErrorStatusITs);
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
