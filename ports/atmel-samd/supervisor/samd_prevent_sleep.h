// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>

// Sleeping causes interrupt delay to increase. These allow common-hal code to
// temporarily disable sleep as-needed.
#ifdef SAMD21
void samd_prevent_sleep(void);
void samd_allow_sleep(void);
#endif
