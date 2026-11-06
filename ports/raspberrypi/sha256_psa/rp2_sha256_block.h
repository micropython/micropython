// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The few operations the SHA-256 driver needs from the block. The RP2350 port
// implements them on the hardware; the desktop port has a software model so the
// driver and the PSA dispatch can be tested without a board.

#include <stdbool.h>
#include <stdint.h>

// Take the block's lock. Returns false if someone else holds it.
bool rp2_sha256_block_try_lock(void);
void rp2_sha256_block_unlock(void);
// Load the SHA-256 initial value.
void rp2_sha256_block_start(void);
// Hash one 64-byte block, given as message bytes. data may be unaligned.
void rp2_sha256_block_write(const uint8_t *data);
// Read the chaining state after the last whole block.
void rp2_sha256_block_read(uint32_t state[8]);
