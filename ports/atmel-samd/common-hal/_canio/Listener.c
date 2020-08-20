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

#include <math.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/_canio/__init__.h"
#include "common-hal/_canio/Listener.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/tick.h"

void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {
    if (nmatch) {
        mp_raise_NotImplementedError(NULL);
    }

    if (!can->fifo0_in_use) {
        self->fifo_idx = 0;
        self->fifo = can->state->rx0_fifo;
        self->hw = (canio_rxfifo_reg_t*)&can->hw->RXF0C;
        can->hw->IR.reg = CAN_IR_RF0N | CAN_IR_RF0W | CAN_IR_RF0F | CAN_IR_RF0L;
        can->fifo0_in_use = true;
    } else if (!can->fifo1_in_use) {
        self->fifo_idx = 1;
        self->fifo = can->state->rx1_fifo;
        self->hw = (canio_rxfifo_reg_t*)&can->hw->RXF1C;
        can->fifo1_in_use = true;
        can->hw->IR.reg = CAN_IR_RF1N | CAN_IR_RF1W | CAN_IR_RF1F | CAN_IR_RF1L;
    } else {
        mp_raise_ValueError(translate("All RX FIFOs in use"));
    }

    self->can = can;
    common_hal_canio_listener_set_timeout(self, timeout);
}

void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout) {
    self->timeout_ms = (int)MICROPY_FLOAT_C_FUN(ceil)(timeout * 1000);
}

float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self) {
    return self->timeout_ms / 1000.0f;
}

void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self) {
    if(!self->can) {
        raise_deinited_error();
    }
    common_hal_canio_can_check_for_deinit(self->can);
}

int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self) {
    return self->hw->RXFS.bit.F0FL;
}

bool common_hal_canio_listener_readinto(canio_listener_obj_t *self, canio_message_obj_t *message) {
    if (!common_hal_canio_listener_in_waiting(self)) {
        uint64_t deadline = supervisor_ticks_ms64() + self->timeout_ms;
        do {
            if(supervisor_ticks_ms64() > deadline) {
                return false;
            }
        } while (!common_hal_canio_listener_in_waiting(self));
    }
    int index = self->hw->RXFS.bit.F0GI;
    canio_can_fifo_t *hw_message = &self->fifo[index];
    message->id = hw_message->rxb0.bit.ID >> 18; // short addresses are left-justified
    message->rtr = hw_message->rxb0.bit.RTR;
    message->size = hw_message->rxb1.bit.DLC;
    if(!message->rtr) {
        memcpy(message->data, hw_message->data, message->size);
    }
    self->hw->RXFA.bit.F0AI = index;
    return true;
}

void common_hal_canio_listener_deinit(canio_listener_obj_t *self) {
    // free our FIFO, clear our matches, SOMETHING
    if(self->can) {
        if(self->fifo_idx == 0) {
            self->can->fifo0_in_use = false;
        }
        if(self->fifo_idx == 1) {
            self->can->fifo1_in_use = false;
        }
    }
    self->fifo_idx = -1;
    self->fifo = NULL;
    self->can = NULL;
    self->hw = NULL;
}
