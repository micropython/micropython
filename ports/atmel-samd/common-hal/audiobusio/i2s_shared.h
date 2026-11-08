// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Shared reference counting for the single I2S peripheral, used by both
// audiobusio I2SOut and PDMIn. The implementations are in I2SOut.c, outside
// its CIRCUITPY_AUDIOBUSIO_I2SOUT guard, so they exist on boards with PDMIn
// but no I2SOut.

#pragma once

// Turn the I2S peripheral's clocks on (idempotent; call once per user).
void i2s_acquire(void);

// Drop one reference and fully power the I2S peripheral down if this was the
// last user.
void i2s_release(void);
