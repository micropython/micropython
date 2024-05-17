// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_FONTIO_BUILTINFONT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_FONTIO_BUILTINFONT_H

#include "shared-module/fontio/BuiltinFont.h"

extern const mp_obj_type_t fontio_builtinfont_type;

mp_obj_t common_hal_fontio_builtinfont_get_bitmap(const fontio_builtinfont_t *self);
mp_obj_t common_hal_fontio_builtinfont_get_bounding_box(const fontio_builtinfont_t *self);
mp_obj_t common_hal_fontio_builtinfont_get_glyph(const fontio_builtinfont_t *self, mp_uint_t codepoint);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_FONTIO_BUILTINFONT_H
