// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// C-callable shim around the vendored C++ PioSdioCard driver. This is the only
// translation unit that includes the C++ class; everything the common-hal C
// code needs is exposed through the extern "C" entry points in shim.h.

#include <new>

#include "hardware/clocks.h"

#include "SdCard/PioSdio/PioSdioCard.h"
#include "shim.h"

static_assert(sizeof(PioSdioCard) <= SDIOIO_PIO_CARD_STORAGE_SIZE,
    "SDIOIO_PIO_CARD_STORAGE_SIZE is too small for PioSdioCard");
static_assert(alignof(PioSdioCard) <= alignof(sdioio_pio_card_storage_t),
    "sdioio_pio_card_storage_t is not aligned enough for PioSdioCard");

extern "C" {

void sdfat_pio_card_new(void *storage) {
    new (storage) PioSdioCard();
}

void sdfat_pio_card_free(void *storage) {
    reinterpret_cast<PioSdioCard *>(storage)->~PioSdioCard();
}

bool sdfat_pio_card_begin(void *storage, uint32_t clk_pin, uint32_t cmd_pin,
    uint32_t dat0_pin, uint32_t frequency, uint32_t *actual_frequency_out) {
    PioSdioCard *card = reinterpret_cast<PioSdioCard *>(storage);

    // The PIO driver clocks four PIO cycles per SD clock, so the resulting SD
    // clock is clk_sys / (4 * clkDiv). Invert that to turn the requested rate
    // into a divisor, clamped to the fastest the divisor allows (clkDiv >= 1).
    float sys_hz = (float)clock_get_hz(clk_sys);
    float clk_div = sys_hz / (4.0f * (float)frequency);
    if (clk_div < 1.0f) {
        clk_div = 1.0f;
    }

    bool ok = card->begin(PioSdioConfig(clk_pin, cmd_pin, dat0_pin, clk_div));

    if (actual_frequency_out != nullptr) {
        *actual_frequency_out = (uint32_t)(sys_hz / (4.0f * clk_div));
    }
    return ok;
}

uint32_t sdfat_pio_card_sector_count(void *storage) {
    return reinterpret_cast<PioSdioCard *>(storage)->sectorCount();
}

bool sdfat_pio_card_read_sectors(void *storage, uint32_t start_sector,
    uint8_t *dst, size_t num_sectors) {
    return reinterpret_cast<PioSdioCard *>(storage)->readSectors(start_sector, dst, num_sectors);
}

bool sdfat_pio_card_write_sectors(void *storage, uint32_t start_sector,
    const uint8_t *src, size_t num_sectors) {
    return reinterpret_cast<PioSdioCard *>(storage)->writeSectors(start_sector, src, num_sectors);
}

bool sdfat_pio_card_sync(void *storage) {
    return reinterpret_cast<PioSdioCard *>(storage)->syncDevice();
}

uint8_t sdfat_pio_card_error_code(void *storage) {
    return reinterpret_cast<PioSdioCard *>(storage)->errorCode();
}

void sdfat_pio_card_end(void *storage) {
    reinterpret_cast<PioSdioCard *>(storage)->end();
}

}  // extern "C"
