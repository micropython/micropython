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
#include "fsl_snvs_lp.h"
#include "extmod/modmachine.h"

// Boards with a UF2 bootloader use LPGPR[3] for double-tap entry detection
// and should override MICROPY_HW_SNVS_LPGPR_SAFE_COUNT to 3 in mpconfigboard.h.
#ifndef MICROPY_HW_SNVS_LPGPR_SAFE_COUNT
#define MICROPY_HW_SNVS_LPGPR_SAFE_COUNT SNVS_LPGPR_COUNT
#endif

const mp_obj_array_t machine_mem_backup_regions[] = {
    BACKUP_MV('I', MICROPY_HW_SNVS_LPGPR_SAFE_COUNT, (void *)&SNVS->LPGPR[0]),
};

const size_t machine_mem_backup_region_count = MP_ARRAY_SIZE(machine_mem_backup_regions);

#endif // MICROPY_PY_MACHINE_MEM_BACKUP
