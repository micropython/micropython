// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "supervisor/shared/lock.h"

typedef struct _keypad_scanner_funcs_t {
    void (*scan_now)(void *self_in, mp_obj_t timestamp);
    size_t (*get_key_count)(void *self_in);
} keypad_scanner_funcs_t;

// All scanners must begin with these common fields.
// This is an ad hoc "superclass" struct for scanners, though they do
// not actually have a superclass relationship.
#define KEYPAD_SCANNER_COMMON_FIELDS \
    mp_obj_base_t base; \
    struct _keypad_scanner_obj_t *next; \
    keypad_scanner_funcs_t *funcs; \
    uint64_t next_scan_ticks; \
    int8_t *debounce_counter; \
    struct _keypad_eventqueue_obj_t *events; \
    mp_uint_t interval_ticks; \
    uint8_t debounce_threshold; \
    bool never_reset

typedef struct _keypad_scanner_obj_t {
    KEYPAD_SCANNER_COMMON_FIELDS;
} keypad_scanner_obj_t;

extern supervisor_lock_t keypad_scanners_linked_list_lock;

void keypad_tick(void);
void keypad_reset(void);

void keypad_register_scanner(keypad_scanner_obj_t *scanner);
void keypad_deregister_scanner(keypad_scanner_obj_t *scanner);
void keypad_construct_common(keypad_scanner_obj_t *scanner, mp_float_t interval, size_t max_events, uint8_t debounce_cycles);
bool keypad_debounce(keypad_scanner_obj_t *self, mp_uint_t key_number, bool current);
void keypad_never_reset(keypad_scanner_obj_t *self);

size_t common_hal_keypad_generic_get_key_count(void *scanner);
void common_hal_keypad_deinit_core(void *scanner);
