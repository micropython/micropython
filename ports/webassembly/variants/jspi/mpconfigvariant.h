// Experimental JSPI build of the webassembly port: uses Wasm stack switching
// (JSPI) instead of Asyncify, and enables the cooperative VM yield so the page
// stays responsive during long-running or self-looping Python.
#define MICROPY_ENABLE_VM_YIELD (1)
