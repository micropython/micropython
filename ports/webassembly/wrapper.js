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

var Module = {};

var mainProgram = function()
{
  mp_js_init = Module.cwrap('mp_js_init', 'null', ['number']);
  mp_js_do_str = Module.cwrap('mp_js_do_str', 'number', ['string']);
  mp_js_init_repl = Module.cwrap('mp_js_init_repl', 'null', ['null']);
  mp_js_process_char = Module.cwrap('mp_js_process_char', 'number', ['number']);

  MP_JS_EPOCH = Date.now();

  if (typeof window === 'undefined' && require.main === module) {
      var fs = require('fs');
      var heap_size = 128 * 1024;
      var contents = '';
      var repl = true;

      for (var i = 0; i < process.argv.length; i++) {
          if (process.argv[i] === '-X' && i < process.argv.length - 1) {
              if (process.argv[i + 1].includes('heapsize=')) {
                  heap_size = parseInt(process.argv[i + 1].split('heapsize=')[1]);
                  if (process.argv[i + 1].substr(-1).toLowerCase() === 'k') {
                      heap_size *= 1024;
                  } else if (process.argv[i + 1].substr(-1).toLowerCase() === 'm') {
                      heap_size *= 1024 * 1024;
                  }
              }
          } else if (process.argv[i].includes('.py')) {
              contents += fs.readFileSync(process.argv[i], 'utf8');
              repl = false;;
          }
      }

      if (process.stdin.isTTY === false) {
          contents = fs.readFileSync(0, 'utf8');
          repl = 0;
      }

      mp_js_init(heap_size);

      if (repl) {
          mp_js_init_repl();
          process.stdin.setRawMode(true);
          process.stdin.on('data', function (data) {
              for (var i = 0; i < data.length; i++) {
                  if (mp_js_process_char(data[i])) {
                      process.exit()
                  }
              }
          });
      } else {
          process.exitCode = mp_js_do_str(contents);
      }
  }
}

Module["onRuntimeInitialized"] = mainProgram;
