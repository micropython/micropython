# Experimental JSPI variant of the webassembly port.
#
# Builds with Emscripten's JSPI (Wasm stack switching) rather than Asyncify, so
# the MicroPython entry points (mp_js_do_exec / _async / _import) become
# suspending exports that can yield to the JS event loop mid-execution. Combined
# with the cooperative VM yield (MICROPY_ENABLE_VM_YIELD, set in
# mpconfigvariant.h) this keeps the page responsive during long-running Python
# without the script having to cooperate.
#
# NOTE: JSPI is still experimental in Emscripten and requires a browser/runtime
# with Wasm stack-switching support (and, under Node, an --experimental flag).
#
# Suspending the stack under JSPI requires a pure-Wasm call stack. The default
# emscripten setjmp/longjmp backend implements longjmp via JS invoke_* wrappers,
# which inject non-promising JS frames (MicroPython's nlr uses setjmp/longjmp for
# every exception frame) and break suspension. So build setjmp/longjmp with the
# Wasm backend, at both compile and link time.
SUPPORT_LONGJMP = wasm
CFLAGS += -s SUPPORT_LONGJMP=wasm

# emscripten_scan_registers() needs Asyncify; under JSPI the GC scans the C stack
# only (see main.c MICROPY_GC_SCAN_REGISTERS).
CFLAGS += -DMICROPY_GC_SCAN_REGISTERS=0

JSFLAGS += -s ASYNCIFY=0
JSFLAGS += -s JSPI=1
JSFLAGS += -s JSPI_EXPORTS=["mp_js_do_exec","mp_js_do_exec_async","mp_js_do_import"]
JSFLAGS += -s ALLOW_TABLE_GROWTH=1
JSFLAGS += -s WASM_BIGINT=1
JSFLAGS += -s ALLOW_MEMORY_GROWTH

# Tell api.js to use the JSPI (direct promising call) invocation path.
SRC_JS += async_jspi.js
