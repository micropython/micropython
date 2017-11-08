/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_SUPERVISOR_MESSAGES_DEFAULT_H
#define MICROPY_SUPERVISOR_MESSAGES_DEFAULT_H

#ifndef MSG_OUTPUT_SUFFIX
#define MSG_OUTPUT_SUFFIX " output:\r\n"
#endif

#ifndef MSG_NEWLINE
#define MSG_NEWLINE "\r\n"
#endif

#ifndef MSG_AUTORELOAD_ON
#define MSG_AUTORELOAD_ON "Auto-reload is on. Simply save files over USB to run them or enter REPL to disable.\r\n"
#endif

#ifndef MSG_AUTORELOAD_OFF
#define MSG_AUTORELOAD_OFF "Auto-reload is off.\r\n"
#endif

#ifndef MSG_SAFE_MODE_ON
#define MSG_SAFE_MODE_ON "Running in safe mode! Auto-reload is off.\r\n"
#endif

#ifndef MSG_SAFE_MODE_NO_MAIN
#define MSG_SAFE_MODE_NO_MAIN "Running in safe mode! Not running saved code.\r\n"
#endif

#ifndef MSG_SAFE_MODE_USER_REQUESTED
#define MSG_SAFE_MODE_USER_REQUESTED "You requested starting safe mode by "
#endif

#ifndef MSG_SAFE_MODE_USER_EXIT
#define MSG_SAFE_MODE_USER_EXIT "To exit, please reset the board without "
#endif

#ifndef MSG_BAD_SAFE_MODE
#define MSG_BAD_SAFE_MODE "You are running in safe mode which means something really bad happened."
#endif

#ifndef MSG_SAFE_MODE_CRASH
#define MSG_SAFE_MODE_CRASH "Looks like our core CircuitPython code crashed hard. Whoops!"
#endif

#ifndef MSG_SAFE_MODE_FILE_ISSUE
#define MSG_SAFE_MODE_FILE_ISSUE "Please file an issue here with the contents of your CIRCUITPY drive:"
#endif

#ifndef MSG_SAFE_MODE_ISSUE_LINK
#define MSG_SAFE_MODE_ISSUE_LINK "https://github.com/adafruit/circuitpython/issues"
#endif

#ifndef MSG_SAFE_MODE_BROWN_OUT_LINE_1
#define MSG_SAFE_MODE_BROWN_OUT_LINE_1 "The microcontroller's power dipped. Please make sure your power supply provides"
#endif

#ifndef MSG_SAFE_MODE_BROWN_OUT_LINE_2
#define MSG_SAFE_MODE_BROWN_OUT_LINE_2 "enough power for the whole circuit and press reset (after ejecting CIRCUITPY)."
#endif

#ifndef MSG_WAIT_BEFORE_REPL
#define MSG_WAIT_BEFORE_REPL "Press any key to enter the REPL. Use CTRL-D to reload."
#endif

// Be careful, some tools depend on this.
#ifndef MSG_SOFT_REBOOT
#define MSG_SOFT_REBOOT "soft reboot"
#endif

#ifndef MSG_DOUBLE_FILE_EXTENSION
#define MSG_DOUBLE_FILE_EXTENSION "WARNING: Your code filename has two extensions\r\n"
#endif

#endif  // MICROPY_SUPERVISOR_MESSAGES_DEFAULT_H
