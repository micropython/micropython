/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025
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

#ifndef MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_AARCH64_H
#define MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_AARCH64_H

/*

AArch64 semihosting support for QEMU.

This follows the "Semihosting for AArch32 and AArch64" specification.
On AArch64, semihosting is invoked via the HLT instruction with immediate 0xF000.

To use semihosting for a replacement UART:
- Add shared/runtime/semihosting_aarch64.c to the Makefile sources.
- Call mp_semihosting_init() in main(), around the time UART is initialized.
- Replace mp_hal_stdin_rx_chr and similar in mphalport.c with the semihosting equivalent.
- Include shared/runtime/semihosting_aarch64.h in the relevant files.

When running under QEMU, use the -semihosting flag to enable semihosting support.

*/

#include <stddef.h>
#include <stdint.h>

void mp_semihosting_init(void);
void mp_semihosting_exit(int status);
int mp_semihosting_rx_char(void);
int mp_semihosting_rx_chars(char *str, size_t len);
uint64_t mp_semihosting_tx_strn(const char *str, size_t len);
uint64_t mp_semihosting_tx_strn_cooked(const char *str, size_t len);

#endif // MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_AARCH64_H
