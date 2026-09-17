# The jspi variant is the pyscript variant plus JavaScript Promise
# Integration for stack suspension (jsffi.run_sync).  Nothing needs
# removing from pyscript, which uses no Asyncify.  Requires an
# Emscripten toolchain and JS engine with JSPI support.

# Mirror of variants/pyscript/mpconfigvariant.mk, pointing at this
# variant's manifest (which includes pyscript's).
JSFLAGS += -s ALLOW_MEMORY_GROWTH
FROZEN_MANIFEST ?= variants/jspi/manifest.py

# The emscripten longjmp mode's JS invoke_* trampolines are
# incompatible with JSPI suspension, so use native wasm exceptions for
# longjmp at both compile and link time.
SUPPORT_LONGJMP = wasm
CFLAGS += -sSUPPORT_LONGJMP=wasm

JSFLAGS += -s JSPI
# The promising entries: runPythonAsync(), and the REPL character
# processor (the CLI drives the REPL through ccall's async path, which
# under JSPI requires a promising export; making it suspendable also
# lets run_sync() work when typed at the REPL).  Everything else stays
# synchronous, matching Pyodide's entry semantics.
JSFLAGS += -s JSPI_EXPORTS=mp_js_do_exec_async,mp_js_repl_process_char
