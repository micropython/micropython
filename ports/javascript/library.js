/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Rami Ali
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

mergeInto(LibraryManager.library, {
    mp_js_write: function(ptr, len) {
        for (var i = 0; i < len; ++i) {
            c = String.fromCharCode(getValue(ptr + i, 'i8'));
            if (typeof window === 'undefined') {
                process.stdout.write(c);
            } else {
                var mp_js_stdout = document.getElementById('mp_js_stdout');
                var print = new Event('print');
                print.data = c;
                mp_js_stdout.dispatchEvent(print);
            }
        }
    },

    mp_js_ticks_ms: function() {
        return (new Date()).getTime() - MP_JS_EPOCH;
    },

    mp_js_hook: function() {
        if (typeof window === 'undefined') {
            var mp_interrupt_char = Module.ccall('mp_hal_get_interrupt_char', 'number', ['number'], ['null']);
            var fs = require('fs');

            var buf = Buffer.alloc(1);
            try {
                var n = fs.readSync(process.stdin.fd, buf, 0, 1);
                if (n > 0) {
                    if (buf[0] == mp_interrupt_char) {
                        Module.ccall('mp_keyboard_interrupt', 'null', ['null'], ['null']);
                    } else {
                        process.stdout.write(String.fromCharCode(buf[0]));
                    }
                }
            } catch (e) {
                if (e.code === 'EAGAIN') {
                } else {
                    throw e;
                }
            }
        }
    },
});
