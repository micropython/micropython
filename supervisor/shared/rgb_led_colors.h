/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2021 Scott Shawcroft for Adafruit Industries
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

#define COLOR(r, g, b) (((r) << 16) | ((g) << 8) | (b))
// For brightness == 255 (full). This will be adjusted downward for various different RGB indicators,
// which vary in brightness.
#define INTENSITY (0x30)

#define BLACK   COLOR(0, 0, 0)
#define GREEN   COLOR(0, INTENSITY, 0)
#define BLUE    COLOR(0, 0, INTENSITY)
#define CYAN    COLOR(0, INTENSITY, INTENSITY)
#define RED     COLOR(INTENSITY, 0, 0)
#define ORANGE  COLOR(INTENSITY, INTENSITY * 2 / 3, 0)
#define YELLOW  COLOR(INTENSITY, INTENSITY, 0)
#define PURPLE  COLOR(INTENSITY, 0, INTENSITY)
#define WHITE   COLOR(INTENSITY, INTENSITY, INTENSITY)

#define ALL_DONE     GREEN
#define EXCEPTION    RED
#define SAFE_MODE    YELLOW
#define REPL_RUNNING WHITE

#define ALL_DONE_BLINKS 1
#define EXCEPTION_BLINKS 2
#define SAFE_MODE_BLINKS 3

#define OFF_ON_RATIO 3

#define LED_SLEEP_TIME_MS 5000u

#define BLINK_TIME_MS 100u
