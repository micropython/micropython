// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#ifdef SOC_DAC_SUPPORTED
#include "driver/dac_oneshot.h"
#endif
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    #ifdef SOC_DAC_SUPPORTED
    dac_oneshot_handle_t handle;
    uint8_t channel;
    #endif
} analogio_analogout_obj_t;

void analogout_reset(void);
