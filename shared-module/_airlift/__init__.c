// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/_airlift/__init__.h"

#include "py/mperrno.h"
#include "py/objlist.h"
#include "py/runtime.h"

// Replies to VM commands land here before they are copied into bytearrays. Only
// the VM uses these, so a background command can't overwrite them.
static uint8_t vm_reply_buf[AIRLIFT_MAX_TRANSFER];
static uint16_t vm_reply_lens[255];

void common_hal__airlift_init(void) {
    if (!airlift_transport_init()) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_pin);
    }
}

bool common_hal__airlift_active(void) {
    return airlift_transport_active();
}

static void check_active(void) {
    if (!airlift_transport_active()) {
        mp_raise_OSError(MP_ENODEV);
    }
}

void common_hal__airlift_reset(void) {
    check_active();
    airlift_transport_reset();
}

void common_hal__airlift_release(void) {
    airlift_transport_deinit();
}

// Raises the exception type ESP32SPI raises for the same failure, so its error
// handling keeps working.
static void raise_for(airlift_result_t result, uint8_t cmd) {
    switch (result) {
        case AIRLIFT_OK:
            return;
        case AIRLIFT_NOT_ACTIVE:
            mp_raise_OSError(MP_ENODEV);
        case AIRLIFT_BUSY:
            mp_raise_OSError(MP_EBUSY);
        case AIRLIFT_INTERRUPTED:
            // Nothing was sent. Raise the pending KeyboardInterrupt now.
            mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);
            mp_raise_OSError(MP_EINTR);
        case AIRLIFT_BAD_SOCKET:
            mp_raise_OSError(MP_EBADF);
        case AIRLIFT_TOO_BIG:
            if (cmd == AIRLIFT_GET_DATABUF_TCP) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be <= %d"), MP_QSTR_size, AIRLIFT_MAX_READ);
            }
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q length must be <= %d"), MP_QSTR_params, AIRLIFT_MAX_TRANSFER);
        case AIRLIFT_TIMEOUT_READY:
        case AIRLIFT_TIMEOUT_SELECT:
        case AIRLIFT_TIMEOUT_START:
            mp_raise_msg(&mp_type_TimeoutError, MP_ERROR_TEXT("Operation timed out"));
        case AIRLIFT_ERR_RESPONSE:
        case AIRLIFT_BAD_REPLY:
            mp_raise_msg(&mp_type_BrokenPipeError, MP_ERROR_TEXT("Received response was invalid"));
        case AIRLIFT_REPLY_TOO_BIG:
            mp_raise_OSError(MP_ENOBUFS);
    }
}

mp_obj_t common_hal__airlift_transact(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, int expected_replies, bool reply_len_16) {
    airlift_reply_t reply = {
        .buf = vm_reply_buf,
        .buf_len = sizeof(vm_reply_buf),
        .lens = vm_reply_lens,
        .max_count = MP_ARRAY_SIZE(vm_reply_lens),
    };
    airlift_result_t result = airlift_transact(cmd, params, n_params, param_len_16, &reply,
        expected_replies, reply_len_16, true);
    raise_for(result, cmd);

    mp_obj_list_t *list = MP_OBJ_TO_PTR(mp_obj_new_list(reply.count, NULL));
    size_t offset = 0;
    for (size_t i = 0; i < reply.count; i++) {
        list->items[i] = mp_obj_new_bytearray(vm_reply_lens[i], vm_reply_buf + offset);
        offset += vm_reply_lens[i];
    }
    return MP_OBJ_FROM_PTR(list);
}
