// Asyncify build of the webassembly port with the cooperative VM yield enabled,
// so long-running or self-looping Python suspends to the JS event loop and keeps
// the page responsive without the script having to await. This is the reference
// asynchronous build; variants/asyncify-fast trims the instrumentation for speed,
// and variants/jspi does the same job with Wasm stack switching instead.
#define MICROPY_VARIANT_ENABLE_JS_HOOK (1)
#define MICROPY_ENABLE_VM_YIELD (1)

// A long-running loop needs the heap to grow, and the auto split heap keeps
// collection bounded mid-loop (see main.c gc_collect_if_pending()).
#define MICROPY_GC_SPLIT_HEAP (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO (1)
