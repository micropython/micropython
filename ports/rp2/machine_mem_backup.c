/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Andrew Leech
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

#include "py/mpconfig.h"

#if MICROPY_PY_MACHINE_MEM_BACKUP

#include "py/obj.h"
#include "py/objarray.h"
#include "hardware/watchdog.h"
#include "extmod/modmachine.h"

#if PICO_RP2350
#include "hardware/powman.h"
#endif

// scratch[4] is reserved for pico-sdk watchdog/reboot bookkeeping and is
// excluded.  scratch[5..7] are clobbered only by watchdog_reboot() with a
// non-zero pc; MicroPython's reset paths pass pc=0 so they survive.
// Powman scratch (RP2350 only) survives deeper sleep modes.

const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('I', 4, (void *)&watchdog_hw->scratch[0]),  // scratch[0..3], 16 bytes
    BACKUP_MV('I', 3, (void *)&watchdog_hw->scratch[5]),  // scratch[5..7], 12 bytes
    #if PICO_RP2350
    BACKUP_MV('I', 8, (void *)&powman_hw->scratch[0]),    // powman scratch, 32 bytes
    #endif
};

const size_t machine_mem_backup_region_count = MP_ARRAY_SIZE(machine_mem_backup_regions);

#endif // MICROPY_PY_MACHINE_MEM_BACKUP
