// Announce the async backend to api.js: this build uses Asyncify, so the
// MicroPython entry points suspend via ccall({async: true}) and return a
// Promise. Added to SRC_JS by variants that build with -sASYNCIFY=1.
globalThis.__MICROPYTHON_ASYNC__ = "asyncify";
