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
#include "shared-bindings/_bleio/ScanResults.h"

//| class ScanResults:
//|     """Iterates over advertising data received while scanning. This object is always created
//|     by a `_bleio.Adapter`: it has no user-visible constructor."""
//|
static mp_obj_t scanresults_iternext(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &bleio_scanresults_type));
    bleio_scanresults_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t scan_entry = common_hal_bleio_scanresults_next(self);
    if (scan_entry != mp_const_none) {
        return scan_entry;
    }
    return MP_OBJ_STOP_ITERATION;
}

//|     def __init__(self) -> None:
//|         """Cannot be instantiated directly. Use `_bleio.Adapter.start_scan`."""
//|         ...
//|
//|     def __iter__(self) -> Iterator[ScanEntry]:
//|         """Returns itself since it is the iterator."""
//|         ...
//|
//|     def __next__(self) -> ScanEntry:
//|         """Returns the next `_bleio.ScanEntry`. Blocks if none have been received and scanning is still
//|         active. Raises `StopIteration` if scanning is finished and no other results are available.
//|         """
//|         ...
//|

MP_DEFINE_CONST_OBJ_TYPE(
    bleio_scanresults_type,
    MP_QSTR_ScanResults,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, scanresults_iternext
    );
