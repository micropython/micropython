// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

void shared_modules_struct_pack_into(mp_obj_t fmt_in, byte *p, byte *end_p, size_t n_args, const mp_obj_t *args);
mp_uint_t shared_modules_struct_calcsize(mp_obj_t fmt_in);
mp_obj_tuple_t *shared_modules_struct_unpack_from(mp_obj_t fmt_in, byte *p, byte *end_p, bool exact_size);
