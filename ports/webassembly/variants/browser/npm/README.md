MicroPython WebAssembly, browser variant
========================================

This is the port of MicroPython to WebAssembly for use in browsers. It
includes JavaScript Promise Integration, so Python code can block on a
JavaScript promise with `jsffi.run_sync()` while the browser's event
loop keeps running.

The package holds two builds, each a `micropython.mjs` with its
`micropython.wasm` beside it: the standard build at the top level, and
in `settrace/` the same build with `sys.settrace` support for
debuggers. Import whichever `micropython.mjs` you want; each loads the
`.wasm` next to it.
