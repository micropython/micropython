// Announce the async backend to api.js: this build uses JSPI (Wasm stack
// switching), so the MicroPython entry points are promising exports — api.js
// calls them directly and awaits the returned Promise. Added to SRC_JS by
// variants that build with -sJSPI.
globalThis.__MICROPYTHON_ASYNC__ = "jspi";
