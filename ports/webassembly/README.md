MicroPython WebAssembly
=======================

MicroPython for [WebAssembly](https://webassembly.org/).

Dependencies
------------

Building the webassembly port bears the same requirements as the standard
MicroPython ports with the addition of Emscripten, and optionally terser for
the minified file.

The output includes `micropython.mjs` (a JavaScript wrapper for the
MicroPython runtime) and `micropython.wasm` (actual MicroPython compiled to
WASM).

Build instructions
------------------

In order to build `micropython.mjs`, run:

    $ make

To generate the minified file `micropython.min.mjs`, run:

    $ make min

Build variants
--------------

The port builds in several variants, selected with `make VARIANT=<name>` (the
output goes to `build-<name>/`):

- `standard` (default): the synchronous build.
- `pyscript`: the standard build plus a frozen manifest, used by PyScript.
- `asyncify`: adds a cooperative yield so long-running or self-looping Python
  periodically hands the JS event loop a turn (keeping the page responsive
  without the script having to `await`), and enables `jsffi.run_sync()`.  Built
  with Emscripten's Asyncify.
- `asyncify-fast`: the `asyncify` variant with the Asyncify instrumentation
  narrowed to a curated set of functions, for a much smaller and faster build.
- `jspi`: experimental; the same asynchronous behaviour built on Emscripten's
  JSPI (Wasm stack switching) instead of Asyncify.  Requires a runtime with
  Wasm stack-switching support.

The asynchronous variants announce their backend to the JavaScript wrapper, so
`runPython`/`runPythonAsync` transparently suspend to the event loop where the
build supports it; the wrapper API is otherwise identical across variants.

Running with Node.js
--------------------

Access the repl with:

    $ make repl

This is the same as running:

    $ node build-standard/micropython.mjs

The initial MicroPython GC heap size may be modified using:

    $ node build-standard/micropython.mjs -X heapsize=64k

Where stack size may be represented in bytes, or have a `k` or `m` suffix.

MicroPython scripts may be executed using:

    $ node build-standard/micropython.mjs hello.py

Alternatively `micropython.mjs` may by accessed by other JavaScript programs in node
using the require command and the general API outlined below. For example:

```javascript
const mp_mjs = await import("micropython.mjs");
const mp = await mp_mjs.loadMicroPython();

mp.runPython("print('hello world')");
```

Or without await notation:

```javascript
import("micropython.mjs").then((mp_mjs) => {
    mp_mjs.loadMicroPython().then((mp) => {
        mp.runPython("print('hello world')");
    });
});
```

Running with HTML
-----------------

The following code demonstrates the simplest way to load `micropython.mjs` in a
browser, create an interpreter context, and run some Python code:

```html
<!doctype html>
<html>
  <head>
    <script src="build-standard/micropython.mjs" type="module"></script>
  </head>
  <body>
    <script type="module">
      const mp = await loadMicroPython();
      mp.runPython("print('hello world')");
    </script>
  </body>
</html>
```

The output in the above example will go to the JavaScript console.  It's possible
to instead capture the output and print it somewhere else, for example in an
HTML element.  The following example shows how to do this, and also demonstrates
the use of top-level await and the `js` module:

```html
<!doctype html>
<html>
  <head>
    <script src="build-standard/micropython.mjs" type="module"></script>
  </head>
  <body>
    <pre id="micropython-stdout"></pre>
    <script type="module">
      const stdoutWriter = (line) => {
        document.getElementById("micropython-stdout").innerText += line + "\n";
      };
      const mp = await loadMicroPython({stdout:stdoutWriter});
      await mp.runPythonAsync(`
        import js
        url = "https://api.github.com/users/micropython"
        print(f"fetching {url}...")
        res = await js.fetch(url)
        json = await res.json()
        for i in dir(json):
          print(f"{i}: {json[i]}")
      `);
    </script>
  </body>
</html>
```

MicroPython code execution will suspend the browser so be sure to atomize usage
within this environment. Unfortunately interrupts have not been implemented for the
browser.

In Node.js, pressing Ctrl+C during Python execution sends a keyboard interrupt
(``KeyboardInterrupt``) to the running MicroPython instance via ``mp_js_hook``,
which periodically polls stdin for the interrupt character.

Testing
-------

Run the test suite using:

    $ make test

API
---

The following functions have been exposed to JavaScript through the interpreter
context, created and returned by `loadMicroPython()`.

- `PyProxy`: the type of the object that proxies Python objects.

- `FS`: the Emscripten filesystem object.

- `globals`: an object exposing the globals from the Python `__main__` module,
  with methods `get(key)`, `set(key, value)` and `delete(key)`.

- `registerJsModule(name, module)`: register a JavaScript object as importable
  from Python with the given name.

- `pyimport`: import a Python module and return it.

- `runPython(code)`: execute Python code and return the result.

- `runPythonAsync(code)`: execute Python code and return the result, allowing for
  top-level await expressions (this call must be await'ed on the JavaScript side).

- `interrupt()`: schedule a `KeyboardInterrupt` on the running VM.  On a
  suspend-capable build (see Build variants) this is safe to call while a
  `runPython`/`runPythonAsync` call is in flight, and unwinds it at the next
  bytecode check; a host can use it to stop a running script without tearing down
  the instance.

- `replInit()`: initialise the REPL.

- `replProcessChar(chr)`: process an incoming character at the REPL.

- `replProcessCharWithAsyncify(chr)`: process an incoming character at the REPL,
  for use when ASYNCIFY is enabled.

Type conversions
----------------

Read-only objects (booleanns, numbers, strings, etc) are converted when passed between
Python and JavaScript.  The conversions are:

- JavaScript `null` converts to/from Python `None`.
- JavaScript `undefined` converts to/from Python `js.undefined`.

The conversion between `null` and `None` matches the behaviour of the Python `json`
module.

Proxying
--------

A Python `dict` instance is proxied such that:

    for (const key in dict) {
        print(key, dict[key]);
    }

works as expected on the JavaScript side and iterates through the keys of the
Python `dict`.  Furthermore, when JavaScript accesses a key that does not exist
in the Python dict, the JavaScript code receives `undefined` instead of a
`KeyError` exception being raised.
