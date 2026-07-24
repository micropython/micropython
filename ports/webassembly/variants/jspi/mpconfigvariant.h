// Experimental JSPI build of the webassembly port: uses Wasm stack switching
// (JSPI) instead of Asyncify, and enables the cooperative VM yield so the page
// stays responsive during long-running or self-looping Python.
#define MICROPY_ENABLE_VM_YIELD (1)

// A long-running loop needs the heap to grow, and the auto split heap keeps
// collection bounded mid-loop (see main.c gc_collect_if_pending()).
#define MICROPY_GC_SPLIT_HEAP (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO (1)
