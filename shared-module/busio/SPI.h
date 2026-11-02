// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

// busio.SPI built on a port's async_spi implementation: every blocking call starts an async
// transfer and waits for it. A port that has async_spi includes this header from its
// common-hal/busio/SPI.h and sets CIRCUITPY_ASYNC_SPI = 1 in its mpconfigport.mk.

#pragma once

#include "shared-bindings/async_spi/SPI.h"

typedef struct {
    mp_obj_base_t base;
    async_spi_spi_obj_t async_spi;
    circuitpy_async_flag_t done;        // of the blocking transfer in progress
    circuitpy_async_flag_t *sent;       // of a write_start() not yet ended, NULL otherwise
    void *sent_context;
} busio_spi_obj_t;
