// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern void rtc_reset(void);
extern mp_obj_t rtc_get_time_source_time(void);
