// SPDX-FileCopyrightText: Copyright (c) 2013-2016 Damien P. George
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_WINDOWS_FMODE_H
#define MICROPY_INCLUDED_WINDOWS_FMODE_H

// Treat files opened by open() as binary. No line ending translation is done.
void set_fmode_binary(void);

// Treat files opened by open() as text.
// When reading from the file \r\n will be converted to \n.
// When writing to the file \n will be converted into \r\n.
void set_fmode_text(void);

#endif // MICROPY_INCLUDED_WINDOWS_FMODE_H
