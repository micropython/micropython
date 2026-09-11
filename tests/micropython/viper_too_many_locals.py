# Test that native emitters report a clean error when a function has more
# locals than the emitter can address: on AArch64 the scaled 12-bit LDR/STR
# immediate caps slots at 4096, so 4097 locals must raise NotImplementedError
# (Thumb caps at 511; small targets may raise MemoryError -- all SKIP).
# Targets that accept it print "compiled ok"; the function is never called,
# so no ~32 KiB C stack frame is needed.

NUM_LOCALS = 4097  # first local count whose slot index exceeds the AArch64 limit

# Locals are assigned from the runtime argument, not immediates: the emitter
# lazily folds immediates and would never touch the high slots otherwise.
code = "@micropython.viper\ndef f(a: int) -> int:\n"
code += "".join("    v%d = a\n" % i for i in range(NUM_LOCALS))
code += "    return v0 + v%d\n" % (NUM_LOCALS - 1)

try:
    exec(code)
except (MemoryError, NotImplementedError):
    print("SKIP")
    raise SystemExit

print("compiled ok")
