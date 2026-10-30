# The browser variant: full features plus JavaScript Promise Integration
# for stack suspension (jsffi.run_sync).  Requires an Emscripten
# toolchain and a JS engine with JSPI support.

JSFLAGS += -s ALLOW_MEMORY_GROWTH
FROZEN_MANIFEST ?= variants/browser/manifest.py

# The emscripten longjmp mode's JS invoke_* trampolines are
# incompatible with JSPI suspension, so use native wasm exceptions for
# longjmp at both compile and link time.
SUPPORT_LONGJMP = wasm
CFLAGS += -sSUPPORT_LONGJMP=wasm

JSFLAGS += -s JSPI
# The promising entries: runPythonAsync(), the generator resume used
# for code that awaits (so run_sync() also works after an await), and
# the async REPL character processor (used by the CLI, so run_sync()
# works when typed at the REPL).  Everything else stays synchronous,
# matching Pyodide's entry semantics.
JSFLAGS += -s JSPI_EXPORTS=mp_js_do_exec_async,proxy_c_to_js_resume,mp_js_repl_process_char_async
