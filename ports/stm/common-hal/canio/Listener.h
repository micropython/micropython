// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/canio/CAN.h"
#include "shared-module/canio/Match.h"

typedef struct canio_listener_obj {
    mp_obj_base_t base;
    canio_can_obj_t *can;
    CAN_FIFOMailBox_TypeDef *mailbox;
    __IO uint32_t *rfr;
    uint32_t timeout_ms;
    uint8_t fifo_idx;
} canio_listener_obj_t;
