/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#include "py/obj.h"
#include "py/objstr.h"
#include "py/objarray.h"
#include "py/binary.h"
#include "py/runtime.h"
#include "py/qstr.h"
#include "extmod/modbluetooth.h"
#include <string.h>

#if MICROPY_PY_BLUETOOTH

STATIC const mp_obj_type_t bluetooth_type;
STATIC const mp_obj_type_t device_type;
STATIC const mp_obj_type_t service_type;
STATIC const mp_obj_type_t characteristic_type;

STATIC volatile uint16_t active_connections[MP_BT_MAX_CONNECTED_DEVICES];

// A device that has just connected/disconnected.
#if MICROPY_ENABLE_SCHEDULER
STATIC mp_bt_device_t global_device_object = {
    { &device_type },
};
#endif
STATIC uint8_t bt_event_trigger;

// This is a circular buffer of incoming packets.
// Packets are length-prefixed chunks of data in the circular buffer. The
// length is a single byte with the size of the packet excluding the
// length byte itself.
// The head/tail indices are like a normal ring buffer, except that they
// do not wrap around at UPDATE_BUF_SIZE but instead continue to increase
// in size. This means that the head/tail indices rely on unsigned
// wraparound. That is, if there is no data in the buffer head equals
// tail. If there is data in the queue, head is always ahead of tail
// modulo 2**16.
// If there is data in the buffer, this is the number of bytes currently
// in the buffer:
//   head - tail
// and this is the size of the first packet:
//   data[tail % UPDATE_BUF_SIZE]
// Similarly, head always points to the first unused byte (or the same
// byte if the buffer is exactly filled).
//
// Additionally, there is a counter of dropped packets. When packets are
// dropped, it are always the oldest packets. So by incrementing the count
// of dropped packets when the oldest packet is dropped, the next event
// that is handled knows that its packet was dropped due to a buffer
// overrun. This ensures that it is known exactly how many packets are
// dropped and the buffer can keep on accepting new packets.
//
#define UPDATE_BUF_SIZE 32
STATIC struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t dropped_packets;
    volatile uint8_t data[UPDATE_BUF_SIZE];
} update_buf;

typedef struct _mp_obj_bluetooth_t {
    mp_obj_base_t base;
} mp_obj_bluetooth_t;


// instantiated Bluetooth object
STATIC const mp_obj_bluetooth_t bluetooth_obj = {
    { &bluetooth_type },
};

// Easier (hopefully tail-called) error handling.
STATIC mp_obj_t bluetooth_handle_errno(int errno_) {
    if (errno_ != 0) {
        mp_raise_OSError(errno_);
    }
    return mp_const_none;
}

// Parse string UUIDs, which are probably 128-bit UUIDs.
void mp_bt_parse_uuid_str(mp_obj_t obj, uint8_t *uuid) {
    GET_STR_DATA_LEN(obj, str_data, str_len);
    int uuid_i = 32;
    for (int i = 0; i < str_len; i++) {
        char c = str_data[i];
        if (c == '-') {
            continue;
        }
        if (c >= '0' && c <= '9') {
            c = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            c = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            c = c - 'A' + 10;
        } else {
            mp_raise_ValueError("unknown char in UUID");
        }
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

// Format string UUID. Example output:
// '6e400001-b5a3-f393-e0a9-e50e24dcca9e'
mp_obj_t mp_bt_format_uuid_str(const uint8_t *uuid) {
    char str[36];
    char *s = str;
    for (int i = 15; i >= 0; i--) {
        char nibble = uuid[i] >> 4;
        if (nibble >= 10) {
            nibble += 'a' - 10;
        } else {
            nibble += '0';
        }
        *(s++) = nibble;

        nibble = uuid[i] & 0xf;
        if (nibble >= 10) {
            nibble += 'a' - 10;
        } else {
            nibble += '0';
        }
        *(s++) = nibble;

        if (i == 12 || i == 10 || i == 8 || i == 6) {
            *(s++) = '-';
        }
    }
    return mp_obj_new_str(str, MP_ARRAY_SIZE(str));
}

// Add this connection handle to the list of connected centrals.
void mp_bt_connected(uint16_t conn_handle, const uint8_t *address) {
    for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; i++) {
        if (active_connections[i] == MP_BT_INVALID_CONN_HANDLE) {
            active_connections[i] = conn_handle;
            break;
        }
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if ((bt_event_trigger & MP_BT_IRQ_CONNECT) != 0 && MP_STATE_PORT(bt_event_handler) != mp_const_none) {
        #if MICROPY_ENABLE_SCHEDULER
        global_device_object.conn_handle = conn_handle;
        memcpy(global_device_object.address, address, 6);
        mp_sched_schedule(MP_STATE_PORT(bt_event_handler), &global_device_object);
        #else
        mp_bt_device_t device = {0};
        device.base.type = &device_type;
        device.conn_handle = conn_handle;
        memcpy(device.address, address, 6);
        mp_call_function_1_protected(MP_STATE_PORT(bt_event_handler), &device);
        #endif
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

// Remove this connection handle from the list of connected centrals.
void mp_bt_disconnected(uint16_t conn_handle, const uint8_t *address) {
    for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; i++) {
        if (active_connections[i] == conn_handle) {
            active_connections[i] = MP_BT_INVALID_CONN_HANDLE;
            break;
        }
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (MP_STATE_PORT(bt_event_handler) != mp_const_none && (bt_event_trigger & MP_BT_IRQ_DISCONNECT) != 0) {
        #if MICROPY_ENABLE_SCHEDULER
        if (address != NULL) {
            memcpy(global_device_object.address, address, 6);
        } else {
            memset(global_device_object.address, 0, 6);
        }
        global_device_object.conn_handle = MP_BT_INVALID_CONN_HANDLE;
        mp_sched_schedule(MP_STATE_PORT(bt_event_handler), &global_device_object);
        #else
        mp_bt_device_t device = {0};
        device.base.type = &device_type;
        device.conn_handle = MP_BT_INVALID_CONN_HANDLE;
        if (address != NULL) {
            memcpy(device.address, address, 6);
        } else {
            memset(device.address, 0, 6);
        }
        mp_call_function_1_protected(MP_STATE_PORT(bt_event_handler), &device);
        #endif
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

STATIC mp_obj_t bluetooth_write_callback(mp_obj_t char_in) {
    mp_bt_characteristic_t *characteristic = char_in;

    // Copy the incoming buffer.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint8_t value[20]; // maximum BLE packet size
    uint16_t tail = update_buf.tail;
    size_t value_len;
    uint16_t conn_handle = MP_BT_INVALID_CONN_HANDLE;
    if (update_buf.dropped_packets) {
        // Handle dropped packet.
        update_buf.dropped_packets--;
        value_len = (size_t)-1;
    } else {
        // Copy regular incoming packet.
        size_t data_len = update_buf.data[tail++ % UPDATE_BUF_SIZE];
        value_len = data_len - 2;
        update_buf.tail = tail + data_len;
        if (value_len > sizeof(value)) {
            // Packet was too big, only pass the first N bytes.
            value_len = sizeof(value);
        }
        conn_handle = update_buf.data[tail++ % UPDATE_BUF_SIZE];
        conn_handle |= update_buf.data[tail++ % UPDATE_BUF_SIZE] << 8;
        for (size_t i = 0; i < value_len; i++) {
            value[i] = update_buf.data[tail++ % UPDATE_BUF_SIZE];
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    // Look for the callback in the linked list of callbacks.
    mp_bt_characteristic_callback_t *item = MP_STATE_PORT(bt_characteristic_callbacks);
    while (item != NULL && item->characteristic->value_handle != characteristic->value_handle) {
        item = item->next;
    }
    if (item == NULL) {
        // Callback has been removed?
        // This can happen when the callback is removed between the
        // interrupt and handling the interrupt.
        return mp_const_none;
    }

    if (value_len == (size_t)-1) {
        // Unfortunately, there was a dropped packet.
        // Report this event by passing None.
        mp_obj_t args[3] = {
            mp_const_none,
            MP_OBJ_FROM_PTR(item->characteristic),
            mp_const_none,
        };
        mp_call_function_n_kw(item->callback, 3, 0, args);
    } else {
        // Pass the written data directly as a bytearray to the callback.
        // WARNING: this array must not be modified by the callee.
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, value_len, value};
        mp_bt_device_t device = {0};
        device.base.type = &device_type;
        device.conn_handle = conn_handle;
        mp_obj_t args[3] = {
            &device,
            MP_OBJ_FROM_PTR(item->characteristic),
            MP_OBJ_FROM_PTR(&ar),
        };
        mp_call_function_n_kw(item->callback, 3, 0, args);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluetooth_write_callback_obj, bluetooth_write_callback);

// Call the registered callback for this characteristic, if one has been
// registered.
void mp_bt_characteristic_on_write(uint16_t conn_handle, uint16_t value_handle, const void *value, size_t value_len) {
    // Iterate through the linked list to find to find the characteristic
    // with the given handle.
    mp_bt_characteristic_callback_t *item = MP_STATE_PORT(bt_characteristic_callbacks);
    while (item != NULL) {
        if (item->characteristic->value_handle == value_handle) {
            if ((item->triggers & MP_BT_IRQ_WRITE) == 0) {
                // This callback should not be called for writes.
                break;
            }

            // Insert packet into queue.
            uint16_t head = update_buf.head;
            uint16_t tail = update_buf.tail;
            size_t bytes_left = ((uint16_t)UPDATE_BUF_SIZE - (head - tail));
            // A packet has the following components:
            //  - 1 byte packet size (excluding this byte)
            //  - 2 byte conn_handle
            //  - N bytes data
            size_t packet_len = value_len + 3;
            while (bytes_left < packet_len) {
                // Drop oldest packet.
                uint8_t packet_len = update_buf.data[tail % UPDATE_BUF_SIZE];
                tail += packet_len + 1;
                update_buf.tail = tail;
                bytes_left = ((uint16_t)UPDATE_BUF_SIZE - (head - tail));
                update_buf.dropped_packets++;
            }
            update_buf.data[head++ % UPDATE_BUF_SIZE] = (uint8_t)(packet_len - 1);
            update_buf.data[head++ % UPDATE_BUF_SIZE] = (uint8_t)(conn_handle & 0xff); // low bits
            update_buf.data[head++ % UPDATE_BUF_SIZE] = (uint8_t)(conn_handle >> 8);   // high bits
            for (size_t i = 0; i < value_len; i++) {
                update_buf.data[head++ % UPDATE_BUF_SIZE] = ((uint8_t*)value)[i];
            }
            update_buf.head = head;

            // Queue callback.
            #if MICROPY_ENABLE_SCHEDULER
            if (!mp_sched_schedule(MP_OBJ_FROM_PTR(&bluetooth_write_callback_obj), item->characteristic)) {
                // Failed to schedule a callback: the queue is full.
                // There's not much we can do now.
                return;
            }
            #else
            mp_call_function_1_protected(MP_OBJ_FROM_PTR(&bluetooth_write_callback_obj), item->characteristic);
            #endif

            return;
        }
        item = item->next;
    }
}

STATIC mp_obj_t bluetooth_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    return MP_OBJ_FROM_PTR(&bluetooth_obj);
}

STATIC mp_obj_t bluetooth_active(size_t n_args, const mp_obj_t *args) {
    if (n_args == 2) { // boolean enable/disable argument supplied
        if (mp_obj_is_true(args[1])) {
            for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; i++) {
                active_connections[i] = MP_BT_INVALID_CONN_HANDLE;
            }
            int errno_ = mp_bt_enable();
            if (errno_ != 0) {
                mp_raise_OSError(errno_);
            }
        } else {
            mp_bt_disable();
        }
    }
    return mp_obj_new_bool(mp_bt_is_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_active_obj, 1, 2, bluetooth_active);

STATIC mp_obj_t bluetooth_advertise(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_interval, ARG_name, ARG_adv_data, ARG_resp_data, ARG_connectable };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval,    MP_ARG_INT,                  {.u_int = 100} },
        { MP_QSTR_name,        MP_ARG_OBJ,                  {.u_obj = mp_const_none } },
        { MP_QSTR_adv_data,    MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_resp_data,   MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_connectable, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t interval = args[ARG_interval].u_int;
    if (interval == 0) {
        mp_bt_advertise_stop();
        return mp_const_none;
    }
    interval = interval * 8 / 5; // convert from 1ms to 0.625ms units
    if (interval < 0x20 || interval > 0x4000) {
        mp_raise_ValueError("interval out of range");
    }

    mp_bt_adv_type_t adv_type = MP_BT_ADV_TYPE_ADV_IND; // connectable=True
    if (!mp_obj_is_true(args[ARG_connectable].u_obj)) {
        adv_type = MP_BT_ADV_TYPE_ADV_NONCONN_IND; // connectable=False
    }

    uint8_t adv_data_buf[31];
    const uint8_t *adv_data = NULL;
    size_t adv_data_len = 0;

    // Pick advertisement data.
    if (args[ARG_name].u_obj != mp_const_none) {
        // Base the advertisement on the 'name' keyword argument.
        size_t name_len;
        const char *name = mp_obj_str_get_data(args[ARG_name].u_obj, &name_len);
        adv_data_buf[adv_data_len++] = 2; // 1 byte type + 1 byte flags data
        adv_data_buf[adv_data_len++] = MP_BLE_GAP_AD_TYPE_FLAG;
        adv_data_buf[adv_data_len++] = MP_BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

        if (name_len + 3 > sizeof(adv_data_buf) - adv_data_len) {
            mp_raise_ValueError("advertisement packet overflow");
        }
        adv_data_buf[adv_data_len++] = name_len + 1;
        adv_data_buf[adv_data_len++] = MP_BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        for (size_t i=0; i<name_len; i++) {
            adv_data_buf[adv_data_len++] = name[i];
        }
        adv_data = adv_data_buf;
    } else if (args[ARG_adv_data].u_obj != mp_const_none) {
        // Base the advertisement on the raw advertisement data field.
        adv_data = (const uint8_t*)mp_obj_str_get_data(args[ARG_adv_data].u_obj, &adv_data_len);
    }

    // Pick scan response data, if provided.
    const uint8_t *resp_data = NULL;
    size_t resp_data_len = 0;
    if (args[ARG_resp_data].u_obj != mp_const_none) {
        resp_data = (const uint8_t*)mp_obj_str_get_data(args[ARG_resp_data].u_obj, &resp_data_len);
    }

    int errno_ = mp_bt_advertise_start(adv_type, interval, adv_data_len ? adv_data : NULL, adv_data_len, resp_data, resp_data_len);
    return bluetooth_handle_errno(errno_);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_advertise_obj, 1, bluetooth_advertise);

STATIC mp_obj_t bluetooth_add_service(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_uuid, ARG_characteristics };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid,            MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_characteristics, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_list_t *characteristics = args[ARG_characteristics].u_obj;
    if (characteristics == NULL || !mp_obj_is_type(args[ARG_characteristics].u_obj, &mp_type_list)) {
        mp_raise_ValueError("characteristics must be a list");
    }
    for (int i = 0; i < characteristics->len; i++) {
        mp_obj_t characteristic = characteristics->items[i];
        if (characteristic == NULL || !mp_obj_is_type(characteristic, &characteristic_type)) {
            mp_raise_ValueError("not a Characteristic");
        }
        if (((mp_bt_characteristic_t*)characteristic)->service != NULL) {
            mp_raise_ValueError("Characteristic already added to Service");
        }
    }

    mp_bt_service_t *service = m_new_obj(mp_bt_service_t);
    service->base.type = &service_type;
    mp_bt_parse_uuid(args[ARG_uuid].u_obj, &service->uuid);
    int errno_ = mp_bt_add_service(service, characteristics->len, (mp_bt_characteristic_t**)characteristics->items);
    bluetooth_handle_errno(errno_);
    return service;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_add_service_obj, 1, bluetooth_add_service);

STATIC mp_obj_t bluetooth_config(mp_obj_t self_in, mp_obj_t param) {
    switch ((uintptr_t)param) {
    case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_mac): {
        uint8_t address[6];
        mp_bt_get_address(address);
        return mp_obj_new_bytes(address, MP_ARRAY_SIZE(address));
    }
    default:
        mp_raise_ValueError("unknown config param");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bluetooth_config_obj, bluetooth_config);

STATIC mp_obj_t bluetooth_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ|MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT|MP_ARG_REQUIRED, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t callback = args[ARG_handler].u_obj;
    if (callback != mp_const_none && !mp_obj_is_fun(callback)) {
        mp_raise_ValueError("invalid callback");
    }

    // Update the callback.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    MP_STATE_PORT(bt_event_handler) = callback;
    bt_event_trigger = args[ARG_trigger].u_int;
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_irq_obj, 1, bluetooth_irq);

STATIC mp_obj_t device_config(mp_obj_t self_in, mp_obj_t param) {
    mp_bt_device_t *device = self_in;
    switch ((uintptr_t)param) {
    case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_mac): {
        bool has_address = false;
        for (int i = 0; i < 6; i++) {
            if (device->address[i] != 0) {
                has_address = true;
            }
        }
        if (has_address) {
            return mp_obj_new_bytes(device->address, 6);
        } else {
            return mp_const_none;
        }
    }
    default:
        mp_raise_ValueError("unknown config param");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(device_config_obj, device_config);

STATIC mp_obj_t device_connected(mp_obj_t self_in) {
    mp_bt_device_t *device = self_in;
    return mp_obj_new_bool(device->conn_handle != MP_BT_INVALID_CONN_HANDLE);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(device_connected_obj, device_connected);

STATIC mp_obj_t device_disconnect(mp_obj_t self_in) {
    mp_bt_device_t *device = self_in;
    if (device->conn_handle != MP_BT_INVALID_CONN_HANDLE) {
        uint16_t conn_handle = device->conn_handle;
        device->conn_handle = MP_BT_INVALID_CONN_HANDLE;
        int errno_ = mp_bt_device_disconnect(conn_handle);
        return bluetooth_handle_errno(errno_);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(device_disconnect_obj, device_disconnect);

STATIC const mp_rom_map_elem_t device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_config),     MP_ROM_PTR(&device_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_connected),  MP_ROM_PTR(&device_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&device_disconnect_obj) },
};
STATIC MP_DEFINE_CONST_DICT(device_locals_dict, device_locals_dict_table);

STATIC const mp_obj_type_t device_type = {
    { &mp_type_type },
    .name = MP_QSTR_Device,
    .locals_dict = (void*)&device_locals_dict,
};

STATIC mp_obj_t service_uuid(mp_obj_t self_in) {
    mp_bt_service_t *service = self_in;
    return mp_bt_format_uuid(&service->uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(service_uuid_obj, service_uuid);

STATIC const mp_rom_map_elem_t service_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_uuid), MP_ROM_PTR(&service_uuid_obj) },
};
STATIC MP_DEFINE_CONST_DICT(service_locals_dict, service_locals_dict_table);

STATIC const mp_obj_type_t service_type = {
    { &mp_type_type },
    .name = MP_QSTR_Service,
    .locals_dict = (void*)&service_locals_dict,
};

STATIC mp_obj_t characteristic_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_uuid, ARG_flags };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid,  MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_flags, MP_ARG_INT | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((uint8_t)(args[ARG_flags].u_int) != args[ARG_flags].u_int) {
        // Flags don't fit in 8 bits.
        mp_raise_ValueError("invalid flags");
    }

    mp_bt_characteristic_t *characteristic = m_new_obj(mp_bt_characteristic_t);
    characteristic->base.type = &characteristic_type;
    mp_bt_parse_uuid(args[0].u_obj, &characteristic->uuid);
    characteristic->flags = (uint8_t)(args[ARG_flags].u_int);
    return characteristic;
}

STATIC mp_obj_t characteristic_service(mp_obj_t self_in) {
    mp_bt_characteristic_t *characteristic = self_in;
    if (characteristic->service == NULL) {
        return mp_const_none;
    }
    return characteristic->service;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(characteristic_service_obj, characteristic_service);

STATIC mp_obj_t characteristic_uuid(mp_obj_t self_in) {
    mp_bt_characteristic_t *characteristic = self_in;
    return mp_bt_format_uuid(&characteristic->uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(characteristic_uuid_obj, characteristic_uuid);

STATIC mp_obj_t characteristic_write(mp_obj_t self_in, mp_obj_t value_in) {
    mp_bt_characteristic_t *characteristic = self_in;
    GET_STR_DATA_LEN(value_in, str_data, str_len);
    int errno_;
    if ((characteristic->flags & MP_BLE_FLAG_NOTIFY) != 0) {
        bool updated = false;
        for (size_t i = 0; i < MP_BT_MAX_CONNECTED_DEVICES; i++) {
            uint16_t conn_handle = active_connections[i];
            if (conn_handle == MP_BT_INVALID_CONN_HANDLE) {
                continue;
            }
            errno_ = mp_bt_characteristic_value_notify(characteristic, conn_handle, str_data, str_len);
            if (errno_ != 0) {
                break;
            }
            updated = true;
        }
        if (!updated) {
            errno_ = mp_bt_characteristic_value_set(characteristic, str_data, str_len);
        }
    } else {
        errno_ = mp_bt_characteristic_value_set(characteristic, str_data, str_len);
    }
    return bluetooth_handle_errno(errno_);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(characteristic_write_obj, characteristic_write);

STATIC mp_obj_t characteristic_read(mp_obj_t self_in) {
    mp_bt_characteristic_t *characteristic = self_in;
    uint8_t data[MP_BT_MAX_ATTR_SIZE];
    size_t value_len = MP_BT_MAX_ATTR_SIZE;
    int errno_ = mp_bt_characteristic_value_get(characteristic, data, &value_len);
    if (errno_ != 0) {
        mp_raise_OSError(errno_);
    }
    return mp_obj_new_bytes(data, value_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(characteristic_read_obj, characteristic_read);

STATIC mp_obj_t characteristic_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ|MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT|MP_ARG_REQUIRED, {.u_int = 0} },
    };
    mp_bt_characteristic_t *characteristic = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t callback = args[ARG_handler].u_obj;
    if (callback != mp_const_none && !mp_obj_is_fun(callback)) {
        mp_raise_ValueError("invalid callback");
    }

    // A singly linked list of callbacks. In pseudocode:
    // If the new callback is none:
    //   Find a registered callback for this characteristic and remove it.
    // Else:
    //   Replace a registered callback for this characteristic.
    //   If none exists, add it at the end of the list.
    mp_bt_characteristic_callback_t **entry = &MP_STATE_PORT(bt_characteristic_callbacks);
    while (1) {
        if (*entry == NULL) {
            // found the end of the list
            if (callback != mp_const_none) {
                // add callback to the end of the list
                *entry = m_new_obj(mp_bt_characteristic_callback_t);
                (*entry)->characteristic = characteristic;
                (*entry)->callback = callback;
                (*entry)->triggers = args[ARG_trigger].u_int;
            }
            break;
        }
        if ((*entry)->characteristic == characteristic) {
            // found existing entry
            if (callback == mp_const_none) {
                // delete this callback
                *entry = (*entry)->next;
            } else {
                // update the entry with the new callback
                (*entry)->callback = callback;
                (*entry)->triggers = args[ARG_trigger].u_int;
            }
            break;
        }
        entry = &(*entry)->next;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(characteristic_irq_obj, 1, characteristic_irq);

STATIC const mp_rom_map_elem_t characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_service), MP_ROM_PTR(&characteristic_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),    MP_ROM_PTR(&characteristic_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),   MP_ROM_PTR(&characteristic_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),    MP_ROM_PTR(&characteristic_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),     MP_ROM_PTR(&characteristic_irq_obj) },
};
STATIC MP_DEFINE_CONST_DICT(characteristic_locals_dict, characteristic_locals_dict_table);

STATIC const mp_obj_type_t characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .make_new = characteristic_make_new,
    .locals_dict = (void*)&characteristic_locals_dict,
};

STATIC const mp_rom_map_elem_t bluetooth_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),        MP_ROM_PTR(&bluetooth_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertise),     MP_ROM_PTR(&bluetooth_advertise_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_service),   MP_ROM_PTR(&bluetooth_add_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),        MP_ROM_PTR(&bluetooth_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),           MP_ROM_PTR(&bluetooth_irq_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bluetooth_locals_dict, bluetooth_locals_dict_table);

STATIC const mp_obj_type_t bluetooth_type = {
    { &mp_type_type },
    .name = MP_QSTR_Bluetooth,
    .make_new = bluetooth_make_new,
    .locals_dict = (void*)&bluetooth_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_bluetooth_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),       MP_ROM_QSTR(MP_QSTR_bluetooth) },
    { MP_ROM_QSTR(MP_QSTR_Bluetooth),      MP_ROM_PTR(&bluetooth_type) },
    { MP_ROM_QSTR(MP_QSTR_Device),         MP_ROM_PTR(&device_type) },
    { MP_ROM_QSTR(MP_QSTR_Service),        MP_ROM_PTR(&service_type) },
    { MP_ROM_QSTR(MP_QSTR_Characteristic), MP_ROM_PTR(&characteristic_type) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_READ),      MP_ROM_INT(MP_BLE_FLAG_READ) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_WRITE),     MP_ROM_INT(MP_BLE_FLAG_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_FLAG_NOTIFY),    MP_ROM_INT(MP_BLE_FLAG_NOTIFY) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_CONNECT),    MP_ROM_INT(MP_BT_IRQ_CONNECT) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_DISCONNECT), MP_ROM_INT(MP_BT_IRQ_DISCONNECT) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_WRITE),      MP_ROM_INT(MP_BT_IRQ_WRITE) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_bluetooth_globals, mp_module_bluetooth_globals_table);

const mp_obj_module_t mp_module_bluetooth = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_bluetooth_globals,
};

#endif //MICROPY_PY_BLUETOOTH
