/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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
#ifndef MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_ARM_H
#define MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_ARM_H

/*

To use semi-hosting for a replacement UART:
- Add shared/runtime/semihosting_arm.c to the Makefile sources.
- Call mp_semihosting_init() in main(), around the time UART is initialized.
- Replace mp_hal_stdin_rx_chr and similar in mphalport.c with the semihosting equivalent.
- Include shared/runtime/semihosting_arm.h in the relevant files.

Then make sure the debugger is attached and enables semihosting.  In OpenOCD this is
done with ARM semihosting enable followed by reset.  The terminal will need further
configuration to work with MicroPython (bash: stty raw -echo).

If mp_semihosting_rx_char() doesn't work then try mp_semihosting_rx_chars(str, 1).

*/

#include <stddef.h>
#include <stdint.h>

void mp_semihosting_init();
void mp_semihosting_exit(int status);
int mp_semihosting_rx_char();
int mp_semihosting_rx_chars(char *str, size_t len);
uint32_t mp_semihosting_tx_strn(const char *str, size_t len);
uint32_t mp_semihosting_tx_strn_cooked(const char *str, size_t len);

#endif // MICROPY_INCLUDED_SHARED_RUNTIME_SEMIHOSTING_ARM_H
