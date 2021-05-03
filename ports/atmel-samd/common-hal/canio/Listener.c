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
#include "component/can.h"

STATIC void allow_config_change(canio_can_obj_t *can) {
    can->hw->CCCR.bit.INIT = 1;
    while (!can->hw->CCCR.bit.INIT) {
    }
    can->hw->CCCR.bit.CCE = 1;
}

STATIC void prevent_config_change(canio_can_obj_t *can) {
    can->hw->CCCR.bit.CCE = 0;
    can->hw->CCCR.bit.INIT = 0;
    while (can->hw->CCCR.bit.INIT) {
    }
}

__attribute__((unused))
STATIC void static_assertions(void) {
    MP_STATIC_ASSERT(CAN_GFC_ANFE_RXF0_Val + 1 == CAN_GFC_ANFE_RXF1_Val);
    MP_STATIC_ASSERT(CAN_GFC_ANFS_RXF0_Val + 1 == CAN_GFC_ANFS_RXF1_Val);
    MP_STATIC_ASSERT(CAN_SIDFE_0_SFEC_STF0M_Val + 1 == CAN_SIDFE_0_SFEC_STF1M_Val);
    MP_STATIC_ASSERT(CAN_XIDFE_0_EFEC_STF0M_Val + 1 == CAN_XIDFE_0_EFEC_STF1M_Val);
}

STATIC bool single_id_filter(canio_match_obj_t *match) {
    return match->mask == 0 || match->mask == match->id;
}

STATIC bool standard_filter_in_use(CanMramSidfe *filter) {
    return filter->SIDFE_0.bit.SFEC != CAN_SIDFE_0_SFEC_DISABLE_Val;
}

STATIC bool extended_filter_in_use(CanMramXidfe *filter) {
    return filter->XIDFE_0.bit.EFEC != CAN_XIDFE_0_EFEC_DISABLE_Val;
}

STATIC size_t num_filters_needed(size_t nmatch, canio_match_obj_t **matches, bool extended) {
    size_t num_half_filters_needed = 1;
    for (size_t i = 0; i < nmatch; i++) {
        if (extended != matches[i]->extended) {
            continue;
        }
        if (single_id_filter(matches[i])) {
            num_half_filters_needed += 1;
        } else {
            num_half_filters_needed += 2;
        }
    }
    return num_half_filters_needed / 2;
}

STATIC size_t num_filters_available(canio_can_obj_t *can, bool extended) {
    size_t available = 0;
    if (extended) {
        for (size_t i = 0; i < MP_ARRAY_SIZE(can->state->extended_rx_filter); i++) {
            if (!extended_filter_in_use(&can->state->extended_rx_filter[i])) {
                available++;
            }
        }
    } else {
        for (size_t i = 0; i < MP_ARRAY_SIZE(can->state->standard_rx_filter); i++) {
            if (!standard_filter_in_use(&can->state->standard_rx_filter[i])) {
                available++;
            }
        }
    }
    return available;
}

STATIC void clear_filters(canio_listener_obj_t *self) {
    canio_can_obj_t *can = self->can;
    int fifo = self->fifo_idx;

    // If it was a global accept, clear it
    allow_config_change(can);
    if (can->hw->GFC.bit.ANFS == CAN_GFC_ANFS_RXF0 + fifo) {
        can->hw->GFC.bit.ANFS = CAN_GFC_ANFS_REJECT_Val;
    }
    if (can->hw->GFC.bit.ANFE == CAN_GFC_ANFE_RXF0 + fifo) {
        can->hw->GFC.bit.ANFE = CAN_GFC_ANFE_REJECT_Val;
    }
    prevent_config_change(can);

    // For each filter entry, if it pointed at this FIFO set it to DISABLE
    for (size_t i = 0; i < MP_ARRAY_SIZE(can->state->extended_rx_filter); i++) {
        int val = CAN_XIDFE_0_EFEC_STF0M_Val + fifo;
        if (can->state->extended_rx_filter[i].XIDFE_0.bit.EFEC == val) {
            can->state->extended_rx_filter[i].XIDFE_0.bit.EFEC = CAN_XIDFE_0_EFEC_DISABLE_Val;
        }
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(can->state->standard_rx_filter); i++) {
        int val = CAN_SIDFE_0_SFEC_STF1M_Val + fifo;
        if (can->state->standard_rx_filter[i].SIDFE_0.bit.SFEC == val) {
            can->state->standard_rx_filter[i].SIDFE_0.bit.SFEC = CAN_SIDFE_0_SFEC_DISABLE_Val;
        }
    }
}

STATIC CanMramXidfe *next_extended_filter(canio_listener_obj_t *self, CanMramXidfe *start) {
    CanMramXidfe *end = &self->can->state->extended_rx_filter[MP_ARRAY_SIZE(self->can->state->extended_rx_filter)];
    if (start == NULL) {
        start = self->can->state->extended_rx_filter;
    } else {
        start = start + 1;
    }
    while (extended_filter_in_use(start)) {
        if (start == end) {
            return NULL;
        }
        start = start + 1;
    }
    return start;
}

STATIC CanMramSidfe *next_standard_filter(canio_listener_obj_t *self, CanMramSidfe *start) {
    CanMramSidfe *end = &self->can->state->standard_rx_filter[MP_ARRAY_SIZE(self->can->state->standard_rx_filter)];
    if (start == NULL) {
        start = self->can->state->standard_rx_filter;
    } else {
        start = start + 1;
    }
    while (standard_filter_in_use(start)) {
        if (start == end) {
            return NULL;
        }
        start = start + 1;
    }
    return start;
}

STATIC void install_standard_filter(CanMramSidfe *standard, int id1, int id2, int sfec, int sft) {
    assert(standard);
    CAN_SIDFE_0_Type val = {
        .bit.SFID1 = id1,
        .bit.SFID2 = id2,
        .bit.SFEC = sfec,
        .bit.SFT = sft,
    };
    standard->SIDFE_0 = val;
}

STATIC void install_extended_filter(CanMramXidfe *extended, int id1, int id2, int efec, int eft) {
    assert(extended);
    CAN_XIDFE_0_Type val0 = {
        .bit.EFID1 = id1,
        .bit.EFEC = efec,
    };
    CAN_XIDFE_1_Type val1 = {
        .bit.EFID2 = id2,
        .bit.EFT = eft,
    };
    // Set entry 0 second, because it has the enable bits (XIDFE_0_EFEC)
    extended->XIDFE_1 = val1;
    extended->XIDFE_0 = val0;
}


#define NO_ID (-1)
void set_filters(canio_listener_obj_t *self, size_t nmatch, canio_match_obj_t **matches) {
    int fifo = self->fifo_idx;

    if (!nmatch) {
        allow_config_change(self->can);
        self->can->hw->GFC.bit.ANFS = CAN_GFC_ANFS_RXF0_Val + fifo;
        self->can->hw->GFC.bit.ANFE = CAN_GFC_ANFE_RXF0_Val + fifo;
        self->can->hw->CCCR.bit.CCE = 0;
        prevent_config_change(self->can);
        return;
    }

    CanMramSidfe *standard = next_standard_filter(self, NULL);
    CanMramXidfe *extended = next_extended_filter(self, NULL);

    int first_id = NO_ID;

    // step 1: single id standard matches
    // we have to gather up pairs and stuff them in a single filter entry
    for (size_t i = 0; i < nmatch; i++) {
        canio_match_obj_t *match = matches[i];
        if (match->extended) {
            continue;
        }
        if (!single_id_filter(match)) {
            continue;
        }
        if (first_id != NO_ID) {
            install_standard_filter(standard, first_id, match->id, CAN_SIDFE_0_SFEC_STF0M_Val + fifo, CAN_SIDFE_0_SFT_DUAL_Val);
            first_id = NO_ID;
            standard = next_standard_filter(self, standard);
        } else {
            first_id = match->id;
        }
    }
    // step 1.5. odd single id standard match
    if (first_id != NO_ID) {
        install_standard_filter(standard, first_id, first_id, CAN_SIDFE_0_SFEC_STF0M_Val + fifo, CAN_SIDFE_0_SFT_DUAL_Val);
        standard = next_standard_filter(self, standard);
        first_id = NO_ID;
    }

    // step 2: standard mask filter
    for (size_t i = 0; i < nmatch; i++) {
        canio_match_obj_t *match = matches[i];
        if (match->extended) {
            continue;
        }
        if (single_id_filter(match)) {
            continue;
        }
        install_standard_filter(standard, match->id, match->mask, CAN_SIDFE_0_SFEC_STF0M_Val + fifo, CAN_SIDFE_0_SFT_CLASSIC_Val);
        standard = next_standard_filter(self, standard);
    }

    // step 3: single id extended matches
    // we have to gather up pairs and stuff them in a single filter entry
    for (size_t i = 0; i < nmatch; i++) {
        canio_match_obj_t *match = matches[i];
        if (!match->extended) {
            continue;
        }
        if (!single_id_filter(match)) {
            continue;
        }
        if (first_id != NO_ID) {
            install_extended_filter(extended, first_id, match->id, CAN_XIDFE_0_EFEC_STF0M_Val + fifo, CAN_XIDFE_1_EFT_DUAL_Val);
            first_id = NO_ID;
            extended = next_extended_filter(self, extended);
        } else {
            first_id = match->id;
        }
    }
    // step 3.5. odd single id standard match
    if (first_id != NO_ID) {
        install_extended_filter(extended, first_id, first_id, CAN_XIDFE_0_EFEC_STF0M_Val + fifo, CAN_XIDFE_1_EFT_DUAL_Val);
        extended = next_extended_filter(self, extended);
        first_id = NO_ID;
    }

    // step 4: extended mask filters
    for (size_t i = 0; i < nmatch; i++) {
        canio_match_obj_t *match = matches[i];
        if (!match->extended) {
            continue;
        }
        if (single_id_filter(match)) {
            continue;
        }
        install_extended_filter(extended, match->id, match->mask, CAN_XIDFE_0_EFEC_STF0M_Val + fifo, CAN_XIDFE_1_EFT_CLASSIC_Val);
        extended = next_extended_filter(self, extended);
    }

    // phew, easy(!)
}


void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {
    if (!can->fifo0_in_use) {
        self->fifo_idx = 0;
        self->fifo = can->state->rx0_fifo;
        self->hw = (canio_rxfifo_reg_t *)&can->hw->RXF0C;
        can->hw->IR.reg = CAN_IR_RF0N | CAN_IR_RF0W | CAN_IR_RF0F | CAN_IR_RF0L;
        can->fifo0_in_use = true;
    } else if (!can->fifo1_in_use) {
        self->fifo_idx = 1;
        self->fifo = can->state->rx1_fifo;
        self->hw = (canio_rxfifo_reg_t *)&can->hw->RXF1C;
        can->fifo1_in_use = true;
        can->hw->IR.reg = CAN_IR_RF1N | CAN_IR_RF1W | CAN_IR_RF1F | CAN_IR_RF1L;
    } else {
        mp_raise_ValueError(translate("All RX FIFOs in use"));
    }

    if (!nmatch) {
        if (can->hw->GFC.bit.ANFS == CAN_GFC_ANFS_RXF1_Val - self->fifo_idx) {
            mp_raise_ValueError(translate("Already have all-matches listener"));
        }
        if (can->hw->GFC.bit.ANFE == CAN_GFC_ANFE_RXF1_Val - self->fifo_idx) {
            mp_raise_ValueError(translate("Already have all-matches listener"));
        }
    }

    if (num_filters_needed(nmatch, matches, false) > num_filters_available(can, false)) {
        mp_raise_ValueError(translate("Filters too complex"));
    }

    if (num_filters_needed(nmatch, matches, true) > num_filters_available(can, true)) {
        mp_raise_ValueError(translate("Filters too complex"));
    }

    // Nothing can fail now so it's safe to assign self->can
    self->can = can;
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
    return self->hw->RXFS.bit.F0FL;
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
    int index = self->hw->RXFS.bit.F0GI;
    canio_can_rx_fifo_t *hw_message = &self->fifo[index];
    bool rtr = hw_message->rxf0.bit.RTR;
    canio_message_obj_t *message = m_new_obj(canio_message_obj_t);
    message->base.type = rtr ? &canio_remote_transmission_request_type : &canio_message_type;
    message->extended = hw_message->rxf0.bit.XTD;
    if (message->extended) {
        message->id = hw_message->rxf0.bit.ID;
    } else {
        message->id = hw_message->rxf0.bit.ID >> 18; // short ids are left-justified
    }
    message->size = hw_message->rxf1.bit.DLC;
    if (!rtr) {
        memcpy(message->data, hw_message->data, message->size);
    }
    self->hw->RXFA.bit.F0AI = index;
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
    self->fifo = NULL;
    self->can = NULL;
    self->hw = NULL;
}
