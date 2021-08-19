/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Armink (armink.ztl@gmail.com)
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

#include <rtthread.h>
#include <py/runtime.h>

#ifdef MICROPYTHON_USING_FFI

/*
 * Native module implement by C function export.
 * The python module will using ffi to import all of functions.
 * You can call those functions by:
 *
 * STEP1:
 *     generate the python module from 'native_module.c' to 'native_module.py'
 *     run 'python native_gen.py native_module.c' command
 *
 * STEP2:
 *     push the 'native_module.py' file to '/libs/mpy/' folder on target
 *
 * STEP3:
 *     import native_modbule
 *
 * STEP4:
 *     native_module.show("Hello native module")
 *     native_module.add(1, 2)
 */

void native_module_show(const char *str)
{
    mp_printf(&mp_plat_print, "Native module show: %s\n", str);
}
RTM_EXPORT(native_module_show)

int native_module_add(int a, int b)
{
    return a + b;
}
RTM_EXPORT(native_module_add)

#endif /* MICROPYTHON_USING_FFI */
