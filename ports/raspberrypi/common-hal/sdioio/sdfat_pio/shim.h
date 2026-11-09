// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// C-callable shim around the vendored C++ PioSdioCard driver. All use of the
// C++ class is confined to shim.cpp so the common-hal C sources never include
// any C++ headers.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Size of the opaque storage the C object struct reserves for an in-place
// PioSdioCard instance. shim.cpp static_asserts that the real class fits. The
// real object is well under this (~320 bytes); the slack is intentional so the
// build does not break if the upstream class grows slightly.
#define SDIOIO_PIO_CARD_STORAGE_SIZE 512

// Opaque storage for one PioSdioCard, constructed in place by
// sdfat_pio_card_new(). The class's members are at most pointer/word aligned, so
// a pointer member is enough to align the buffer (and matches what the GC heap
// guarantees); shim.cpp static_asserts that this is sufficient. Kept as a plain
// union so the C struct needs no knowledge of the C++ type.
typedef union {
    void *for_alignment;
    uint8_t bytes[SDIOIO_PIO_CARD_STORAGE_SIZE];
} sdioio_pio_card_storage_t;

#ifdef __cplusplus
extern "C" {
#endif

// Construct a PioSdioCard in place in the given storage buffer.
void sdfat_pio_card_new(void *storage);

// Run the destructor for the in-place PioSdioCard.
void sdfat_pio_card_free(void *storage);

// Initialize the card. dat0_pin is the base of four consecutive data GPIOs.
// frequency is the requested SD clock in Hz; the achieved rate is written to
// actual_frequency_out (in Hz) when it is non-NULL. Returns true on success.
bool sdfat_pio_card_begin(void *storage, uint32_t clk_pin, uint32_t cmd_pin,
    uint32_t dat0_pin, uint32_t frequency, uint32_t *actual_frequency_out);

// Number of 512-byte sectors on the card (0 if unknown).
uint32_t sdfat_pio_card_sector_count(void *storage);

// Read `num_sectors` 512-byte sectors starting at `start_sector` into `dst`.
// Returns true on success. The driver is synchronous/polling, so this blocks.
bool sdfat_pio_card_read_sectors(void *storage, uint32_t start_sector,
    uint8_t *dst, size_t num_sectors);

// Write `num_sectors` 512-byte sectors from `src` starting at `start_sector`.
// Returns true on success. Blocks like the read path.
bool sdfat_pio_card_write_sectors(void *storage, uint32_t start_sector,
    const uint8_t *src, size_t num_sectors);

// Ends any open multi-block transfer (CMD12) and waits for the card to finish
// programming. Returns false on timeout.
bool sdfat_pio_card_sync(void *storage);

// Last error code from the driver (see SdCardInfo.h).
uint8_t sdfat_pio_card_error_code(void *storage);

// Release the card's PIO/state-machine resources.
void sdfat_pio_card_end(void *storage);

#ifdef __cplusplus
}
#endif
