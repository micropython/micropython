MicroPython WebAssembly, browser variant
========================================

This is the port of MicroPython to WebAssembly for use in browsers. It
includes JavaScript Promise Integration, so Python code can block on a
JavaScript promise with `jsffi.run_sync()` while the browser's event
loop keeps running.

The package holds `micropython.mjs` and two WebAssembly binaries it can
load: `micropython.wasm`, and `micropython-settrace.wasm`, which is the
same build with `sys.settrace` support for debuggers.
