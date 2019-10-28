/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
 * Copyright (c) 2019 Jim Mussared
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

#include "py/binary.h"
#include "py/misc.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objarray.h"
#include "py/qstr.h"
#include "py/runtime.h"
#include "extmod/modbluetooth.h"
#include <string.h>

#if MICROPY_PY_BLUETOOTH

#if !MICROPY_ENABLE_SCHEDULER
#error modbluetooth requires MICROPY_ENABLE_SCHEDULER
#endif

// This is used to protect the ringbuffer.
#ifndef MICROPY_PY_BLUETOOTH_ENTER
#define MICROPY_PY_BLUETOOTH_ENTER mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
#define MICROPY_PY_BLUETOOTH_EXIT MICROPY_END_ATOMIC_SECTION(atomic_state);
#endif

#define MP_BLUETOOTH_CONNECT_DEFAULT_SCAN_DURATION_MS 2000

#define MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_TUPLE_LEN 5
#define MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_BYTES_LEN (MICROPY_PY_BLUETOOTH_RINGBUF_SIZE / 2)

STATIC const mp_obj_type_t bluetooth_ble_type;
STATIC const mp_obj_type_t bluetooth_uuid_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t irq_handler;
    mp_obj_t irq_data_tuple;
    uint8_t irq_addr_bytes[6];
    uint8_t irq_data_bytes[MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_BYTES_LEN];
    mp_obj_t irq_data_uuid;
    uint16_t irq_trigger;
    ringbuf_t ringbuf;
} mp_obj_bluetooth_ble_t;

// TODO: this seems like it could be generic?
STATIC mp_obj_t bluetooth_handle_errno(int err) {
    if (err != 0) {
        mp_raise_OSError(err);
    }
    return mp_const_none;
}

// ----------------------------------------------------------------------------
// UUID object
// ----------------------------------------------------------------------------

// Parse string UUIDs, which are expected to be 128-bit UUIDs.
STATIC void mp_bluetooth_parse_uuid_128bit_str(mp_obj_t obj, uint8_t *uuid) {
    size_t str_len;
    const char *str_data = mp_obj_str_get_data(obj, &str_len);
    int uuid_i = 32;
    for (int i = 0; i < str_len; i++) {
        char c = str_data[i];
        if (c == '-') {
            continue;
        }
        if (!unichar_isxdigit(c)) {
            mp_raise_ValueError("invalid char in UUID");
        }
        c = unichar_xdigit_value(c);
        uuid_i--;
        if (uuid_i < 0) {
            mp_raise_ValueError("UUID too long");
        }
        if (uuid_i % 2 == 0) {
            // lower nibble
            uuid[uuid_i/2] |= c;
        } else {
            // upper nibble
            uuid[uuid_i/2] = c << 4;
        }
    }
    if (uuid_i > 0) {
        mp_raise_ValueError("UUID too short");
    }
}

STATIC mp_obj_t bluetooth_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_bluetooth_uuid_t *self = m_new_obj(mp_obj_bluetooth_uuid_t);
    self->base.type = &bluetooth_uuid_type;

    if (mp_obj_is_int(all_args[0])) {
        self->type = MP_BLUETOOTH_UUID_TYPE_16;
        mp_int_t value = mp_obj_get_int(all_args[0]);
        if (value > 65535) {
            mp_raise_ValueError("invalid UUID");
        }
        self->data[0] = value & 0xff;
        self->data[1] = (value >> 8) & 0xff;
    } else {
        self->type = MP_BLUETOOTH_UUID_TYPE_128;
        mp_bluetooth_parse_uuid_128bit_str(all_args[0], self->data);
    }

    return self;
}

STATIC mp_obj_t bluetooth_uuid_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_bluetooth_uuid_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_HASH: {
            // Use the QSTR hash function.
            return MP_OBJ_NEW_SMALL_INT(qstr_compute_hash(self->data, self->type));
        }
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC void bluetooth_uuid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_bluetooth_uuid_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UUID%u(%s", self->type * 8, self->type <= 4 ? "0x" : "'");
    for (int i = 0; i < self->type; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            mp_printf(print, "-");
        }
        mp_printf(print, "%02x", self->data[self->type - 1 - i]);
    }
    if (self->type == MP_BLUETOOTH_UUID_TYPE_128) {
        mp_printf(print, "'");
    }
    mp_printf(print, ")");
}

mp_int_t bluetooth_uuid_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_bluetooth_uuid_t *self = MP_OBJ_TO_PTR(self_in);

    if (flags != MP_BUFFER_READ) {
        return 1;
    }

    bufinfo->buf = self->data;
    bufinfo->len = self->type;
    bufinfo->typecode = 'B';
    return 0;
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC void ringbuf_put_uuid(ringbuf_t *ringbuf, mp_obj_bluetooth_uuid_t *uuid) {
    assert(ringbuf_free(ringbuf) >= uuid->type + 1);
    ringbuf_put(ringbuf, uuid->type);
    for (int i = 0; i < uuid->type; ++i) {
        ringbuf_put(ringbuf, uuid->data[i]);
    }
}

STATIC void ringbuf_get_uuid(ringbuf_t *ringbuf, mp_obj_bluetooth_uuid_t *uuid) {
    assert(ringbuf_avail(ringbuf) >= 1);
    uuid->type = ringbuf_get(ringbuf);
    assert(ringbuf_avail(ringbuf) >= uuid->type);
    for (int i = 0; i < uuid->type; ++i) {
        uuid->data[i] = ringbuf_get(ringbuf);
    }
}
#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC const mp_obj_type_t bluetooth_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .make_new = bluetooth_uuid_make_new,
    .unary_op = bluetooth_uuid_unary_op,
    .locals_dict = NULL,
    .print = bluetooth_uuid_print,
    .buffer_p = { .get_buffer = bluetooth_uuid_get_buffer },
};

// ----------------------------------------------------------------------------
// Bluetooth object: General
// ----------------------------------------------------------------------------

STATIC mp_obj_t bluetooth_ble_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    if (MP_STATE_VM(bluetooth) == MP_OBJ_NULL) {
        mp_obj_bluetooth_ble_t *o = m_new_obj(mp_obj_bluetooth_ble_t);
        o->base.type = &bluetooth_ble_type;
        o->irq_handler = mp_const_none;
        // Pre-allocate the event data tuple to prevent needing to allocate in the IRQ handler.
        o->irq_data_tuple = mp_obj_new_tuple(MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_TUPLE_LEN, NULL);
        mp_obj_bluetooth_uuid_t *uuid = m_new_obj(mp_obj_bluetooth_uuid_t);
        uuid->base.type = &bluetooth_uuid_type;
        o->irq_data_uuid = MP_OBJ_FROM_PTR(uuid);
        o->irq_trigger = 0;
        ringbuf_alloc(&o->ringbuf, MICROPY_PY_BLUETOOTH_RINGBUF_SIZE);
        MP_STATE_VM(bluetooth) = MP_OBJ_FROM_PTR(o);
    }
    return MP_STATE_VM(bluetooth);
}

STATIC mp_obj_t bluetooth_ble_active(size_t n_args, const mp_obj_t *args) {
    if (n_args == 2) {
        // Boolean enable/disable argument supplied, set current state.
        if (mp_obj_is_true(args[1])) {
            int err = mp_bluetooth_init();
            if (err != 0) {
                mp_raise_OSError(err);
            }
        } else {
            mp_bluetooth_deinit();
        }
    }
    // Return current state.
    return mp_obj_new_bool(mp_bluetooth_is_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_active_obj, 1, 2, bluetooth_ble_active);

STATIC mp_obj_t bluetooth_ble_config(mp_obj_t self_in, mp_obj_t param) {
    if (param == MP_OBJ_NEW_QSTR(MP_QSTR_mac)) {
        uint8_t addr[6];
        mp_bluetooth_get_device_addr(addr);
        return mp_obj_new_bytes(addr, MP_ARRAY_SIZE(addr));
    } else {
        mp_raise_ValueError("unknown config param");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_ble_config_obj, bluetooth_ble_config);

STATIC mp_obj_t bluetooth_ble_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ|MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = MP_BLUETOOTH_IRQ_ALL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t callback = args[ARG_handler].u_obj;
    if (callback != mp_const_none && !mp_obj_is_callable(callback)) {
        mp_raise_ValueError("invalid callback");
    }

    // Update the callback.
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t* o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    o->irq_handler = callback;
    o->irq_trigger = args[ARG_trigger].u_int;
    MICROPY_PY_BLUETOOTH_EXIT

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_ble_irq_obj, 1, bluetooth_ble_irq);

// ----------------------------------------------------------------------------
// Bluetooth object: GAP
// ----------------------------------------------------------------------------

STATIC mp_obj_t bluetooth_ble_gap_advertise(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_interval_us, ARG_adv_data, ARG_resp_data, ARG_connectable };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval_us, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(500000)} },
        { MP_QSTR_adv_data, MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_resp_data, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_connectable, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_interval_us].u_obj == mp_const_none) {
        mp_bluetooth_gap_advertise_stop();
        return mp_const_none;
    }

    mp_int_t interval_us = mp_obj_get_int(args[ARG_interval_us].u_obj);
    bool connectable = mp_obj_is_true(args[ARG_connectable].u_obj);

    mp_buffer_info_t adv_bufinfo = {0};
    if (args[ARG_adv_data].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_adv_data].u_obj, &adv_bufinfo, MP_BUFFER_READ);
    }

    mp_buffer_info_t resp_bufinfo = {0};
    if (args[ARG_resp_data].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_resp_data].u_obj, &resp_bufinfo, MP_BUFFER_READ);
    }

    return bluetooth_handle_errno(mp_bluetooth_gap_advertise_start(connectable, interval_us, adv_bufinfo.buf, adv_bufinfo.len, resp_bufinfo.buf, resp_bufinfo.len));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_ble_gap_advertise_obj, 1, bluetooth_ble_gap_advertise);

STATIC int bluetooth_gatts_register_service(mp_obj_t uuid_in, mp_obj_t characteristics_in, uint16_t **handles, size_t *num_handles) {
    if (!mp_obj_is_type(uuid_in, &bluetooth_uuid_type)) {
        mp_raise_ValueError("invalid service UUID");
    }
    mp_obj_bluetooth_uuid_t *service_uuid = MP_OBJ_TO_PTR(uuid_in);

    mp_obj_t len_in = mp_obj_len(characteristics_in);
    size_t len = mp_obj_get_int(len_in);
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(characteristics_in, &iter_buf);
    mp_obj_t characteristic_obj;

    // Lists of characteristic uuids and flags.
    mp_obj_bluetooth_uuid_t **characteristic_uuids = m_new(mp_obj_bluetooth_uuid_t*, len);
    uint8_t *characteristic_flags = m_new(uint8_t, len);

    // Flattened list of descriptor uuids and flags. Grows (realloc) as more descriptors are encountered.
    mp_obj_bluetooth_uuid_t **descriptor_uuids = NULL;
    uint8_t *descriptor_flags = NULL;
    // How many descriptors in the flattened list per characteristic.
    uint8_t *num_descriptors = m_new(uint8_t, len);

    // Inititally allocate enough room for the number of characteristics.
    // Will be grown to accommodate descriptors as necessary.
    *num_handles = len;
    *handles = m_new(uint16_t, *num_handles);

    // Extract out characteristic uuids & flags.

    int characteristic_index = 0; // characteristic index.
    int handle_index = 0; // handle index.
    int descriptor_index = 0; // descriptor index.
    while ((characteristic_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        // (uuid, flags, (optional descriptors),)
        size_t characteristic_len;
        mp_obj_t *characteristic_items;
        mp_obj_get_array(characteristic_obj, &characteristic_len, &characteristic_items);

        if (characteristic_len < 2 || characteristic_len > 3) {
            mp_raise_ValueError("invalid characteristic tuple");
        }
        mp_obj_t uuid_obj = characteristic_items[0];
        if (!mp_obj_is_type(uuid_obj, &bluetooth_uuid_type)) {
            mp_raise_ValueError("invalid characteristic UUID");
        }

        (*handles)[handle_index++] = 0xffff;

        // Optional third element, iterable of descriptors.
        if (characteristic_len >= 3) {
            mp_obj_t descriptors_len_in = mp_obj_len(characteristic_items[2]);
            num_descriptors[characteristic_index] = mp_obj_get_int(descriptors_len_in);

            if (num_descriptors[characteristic_index] == 0) {
                continue;
            }

            // Grow the flattened uuids and flags arrays with this many more descriptors.
            descriptor_uuids = m_renew(mp_obj_bluetooth_uuid_t*, descriptor_uuids, descriptor_index, descriptor_index + num_descriptors[characteristic_index]);
            descriptor_flags = m_renew(uint8_t, descriptor_flags, descriptor_index, descriptor_index + num_descriptors[characteristic_index]);

            // Also grow the handles array.
            *handles = m_renew(uint16_t, *handles, *num_handles, *num_handles + num_descriptors[characteristic_index]);

            mp_obj_iter_buf_t iter_buf_desc;
            mp_obj_t iterable_desc = mp_getiter(characteristic_items[2], &iter_buf_desc);
            mp_obj_t descriptor_obj;

            // Extract out descriptors for this characteristic.
            while ((descriptor_obj = mp_iternext(iterable_desc)) != MP_OBJ_STOP_ITERATION) {
                // (uuid, flags,)
                mp_obj_t *descriptor_items;
                mp_obj_get_array_fixed_n(descriptor_obj, 2, &descriptor_items);
                mp_obj_t desc_uuid_obj = descriptor_items[0];
                if (!mp_obj_is_type(desc_uuid_obj, &bluetooth_uuid_type)) {
                    mp_raise_ValueError("invalid descriptor UUID");
                }

                descriptor_uuids[descriptor_index] = MP_OBJ_TO_PTR(desc_uuid_obj);
                descriptor_flags[descriptor_index] = mp_obj_get_int(descriptor_items[1]);
                ++descriptor_index;

                (*handles)[handle_index++] = 0xffff;
            }

            // Reflect that we've grown the handles array.
            *num_handles += num_descriptors[characteristic_index];
        }

        characteristic_uuids[characteristic_index] = MP_OBJ_TO_PTR(uuid_obj);
        characteristic_flags[characteristic_index] = mp_obj_get_int(characteristic_items[1]);
        ++characteristic_index;
    }

    // Add service.
    return mp_bluetooth_gatts_register_service(service_uuid, characteristic_uuids, characteristic_flags, descriptor_uuids, descriptor_flags, num_descriptors, *handles, len);
}

STATIC mp_obj_t bluetooth_ble_gatts_register_services(mp_obj_t self_in, mp_obj_t services_in) {
    mp_obj_t len_in = mp_obj_len(services_in);
    size_t len = mp_obj_get_int(len_in);
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(services_in, &iter_buf);
    mp_obj_t service_tuple_obj;

    mp_obj_tuple_t *result = mp_obj_new_tuple(len, NULL);

    uint16_t **handles = m_new0(uint16_t*, len);
    size_t *num_handles = m_new0(size_t, len);

    // TODO: Add a `append` kwarg (defaulting to False) to make this behavior optional.
    bool append = false;
    int err = mp_bluetooth_gatts_register_service_begin(append);
    if (err != 0) {
        return bluetooth_handle_errno(err);
    }

    int i = 0;
    while ((service_tuple_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        // (uuid, chars)
        mp_obj_t *service_items;
        mp_obj_get_array_fixed_n(service_tuple_obj, 2, &service_items);
        err = bluetooth_gatts_register_service(service_items[0], service_items[1], &handles[i], &num_handles[i]);
        if (err != 0) {
            return bluetooth_handle_errno(err);
        }

        ++i;
    }

    // On Nimble, this will actually perform the registration, making the handles valid.
    err = mp_bluetooth_gatts_register_service_end();
    if (err != 0) {
        return bluetooth_handle_errno(err);
    }

    // Return tuple of tuple of value handles.
    // TODO: Also the Generic Access service characteristics?
    for (i = 0; i < len; ++i) {
        mp_obj_tuple_t *service_handles = mp_obj_new_tuple(num_handles[i], NULL);
        for (int j = 0; j < num_handles[i]; ++j) {
            service_handles->items[j] = MP_OBJ_NEW_SMALL_INT(handles[i][j]);
        }
        result->items[i] = MP_OBJ_FROM_PTR(service_handles);
    }
    return MP_OBJ_FROM_PTR(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_ble_gatts_register_services_obj, bluetooth_ble_gatts_register_services);

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
STATIC mp_obj_t bluetooth_ble_gap_connect(size_t n_args, const mp_obj_t *args) {
    uint8_t addr_type = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len != 6) {
        mp_raise_ValueError("invalid addr");
    }
    mp_int_t scan_duration_ms = MP_BLUETOOTH_CONNECT_DEFAULT_SCAN_DURATION_MS;
    if (n_args == 4) {
        scan_duration_ms = mp_obj_get_int(args[3]);
    }

    int err = mp_bluetooth_gap_peripheral_connect(addr_type, bufinfo.buf, scan_duration_ms);
    return bluetooth_handle_errno(err);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gap_connect_obj, 3, 4, bluetooth_ble_gap_connect);

STATIC mp_obj_t bluetooth_ble_gap_scan(size_t n_args, const mp_obj_t *args) {
    // Default is indefinite scan, with the NimBLE "background scan" interval and window.
    mp_int_t duration_ms = 0;
    mp_int_t interval_us = 1280000;
    mp_int_t window_us = 11250;
    if (n_args > 1) {
        if (args[1] == mp_const_none) {
            // scan(None) --> stop scan.
            return bluetooth_handle_errno(mp_bluetooth_gap_scan_stop());
        }
        duration_ms = mp_obj_get_int(args[1]);
        if (n_args > 2) {
            interval_us = mp_obj_get_int(args[2]);
            if (n_args > 3) {
                window_us = mp_obj_get_int(args[3]);
            }
        }
    }
    return bluetooth_handle_errno(mp_bluetooth_gap_scan_start(duration_ms, interval_us, window_us));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gap_scan_obj, 1, 4, bluetooth_ble_gap_scan);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC mp_obj_t bluetooth_ble_gap_disconnect(mp_obj_t self_in, mp_obj_t conn_handle_in) {
    uint16_t conn_handle = mp_obj_get_int(conn_handle_in);
    int err = mp_bluetooth_gap_disconnect(conn_handle);
    if (err == 0) {
        return mp_const_true;
    } else if (err == MP_ENOTCONN) {
        return mp_const_false;
    } else {
        return bluetooth_handle_errno(err);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_ble_gap_disconnect_obj, bluetooth_ble_gap_disconnect);

// ----------------------------------------------------------------------------
// Bluetooth object: GATTS (Peripheral/Advertiser role)
// ----------------------------------------------------------------------------

STATIC mp_obj_t bluetooth_ble_gatts_read(mp_obj_t self_in, mp_obj_t value_handle_in) {
    size_t len = 0;
    uint8_t* buf;
    mp_bluetooth_gatts_read(mp_obj_get_int(value_handle_in), &buf, &len);
    return mp_obj_new_bytes(buf, len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_ble_gatts_read_obj, bluetooth_ble_gatts_read);

STATIC mp_obj_t bluetooth_ble_gatts_write(mp_obj_t self_in, mp_obj_t value_handle_in, mp_obj_t data) {
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    int err = mp_bluetooth_gatts_write(mp_obj_get_int(value_handle_in), bufinfo.buf, bufinfo.len);
    if (err != 0) {
        mp_raise_OSError(err);
    }
    return MP_OBJ_NEW_SMALL_INT(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bluetooth_ble_gatts_write_obj, bluetooth_ble_gatts_write);

STATIC mp_obj_t bluetooth_ble_gatts_notify(size_t n_args, const mp_obj_t *args) {
    mp_int_t conn_handle = mp_obj_get_int(args[1]);
    mp_int_t value_handle = mp_obj_get_int(args[2]);

    if (n_args == 4) {
        mp_buffer_info_t bufinfo = {0};
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        size_t len = bufinfo.len;
        int err = mp_bluetooth_gatts_notify_send(conn_handle, value_handle, bufinfo.buf, &len);
        if (err != 0) {
            mp_raise_OSError(err);
        }
        return MP_OBJ_NEW_SMALL_INT(len);
    } else {
        int err = mp_bluetooth_gatts_notify(conn_handle, value_handle);
        return bluetooth_handle_errno(err);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gatts_notify_obj, 3, 4, bluetooth_ble_gatts_notify);

// ----------------------------------------------------------------------------
// Bluetooth object: GATTC (Central/Scanner role)
// ----------------------------------------------------------------------------

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

STATIC mp_obj_t bluetooth_ble_gattc_discover_services(mp_obj_t self_in, mp_obj_t conn_handle_in) {
    mp_int_t conn_handle = mp_obj_get_int(conn_handle_in);
    return bluetooth_handle_errno(mp_bluetooth_gattc_discover_primary_services(conn_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_ble_gattc_discover_services_obj, bluetooth_ble_gattc_discover_services);

STATIC mp_obj_t bluetooth_ble_gattc_discover_characteristics(size_t n_args, const mp_obj_t *args) {
    mp_int_t conn_handle = mp_obj_get_int(args[1]);
    mp_int_t start_handle = mp_obj_get_int(args[2]);
    mp_int_t end_handle = mp_obj_get_int(args[3]);
    return bluetooth_handle_errno(mp_bluetooth_gattc_discover_characteristics(conn_handle, start_handle, end_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gattc_discover_characteristics_obj, 4, 4, bluetooth_ble_gattc_discover_characteristics);

STATIC mp_obj_t bluetooth_ble_gattc_discover_descriptors(size_t n_args, const mp_obj_t *args) {
    mp_int_t conn_handle = mp_obj_get_int(args[1]);
    mp_int_t start_handle = mp_obj_get_int(args[2]);
    mp_int_t end_handle = mp_obj_get_int(args[3]);
    return bluetooth_handle_errno(mp_bluetooth_gattc_discover_descriptors(conn_handle, start_handle, end_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gattc_discover_descriptors_obj, 4, 4, bluetooth_ble_gattc_discover_descriptors);

STATIC mp_obj_t bluetooth_ble_gattc_read(mp_obj_t self_in, mp_obj_t conn_handle_in, mp_obj_t value_handle_in) {
    mp_int_t conn_handle = mp_obj_get_int(conn_handle_in);
    mp_int_t value_handle = mp_obj_get_int(value_handle_in);
    return bluetooth_handle_errno(mp_bluetooth_gattc_read(conn_handle, value_handle));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bluetooth_ble_gattc_read_obj, bluetooth_ble_gattc_read);

STATIC mp_obj_t bluetooth_ble_gattc_write(size_t n_args, const mp_obj_t *args) {
    mp_int_t conn_handle = mp_obj_get_int(args[1]);
    mp_int_t value_handle = mp_obj_get_int(args[2]);
    mp_obj_t data = args[3];
    mp_buffer_info_t bufinfo = {0};
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    size_t len = bufinfo.len;
    return bluetooth_handle_errno(mp_bluetooth_gattc_write(conn_handle, value_handle, bufinfo.buf, &len));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_ble_gattc_write_obj, 4, 4, bluetooth_ble_gattc_write);

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

// ----------------------------------------------------------------------------
// Bluetooth object: Definition
// ----------------------------------------------------------------------------

STATIC const mp_rom_map_elem_t bluetooth_ble_locals_dict_table[] = {
    // General
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&bluetooth_ble_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&bluetooth_ble_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&bluetooth_ble_irq_obj) },
    // GAP
    { MP_ROM_QSTR(MP_QSTR_gap_advertise), MP_ROM_PTR(&bluetooth_ble_gap_advertise_obj) },
    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    { MP_ROM_QSTR(MP_QSTR_gap_connect), MP_ROM_PTR(&bluetooth_ble_gap_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_gap_scan), MP_ROM_PTR(&bluetooth_ble_gap_scan_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_gap_disconnect), MP_ROM_PTR(&bluetooth_ble_gap_disconnect_obj) },
    // GATT Server (i.e. peripheral/advertiser role)
    { MP_ROM_QSTR(MP_QSTR_gatts_register_services), MP_ROM_PTR(&bluetooth_ble_gatts_register_services_obj) },
    { MP_ROM_QSTR(MP_QSTR_gatts_read), MP_ROM_PTR(&bluetooth_ble_gatts_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_gatts_write), MP_ROM_PTR(&bluetooth_ble_gatts_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_gatts_notify), MP_ROM_PTR(&bluetooth_ble_gatts_notify_obj) },
    #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
    // GATT Client (i.e. central/scanner role)
    { MP_ROM_QSTR(MP_QSTR_gattc_discover_services), MP_ROM_PTR(&bluetooth_ble_gattc_discover_services_obj) },
    { MP_ROM_QSTR(MP_QSTR_gattc_discover_characteristics), MP_ROM_PTR(&bluetooth_ble_gattc_discover_characteristics_obj) },
    { MP_ROM_QSTR(MP_QSTR_gattc_discover_descriptors), MP_ROM_PTR(&bluetooth_ble_gattc_discover_descriptors_obj) },
    { MP_ROM_QSTR(MP_QSTR_gattc_read), MP_ROM_PTR(&bluetooth_ble_gattc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_gattc_write), MP_ROM_PTR(&bluetooth_ble_gattc_write_obj) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(bluetooth_ble_locals_dict, bluetooth_ble_locals_dict_table);

STATIC const mp_obj_type_t bluetooth_ble_type = {
    { &mp_type_type },
    .name = MP_QSTR_BLE,
    .make_new = bluetooth_ble_make_new,
    .locals_dict = (void*)&bluetooth_ble_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_bluetooth_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ubluetooth) },
    { MP_ROM_QSTR(MP_QSTR_BLE), MP_ROM_PTR(&bluetooth_ble_type) },
    { MP_ROM_QSTR(MP_QSTR_UUID), MP_ROM_PTR(&bluetooth_uuid_type) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_READ), MP_ROM_INT(MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_WRITE), MP_ROM_INT(MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_NOTIFY), MP_ROM_INT(MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_bluetooth_globals, mp_module_bluetooth_globals_table);

const mp_obj_module_t mp_module_ubluetooth = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_bluetooth_globals,
};

// Helpers

#include <stdio.h>

STATIC void ringbuf_extract(ringbuf_t* ringbuf, mp_obj_tuple_t *data_tuple, size_t n_u16, size_t n_u8, mp_obj_str_t *bytes_addr, size_t n_b, size_t n_i8, mp_obj_bluetooth_uuid_t *uuid, mp_obj_str_t *bytes_data) {
    assert(ringbuf_avail(ringbuf) >= n_u16 * 2 + n_u8 + (bytes_addr ? 6 : 0) + n_b + n_i8 + (uuid ? 1 : 0) + (bytes_data ? 1 : 0));
    int j = 0;

    for (int i = 0; i < n_u16; ++i) {
        data_tuple->items[j++] = MP_OBJ_NEW_SMALL_INT(ringbuf_get16(ringbuf));
    }
    if (n_u8) {
        data_tuple->items[j++] = MP_OBJ_NEW_SMALL_INT(ringbuf_get(ringbuf));
    }
    if (bytes_addr) {
        bytes_addr->len = 6;
        for (int i = 0; i < bytes_addr->len; ++i) {
            // cast away const, this is actually bt->irq_addr_bytes.
            ((uint8_t*)bytes_addr->data)[i] = ringbuf_get(ringbuf);
        }
        data_tuple->items[j++] = MP_OBJ_FROM_PTR(bytes_addr);
    }
    if (n_b) {
        data_tuple->items[j++] = mp_obj_new_bool(ringbuf_get(ringbuf));
    }
    if (n_i8) {
        // Note the int8_t got packed into the ringbuf as a uint8_t.
        data_tuple->items[j++] = MP_OBJ_NEW_SMALL_INT((int8_t)ringbuf_get(ringbuf));
    }
    if (uuid) {
        ringbuf_get_uuid(ringbuf, uuid);
        data_tuple->items[j++] = MP_OBJ_FROM_PTR(uuid);
    }
    // The code that enqueues into the ringbuf should ensure that it doesn't
    // put more than MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_BYTES_LEN bytes into
    // the ringbuf.
    if (bytes_data) {
        bytes_data->len = ringbuf_get(ringbuf);
        for (int i = 0; i < bytes_data->len; ++i) {
            // cast away const, this is actually bt->irq_data_bytes.
            ((uint8_t*)bytes_data->data)[i] = ringbuf_get(ringbuf);
        }
        data_tuple->items[j++] = MP_OBJ_FROM_PTR(bytes_data);
    }

    data_tuple->len = j;
}

STATIC mp_obj_t bluetooth_ble_invoke_irq(mp_obj_t none_in) {
    // This is always executing in schedule context.
    for (;;) {
        MICROPY_PY_BLUETOOTH_ENTER
        mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));

        mp_int_t event = event = ringbuf_get16(&o->ringbuf);
        if (event < 0) {
            // Nothing available in ringbuf.
            MICROPY_PY_BLUETOOTH_EXIT
            break;
        }

        // Although we're in schedule context, this code still avoids using any allocations:
        //  - IRQs are disabled (to protect the ringbuf), and we need to avoid triggering GC
        //  - The user's handler might not alloc, so we shouldn't either.

        mp_obj_t handler = handler = o->irq_handler;
        mp_obj_tuple_t *data_tuple = MP_OBJ_TO_PTR(o->irq_data_tuple);

        // Some events need to pass bytes objects to their handler, using the
        // pre-allocated bytes array.
        mp_obj_str_t irq_data_bytes_addr = {{&mp_type_bytes}, 0, 6, o->irq_addr_bytes};
        mp_obj_str_t irq_data_bytes_data = {{&mp_type_bytes}, 0, 0, o->irq_data_bytes};

        if (event == MP_BLUETOOTH_IRQ_CENTRAL_CONNECT || event == MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT || event == MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT || event == MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT) {
            // conn_handle, addr_type, addr
            ringbuf_extract(&o->ringbuf, data_tuple, 1, 1, &irq_data_bytes_addr, 0, 0, NULL, NULL);
        } else if (event == MP_BLUETOOTH_IRQ_GATTS_WRITE) {
            // conn_handle, value_handle
            ringbuf_extract(&o->ringbuf, data_tuple, 2, 0, NULL, 0, 0, NULL, NULL);
        #if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
        } else if (event == MP_BLUETOOTH_IRQ_SCAN_RESULT) {
            // addr_type, addr, connectable, rssi, adv_data
            ringbuf_extract(&o->ringbuf, data_tuple, 0, 1, &irq_data_bytes_addr, 1, 1, NULL, &irq_data_bytes_data);
        } else if (event == MP_BLUETOOTH_IRQ_SCAN_COMPLETE) {
            // No params required.
            data_tuple->len = 0;
        } else if (event == MP_BLUETOOTH_IRQ_GATTC_SERVICE_RESULT) {
            // conn_handle, start_handle, end_handle, uuid
            ringbuf_extract(&o->ringbuf, data_tuple, 3, 0, NULL, 0, 0, MP_OBJ_TO_PTR(o->irq_data_uuid), NULL);
        } else if (event == MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_RESULT) {
            // conn_handle, def_handle, value_handle, properties, uuid
            ringbuf_extract(&o->ringbuf, data_tuple, 3, 1, NULL, 0, 0, MP_OBJ_TO_PTR(o->irq_data_uuid), NULL);
        } else if (event == MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_RESULT) {
            // conn_handle, handle, uuid
            ringbuf_extract(&o->ringbuf, data_tuple, 2, 0, NULL, 0, 0, MP_OBJ_TO_PTR(o->irq_data_uuid), NULL);
        } else if (event == MP_BLUETOOTH_IRQ_GATTC_READ_RESULT || event == MP_BLUETOOTH_IRQ_GATTC_NOTIFY || event == MP_BLUETOOTH_IRQ_GATTC_INDICATE) {
            // conn_handle, value_handle, data
            ringbuf_extract(&o->ringbuf, data_tuple, 2, 0, NULL, 0, 0, NULL, &irq_data_bytes_data);
        } else if (event == MP_BLUETOOTH_IRQ_GATTC_WRITE_STATUS) {
            // conn_handle, value_handle, status
            ringbuf_extract(&o->ringbuf, data_tuple, 3, 0, NULL, 0, 0, NULL, NULL);
        #endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
        }

        MICROPY_PY_BLUETOOTH_EXIT

        mp_call_function_2(handler, MP_OBJ_NEW_SMALL_INT(event), MP_OBJ_FROM_PTR(data_tuple));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluetooth_ble_invoke_irq_obj, bluetooth_ble_invoke_irq);

// ----------------------------------------------------------------------------
// Port API
// ----------------------------------------------------------------------------

// Callbacks are called in interrupt context (i.e. can't allocate), so we need to push the data
// into the ringbuf and schedule the callback via mp_sched_schedule.

STATIC bool enqueue_irq(mp_obj_bluetooth_ble_t *o, size_t len, uint16_t event, bool *sched) {
    *sched = false;

    if (o && ringbuf_free(&o->ringbuf) >= len + 2 && (o->irq_trigger & event) && o->irq_handler != mp_const_none) {
        *sched = ringbuf_avail(&o->ringbuf) == 0;
        ringbuf_put16(&o->ringbuf, event);
        return true;
    } else {
        return false;
    }
}

STATIC void schedule_ringbuf(bool sched) {
    if (sched) {
        mp_sched_schedule(MP_OBJ_FROM_PTR(MP_ROM_PTR(&bluetooth_ble_invoke_irq_obj)), mp_const_none);
    }
}

void mp_bluetooth_gap_on_connected_disconnected(uint16_t event, uint16_t conn_handle, uint8_t addr_type, const uint8_t *addr) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 1 + 6, event, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put(&o->ringbuf, addr_type);
        for (int i = 0; i < 6; ++i) {
            ringbuf_put(&o->ringbuf, addr[i]);
        }
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gatts_on_write(uint16_t conn_handle, uint16_t value_handle) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 2, MP_BLUETOOTH_IRQ_GATTS_WRITE, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, value_handle);
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
void mp_bluetooth_gap_on_scan_complete(void) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 0, MP_BLUETOOTH_IRQ_SCAN_COMPLETE, &sched)) {
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gap_on_scan_result(uint8_t addr_type, const uint8_t *addr, bool connectable, const int8_t rssi, const uint8_t *data, size_t data_len) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    data_len = MIN(MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_BYTES_LEN, data_len);
    if (enqueue_irq(o, 1 + 6 + 1 + 1 + 1 + data_len, MP_BLUETOOTH_IRQ_SCAN_RESULT, &sched)) {
        ringbuf_put(&o->ringbuf, addr_type);
        for (int i = 0; i < 6; ++i) {
            ringbuf_put(&o->ringbuf, addr[i]);
        }
        ringbuf_put(&o->ringbuf, connectable ? 1 : 0);
        // Note conversion of int8_t rssi to uint8_t. Must un-convert on the way out.
        ringbuf_put(&o->ringbuf, (uint8_t)rssi);
        ringbuf_put(&o->ringbuf, data_len);
        for (int i = 0; i < data_len; ++i) {
            ringbuf_put(&o->ringbuf, data[i]);
        }
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gattc_on_primary_service_result(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, mp_obj_bluetooth_uuid_t *service_uuid) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 2 + 2 + 1 + service_uuid->type, MP_BLUETOOTH_IRQ_GATTC_SERVICE_RESULT, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, start_handle);
        ringbuf_put16(&o->ringbuf, end_handle);
        ringbuf_put_uuid(&o->ringbuf, service_uuid);
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gattc_on_characteristic_result(uint16_t conn_handle, uint16_t def_handle, uint16_t value_handle, uint8_t properties, mp_obj_bluetooth_uuid_t *characteristic_uuid) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 2 + 2 + 1 + characteristic_uuid->type, MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_RESULT, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, def_handle);
        ringbuf_put16(&o->ringbuf, value_handle);
        ringbuf_put(&o->ringbuf, properties);
        ringbuf_put_uuid(&o->ringbuf, characteristic_uuid);
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gattc_on_descriptor_result(uint16_t conn_handle, uint16_t handle, mp_obj_bluetooth_uuid_t *descriptor_uuid) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 2 + 1 + descriptor_uuid->type, MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_RESULT, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, handle);
        ringbuf_put_uuid(&o->ringbuf, descriptor_uuid);
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gattc_on_data_available(uint16_t event, uint16_t conn_handle, uint16_t value_handle, const uint8_t *data, size_t data_len) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    data_len = MIN(MICROPY_PY_BLUETOOTH_MAX_EVENT_DATA_BYTES_LEN, data_len);
    if (enqueue_irq(o, 2 + 2 + 1 + data_len, event, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, value_handle);
        ringbuf_put(&o->ringbuf, data_len);
        for (int i = 0; i < data_len; ++i) {
            ringbuf_put(&o->ringbuf, data[i]);
        }
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

void mp_bluetooth_gattc_on_write_status(uint16_t conn_handle, uint16_t value_handle, uint16_t status) {
    MICROPY_PY_BLUETOOTH_ENTER
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    bool sched;
    if (enqueue_irq(o, 2 + 2 + 2, MP_BLUETOOTH_IRQ_GATTC_WRITE_STATUS, &sched)) {
        ringbuf_put16(&o->ringbuf, conn_handle);
        ringbuf_put16(&o->ringbuf, value_handle);
        ringbuf_put16(&o->ringbuf, status);
    }
    MICROPY_PY_BLUETOOTH_EXIT
    schedule_ringbuf(sched);
}

#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#if MICROPY_PY_BLUETOOTH_GATTS_ON_READ_CALLBACK
// This can only be enabled when the thread invoking this is a MicroPython thread.
// On ESP32, for example, this is not the case.
bool mp_bluetooth_gatts_on_read_request(uint16_t conn_handle, uint16_t value_handle) {
    mp_obj_bluetooth_ble_t *o = MP_OBJ_TO_PTR(MP_STATE_VM(bluetooth));
    if ((o->irq_trigger & MP_BLUETOOTH_IRQ_GATTS_READ_REQUEST) && o->irq_handler != mp_const_none) {
        // Use pre-allocated tuple because this is a hard IRQ.
        mp_obj_tuple_t *data = MP_OBJ_FROM_PTR(o->irq_data_tuple);
        data->items[0] = MP_OBJ_NEW_SMALL_INT(conn_handle);
        data->items[1] = MP_OBJ_NEW_SMALL_INT(value_handle);
        data->len = 2;
        mp_obj_t irq_ret = mp_call_function_2_protected(o->irq_handler, MP_OBJ_NEW_SMALL_INT(MP_BLUETOOTH_IRQ_GATTS_READ_REQUEST), o->irq_data_tuple);
        // If the IRQ handler explicitly returned false, then deny the read. Otherwise if it returns None/True, allow it.
        return irq_ret != MP_OBJ_NULL && (irq_ret == mp_const_none || mp_obj_is_true(irq_ret));
    } else {
        // No IRQ handler, allow the read.
        return true;
    }
}
#endif

#endif // MICROPY_PY_BLUETOOTH
