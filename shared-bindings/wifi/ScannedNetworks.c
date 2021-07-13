/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

//| class ScannedNetworks:
//|     """Iterates over all `wifi.Network` objects found while scanning. This object is always created
//|     by a `wifi.Radio`: it has no user-visible constructor."""
//|
STATIC mp_obj_t scannednetworks_iternext(mp_obj_t self_in) {
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
//|            Raises `StopIteration` if scanning is finished and no other results are available."""
//|         ...
//|

const mp_obj_type_t wifi_scannednetworks_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_ScannedNetworks,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = scannednetworks_iternext,
        )
};
