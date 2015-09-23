/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Alex March
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

#include <stdbool.h>

#include "diskio.h"
#include "sflash_diskio.h"

static bool sflash_init_done = false;
static bool sflash_cache_is_dirty;

DRESULT sflash_disk_init (void) {
    if (!sflash_init_done) {

    }
    return RES_OK;
}

DRESULT sflash_disk_status(void) {
    if (!sflash_init_done) {
        return STA_NOINIT;
    }
    return 0; // init done
}

DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count) {
    return RES_OK;
}

DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count) {
    return RES_OK;
}

DRESULT sflash_disk_flush (void) {
    // write back the cache if it's dirty
    if (sflash_cache_is_dirty) {
        // check access
        // if (!sflash_access()) {
        //     return RES_ERROR;
        // }
    }
    sflash_cache_is_dirty = false;
    return RES_OK;
}
