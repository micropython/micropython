Skip to content
 
Search or jump to…

Pull requests
Issues
Marketplace
Explore
 @bwshockley Sign out
1
0 1,975 bwshockley/circuitpython
forked from adafruit/circuitpython
 Code  Pull requests 0  Projects 0  Insights  Settings
circuitpython/ports/atmel-samd/boards/itsybitsy_m4_express/board.c
2a26dac  on Apr 12
@tannewt tannewt Add initial ItsyBitsy M4 support
     
40 lines (35 sloc)  1.42 KB
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

#include "boards/board.h"
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"

void board_init(void) {
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
