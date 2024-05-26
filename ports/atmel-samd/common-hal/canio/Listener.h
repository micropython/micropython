// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/canio/CAN.h"
#include "shared-module/canio/Match.h"

typedef struct {
    __IO CAN_RXF0C_Type RXFC;                /**< \brief (R/W 32) Rx FIFO n Configuration */
    __I CAN_RXF0S_Type RXFS;                 /**< \brief (R/  32) Rx FIFO n Status */
    __IO CAN_RXF0A_Type RXFA;                /**< \brief (R/W 32) Rx FIFO n Acknowledge */
} canio_rxfifo_reg_t;

typedef struct canio_listener_obj {
    mp_obj_base_t base;
    canio_can_obj_t *can;
    canio_can_rx_fifo_t *fifo;
    canio_rxfifo_reg_t *hw;
    uint32_t timeout_ms;
    uint8_t fifo_idx;
} canio_listener_obj_t;
