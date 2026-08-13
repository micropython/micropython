/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 *
 * Compiler and architecture hooks required by morselib.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_HALOW_MMPORT_H
#define MICROPY_INCLUDED_DRIVERS_HALOW_MMPORT_H

#define MMPORT_BREAKPOINT() __asm("bkpt 0\n\t")
#define MMPORT_GET_LR()     __builtin_return_address(0)
#define MMPORT_GET_PC(_a)   __asm volatile ("mov %0, pc" : "=r" (_a))
#define MMPORT_MEM_SYNC()   __sync_synchronize()

#endif // MICROPY_INCLUDED_DRIVERS_HALOW_MMPORT_H
