// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrf_soc.h"

// Helpers for common usage of nrf_mutex.

// Try to acquire a mutex right now. Raise exception if we can't get it.
void sd_mutex_acquire_check(nrf_mutex_t *p_mutex);

// Wait for a mutex to become available. Run VM background tasks while waiting.
void sd_mutex_acquire_wait(nrf_mutex_t *p_mutex);

// Wait for a mutex to become available.. Block VM while waiting.
void sd_mutex_acquire_wait_no_vm(nrf_mutex_t *p_mutex);

// Release a mutex, and raise exception on error.
void sd_mutex_release_check(nrf_mutex_t *p_mutex);
