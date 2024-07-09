// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

char get_fmt_type(const char **fmt);
mp_uint_t get_fmt_num(const char **p);
mp_uint_t calcsize_items(const char *fmt);
