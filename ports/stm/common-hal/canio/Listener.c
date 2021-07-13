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

#include "common-hal/canio/__init__.h"
#include "common-hal/canio/Listener.h"
#include "shared-bindings/canio/Listener.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/safe_mode.h"

STATIC void allow_filter_change(canio_can_obj_t *can) {
    can->filter_hw->FMR |= CAN_FMR_FINIT;
}

STATIC void prevent_filter_change(canio_can_obj_t *can) {
    can->filter_hw->FMR &= ~CAN_FMR_FINIT;
}

STATIC bool filter_in_use(canio_can_obj_t *can, int idx) {
    return can->filter_hw->FA1R & (1 << idx);
}

// One filter bank can hold:
//  * one extended mask
//  * two extended ids
//  * two standard masks
//  * four extended ids
// However, stm needs two filters to permit RTR and non-RTR messages
// so we ONLY use mask-type filter banks
STATIC size_t num_filters_needed(size_t nmatch, canio_match_obj_t **matches) {
    if (nmatch == 0) {
        return 1;
    }
    size_t num_extended_mask = 0;
    size_t num_standard_mask = 1;
    for (size_t i = 0; i < nmatch; i++) {
        if (matches[i]->extended) {
            num_extended_mask += 1;
        } else {
            num_standard_mask += 1;
        }
    }
    return num_extended_mask + num_standard_mask / 2;
}

STATIC size_t num_filters_available(canio_can_obj_t *can) {
    size_t available = 0;
    for (size_t i = can->start_filter_bank; i < can->end_filter_bank; i++) {
        if (!filter_in_use(can, i)) {
            available++;
        }
    }
    return available;
}

STATIC void clear_filters(canio_listener_obj_t *self) {
    canio_can_obj_t *can = self->can;

    allow_filter_change(can);
    uint32_t fa1r = can->filter_hw->FA1R;
    for (size_t i = can->start_filter_bank; i < can->end_filter_bank; i++) {
        if (((can->filter_hw->FFA1R >> i) & 1) == self->fifo_idx) {
            fa1r &= ~(1 << i);
        }
    }
    can->filter_hw->FA1R = fa1r;
    prevent_filter_change(can);
}

STATIC int next_filter(canio_can_obj_t *can) {
    uint32_t fa1r = can->filter_hw->FA1R;
    for (size_t i = can->start_filter_bank; i < can->end_filter_bank; i++) {
        if (!(fa1r & (1 << i))) {
            return i;
        }
    }
    reset_into_safe_mode(MICROPY_FATAL_ERROR);
    return -1;
}

// IDE = "extended ID" flag of packet header.  We always add this bit to the
// mask because a match is always for just one kind of address length
#define FILTER16_IDE (1 << 3)
#define FILTER32_IDE (1 << 2)

STATIC void install_standard_filter(canio_listener_obj_t *self, canio_match_obj_t *match1, canio_match_obj_t *match2) {
    int bank = next_filter(self->can);

    // filter is already deactivated, so we skip deactivating it here
    // CLEAR_BIT(self->can->filter_hw->FA1R, bank);

    self->can->filter_hw->sFilterRegister[bank].FR1 =
        (((match1->id & 0x7ff) << 5)) |
        (((match1->mask & 0x7ff) << 5 | FILTER16_IDE)) << 16;
    self->can->filter_hw->sFilterRegister[bank].FR2 =
        (((match2->id & 0x7ff) << 5)) |
        (((match2->mask & 0x7ff) << 5 | FILTER16_IDE)) << 16;

    // filter mode: 0 = mask
    // (this bit should be clear already, we never set it; but just in case)
    CLEAR_BIT(self->can->filter_hw->FM1R, 1 << bank);
    // filter scale: 0 = 16 bits
    CLEAR_BIT(self->can->filter_hw->FS1R, 1 << bank);
    // fifo assignment: 1 = FIFO 1
    if (self->fifo_idx) {
        SET_BIT(self->can->filter_hw->FFA1R, 1 << bank);
    } else {
        CLEAR_BIT(self->can->filter_hw->FFA1R, 1 << bank);
    }

    // filter activation: 1 = enabled
    SET_BIT(self->can->filter_hw->FA1R, 1 << bank);
}

STATIC void install_extended_filter(canio_listener_obj_t *self, canio_match_obj_t *match) {
    int bank = next_filter(self->can);

    // filter is already deactivated, so we skip deactivating it here
    // CLEAR_BIT(self->can->filter_hw->FA1R, bank);

    self->can->filter_hw->sFilterRegister[bank].FR1 =
        ((match->id << 3) | FILTER32_IDE);
    self->can->filter_hw->sFilterRegister[bank].FR2 =
        ((match->mask << 3) | FILTER32_IDE);

    // filter mode: 0 = mask
    // (this bit should be clear already, we never set it; but just in case)
    CLEAR_BIT(self->can->filter_hw->FM1R, 1 << bank);
    // filter scale: 1 = 32 bits
    SET_BIT(self->can->filter_hw->FS1R, 1 << bank);
    // fifo assignment: 1 = FIFO 1
    if (self->fifo_idx) {
        SET_BIT(self->can->filter_hw->FFA1R, 1 << bank);
    } else {
        CLEAR_BIT(self->can->filter_hw->FFA1R, 1 << bank);
    }

    // filter activation: 1 = enabled
    SET_BIT(self->can->filter_hw->FA1R, 1 << bank);
}

STATIC void install_all_match_filter(canio_listener_obj_t *self) {
    int bank = next_filter(self->can);

    // filter is already deactivated, so we skip deactivating it here
    // CLEAR_BIT(self->can->filter_hw->FA1R, bank);

    self->can->filter_hw->sFilterRegister[bank].FR1 = 0;
    self->can->filter_hw->sFilterRegister[bank].FR2 = 0;

    // filter mode: 0 = mask
    // (this bit should be clear already, we never set it; but just in case)
    CLEAR_BIT(self->can->filter_hw->FM1R, bank);
    // filter scale: 1 = 32 bits
    SET_BIT(self->can->filter_hw->FS1R, bank);
    // fifo assignment: 1 = FIFO 1
    if (self->fifo_idx) {
        SET_BIT(self->can->filter_hw->FFA1R, bank);
    } else {
        CLEAR_BIT(self->can->filter_hw->FFA1R, bank);
    }

    // filter activation: 1 = enabled
    SET_BIT(self->can->filter_hw->FA1R, (1 << bank));
}


#define NO_ADDRESS (-1)
void set_filters(canio_listener_obj_t *self, size_t nmatch, canio_match_obj_t **matches) {
    allow_filter_change(self->can);
    if (!nmatch) {
        install_all_match_filter(self);
    } else {
        canio_match_obj_t *first_match = NULL;
        for (size_t i = 0; i < nmatch; i++) {
            if (matches[i]->extended) {
                install_extended_filter(self, matches[i]);
            } else {
                if (first_match) {
                    install_standard_filter(self, first_match, matches[i]);
                    first_match = NULL;
                } else {
                    first_match = matches[i];
                }
            }
        }
        if (first_match) {
            install_standard_filter(self, first_match, first_match);
        }
    }
    prevent_filter_change(self->can);
}


void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {
    if (!can->fifo0_in_use) {
        self->fifo_idx = 0;
        self->rfr = &can->handle.Instance->RF0R;
        can->fifo0_in_use = true;
    } else if (!can->fifo1_in_use) {
        self->fifo_idx = 1;
        self->rfr = &can->handle.Instance->RF1R;
        can->fifo1_in_use = true;
    } else {
        mp_raise_ValueError(translate("All RX FIFOs in use"));
    }

    if (num_filters_needed(nmatch, matches) > num_filters_available(can)) {
        mp_raise_ValueError(translate("Filters too complex"));
    }

    // Nothing can fail now so it's safe to assign self->can
    self->can = can;

    self->mailbox = &can->handle.Instance->sFIFOMailBox[self->fifo_idx];
    set_filters(self, nmatch, matches);
    common_hal_canio_listener_set_timeout(self, timeout);
}

void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout) {
    self->timeout_ms = (int)MICROPY_FLOAT_C_FUN(ceil)(timeout * 1000);
}

float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self) {
    return self->timeout_ms / 1000.0f;
}

void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self) {
    if (!self->can) {
        raise_deinited_error();
    }
    common_hal_canio_can_check_for_deinit(self->can);
}

int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self) {
    return *(self->rfr) & CAN_RF0R_FMP0;
}

mp_obj_t common_hal_canio_listener_receive(canio_listener_obj_t *self) {
    if (!common_hal_canio_listener_in_waiting(self)) {
        uint64_t deadline = supervisor_ticks_ms64() + self->timeout_ms;
        do {
            if (supervisor_ticks_ms64() > deadline) {
                return NULL;
            }
        } while (!common_hal_canio_listener_in_waiting(self));
    }

    uint32_t rir = self->mailbox->RIR;
    uint32_t rdtr = self->mailbox->RDTR;

    bool rtr = rir & CAN_RI0R_RTR;
    canio_message_obj_t *message = m_new_obj(canio_message_obj_t);
    message->base.type = rtr ? &canio_remote_transmission_request_type : &canio_message_type;
    message->extended = rir & CAN_RI0R_IDE;
    if (message->extended) {
        message->id = rir >> 3;
    } else {
        message->id = rir >> 21;
    }
    message->size = rdtr & CAN_RDT0R_DLC;
    if (!rtr) {
        uint32_t payload[] = { self->mailbox->RDLR, self->mailbox->RDHR };
        MP_STATIC_ASSERT(sizeof(payload) == sizeof(message->data));
        memcpy(message->data, payload, sizeof(payload));
    }
    // Release the mailbox
    SET_BIT(*self->rfr, CAN_RF0R_RFOM0);
    return message;
}

void common_hal_canio_listener_deinit(canio_listener_obj_t *self) {
    if (self->can) {
        clear_filters(self);
        if (self->fifo_idx == 0) {
            self->can->fifo0_in_use = false;
        }
        if (self->fifo_idx == 1) {
            self->can->fifo1_in_use = false;
        }
    }
    self->fifo_idx = -1;
    self->can = NULL;
    self->mailbox = NULL;
    self->rfr = NULL;
}
