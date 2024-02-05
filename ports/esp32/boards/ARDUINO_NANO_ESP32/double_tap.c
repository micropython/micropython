/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include "py/mphal.h"

#include <esp32s3/rom/cache.h>

// for get_extram_data_high()
#include <esp_psram.h>
#include <esp_private/esp_psram_extram.h>

#include "double_tap.h"

#define NUM_TOKENS 3
static const uint32_t MAGIC_TOKENS[NUM_TOKENS] = {
    0xf01681de, 0xbd729b29, 0xd359be7a,
};

static void *magic_area;
static uint32_t backup_area[NUM_TOKENS];

// Current IDF does not map external RAM to a fixed address.
// The actual VMA depends on other enabled devices, so the precise
// location must be discovered.
static uintptr_t get_extram_data_high(void) {
    // get a pointer into SRAM area (only the address is useful)
    void *psram_ptr = heap_caps_malloc(16, MALLOC_CAP_SPIRAM);
    heap_caps_free(psram_ptr);

    // keep moving backwards until leaving PSRAM area
    uintptr_t psram_base_addr = (uintptr_t)psram_ptr;
    psram_base_addr &= ~(CONFIG_MMU_PAGE_SIZE - 1); // align to start of page
    while (esp_psram_check_ptr_addr((void *)psram_base_addr)) {
        psram_base_addr -= CONFIG_MMU_PAGE_SIZE;
    }

    // offset is one page from start of PSRAM
    return psram_base_addr + CONFIG_MMU_PAGE_SIZE + esp_psram_get_size();
}

void double_tap_init(void) {
    // magic location block ends 0x20 bytes from end of PSRAM
    magic_area = (void *)(get_extram_data_high() - 0x20 - sizeof(MAGIC_TOKENS));
}

void double_tap_mark() {
    memcpy(backup_area, magic_area, sizeof(MAGIC_TOKENS));
    memcpy(magic_area, MAGIC_TOKENS, sizeof(MAGIC_TOKENS));
    Cache_WriteBack_Addr((uintptr_t)magic_area, sizeof(MAGIC_TOKENS));
}

void double_tap_invalidate() {
    if (memcmp(backup_area, MAGIC_TOKENS, sizeof(MAGIC_TOKENS))) {
        // different contents: restore backup
        memcpy(magic_area, backup_area, sizeof(MAGIC_TOKENS));
    } else {
        // clear memory
        memset(magic_area, 0, sizeof(MAGIC_TOKENS));
    }
    Cache_WriteBack_Addr((uintptr_t)magic_area, sizeof(MAGIC_TOKENS));
}

bool double_tap_check_match() {
    return memcmp(magic_area, MAGIC_TOKENS, sizeof(MAGIC_TOKENS)) == 0;
}
