// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/rp2pio/StateMachine.h"
#include "shared-bindings/imagecapture/ParallelImageCapture.h"

struct imagecapture_parallelimagecapture_obj {
    mp_obj_base_t base;
    rp2pio_statemachine_obj_t state_machine;
};
