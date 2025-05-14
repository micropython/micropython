/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// qstrs specific to this port
// *FORMAT-OFF*

// Entries for sys.path
Q(/flash)
Q(/flash/lib)
Q(/sd)
Q(/sd/lib)

// For os.sep
Q(/)

#if MICROPY_HW_ENABLE_USB
// for usb modes
Q(VCP)
Q(MSC)
Q(VCP+MSC)
Q(VCP+HID)
Q(VCP+MSC+HID)
#if MICROPY_HW_USB_CDC_NUM >= 2
Q(2xVCP)
Q(2xVCP+MSC)
Q(2xVCP+MSC+HID)
#endif
#if MICROPY_HW_USB_CDC_NUM >= 3
Q(3xVCP)
Q(3xVCP+MSC)
Q(3xVCP+MSC+HID)
#endif
#endif
