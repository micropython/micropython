// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Forget which mbedtls SHA-256 context owns the RP2350 SHA-256 block and release
// its bootrom lock. Call when the VM ends, since no context outlives it.
// Doesn't allocate or raise.
void rp2_sha256_hw_reset(void);
