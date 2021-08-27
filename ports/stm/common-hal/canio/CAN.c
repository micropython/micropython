/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#include "common-hal/canio/CAN.h"
#include "peripherals/periph.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

STATIC bool reserved_can[MP_ARRAY_SIZE(mcu_can_banks)];

STATIC const mcu_periph_obj_t *find_pin_function(const mcu_periph_obj_t *table, size_t sz, const mcu_pin_obj_t *pin, int periph_index) {
    for (size_t i = 0; i < sz; i++, table++) {
        if (periph_index != -1 && periph_index != table->periph_index) {
            continue;
        }
        if (pin == table->pin) {
            return table;
        }
    }
    return NULL;
}


void common_hal_canio_can_construct(canio_can_obj_t *self, mcu_pin_obj_t *tx, mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent) {
#define DIV_ROUND(a, b) (((a) + (b) / 2) / (b))
#define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))

    const uint8_t can_tx_len = MP_ARRAY_SIZE(mcu_can_tx_list);
    const uint8_t can_rx_len = MP_ARRAY_SIZE(mcu_can_rx_list);

    const mcu_periph_obj_t *mcu_tx = find_pin_function(mcu_can_tx_list, can_tx_len, tx, -1);
    if (!mcu_tx) {
        mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_tx);
    }
    int periph_index = mcu_tx->periph_index;

    const mcu_periph_obj_t *mcu_rx = find_pin_function(mcu_can_rx_list, can_rx_len, rx, periph_index);
    if (!mcu_rx) {
        mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_rx);
    }

    if (reserved_can[periph_index]) {
        mp_raise_ValueError(translate("Hardware busy, try alternative pins"));
    }

    const uint32_t can_frequency = 42000000;
    uint32_t clocks_per_bit = DIV_ROUND(can_frequency, baudrate);
    uint32_t clocks_to_sample = DIV_ROUND(clocks_per_bit * 7, 8);
    uint32_t clocks_after_sample = clocks_per_bit - clocks_to_sample;
    uint32_t divisor = MAX(DIV_ROUND_UP(clocks_to_sample, 16), DIV_ROUND_UP(clocks_after_sample, 8));
    const uint32_t sjw = 3;

    uint32_t tq_per_bit = DIV_ROUND(clocks_per_bit, divisor);
    uint32_t tq_to_sample = DIV_ROUND(clocks_to_sample, divisor);
    uint32_t tq_after_sample = tq_per_bit - tq_to_sample;

    if (divisor > 1023) {
        mp_raise_OSError(MP_EINVAL); // baudrate cannot be attained (16kHz or something is lower bound, should never happen)
    }

    {
        GPIO_InitTypeDef GPIO_InitStruct = {
            .Pin = pin_mask(tx->number),
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_PULLUP,
            .Alternate = mcu_tx->altfn_index,
        };
        HAL_GPIO_Init(pin_port(tx->port), &GPIO_InitStruct);

        GPIO_InitStruct.Pin = pin_mask(rx->number);
        GPIO_InitStruct.Alternate = mcu_rx->altfn_index;
        HAL_GPIO_Init(pin_port(rx->port), &GPIO_InitStruct);
    }

    CAN_TypeDef *hw = mcu_can_banks[periph_index - 1];

    // CAN2 shares resources with CAN1.  So we always enable CAN1, then split
    // the filter banks equally between them.

    __HAL_RCC_CAN1_CLK_ENABLE();

    if (hw == CAN2) {
        __HAL_RCC_CAN2_CLK_ENABLE();
        self->start_filter_bank = 14;
        self->end_filter_bank = 28;
        self->filter_hw = CAN1;
    } else {
        self->start_filter_bank = 0;
        self->end_filter_bank = 14;
        self->filter_hw = hw;
    }

    CAN_InitTypeDef init = {
        .AutoRetransmission = ENABLE,
        .AutoBusOff = ENABLE,
        .Prescaler = divisor,
        .Mode = (loopback ? CAN_MODE_LOOPBACK : 0) | (silent ? CAN_MODE_SILENT_LOOPBACK : 0),
        .SyncJumpWidth = (sjw - 1) << CAN_BTR_SJW_Pos,
            .TimeSeg1 = (tq_to_sample - 2) << CAN_BTR_TS1_Pos,
            .TimeSeg2 = (tq_after_sample - 1) << CAN_BTR_TS2_Pos,
    };

    self->periph_index = periph_index;
    self->silent = silent;
    self->loopback = loopback;
    self->baudrate = baudrate;

    self->handle.Instance = hw;
    self->handle.Init = init;
    self->handle.State = HAL_CAN_STATE_RESET;

    HAL_CAN_Init(&self->handle);

    // Set the filter split as 14:14
    // COULDDO(@jepler): Dynamically allocate filter banks between CAN1/2
    self->filter_hw->FMR |= CAN_FMR_FINIT;
    self->filter_hw->FMR = CAN_FMR_FINIT | (14 << CAN_FMR_CAN2SB_Pos);

    // Clear every filter enable bit for this can HW
    uint32_t fa1r = self->filter_hw->FA1R;
    for (int i = self->start_filter_bank; i < self->end_filter_bank; i++) {
        fa1r &= ~(1 << i);
    }
    self->filter_hw->FA1R = fa1r;
    CLEAR_BIT(self->filter_hw->FMR, CAN_FMR_FINIT);

    HAL_CAN_Start(&self->handle);

    reserved_can[periph_index] = true;
}

bool common_hal_canio_can_loopback_get(canio_can_obj_t *self) {
    return self->loopback;
}

int common_hal_canio_can_baudrate_get(canio_can_obj_t *self) {
    return self->baudrate;
}

int common_hal_canio_can_transmit_error_count_get(canio_can_obj_t *self) {
    return (self->handle.Instance->ESR & CAN_ESR_TEC) >> CAN_ESR_TEC_Pos;
}

int common_hal_canio_can_receive_error_count_get(canio_can_obj_t *self) {
    return (self->handle.Instance->ESR & CAN_ESR_REC) >> CAN_ESR_REC_Pos;
}

canio_bus_state_t common_hal_canio_can_state_get(canio_can_obj_t *self) {
    uint32_t esr = self->handle.Instance->ESR;
    if (READ_BIT(esr, CAN_ESR_BOFF)) {
        return BUS_STATE_OFF;
    }
    if (READ_BIT(esr, CAN_ESR_EPVF)) {
        return BUS_STATE_ERROR_PASSIVE;
    }
    if (READ_BIT(esr, CAN_ESR_EWGF)) {
        return BUS_STATE_ERROR_WARNING;
    }
    return BUS_STATE_ERROR_ACTIVE;
}

void common_hal_canio_can_restart(canio_can_obj_t *self) {
    if (!common_hal_canio_can_auto_restart_get(self)) {
        // "If ABOM is cleared, the software must initiate the recovering
        // sequence by requesting bxCAN to enter and to leave initialization
        // mode."
        self->handle.Instance->MCR |= CAN_MCR_INRQ;
        while ((self->handle.Instance->MSR & CAN_MSR_INAK) == 0) {
        }
        self->handle.Instance->MCR &= ~CAN_MCR_INRQ;
        while ((self->handle.Instance->MSR & CAN_MSR_INAK)) {
        }
    }
}

bool common_hal_canio_can_auto_restart_get(canio_can_obj_t *self) {
    return READ_BIT(self->handle.Instance->MCR, CAN_MCR_ABOM);
}

void common_hal_canio_can_auto_restart_set(canio_can_obj_t *self, bool value) {
    if (value) {
        SET_BIT(self->handle.Instance->MCR, CAN_MCR_ABOM);
    } else {
        CLEAR_BIT(self->handle.Instance->MCR, CAN_MCR_ABOM);
    }
}

void common_hal_canio_can_send(canio_can_obj_t *self, mp_obj_t message_in) {
    canio_message_obj_t *message = message_in;
    uint32_t mailbox;
    bool rtr = message->base.type == &canio_remote_transmission_request_type;
    CAN_TxHeaderTypeDef header = {
        .StdId = message->id,
        .ExtId = message->id,
        .IDE = message->extended ? CAN_ID_EXT : CAN_ID_STD,
        .RTR = rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA,
        .DLC = message->size,
    };
    uint32_t free_level = HAL_CAN_GetTxMailboxesFreeLevel(&self->handle);
    if (free_level == 0) {
        // There's no free Tx mailbox.  We need to cancel some message without
        // transmitting it, because once the bus returns to active state it's
        // preferable to transmit the newest messages instead of older messages.
        //
        // We don't strictly guarantee that we abort the oldest Tx request,
        // rather we just abort a different index each time.  This permits us
        // to just track a single cancel index
        HAL_CAN_AbortTxRequest(&self->handle, 1 << (self->cancel_mailbox));
        self->cancel_mailbox = (self->cancel_mailbox + 1) % 3;
        // The abort request may not have completed immediately, so wait for
        // the Tx mailbox to become free
        do {
            free_level = HAL_CAN_GetTxMailboxesFreeLevel(&self->handle);
        } while (!free_level);
    }
    HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(&self->handle, &header, message->data, &mailbox);
    if (status != HAL_OK) {
        // this is a "shouldn't happen" condition.  we don't throw because the
        // contract of send() is that it queues the packet to be sent if
        // possible and does not signal success or failure to actually send.
        return;
    }

    // wait 8ms (hard coded for now) for TX to occur
    uint64_t deadline = port_get_raw_ticks(NULL) + 8;
    while (port_get_raw_ticks(NULL) < deadline && HAL_CAN_IsTxMessagePending(&self->handle, 1 << mailbox)) {
        RUN_BACKGROUND_TASKS;
    }
}

bool common_hal_canio_can_silent_get(canio_can_obj_t *self) {
    return self->silent;
}

bool common_hal_canio_can_deinited(canio_can_obj_t *self) {
    return !self->handle.Instance;
}

void common_hal_canio_can_check_for_deinit(canio_can_obj_t *self) {
    if (common_hal_canio_can_deinited(self)) {
        raise_deinited_error();
    }
}

void common_hal_canio_can_deinit(canio_can_obj_t *self) {
    if (self->handle.Instance) {
        SET_BIT(self->handle.Instance->MCR, CAN_MCR_RESET);
        while (READ_BIT(self->handle.Instance->MCR, CAN_MCR_RESET)) {
        }
        reserved_can[self->periph_index] = 0;
    }
    self->handle.Instance = NULL;
}

void common_hal_canio_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_can_banks); i++) {
        SET_BIT(mcu_can_banks[i]->MCR, CAN_MCR_RESET);
        reserved_can[i] = 0;
    }
}
