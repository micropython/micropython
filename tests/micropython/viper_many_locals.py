# Test that a native method with many locals compiles, exercising the large
# stack-adjustment path (AArch64 needs >511 locals).  Compiled but not called,
# so no large C stack frame is needed.

code = "@micropython.viper\ndef f() -> int:\n"
for i in range(520):
    code += "    v%d = %d\n" % (i, i)
code += "    return v0 + v1"

try:
    exec(code)
except (MemoryError, NotImplementedError):
    # Emitters may cap the locals count, or lack heap to compile this.
    print("SKIP")
    raise SystemExit

print("compiled ok")
