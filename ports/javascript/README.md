MicroPython.js
==============

MicroPython transmuted into Javascript by Emscripten.

Dependencies
------------

Building micropython.js bears the same requirements as the standard MicroPython
ports with the addition of Emscripten (and uglify-js for the minified file). 

A standard installation of Emscripten should provide functional code, however
if memory errors are encountered it may be worthwhile to modify the tool.
`emscripten-fastcomp/lib/Target/JSBackend.cpp` may require the minor fix 
found in JSBackend.patch. This patch attempts to address situations where
C code running through Emscripten is denied access to Javascript variables 
leading to false-positives in the MicroPython garbage collector as variables 
with pointers exclusively in Javascript will be erased prematurely.
Refer to Emscripten documentation for instructions on building Emscripten 
from source.

Build instructions
------------------

In order to build micropython.js, run:

    $ make

To generate the minified file micropython.min.js, run:

    $ make min

Running with Node.js
--------------------

Access the repl with:

    $ node build/micropython.js

Stack size may be modified using:

	$ node build/micropython.js -X stack=64K

Where stack size may be represented in Bytes, KiB or MiB.

MicroPython scripts may be executed using:

	$ node build/micropython.js hello.py

Alternatively micropython.js may by accessed by other javascript programs in node 
using the require command and the general API outlined below. For example:

```javascript
var mp_js = require('./build/micropython.js');

mp_js_init(64 * 1024);
mp_js_do_str("print('hello world')\n");
```

Running with HTML
-----------------

The prerequisite for browser operation of micropython.js is an element with
the id `mp_js_stdout` which receives `print` events. The following code
demonstrates basic functionality:

```html
<!doctype html>
<html>
  <head>
    <script src="build/micropython.js"></script>
  </head>
  <body>
    <div id='mp_js_stdout'></div>
    <script>
      mp_js_stdout.addEventListener('print', function(e) {
        document.write(e.data);
      }, false);

      mp_js_init(64 * 1024);
      mp_js_do_str('print(\'hello world\')');
    </script>
  </body>
</html>
```

MicroPython code execution will suspend the browser so be sure to atomize usage
within this environment. Unfortunately interrupts have not been implemented for the 
browser.

Testing
-------

Run the test suite using:

    $ make test

API
---

The following functions have been exposed to javascript.

```
mp_js_init(stack_size)
```

Initialize MicroPython with the given stack size in bytes. This must be
called before attempting to interact with MicroPython.

```
mp_js_do_str(code)
```

Execute the input code. `code` must be a `string`.

```
mp_js_init_repl()
```

Initialize MicroPython repl. Must be called before entering characters into
the repl.

```
mp_js_process_char(char)
```

Input character into MicroPython repl. `char` must be of type `number`. This 
will execute MicroPython code when necessary.
