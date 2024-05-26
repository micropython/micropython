// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

typedef volatile bool supervisor_lock_t;

void supervisor_acquire_lock(supervisor_lock_t *lock);
bool supervisor_try_lock(supervisor_lock_t *lock);
void supervisor_release_lock(supervisor_lock_t *lock);
