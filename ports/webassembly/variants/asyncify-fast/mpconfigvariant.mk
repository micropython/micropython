# asyncify-fast: the asyncify variant, but with the Asyncify instrumentation cut
# from whole-program down to a curated set of functions, for a much smaller and
# ~2.5x faster build. Reuse the asyncify configuration, then narrow it.
#
# EXPERIMENTAL: the ASYNCIFY_ADD list below must stay complete — a suspend
# reachable through an indirect call that is not listed corrupts the Asyncify
# unwind/rewind silently. tests/ports/webassembly/asyncify_fast.mjs guards this
# by forcing a suspend through every dispatch path (operators, subscript, call,
# property, iter/next, context managers, recursion, generators, except handlers).
include variants/asyncify/mpconfigvariant.mk

# Asyncify normally instruments every function that could be on the stack at a
# suspend; for MicroPython that is almost the whole runtime, because indirect
# calls (the NLR jump callbacks and the print/type-slot vtables) are
# conservatively assumed to reach a suspend. Turn that off and re-add just the
# indirect-dispatch sites that genuinely sit above a suspend (the VM call
# machinery + protocol slots, in asyncify_add.txt); ASYNCIFY_ADD re-roots the
# analysis so their direct callers are picked up automatically.
JSFLAGS += -s ASYNCIFY_IGNORE_INDIRECT=1
JSFLAGS += -s ASYNCIFY_ADD=@asyncify_add.txt
# Relink when the curated list changes (it is only read at link time).
LINK_DEPS += asyncify_add.txt

# With indirect auto-detection off, MicroPython's NLR setjmp/longjmp must use the
# Wasm exception/longjmp backend: the default emscripten longjmp emits invoke_*
# JS trampolines that sit on the stack during a suspend and abort ("invoke_* not
# in ASYNCIFY_IMPORTS") once they are no longer auto-instrumented.
CFLAGS += -fwasm-exceptions
CFLAGS += -s SUPPORT_LONGJMP=wasm
LDFLAGS += -fwasm-exceptions
SUPPORT_LONGJMP = wasm
