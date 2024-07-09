// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

//| class ScannedNetworks:
//|     """Iterates over all `wifi.Network` objects found while scanning. This object is always created
//|     by a `wifi.Radio`: it has no user-visible constructor."""
//|
static mp_obj_t scannednetworks_iternext(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &wifi_scannednetworks_type));
    wifi_scannednetworks_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t network = common_hal_wifi_scannednetworks_next(self);
    if (network != mp_const_none) {
        return network;
    }

    return MP_OBJ_STOP_ITERATION;
}

//|     def __init__(self) -> None:
//|         """Cannot be instantiated directly. Use `wifi.Radio.start_scanning_networks`."""
//|         ...
//|
//|     def __iter__(self) -> Iterator[Network]:
//|         """Returns itself since it is the iterator."""
//|         ...
//|
//|     def __next__(self) -> Network:
//|         """Returns the next `wifi.Network`.
//|         Raises `StopIteration` if scanning is finished and no other results are available."""
//|         ...
//|

MP_DEFINE_CONST_OBJ_TYPE(
    wifi_scannednetworks_type,
    MP_QSTR_ScannedNetworks,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, scannednetworks_iternext
    );
