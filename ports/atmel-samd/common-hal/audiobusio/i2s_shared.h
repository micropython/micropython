// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Shared reference counting for the single I2S peripheral, used by both
// audiobusio I2SOut and PDMIn. The implementations live in I2SOut.c because
// that file is always compiled when the audiobusio module is enabled.

#pragma once

// Turn the I2S peripheral's clocks on (idempotent; call once per user).
void i2s_acquire(void);

// Drop one reference and fully power the I2S peripheral down if this was the
// last user.
void i2s_release(void);
