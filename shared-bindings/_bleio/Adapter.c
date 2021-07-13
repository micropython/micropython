/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Adapter.h"

#define ADV_INTERVAL_MIN (0.02f)
#define ADV_INTERVAL_MIN_STRING "0.02"
#define ADV_INTERVAL_MAX (10.24f)
#define ADV_INTERVAL_MAX_STRING "10.24"
// 20ms is recommended by Apple
#define ADV_INTERVAL_DEFAULT (0.1f)

#define INTERVAL_DEFAULT (0.1f)
#define INTERVAL_MIN (0.0025f)
#define INTERVAL_MIN_STRING "0.0025"
#define INTERVAL_MAX (40.959375f)
#define INTERVAL_MAX_STRING "40.959375"
#define WINDOW_DEFAULT (0.1f)

//| class Adapter:
//|     """
//|     The BLE Adapter object manages the discovery and connection to other nearby Bluetooth Low Energy devices.
//|     This part of the Bluetooth Low Energy Specification is known as Generic Access Profile (GAP).
//|
//|     Discovery of other devices happens during a scanning process that listens for small packets of
//|     information, known as advertisements, that are broadcast unencrypted. The advertising packets
//|     have two different uses. The first is to broadcast a small piece of data to anyone who cares and
//|     and nothing more. These are known as beacons. The second class of advertisement is to promote
//|     additional functionality available after the devices establish a connection. For example, a
//|     BLE heart rate monitor would advertise that it provides the standard BLE Heart Rate Service.
//|
//|     The Adapter can do both parts of this process: it can scan for other device
//|     advertisements and it can advertise its own data. Furthermore, Adapters can accept incoming
//|     connections and also initiate connections."""
//|

//|     def __init__(self, *, uart: busio.UART, rts: digitalio.DigitalInOut, cts: digitalio.DigitalInOut) -> None:
//|         """On boards that do not have native BLE, you can use an HCI co-processor.
//|         Pass the uart and pins used to communicate with the co-processor, such as an Adafruit AirLift.
//|         The co-processor must have been reset and put into BLE mode beforehand
//|         by the appropriate pin manipulation.
//|         The ``uart``, ``rts``, and ``cts`` objects are used to
//|         communicate with the HCI co-processor in HCI mode.
//|         The `Adapter` object is enabled during this call.
//|
//|         After instantiating an Adapter, call `_bleio.set_adapter()` to set `_bleio.adapter`
//|
//|         On boards with native BLE, you cannot create an instance of `_bleio.Adapter`;
//|         this constructor will raise `NotImplementedError`.
//|         Use `_bleio.adapter` to access the sole instance already available.
//|         """
//|         ...
//|
STATIC mp_obj_t bleio_adapter_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if CIRCUITPY_BLEIO_HCI
    bleio_adapter_obj_t *self = common_hal_bleio_allocate_adapter_or_raise();

    enum { ARG_uart, ARG_rts, ARG_cts };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uart, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    busio_uart_obj_t *uart = args[ARG_uart].u_obj;
    if (!mp_obj_is_type(uart, &busio_uart_type)) {
        mp_raise_ValueError(translate("Expected a UART"));
    }

    digitalio_digitalinout_obj_t *rts = args[ARG_rts].u_obj;
    digitalio_digitalinout_obj_t *cts = args[ARG_cts].u_obj;
    if (!mp_obj_is_type(rts, &digitalio_digitalinout_type) ||
        !mp_obj_is_type(cts, &digitalio_digitalinout_type)) {
        mp_raise_ValueError(translate("Expected a DigitalInOut"));
    }

    // Will enable the adapter.
    common_hal_bleio_adapter_construct_hci_uart(self, uart, rts, cts);

    return MP_OBJ_FROM_PTR(self);
    #else
    mp_raise_NotImplementedError(translate("Cannot create a new Adapter; use _bleio.adapter;"));
    return mp_const_none;
    #endif // CIRCUITPY_BLEIO_HCI
}

//|
//|     enabled: bool
//|     """State of the BLE adapter."""
//|
STATIC mp_obj_t bleio_adapter_get_enabled(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_bleio_adapter_get_enabled(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_enabled_obj, bleio_adapter_get_enabled);

static mp_obj_t bleio_adapter_set_enabled(mp_obj_t self, mp_obj_t value) {
    const bool enabled = mp_obj_is_true(value);

    common_hal_bleio_adapter_set_enabled(self, enabled);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_adapter_set_enabled_obj, bleio_adapter_set_enabled);

const mp_obj_property_t bleio_adapter_enabled_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_enabled_obj,
               (mp_obj_t)&bleio_adapter_set_enabled_obj,
               MP_ROM_NONE },
};

//|     address: Address
//|     """MAC address of the BLE adapter."""
//|
STATIC mp_obj_t bleio_adapter_get_address(mp_obj_t self) {
    return MP_OBJ_FROM_PTR(common_hal_bleio_adapter_get_address(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_address_obj, bleio_adapter_get_address);

STATIC mp_obj_t bleio_adapter_set_address(mp_obj_t self, mp_obj_t new_address) {
    if (!common_hal_bleio_adapter_set_address(self, new_address)) {
        mp_raise_bleio_BluetoothError(translate("Could not set address"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(bleio_adapter_set_address_obj, bleio_adapter_set_address);

const mp_obj_property_t bleio_adapter_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_address_obj,
               (mp_obj_t)&bleio_adapter_set_address_obj,
               MP_ROM_NONE },
};

//|     name: str
//|     """name of the BLE adapter used once connected.
//|     The name is "CIRCUITPY" + the last four hex digits of ``adapter.address``,
//|     to make it easy to distinguish multiple CircuitPython boards."""
//|
STATIC mp_obj_t bleio_adapter_get_name(mp_obj_t self) {
    return MP_OBJ_FROM_PTR(common_hal_bleio_adapter_get_name(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_name_obj, bleio_adapter_get_name);


STATIC mp_obj_t bleio_adapter_set_name(mp_obj_t self, mp_obj_t new_name) {
    common_hal_bleio_adapter_set_name(self, mp_obj_str_get_str(new_name));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(bleio_adapter_set_name_obj, bleio_adapter_set_name);

const mp_obj_property_t bleio_adapter_name_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_name_obj,
               (mp_obj_t)&bleio_adapter_set_name_obj,
               MP_ROM_NONE },
};

//|     def start_advertising(self, data: ReadableBuffer, *,
//|                           scan_response: Optional[ReadableBuffer] = None, connectable: bool = True,
//|                           anonymous: bool = False, timeout: int = 0, interval: float = 0.1,
//|                           tx_power: int = 0, directed_to: Optional[Address] = None) -> None:
//|         """Starts advertising until `stop_advertising` is called or if connectable, another device
//|         connects to us.
//|
//|         .. warning: If data is longer than 31 bytes, then this will automatically advertise as an
//|            extended advertisement that older BLE 4.x clients won't be able to scan for.
//|
//|         .. note: If you set ``anonymous=True``, then a timeout must be specified. If no timeout is
//|            specified, then the maximum allowed timeout will be selected automatically.
//|
//|         :param ~_typing.ReadableBuffer data: advertising data packet bytes
//|         :param ~_typing.ReadableBuffer scan_response: scan response data packet bytes. ``None`` if no scan response is needed.
//|         :param bool connectable:  If `True` then other devices are allowed to connect to this peripheral.
//|         :param bool anonymous:  If `True` then this device's MAC address is randomized before advertising.
//|         :param int timeout:  If set, we will only advertise for this many seconds. Zero means no timeout.
//|         :param float interval:  advertising interval, in seconds
//|         :param tx_power int: transmitter power while advertising in dBm
//|         :param directed_to Address: peer to advertise directly to"""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_start_advertising(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_data, ARG_scan_response, ARG_connectable, ARG_anonymous, ARG_timeout, ARG_interval, ARG_tx_power, ARG_directed_to };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scan_response, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_connectable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_anonymous, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_tx_power, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_directed_to, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t data_bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_bufinfo, MP_BUFFER_READ);

    // Pass an empty buffer if scan_response not provided.
    mp_buffer_info_t scan_response_bufinfo = { 0 };
    if (args[ARG_scan_response].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_scan_response].u_obj, &scan_response_bufinfo, MP_BUFFER_READ);
    }

    if (args[ARG_interval].u_obj == MP_OBJ_NULL) {
        args[ARG_interval].u_obj = mp_obj_new_float(ADV_INTERVAL_DEFAULT);
    }

    const mp_float_t interval = mp_obj_get_float(args[ARG_interval].u_obj);
    if (interval < ADV_INTERVAL_MIN || interval > ADV_INTERVAL_MAX) {
        mp_raise_ValueError_varg(translate("interval must be in range %s-%s"),
            ADV_INTERVAL_MIN_STRING, ADV_INTERVAL_MAX_STRING);
    }

    bool connectable = args[ARG_connectable].u_bool;
    bool anonymous = args[ARG_anonymous].u_bool;
    uint32_t timeout = args[ARG_timeout].u_int;
    if (data_bufinfo.len > 31 && connectable && scan_response_bufinfo.len > 0) {
        mp_raise_bleio_BluetoothError(translate("Cannot have scan responses for extended, connectable advertisements."));
    }

    bleio_address_obj_t *address = MP_OBJ_TO_PTR(args[ARG_directed_to].u_obj);
    if (address != NULL && !connectable) {
        mp_raise_bleio_BluetoothError(translate("Only connectable advertisements can be directed"));
    }

    if (address != NULL && !mp_obj_is_type(address, &bleio_address_type)) {
        mp_raise_TypeError(translate("Expected an Address"));
    }

    common_hal_bleio_adapter_start_advertising(self, connectable, anonymous, timeout, interval,
        &data_bufinfo, &scan_response_bufinfo, args[ARG_tx_power].u_int, address);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_adapter_start_advertising_obj, 2, bleio_adapter_start_advertising);

//|     def stop_advertising(self) -> None:
//|         """Stop sending advertising packets."""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_stop_advertising(mp_obj_t self_in) {
    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_adapter_stop_advertising(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_stop_advertising_obj, bleio_adapter_stop_advertising);

//|     def start_scan(self, prefixes: ReadableBuffer = b"", *, buffer_size: int = 512, extended: bool = False, timeout: Optional[float] = None, interval: float = 0.1, window: float = 0.1, minimum_rssi: int = -80, active: bool = True) -> Iterable[ScanEntry]:
//|         """Starts a BLE scan and returns an iterator of results. Advertisements and scan responses are
//|         filtered and returned separately.
//|
//|         :param ~_typing.ReadableBuffer prefixes: Sequence of byte string prefixes to filter advertising packets
//|             with. A packet without an advertising structure that matches one of the prefixes is
//|             ignored. Format is one byte for length (n) and n bytes of prefix and can be repeated.
//|         :param int buffer_size: the maximum number of advertising bytes to buffer.
//|         :param bool extended: When True, support extended advertising packets. Increasing buffer_size is recommended when this is set.
//|         :param float timeout: the scan timeout in seconds. If None or zero, will scan until `stop_scan` is called.
//|         :param float interval: the interval (in seconds) between the start of two consecutive scan windows
//|            Must be in the range 0.0025 - 40.959375 seconds.
//|         :param float window: the duration (in seconds) to scan a single BLE channel.
//|            window must be <= interval.
//|         :param int minimum_rssi: the minimum rssi of entries to return.
//|         :param bool active: retrieve scan responses for scannable advertisements.
//|         :returns: an iterable of `_bleio.ScanEntry` objects
//|         :rtype: iterable"""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_start_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_prefixes, ARG_buffer_size, ARG_extended, ARG_timeout, ARG_interval, ARG_window, ARG_minimum_rssi, ARG_active };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_prefixes,  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_size,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 512} },
        { MP_QSTR_extended,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_window,   MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_minimum_rssi,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -80} },
        { MP_QSTR_active,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t timeout = 0.0f;
    if (args[ARG_timeout].u_obj != mp_const_none) {
        timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    }

    if (args[ARG_interval].u_obj == MP_OBJ_NULL) {
        args[ARG_interval].u_obj = mp_obj_new_float(INTERVAL_DEFAULT);
    }

    if (args[ARG_window].u_obj == MP_OBJ_NULL) {
        args[ARG_window].u_obj = mp_obj_new_float(WINDOW_DEFAULT);
    }

    const mp_float_t interval = mp_obj_get_float(args[ARG_interval].u_obj);
    if (interval < INTERVAL_MIN || interval > INTERVAL_MAX) {
        mp_raise_ValueError_varg(translate("interval must be in range %s-%s"), INTERVAL_MIN_STRING, INTERVAL_MAX_STRING);
    }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
    if (timeout != 0.0f && timeout < interval) {
        mp_raise_ValueError(translate("non-zero timeout must be >= interval"));
    }
    #pragma GCC diagnostic pop

    const mp_float_t window = mp_obj_get_float(args[ARG_window].u_obj);
    if (window > interval) {
        mp_raise_ValueError(translate("window must be <= interval"));
    }

    mp_buffer_info_t prefix_bufinfo;
    prefix_bufinfo.len = 0;
    if (args[ARG_prefixes].u_obj != MP_OBJ_NULL) {
        mp_get_buffer_raise(args[ARG_prefixes].u_obj, &prefix_bufinfo, MP_BUFFER_READ);
        if (gc_nbytes(prefix_bufinfo.buf) == 0) {
            mp_raise_ValueError(translate("Prefix buffer must be on the heap"));
        }
    }

    return common_hal_bleio_adapter_start_scan(self, prefix_bufinfo.buf, prefix_bufinfo.len, args[ARG_extended].u_bool, args[ARG_buffer_size].u_int, timeout, interval, window, args[ARG_minimum_rssi].u_int, args[ARG_active].u_bool);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_adapter_start_scan_obj, 1, bleio_adapter_start_scan);

//|     def stop_scan(self) -> None:
//|         """Stop the current scan."""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_stop_scan(mp_obj_t self_in) {
    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_adapter_stop_scan(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_stop_scan_obj, bleio_adapter_stop_scan);

//|     advertising: bool
//|     """True when the adapter is currently advertising. (read-only)"""
//|
STATIC mp_obj_t bleio_adapter_get_advertising(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_bleio_adapter_get_advertising(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_advertising_obj, bleio_adapter_get_advertising);

const mp_obj_property_t bleio_adapter_advertising_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_advertising_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     connected: bool
//|     """True when the adapter is connected to another device regardless of who initiated the
//|     connection. (read-only)"""
//|
STATIC mp_obj_t bleio_adapter_get_connected(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_bleio_adapter_get_connected(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_connected_obj, bleio_adapter_get_connected);

const mp_obj_property_t bleio_adapter_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_connected_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     connections: Tuple[Connection]
//|     """Tuple of active connections including those initiated through
//|     :py:meth:`_bleio.Adapter.connect`. (read-only)"""
//|
STATIC mp_obj_t bleio_adapter_get_connections(mp_obj_t self) {
    return common_hal_bleio_adapter_get_connections(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_connections_obj, bleio_adapter_get_connections);

const mp_obj_property_t bleio_adapter_connections_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_connections_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     def connect(self, address: Address, *, timeout: float) -> Connection:
//|         """Attempts a connection to the device with the given address.
//|
//|         :param Address address: The address of the peripheral to connect to
//|         :param float/int timeout: Try to connect for timeout seconds."""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_address, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_address].u_obj, &bleio_address_type)) {
        mp_raise_TypeError(translate("Expected an Address"));
    }

    bleio_address_obj_t *address = MP_OBJ_TO_PTR(args[ARG_address].u_obj);
    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);

    return common_hal_bleio_adapter_connect(self, address, timeout);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_adapter_connect_obj, 2, bleio_adapter_connect);

//|     def erase_bonding(self) -> None:
//|         """Erase all bonding information stored in flash memory."""
//|         ...
//|
STATIC mp_obj_t bleio_adapter_erase_bonding(mp_obj_t self_in) {
    bleio_adapter_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_adapter_erase_bonding(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_erase_bonding_obj, bleio_adapter_erase_bonding);

STATIC const mp_rom_map_elem_t bleio_adapter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled), MP_ROM_PTR(&bleio_adapter_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&bleio_adapter_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_name),    MP_ROM_PTR(&bleio_adapter_name_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_advertising), MP_ROM_PTR(&bleio_adapter_start_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_advertising),  MP_ROM_PTR(&bleio_adapter_stop_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertising),   MP_ROM_PTR(&bleio_adapter_advertising_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_scan), MP_ROM_PTR(&bleio_adapter_start_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_scan),  MP_ROM_PTR(&bleio_adapter_stop_scan_obj) },

    { MP_ROM_QSTR(MP_QSTR_connect),    MP_ROM_PTR(&bleio_adapter_connect_obj) },

    { MP_ROM_QSTR(MP_QSTR_connected),   MP_ROM_PTR(&bleio_adapter_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_connections), MP_ROM_PTR(&bleio_adapter_connections_obj) },

    { MP_ROM_QSTR(MP_QSTR_erase_bonding), MP_ROM_PTR(&bleio_adapter_erase_bonding_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_adapter_locals_dict, bleio_adapter_locals_dict_table);

const mp_obj_type_t bleio_adapter_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Adapter,
    .make_new = bleio_adapter_make_new,
    .locals_dict = (mp_obj_t)&bleio_adapter_locals_dict,
};
