MicroPython WebAssembly
=======================

MicroPython for [WebAssembly](https://webassembly.org/).

Dependencies
------------

Building webassembly port bears the same requirements as the standard
MicroPython ports with the addition of Emscripten (and uglify-js for the
minified file).

The output includes `micropython.js` (a JavaScript wrapper for the
MicroPython runtime) and `firmware.wasm` (actual MicroPython compiled to
WASM).

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

The prerequisite for browser operation of micropython.js is to listen to the
`micropython-print` event, which is passed data when MicroPython code prints
something to stdout.  The following code demonstrates basic functionality:

```html
<!doctype html>
<html>
  <head>
    <script src="build/micropython.js"></script>
  </head>
  <body>
    <pre id="micropython-stdout"></pre>
    <script>
      document.addEventListener("micropython-print", function(e) {
        let output = document.getElementById("micropython-stdout");
        output.innerText += new TextDecoder().decode(e.detail);
      }, false);

      var mp_js_startup = Module["onRuntimeInitialized"];
      Module["onRuntimeInitialized"] = async function() {
        mp_js_startup();
        mp_js_init(64 * 1024);
        mp_js_do_str("print('hello world')");
      };
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
