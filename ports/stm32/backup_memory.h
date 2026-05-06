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
#ifndef MICROPY_INCLUDED_STM32_BACKUP_MEMORY_H
#define MICROPY_INCLUDED_STM32_BACKUP_MEMORY_H

// Backup memory configuration for machine.backup_memory.
// Prefers BKPSRAM (byte-addressable, 4-8 KB) when available, otherwise
// falls back to RTC BKP registers (word-only, typically 80-512 bytes).

// Detect BKPSRAM availability from CMSIS-defined base address macros.
// Unify the various per-family names into a single MICROPY_HW_STM32_BKPSRAM_BASE.
#if defined(BKPSRAM_BASE)
// F4, F7, H5, U5
#define MICROPY_HW_STM32_BKPSRAM_BASE BKPSRAM_BASE
#elif defined(D3_BKPSRAM_BASE)
// H7 (non-7Ax/7Bx)
#define MICROPY_HW_STM32_BKPSRAM_BASE D3_BKPSRAM_BASE
#elif defined(SRD_BKPSRAM_BASE)
// H7A3/H7B3/H7B0
#define MICROPY_HW_STM32_BKPSRAM_BASE SRD_BKPSRAM_BASE
#elif defined(BKPSRAM_BASE_NS)
// N6 (no unsuffixed alias)
#define MICROPY_HW_STM32_BKPSRAM_BASE BKPSRAM_BASE_NS
#endif

#if defined(MICROPY_HW_STM32_BKPSRAM_BASE) && MICROPY_HW_ENABLE_RTC
// BKPSRAM is available: byte-addressable, battery-backed SRAM.
#ifndef MICROPY_PY_MACHINE_BACKUP_MEMORY
#define MICROPY_PY_MACHINE_BACKUP_MEMORY    (1)
#endif
#if MICROPY_PY_MACHINE_BACKUP_MEMORY
#define MICROPY_HW_BACKUP_MEMORY_ITEMSIZE   (1)
#if defined(BKPSRAM_SIZE)
// Families with BKPSRAM_SIZE in CMSIS (H5: 2-4 KB, N6: 8 KB).
#define MICROPY_HW_BACKUP_MEMORY_BYTES      BKPSRAM_SIZE
#else
// F4, F7, H7, U5: 4 KB.
#define MICROPY_HW_BACKUP_MEMORY_BYTES      (4096)
#endif
#define MICROPY_HW_BACKUP_MEMORY_ADDR       ((void *)MICROPY_HW_STM32_BKPSRAM_BASE)
#endif
#else
// No BKPSRAM: fall back to RTC BKP registers (word-only access).
// Normalize backup register count across CMSIS header variants.
#if defined(RTC_BKP_NUMBER)
#define MICROPY_HW_STM32_BKP_REG_COUNT RTC_BKP_NUMBER
#elif defined(TAMP_BKP_NUMBER)
#define MICROPY_HW_STM32_BKP_REG_COUNT TAMP_BKP_NUMBER
#elif defined(RTC_BACKUP_NB)
#define MICROPY_HW_STM32_BKP_REG_COUNT RTC_BACKUP_NB
#elif defined(RTC_BKP_NB)
#define MICROPY_HW_STM32_BKP_REG_COUNT RTC_BKP_NB
#else
#define MICROPY_HW_STM32_BKP_REG_COUNT 0
#endif
#if MICROPY_HW_STM32_BKP_REG_COUNT > 0 && MICROPY_HW_ENABLE_RTC
#ifndef MICROPY_PY_MACHINE_BACKUP_MEMORY
#define MICROPY_PY_MACHINE_BACKUP_MEMORY    (1)
#endif
#if MICROPY_PY_MACHINE_BACKUP_MEMORY
#define MICROPY_HW_BACKUP_MEMORY_BYTES      (MICROPY_HW_STM32_BKP_REG_COUNT * 4)
#define MICROPY_HW_BACKUP_MEMORY_ITEMSIZE   (4)
#if defined(TAMP)
#define MICROPY_HW_BACKUP_MEMORY_ADDR       ((void *)&TAMP->BKP0R)
#else
#define MICROPY_HW_BACKUP_MEMORY_ADDR       ((void *)&RTC->BKP0R)
#endif
#endif
#endif
#endif

#endif // MICROPY_INCLUDED_STM32_BACKUP_MEMORY_H
