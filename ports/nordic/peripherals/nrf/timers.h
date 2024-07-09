// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx.h"
#include "nrfx_timer.h"

void timers_reset(void);
nrfx_timer_t *nrf_peripherals_allocate_timer(void);
nrfx_timer_t *nrf_peripherals_allocate_timer_or_throw(void);
void nrf_peripherals_free_timer(nrfx_timer_t *timer);
void nrf_peripherals_timer_never_reset(nrfx_timer_t *timer);
void nrf_peripherals_timer_reset_ok(nrfx_timer_t *timer);
nrfx_timer_t *nrf_peripherals_timer_from_reg(NRF_TIMER_Type *ptr);
size_t nrf_peripherals_timer_idx_from_timer(nrfx_timer_t *ptr);
