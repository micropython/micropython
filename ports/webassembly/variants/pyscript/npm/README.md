MicroPython WebAssembly, PyScript variant
=========================================

This is the port of MicroPython to WebAssembly, with additions
to support PyScript.

The package holds `micropython.mjs` and two WebAssembly binaries it can
load: `micropython.wasm`, and `micropython-settrace.wasm`, which is the
same build with `sys.settrace` support for debuggers.
