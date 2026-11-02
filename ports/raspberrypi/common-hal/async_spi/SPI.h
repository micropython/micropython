// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/async_flag.h"
#include "supervisor/shared/awaitable.h"

#include "hardware/spi.h"

typedef struct {
    mp_obj_base_t base;
    circuitpy_async_lock_obj_t lock;
    spi_inst_t *peripheral;
    const mcu_pin_obj_t *clock;         // NULL once deinited
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
    uint32_t target_frequency;
    int32_t real_frequency;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    bool async_active;                  // an async DMA transfer may still be running
    bool dma_kept;                      // dma_tx and dma_rx are ours until deinit
    uint8_t dma_tx;
    uint8_t dma_rx;
    uint8_t one_byte;                   // the one-byte side of an async write (RX) or read (TX)
    circuitpy_async_flag_t *async_done;
} async_spi_spi_obj_t;
