/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "common-hal/canio/CAN.h"
#include "shared-module/canio/Match.h"

typedef struct {
  __IO CAN_RXF0C_Type            RXFC;       /**< \brief (R/W 32) Rx FIFO n Configuration */
  __I  CAN_RXF0S_Type            RXFS;       /**< \brief (R/  32) Rx FIFO n Status */
  __IO CAN_RXF0A_Type            RXFA;       /**< \brief (R/W 32) Rx FIFO n Acknowledge */
} canio_rxfifo_reg_t;

typedef struct canio_listener_obj {
    mp_obj_base_t base;
    canio_can_obj_t *can;
    canio_can_rx_fifo_t *fifo;
    canio_rxfifo_reg_t *hw;
    uint32_t timeout_ms;
    uint8_t fifo_idx;
} canio_listener_obj_t;

void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout);
void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self);
void common_hal_canio_listener_deinit(canio_listener_obj_t *self);
bool common_hal_canio_listener_receiveinto(canio_listener_obj_t *self, canio_message_obj_t *message);
int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self);
float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self);
void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout);
