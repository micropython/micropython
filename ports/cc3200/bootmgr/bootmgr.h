/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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
#ifndef MICROPY_INCLUDED_CC3200_BOOTMGR_BOOTMGR_H
#define MICROPY_INCLUDED_CC3200_BOOTMGR_BOOTMGR_H

//****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// User image tokens
//*****************************************************************************
#define FACTORY_IMG_TOKEN       0x5555AAAA
#define UPDATE_IMG_TOKEN        0xAA5555AA
#define USER_BOOT_INFO_TOKEN    0xA5A55A5A

//*****************************************************************************
// Macros
//*****************************************************************************
#define APP_IMG_SRAM_OFFSET     0x20004000
#define DEVICE_IS_CC3101RS      0x18
#define DEVICE_IS_CC3101S       0x1B

//*****************************************************************************
// Function prototype
//*****************************************************************************
extern void Run(unsigned long);

//****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // MICROPY_INCLUDED_CC3200_BOOTMGR_BOOTMGR_H
