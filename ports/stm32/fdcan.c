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

#define FDCAN_IT_RX_FIFO0_MASK (FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
#define FDCAN_IT_RX_FIFO1_MASK (FDCAN_IT_RX_FIFO1_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_FULL | FDCAN_IT_RX_FIFO1_NEW_MESSAGE)
#define FDCAN_IT_ERROR_STATUS_MASK (FDCAN_IT_ERROR_PASSIVE | FDCAN_IT_ERROR_WARNING | FDCAN_IT_BUS_OFF)

#define FDCAN_IT_RX_NEW_MESSAGE_MASK (FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE)
#define FDCAN_IT_RX_FULL_MASK (FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO1_FULL)
#define FDCAN_IT_RX_MESSAGE_LOST_MASK (FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_MESSAGE_LOST)

#if defined(STM32H7)
// adaptations for H7 to G4 naming convention in HAL
#define FDCAN_IT_GROUP_RX_FIFO0         (FDCAN_ILS_RF0NL | FDCAN_ILS_RF0FL | FDCAN_ILS_RF0LL)
#define FDCAN_IT_GROUP_BIT_LINE_ERROR   (FDCAN_ILS_EPE | FDCAN_ILS_ELOE)
#define FDCAN_IT_GROUP_PROTOCOL_ERROR   (FDCAN_ILS_ARAE | FDCAN_ILS_PEDE | FDCAN_ILS_PEAE | FDCAN_ILS_WDIE | FDCAN_ILS_BOE | FDCAN_ILS_EWE)
#define FDCAN_IT_GROUP_RX_FIFO1         (FDCAN_ILS_RF1NL | FDCAN_ILS_RF1FL | FDCAN_ILS_RF1LL)

// The dedicated Message RAM should be 2560 words, but the way it's defined in stm32h7xx_hal_fdcan.c
// as (SRAMCAN_BASE + FDCAN_MESSAGE_RAM_SIZE - 0x4U) limits the usable number of words to 2559 words.
#define FDCAN_MESSAGE_RAM_SIZE  (2560 - 1)
#endif // STM32H7

#if defined(STM32G4)
// These HAL APIs are not implemented for STM32G4, so we implement them here...
static HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxBuffer(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData, uint32_t BufferIndex);
static HAL_StatusTypeDef HAL_FDCAN_EnableTxBufferRequest(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndex);
#endif // STM32G4

// also defined in <PROC>_hal_fdcan.c, but not able to declare extern and reach the variable
static const uint8_t DLCtoBytes[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

#if defined(MICROPY_HW_CAN3_TX)
#error "Support is not yet added for FDCAN CAN3"
#elif defined(MICROPY_HW_CAN2_TX)
#define NUM_CAN 2
#else
#define NUM_CAN 1
#endif

int can_get_transmit_finished(CAN_HandleTypeDef *hcan, bool *is_success) {
    // Note: No HAL API available for the below regs, unless we use the HAL's IRQ handler
    FDCAN_GlobalTypeDef *instance = hcan->Instance;

    uint32_t enabled_tx = instance->TXBTIE; // Which buffers have TX ints enabled?
    uint32_t tx_success = instance->TXBTO & enabled_tx; // Which TX buffers succeeded?
    uint32_t tx_cancel = instance->TXBCF & enabled_tx; // Which TX buffers cancelled?
    int result = -1;

    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        if (tx_success & (1U << i)) {
            *is_success = true;
            result = i;
            break;
        }
        if (tx_cancel & (1U << i)) {
            *is_success = false;
            result = i;
            break;
        }
    }

    if (result != -1) {
        // Clear the TX interrupts for this buffer, will re-enable
        // when next sending
        instance->TXBTIE &= ~(1U << result);
        instance->TXBCIE &= ~(1U << result);
    }

    // Re-enable transmit interrupts
    instance->IE |= (FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_ABORT_COMPLETE);

    return result;
}

bool can_init(CAN_HandleTypeDef *can, int can_id, can_tx_mode_t tx_mode, uint32_t mode, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2, bool auto_restart) {
    uint32_t fifo_queue_mode = (tx_mode == CAN_TX_FIFO) ? FDCAN_TX_FIFO_OPERATION : FDCAN_TX_QUEUE_OPERATION;

    FDCAN_InitTypeDef *init = &can->Init;
    // Configure FDCAN with FD frame and BRS support.
    init->FrameFormat = FDCAN_FRAME_FD_BRS;
    init->Mode = mode;

    init->NominalPrescaler = prescaler; // tq = NominalPrescaler x (1/fdcan_ker_ck)
    init->NominalSyncJumpWidth = sjw;
    init->NominalTimeSeg1 = bs1; // NominalTimeSeg1 = Propagation_segment + Phase_segment_1
    init->NominalTimeSeg2 = bs2;

    init->AutoRetransmission = auto_restart ? ENABLE : DISABLE;
    init->TransmitPause = DISABLE;
    init->ProtocolException = ENABLE;

    init->StdFiltersNbr = CAN_HW_MAX_STD_FILTER;
    init->ExtFiltersNbr = CAN_HW_MAX_EXT_FILTER;

    #if defined(STM32G4)
    init->ClockDivider = FDCAN_CLOCK_DIV1;
    init->DataPrescaler = 1;
    init->DataSyncJumpWidth = 1;
    init->DataTimeSeg1 = 1;
    init->DataTimeSeg2 = 1;
    init->TxFifoQueueMode = fifo_queue_mode;
    #elif defined(STM32H7)
    // The dedicated FDCAN RAM is 2560 32-bit words and shared between the FDCAN instances.
    // To support 2 FDCAN instances simultaneously, the Message RAM is divided in half by
    // setting the second FDCAN memory offset to half the RAM size. With this configuration,
    // the maximum words per FDCAN instance is 1280 32-bit words.
    if (can_id == PYB_CAN_1) {
        init->MessageRAMOffset = 0;
    } else {
        init->MessageRAMOffset = FDCAN_MESSAGE_RAM_SIZE / 2;
    }
    // An element stored in the Message RAM contains an identifier, DLC, control bits, the
    // data field and the specific transmission or reception bits field for control.
    // The following code configures the different Message RAM sections per FDCAN instance.

    // The Tx event FIFO is used to store the message ID and the timestamp of successfully
    // transmitted elements. The Tx event FIFO can store a maximum of 32 (2 words) elements.
    // NOTE: Events are stored in Tx event FIFO only if tx_msg.TxEventFifoControl is enabled.
    init->TxEventsNbr = 0;

    // The Tx FIFO or Queue can store a maximum of 32 elements (or 576 words), each element is 18 words
    // long (to support a maximum of 64 bytes data field):
    // 2 words header + 16 words data field (to support up to 64 bytes of data).
    // The total number of words reserved for the Tx FIFO per FDCAN instance is 288 words.
    if (tx_mode == CAN_TX_FIFO) {
        init->TxBuffersNbr = 0;
        init->TxFifoQueueElmtsNbr = CAN_TX_QUEUE_LEN;
    } else {
        init->TxBuffersNbr = CAN_TX_QUEUE_LEN;
        init->TxFifoQueueElmtsNbr = 0;
    }
    init->TxFifoQueueMode = fifo_queue_mode;
    init->TxElmtSize = FDCAN_DATA_BYTES_64;

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
    #endif // STM32H7

    const machine_pin_obj_t *pins[2];

    switch (can_id) {
        #if defined(MICROPY_HW_CAN1_TX)
        case PYB_CAN_1:
            can->Instance = FDCAN1;
            pins[0] = MICROPY_HW_CAN1_TX;
            pins[1] = MICROPY_HW_CAN1_RX;
            break;
        #endif

        #if defined(MICROPY_HW_CAN2_TX)
        case PYB_CAN_2:
            can->Instance = FDCAN2;
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
        if (!mp_hal_pin_config_alt(pins[i], pin_mode, pin_pull, AF_FN_CAN, can_id)) {
            return false;
        }
    }

    // initialise hardware, catching bad configuration errors.
    if (HAL_FDCAN_Init(can) != HAL_OK) {
        return false;
    }

    // Disable acceptance of non-matching frames (enabled by default)
    HAL_FDCAN_ConfigGlobalFilter(can, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);

    // The configuration registers are locked after CAN is started.
    if (HAL_FDCAN_Start(can) != HAL_OK) {
        return false;
    }

    // Reset all filters
    for (int f = 0; f < init->StdFiltersNbr; ++f) {
        can_clearfilter(can, f, false);
    }

    for (int f = 0; f < init->ExtFiltersNbr; ++f) {
        can_clearfilter(can, f, true);
    }

    switch (can_id) {
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
    HAL_FDCAN_ConfigInterruptLines(can, FDCAN_IT_GROUP_RX_FIFO0 | FDCAN_IT_GROUP_BIT_LINE_ERROR | FDCAN_IT_GROUP_PROTOCOL_ERROR, FDCAN_INTERRUPT_LINE0);
    // FDCAN IT 1
    HAL_FDCAN_ConfigInterruptLines(can, FDCAN_IT_GROUP_RX_FIFO1, FDCAN_INTERRUPT_LINE1);

    // Enable error interrupts for all queue positions.
    // (RX-related interrupts are enabled via can_enable_rx_interrupts(),
    // and TX-related interrupts are enabled during TX
    HAL_FDCAN_ActivateNotification(can,
        FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE, 0);
    return true;
}

void can_deinit(FDCAN_HandleTypeDef *can) {
    HAL_FDCAN_DeInit(can);
    if (can->Instance == FDCAN1) {
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
        // TODO: check this logic if FDCAN2 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #if defined(MICROPY_HW_CAN2_TX)
    } else if (can->Instance == FDCAN2) {
        HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
        // TODO: check this logic if FDCAN1 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #endif
    }
}

uint32_t can_get_source_freq(void) {
    // Find CAN kernel clock
    #if defined(STM32H7)
    switch (__HAL_RCC_GET_FDCAN_SOURCE()) {
        case RCC_FDCANCLKSOURCE_HSE:
            return HSE_VALUE;
        case RCC_FDCANCLKSOURCE_PLL: {
            PLL1_ClocksTypeDef pll1_clocks;
            HAL_RCCEx_GetPLL1ClockFreq(&pll1_clocks);
            return pll1_clocks.PLL1_Q_Frequency;
        }
        case RCC_FDCANCLKSOURCE_PLL2: {
            PLL2_ClocksTypeDef pll2_clocks;
            HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
            return pll2_clocks.PLL2_Q_Frequency;
        }
        default:
            abort(); // Should be unreachable, macro should return one of the above
    }
    #elif defined(STM32G4)
    // STM32G4 CAN clock from reset is HSE, unchanged by MicroPython
    return HSE_VALUE;
    #else // G0, and assume other MCUs too.
    // CAN1/CAN2/CAN3 on APB1 use GetPCLK1Freq, alternatively use the following:
    // can_kern_clk = ((HSE_VALUE / osc_config.PLL.PLLM ) * osc_config.PLL.PLLN) /
    //  (osc_config.PLL.PLLQ * clk_init.AHBCLKDivider * clk_init.APB1CLKDivider);
    return HAL_RCC_GetPCLK1Freq();
    #endif
}


void can_clearfilter(FDCAN_HandleTypeDef *can, uint32_t f, bool ext_id) {
    FDCAN_FilterTypeDef filter = {0};
    if (ext_id) {
        filter.IdType = FDCAN_EXTENDED_ID;
    } else {
        filter.IdType = FDCAN_STANDARD_ID;
    }
    filter.FilterIndex = f;
    filter.FilterConfig = FDCAN_FILTER_DISABLE;
    HAL_FDCAN_ConfigFilter(can, &filter);
}

void can_disable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo) {
    HAL_FDCAN_DeactivateNotification(can, (fifo == CAN_RX_FIFO0) ? FDCAN_IT_RX_FIFO0_MASK : FDCAN_IT_RX_FIFO1_MASK);
}

void can_enable_rx_interrupts(CAN_HandleTypeDef *can, can_rx_fifo_t fifo, bool enable_msg_received) {
    uint32_t ints = (fifo == CAN_RX_FIFO0) ? FDCAN_IT_RX_FIFO0_MASK : FDCAN_IT_RX_FIFO1_MASK;
    if (!enable_msg_received) {
        ints &= FDCAN_IT_RX_NEW_MESSAGE_MASK;
    }
    HAL_FDCAN_ActivateNotification(can, ints, 0);
}

// Fixup the DataLength field from a byte count to a valid DLC value index (rounding up)
static void encode_datalength(CanTxMsgTypeDef *txmsg) {
    // Roundup DataLength to next DLC size and encode to DLC.
    size_t len_bytes = txmsg->DataLength;
    for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(DLCtoBytes); i++) {
        if (len_bytes <= DLCtoBytes[i]) {
            txmsg->DataLength = (i << 16);
            return;
        }
    }
    assert(0); // DataLength value is invalid
}

HAL_StatusTypeDef can_transmit(CAN_HandleTypeDef *can, CanTxMsgTypeDef *txmsg, uint8_t *data, uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    while (HAL_FDCAN_GetTxFifoFreeLevel(can) == 0) {
        if (timeout_ms == 0) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        // Check for the Timeout
        if (timeout_ms != HAL_MAX_DELAY) {
            if (HAL_GetTick() - start >= timeout_ms) {
                mp_raise_OSError(MP_ETIMEDOUT);
            }
        }
        mp_event_wait_ms(1);
    }
    // Note: this function doesn't set up TX interrupts, because it's only used by pyb.CAN which
    // doesn't care about this - machine.CAN calls can_transmit_buf_index()
    encode_datalength(txmsg);

    return HAL_FDCAN_AddMessageToTxFifoQ(can, txmsg, data);
}

HAL_StatusTypeDef can_transmit_buf_index(CAN_HandleTypeDef *hcan, int index, CanTxMsgTypeDef *txmsg, const uint8_t *data) {
    uint32_t tx_loc = 1U << index;

    encode_datalength(txmsg);

    HAL_StatusTypeDef err = HAL_FDCAN_ActivateNotification(hcan, FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_ABORT_COMPLETE, tx_loc);
    if (err == HAL_OK) {
        // Note: casting away const from data, the HAL implementation still treats 'data' as const
        err = HAL_FDCAN_AddMessageToTxBuffer(hcan, txmsg, (void *)data, tx_loc);
    }
    if (err == HAL_OK) {
        err = HAL_FDCAN_EnableTxBufferRequest(hcan, tx_loc);
    }
    return err;
}

bool can_cancel_transmit(CAN_HandleTypeDef *hcan, int index) {
    FDCAN_GlobalTypeDef *instance = hcan->Instance;
    bool result = false;

    if (instance->TXBRP & (1U << index)) {
        result = true;
        HAL_StatusTypeDef err = HAL_FDCAN_AbortTxRequest(hcan, 1U << index);
        assert(err == HAL_OK); // Should only fail if controller not started
        if (err != HAL_OK) {
            return false;
        }
        mp_uint_t start = mp_hal_ticks_us();

        // Wait for the TX buffer to be marked as no longer pending
        while ((instance->TXBRP & (1U << index)) != 0) {
            // we don't expect this to take longer than a few clock cycles, if
            // it does then it probably indicates a bug in the driver. However,
            // either way we don't want to end up stuck here
            mp_uint_t elapsed = mp_hal_ticks_us() - start;
            assert(elapsed < 1000);
            if (elapsed >= 1000) {
                break;
            }
        }
    }

    return result;
}

int can_receive(FDCAN_HandleTypeDef *can, can_rx_fifo_t fifo, FDCAN_RxHeaderTypeDef *hdr, uint8_t *data, uint32_t timeout_ms) {
    volatile uint32_t *rxf, *rxa;
    uint32_t fl;

    if (fifo == CAN_RX_FIFO0) {
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
        mp_event_wait_ms(1);
    }

    // Get pointer to incoming message
    uint32_t index, *address;
    if (fifo == CAN_RX_FIFO0) {
        index = (*rxf & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos;
        #if defined(STM32G4)
        address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * (18U * 4U)));  // SRAMCAN_RF0_SIZE bytes, size not configurable
        #else
        address = (uint32_t *)(can->msgRam.RxFIFO0SA + (index * can->Init.RxFifo0ElmtSize * 4));
        #endif
    } else {
        index = (*rxf & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos;
        #if defined(STM32G4)
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

can_state_t can_get_state(CAN_HandleTypeDef *can) {
    uint32_t psr;

    if (can->State != HAL_FDCAN_STATE_BUSY) {
        return CAN_STATE_STOPPED;
    }

    psr = can->Instance->PSR;
    if (psr & FDCAN_PSR_BO) {
        return CAN_STATE_BUS_OFF;
    } else if (psr & FDCAN_PSR_EP) {
        return CAN_STATE_ERROR_PASSIVE;
    } else if (psr & FDCAN_PSR_EW) {
        return CAN_STATE_ERROR_WARNING;
    } else {
        return CAN_STATE_ERROR_ACTIVE;
    }
}

void can_get_counters(CAN_HandleTypeDef *can, can_counters_t *counters) {
    FDCAN_GlobalTypeDef *inst = can->Instance;
    uint32_t esr = inst->ECR;
    counters->tec = (esr & FDCAN_ECR_TEC_Msk) >> FDCAN_ECR_TEC_Pos;
    if (esr & FDCAN_ECR_RP) {
        counters->rec = 128; // "at least 128"
    } else {
        counters->rec = (esr & FDCAN_ECR_REC_Msk) >> FDCAN_ECR_REC_Pos;
    }
    if (can->Init.TxFifoQueueMode == FDCAN_TX_FIFO_OPERATION) {
        counters->tx_pending = inst->TXEFS & 0x7;
    } else {
        counters->tx_pending = mp_popcount(inst->TXBRP);
    }
    counters->rx_fifo0_pending = (inst->RXF0S & FDCAN_RXF0S_F0FL_Msk) >> FDCAN_RXF0S_F0FL_Pos;
    counters->rx_fifo1_pending = (inst->RXF1S & FDCAN_RXF1S_F1FL_Msk) >> FDCAN_RXF1S_F1FL_Pos;
}

void can_disable_tx_interrupts(CAN_HandleTypeDef *can) {
    HAL_FDCAN_DeactivateNotification(can, FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_ABORT_COMPLETE);
}

void can_restart(CAN_HandleTypeDef *can) {
    HAL_FDCAN_Stop(can);
    HAL_FDCAN_Start(can);
}

// Compatibility shim: call both the pyb.CAN and machine.CAN handlers if necessary,
// allow them to decide which is initialised.
static inline void call_can_irq_handler(uint can_id, can_int_t interrupt, can_rx_fifo_t fifo) {
    #if MICROPY_PY_MACHINE_CAN
    machine_can_irq_handler(can_id, interrupt);
    #endif
    if (interrupt != CAN_INT_TX_COMPLETE) {
        pyb_can_irq_handler(can_id, interrupt, fifo);
    }
    // TODO: Need to do something to clear the transmit state if pyb.CAN is in use, I think
    // (check usage of can_get_transmit_finished() from pyb CAN code)
}

static void can_irq_handler(uint can_id, CAN_TypeDef *instance, can_rx_fifo_t fifo) {
    uint32_t ints, rx_fifo_ints, error_ints, tx_complete_int;

    ints = instance->IR & instance->IE;

    if (fifo == CAN_RX_FIFO0) {
        rx_fifo_ints = ints & FDCAN_IT_RX_FIFO0_MASK;
    } else {
        rx_fifo_ints = ints & FDCAN_IT_RX_FIFO1_MASK;
    }
    error_ints = ints & FDCAN_IT_ERROR_STATUS_MASK;

    tx_complete_int = ints & (FDCAN_IT_TX_COMPLETE | FDCAN_IT_TX_ABORT_COMPLETE);

    // Disable receive interrupts, re-enabled by higher layer after calling can_receive()

    // (Note: can't use __HAL_CAN API as only have a CAN_TypeDef, not CAN_HandleTypeDef)
    instance->IE &= ~rx_fifo_ints;
    instance->IR = rx_fifo_ints | error_ints | tx_complete_int;

    if (rx_fifo_ints) {
        if (rx_fifo_ints & FDCAN_IT_RX_FULL_MASK) {
            call_can_irq_handler(can_id, CAN_INT_FIFO_FULL, fifo);
        }
        if (rx_fifo_ints & FDCAN_IT_RX_MESSAGE_LOST_MASK) {
            call_can_irq_handler(can_id, CAN_INT_FIFO_OVERFLOW, fifo);
        }
        if (rx_fifo_ints & FDCAN_IT_RX_NEW_MESSAGE_MASK) {
            call_can_irq_handler(can_id, CAN_INT_MESSAGE_RECEIVED, fifo);
        }
    }

    if (error_ints) {
        uint32_t Psr = instance->PSR;

        if (error_ints & (FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE | FDCAN_IT_BUS_OFF)) {
            if (Psr & FDCAN_PSR_BO) {
                call_can_irq_handler(can_id, CAN_INT_ERR_BUS_OFF, 0);
            } else if (Psr & FDCAN_PSR_EP) {
                call_can_irq_handler(can_id, CAN_INT_ERR_PASSIVE, 0);
            } else if (Psr & FDCAN_PSR_EW) {
                call_can_irq_handler(can_id, CAN_INT_ERR_WARNING, 0);
            }
        }
    }

    if (tx_complete_int) {
        // Disable TX interrupts until we process these ones
        instance->IE &= ~tx_complete_int;
        call_can_irq_handler(can_id, CAN_INT_TX_COMPLETE, 0);
    }
}

#if defined(MICROPY_HW_CAN1_TX)
void FDCAN1_IT0_IRQHandler(void) {
    IRQ_ENTER(FDCAN1_IT0_IRQn);
    can_irq_handler(PYB_CAN_1, FDCAN1, CAN_RX_FIFO0);
    IRQ_EXIT(FDCAN1_IT0_IRQn);
}

void FDCAN1_IT1_IRQHandler(void) {
    IRQ_ENTER(FDCAN1_IT1_IRQn);
    can_irq_handler(PYB_CAN_1, FDCAN1, CAN_RX_FIFO1);
    IRQ_EXIT(FDCAN1_IT1_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN2_TX)
void FDCAN2_IT0_IRQHandler(void) {
    IRQ_ENTER(FDCAN2_IT0_IRQn);
    can_irq_handler(PYB_CAN_2, FDCAN2, CAN_RX_FIFO0);
    IRQ_EXIT(FDCAN2_IT0_IRQn);
}

void FDCAN2_IT1_IRQHandler(void) {
    IRQ_ENTER(FDCAN2_IT1_IRQn);
    can_irq_handler(PYB_CAN_2, FDCAN2, CAN_RX_FIFO1);
    IRQ_EXIT(FDCAN2_IT1_IRQn);
}
#endif

#if defined(STM32G4)
// These implementations are copied from stm32h7xx_hal_fdcan.c with modifications for different G4 registers & code formatting

// *FORMAT-OFF*
// ^^^ Keep original STM HAL code style for easier comparison

static void FDCAN_CopyMessageToRAM(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData, uint32_t BufferIndex);

static HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxBuffer(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData, uint32_t BufferIndex)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_ID_TYPE(pTxHeader->IdType));
  if (pTxHeader->IdType == FDCAN_STANDARD_ID)
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x7FFU));
  }
  else /* pTxHeader->IdType == FDCAN_EXTENDED_ID */
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x1FFFFFFFU));
  }
  assert_param(IS_FDCAN_FRAME_TYPE(pTxHeader->TxFrameType));
  assert_param(IS_FDCAN_DLC(pTxHeader->DataLength));
  assert_param(IS_FDCAN_ESI(pTxHeader->ErrorStateIndicator));
  assert_param(IS_FDCAN_BRS(pTxHeader->BitRateSwitch));
  assert_param(IS_FDCAN_FDF(pTxHeader->FDFormat));
  assert_param(IS_FDCAN_EFC(pTxHeader->TxEventFifoControl));
  assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->MessageMarker, 0xFFU));
  assert_param(IS_FDCAN_TX_LOCATION(BufferIndex));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the selected buffer has an allocated area into the RAM */
    if (POSITION_VAL(BufferIndex) >= CAN_TX_QUEUE_LEN) // Note: Modified for G4 here
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

      return HAL_ERROR;
    }

    /* Check that there is no transmission request pending for the selected buffer */
    if ((hfdcan->Instance->TXBRP & BufferIndex) != 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PENDING;

      return HAL_ERROR;
    }
    else
    {
      /* Add the message to the Tx buffer */
      FDCAN_CopyMessageToRAM(hfdcan, pTxHeader, pTxData, POSITION_VAL(BufferIndex));
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

static HAL_StatusTypeDef HAL_FDCAN_EnableTxBufferRequest(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndex)
{
  if (hfdcan->State == HAL_FDCAN_STATE_BUSY)
  {
    /* Add transmission request */
    hfdcan->Instance->TXBAR = BufferIndex;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

#define SRAMCAN_TFQ_SIZE            (18U * 4U)         /* TX FIFO/Queue Elements Size in bytes  */

// This function is copied 100% as-is from stm32g4xx_hal_fdcan.c, unfortunately
static void FDCAN_CopyMessageToRAM(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData,
                                   uint32_t BufferIndex)
{
  uint32_t TxElementW1;
  uint32_t TxElementW2;
  uint32_t *TxAddress;
  uint32_t ByteCounter;

  /* Build first word of Tx header element */
  if (pTxHeader->IdType == FDCAN_STANDARD_ID)
  {
    TxElementW1 = (pTxHeader->ErrorStateIndicator |
                   FDCAN_STANDARD_ID |
                   pTxHeader->TxFrameType |
                   (pTxHeader->Identifier << 18U));
  }
  else /* pTxHeader->IdType == FDCAN_EXTENDED_ID */
  {
    TxElementW1 = (pTxHeader->ErrorStateIndicator |
                   FDCAN_EXTENDED_ID |
                   pTxHeader->TxFrameType |
                   pTxHeader->Identifier);
  }

  /* Build second word of Tx header element */
  TxElementW2 = ((pTxHeader->MessageMarker << 24U) |
                 pTxHeader->TxEventFifoControl |
                 pTxHeader->FDFormat |
                 pTxHeader->BitRateSwitch |
                 pTxHeader->DataLength);

  /* Calculate Tx element address */
  TxAddress = (uint32_t *)(hfdcan->msgRam.TxFIFOQSA + (BufferIndex * SRAMCAN_TFQ_SIZE));

  /* Write Tx element header to the message RAM */
  *TxAddress = TxElementW1;
  TxAddress++;
  *TxAddress = TxElementW2;
  TxAddress++;

  /* Write Tx payload to the message RAM */
  for (ByteCounter = 0; ByteCounter < DLCtoBytes[pTxHeader->DataLength >> 16U]; ByteCounter += 4U)
  {
    *TxAddress = (((uint32_t)pTxData[ByteCounter + 3U] << 24U) |
                  ((uint32_t)pTxData[ByteCounter + 2U] << 16U) |
                  ((uint32_t)pTxData[ByteCounter + 1U] << 8U)  |
                  (uint32_t)pTxData[ByteCounter]);
    TxAddress++;
  }
}

#endif // STM32G4

// *FORMAT-ON*
#endif // MICROPY_HW_ENABLE_CAN && MICROPY_HW_ENABLE_FDCAN
