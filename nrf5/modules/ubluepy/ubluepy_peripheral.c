/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objlist.h"

#if MICROPY_PY_UBLUEPY

#include "ble_drv.h"

STATIC void ubluepy_peripheral_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_peripheral_obj_t * self = (ubluepy_peripheral_obj_t *)o;
    (void)self;
    mp_printf(print, "Peripheral");
}

STATIC void gap_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (event_id == 16) {                // connect event
        self->conn_handle = conn_handle;
    } else if (event_id == 17) {         // disconnect event
        self->conn_handle = 0xFFFF;      // invalid connection handle
    }

    if (self->conn_handler != mp_const_none) {
        mp_obj_t args[3];
        mp_uint_t num_of_args = 3;
        args[0] = MP_OBJ_NEW_SMALL_INT(event_id);
        args[1] = MP_OBJ_NEW_SMALL_INT(length);
        if (data != NULL) {
            args[2] = mp_obj_new_bytearray_by_ref(length, data);
        } else {
            args[2] = mp_const_none;
        }

        // for now hard-code all events to conn_handler
        mp_call_function_n_kw(self->conn_handler, num_of_args, 0, args);
    }

    (void)self;
}

STATIC void gatts_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->conn_handler != mp_const_none) {
        mp_obj_t args[3];
        mp_uint_t num_of_args = 3;
        args[0] = MP_OBJ_NEW_SMALL_INT(event_id);
        args[1] = MP_OBJ_NEW_SMALL_INT(length);
        if (data != NULL) {
            args[2] = mp_obj_new_bytearray_by_ref(length, data);
        } else {
            args[2] = mp_const_none;
        }

        // for now hard-code all events to conn_handler
        mp_call_function_n_kw(self->conn_handler, num_of_args, 0, args);
    }

}

STATIC mp_obj_t ubluepy_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_NEW_DEVICE_ADDR,
        ARG_NEW_ADDR_TYPE
    };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_DEVICE_ADDR, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { ARG_NEW_ADDR_TYPE,   MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_peripheral_obj_t *s = m_new_obj(ubluepy_peripheral_obj_t);
    s->base.type = type;

    ble_drv_gap_event_handler_set(MP_OBJ_FROM_PTR(s), gap_event_handler);
    ble_drv_gatts_event_handler_set(MP_OBJ_FROM_PTR(s), gatts_event_handler);

    s->delegate      = mp_const_none;
    s->conn_handler  = mp_const_none;
    s->notif_handler = mp_const_none;
    s->conn_handle   = 0xFFFF;

    s->service_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}

/// \method withDelegate(DefaultDelegate)
/// Set delegate instance for handling Bluetooth LE events.
///
STATIC mp_obj_t peripheral_with_delegate(mp_obj_t self_in, mp_obj_t delegate) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->delegate = delegate;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_peripheral_with_delegate_obj, peripheral_with_delegate);

/// \method setNotificationHandler(func)
/// Set handler for Bluetooth LE notification events.
///
STATIC mp_obj_t peripheral_set_notif_handler(mp_obj_t self_in, mp_obj_t func) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->notif_handler = func;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_peripheral_set_notif_handler_obj, peripheral_set_notif_handler);

/// \method setConnectionHandler(func)
/// Set handler for Bluetooth LE connection events.
///
STATIC mp_obj_t peripheral_set_conn_handler(mp_obj_t self_in, mp_obj_t func) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->conn_handler = func;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_peripheral_set_conn_handler_obj, peripheral_set_conn_handler);

/// \method advertise(device_name, [service=[service1, service2, ...]], [data=bytearray])
/// Start advertising.
///
STATIC mp_obj_t peripheral_advertise(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device_name, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_services,    MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_data,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t device_name_obj = args[0].u_obj;
    mp_obj_t service_obj     = args[1].u_obj;
    mp_obj_t data_obj        = args[2].u_obj;

    ubluepy_advertise_data_t adv_data;
    memset(&adv_data, 0, sizeof(ubluepy_advertise_data_t));

    if (device_name_obj != mp_const_none && MP_OBJ_IS_STR(device_name_obj)) {
        GET_STR_DATA_LEN(device_name_obj, str_data, str_len);

        adv_data.p_device_name = (uint8_t *)str_data;
        adv_data.device_name_len = str_len;
    }

    if (service_obj != mp_const_none) {
        mp_obj_t * services = NULL;
        mp_uint_t  num_services;
        mp_obj_get_array(service_obj, &num_services, &services);

        if (num_services > 0) {
            adv_data.p_services      = services;
            adv_data.num_of_services = num_services;
        }
    }

    if (data_obj != mp_const_none) {

    }

    (void)ble_drv_advertise_data(&adv_data);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ubluepy_peripheral_advertise_obj, 1, peripheral_advertise);

/// \method disconnect()
/// disconnect connection.
///
STATIC mp_obj_t peripheral_disconnect(mp_obj_t self_in) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_peripheral_disconnect_obj, peripheral_disconnect);

/// \method addService(Service)
/// Add service to the Peripheral.
///
STATIC mp_obj_t peripheral_add_service(mp_obj_t self_in, mp_obj_t service) {
    ubluepy_peripheral_obj_t * self = MP_OBJ_TO_PTR(self_in);
    ubluepy_service_obj_t    * p_service = MP_OBJ_TO_PTR(service);

    p_service->p_periph = self;

    mp_obj_list_append(self->service_list, service);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_peripheral_add_service_obj, peripheral_add_service);

/// \method getServices()
/// Return list with all service registered in the Peripheral.
///
STATIC mp_obj_t peripheral_get_services(mp_obj_t self_in) {
    ubluepy_peripheral_obj_t * self = MP_OBJ_TO_PTR(self_in);

    return self->service_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_peripheral_get_services_obj, peripheral_get_services);


STATIC const mp_map_elem_t ubluepy_peripheral_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_withDelegate),           (mp_obj_t)(&ubluepy_peripheral_with_delegate_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setNotificationHandler), (mp_obj_t)(&ubluepy_peripheral_set_notif_handler_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setConnectionHandler),   (mp_obj_t)(&ubluepy_peripheral_set_conn_handler_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getServices),            (mp_obj_t)(&ubluepy_peripheral_get_services_obj) },
#if MICROPY_PY_UBLUEPY_CENTRAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),                (mp_obj_t)(&ubluepy_peripheral_connect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),             (mp_obj_t)(&ubluepy_peripheral_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getServiceByUUID),       (mp_obj_t)(&ubluepy_peripheral_get_service_by_uuid_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getCharacteristics),     (mp_obj_t)(&ubluepy_peripheral_get_chars_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getDescriptors),         (mp_obj_t)(&ubluepy_peripheral_get_descs_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_waitForNotifications),   (mp_obj_t)(&ubluepy_peripheral_wait_for_notif_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCharacteristic),    (mp_obj_t)(&ubluepy_peripheral_write_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCharacteristic),     (mp_obj_t)(&ubluepy_peripheral_read_char_obj) },
#endif
#if MICROPY_PY_UBLUEPY_PERIPHERAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),              (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),             (mp_obj_t)(&ubluepy_peripheral_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_addService),             (mp_obj_t)(&ubluepy_peripheral_add_service_obj) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_addCharacteristic),      (mp_obj_t)(&ubluepy_peripheral_add_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_addDescriptor),          (mp_obj_t)(&ubluepy_peripheral_add_desc_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCharacteristic),    (mp_obj_t)(&ubluepy_peripheral_write_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCharacteristic),     (mp_obj_t)(&ubluepy_peripheral_read_char_obj) },
#endif
#endif
#if MICROPY_PY_UBLUEPY_BROADCASTER
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),            (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
#endif
#if MICROPY_PY_UBLUEPY_OBSERVER
	// Nothing yet.
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_peripheral_locals_dict, ubluepy_peripheral_locals_dict_table);

const mp_obj_type_t ubluepy_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .print = ubluepy_peripheral_print,
    .make_new = ubluepy_peripheral_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_peripheral_locals_dict
};

#endif // MICROPY_PY_UBLUEPY
