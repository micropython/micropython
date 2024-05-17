// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "host/ble_gap.h"

typedef struct ble_event_handler_entry {
    struct ble_event_handler_entry *next;
    void *param;
    ble_gap_event_fn *func;
} ble_event_handler_entry_t;

void ble_event_reset(void);
void ble_event_remove_heap_handlers(void);
void ble_event_add_handler(ble_gap_event_fn *func, void *param);
void ble_event_remove_handler(ble_gap_event_fn *func, void *param);

// Allow for user provided entries to prevent allocations outside the VM.
void ble_event_add_handler_entry(ble_event_handler_entry_t *entry, ble_gap_event_fn *func, void *param);

int ble_event_run_handlers(struct ble_gap_event *event);
