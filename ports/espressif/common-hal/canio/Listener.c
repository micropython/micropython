// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

#include "hal/twai_ll.h"

// There is no logic that dictates TWAI vs TWAI0
#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
#define TWAI TWAI0
#endif

// IDE = "extended ID" flag of packet header.  We always add this bit to the
// mask because a match is always for just one kind of address length
#define FILTER16_IDE (1 << 3)
#define FILTER32_IDE (1 << 2)

static void install_standard_filter(canio_listener_obj_t *self, canio_match_obj_t *match) {
    twai_ll_set_acc_filter(&TWAI, match->id << 21, ~(match->mask << 21), true);
    self->extended = false;
    self->standard = true;
}

static void install_extended_filter(canio_listener_obj_t *self, canio_match_obj_t *match) {
    twai_ll_set_acc_filter(&TWAI, match->id << 3, ~(match->mask << 3), true);
    self->extended = true;
    self->standard = false;
}

static void install_all_match_filter(canio_listener_obj_t *self) {
    twai_ll_set_acc_filter(&TWAI, 0u, ~0u, true);
    self->extended = true;
    self->standard = true;
}

__attribute__((noinline, optimize("O0")))
static void set_filters(canio_listener_obj_t *self, size_t nmatch, canio_match_obj_t **matches) {
    twai_ll_enter_reset_mode(&TWAI);

    if (!nmatch) {
        install_all_match_filter(self);
    } else {
        canio_match_obj_t *match = matches[0];
        if (match->extended) {
            install_extended_filter(self, match);
        } else {
            install_standard_filter(self, match);
        }
    }

    twai_ll_exit_reset_mode(&TWAI);
}


void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {
    if (can->fifo_in_use) {
        mp_raise_ValueError(MP_ERROR_TEXT("All RX FIFOs in use"));
    }
    if (nmatch > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Filters too complex"));
    }

    // Nothing can fail now so it's safe to assign self->can
    can->fifo_in_use = 1;
    self->can = can;
    self->pending = false;

    set_filters(self, nmatch, matches);
    self->extended = self->standard = true;

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

// The API has no peek method so we must receive a packet into a holding area
// and then we can say that we have 1 message pending
int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self) {
    while (!self->pending && twai_receive(&self->message_in, 0) == ESP_OK) {
        if (self->message_in.extd && self->extended) {
            self->pending = true;
        }
        if (!self->message_in.extd && self->standard) {
            self->pending = true;
        }
    }
    return self->pending;
}

mp_obj_t common_hal_canio_listener_receive(canio_listener_obj_t *self) {
    if (!common_hal_canio_listener_in_waiting(self)) {
        uint64_t deadline = supervisor_ticks_ms64() + self->timeout_ms;
        do {
            if (supervisor_ticks_ms64() > deadline) {
                return NULL;
            }
            RUN_BACKGROUND_TASKS;
            // Allow user to break out of a timeout with a KeyboardInterrupt.
            if (mp_hal_is_interrupted()) {
                return NULL;
            }
        } while (!common_hal_canio_listener_in_waiting(self));
    }

    bool rtr = self->message_in.rtr;

    int dlc = self->message_in.data_length_code;
    canio_message_obj_t *message =
        mp_obj_malloc(canio_message_obj_t, rtr ? &canio_remote_transmission_request_type : &canio_message_type);
    message->extended = self->message_in.extd;
    message->id = self->message_in.identifier;
    message->size = dlc;

    if (!rtr) {
        MP_STATIC_ASSERT(sizeof(self->message_in.data) == sizeof(message->data));
        memcpy(message->data, self->message_in.data, sizeof(message->data));
    }

    self->pending = false;

    return message;
}

void common_hal_canio_listener_deinit(canio_listener_obj_t *self) {
    if (self->can) {
        self->can->fifo_in_use = false;
    }
    self->can = NULL;
}
