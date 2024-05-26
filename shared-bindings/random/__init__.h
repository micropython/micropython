// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// This depends on shared_module because nearly all functionality is port
// agnostic. The random module only depends on the common_hal_os_urandom or
// common_hal_time_monotonic_ms to seed it initially.

void shared_modules_random_seed(mp_uint_t seed);
mp_uint_t shared_modules_random_getrandbits(uint8_t n);
mp_int_t shared_modules_random_randrange(mp_int_t start, mp_int_t stop, mp_int_t step);
mp_float_t shared_modules_random_random(void);
mp_float_t shared_modules_random_uniform(mp_float_t a, mp_float_t b);
