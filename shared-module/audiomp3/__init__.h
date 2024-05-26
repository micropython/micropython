// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdlib.h>

extern void *mp3_alloc(size_t sz);
extern void mp3_free(void *ptr);
