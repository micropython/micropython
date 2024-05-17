// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SUPERVISOR_FATFS_H
#define MICROPY_INCLUDED_SUPERVISOR_FATFS_H

#include "lib/oofatfs/ff.h"

void override_fattime(DWORD time);

#endif  // MICROPY_INCLUDED_SUPERVISOR_FATFS_H
