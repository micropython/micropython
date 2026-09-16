# Test that a native method with a large number of local variables compiles.
# This exercises the large stack-adjustment path of the native emitters (AArch64
# needs more than 511 locals to reach it).  The function is compiled but not
# called, so that it does not need a large amount of C stack.

code = "@micropython.viper\ndef f() -> int:\n"
for i in range(520):
    code += "    v%d = %d\n" % (i, i)
code += "    return v0 + v1"

try:
    exec(code)
except (MemoryError, NotImplementedError):
    # Some native emitters limit the number of locals in a native method, and
    # some targets do not have enough heap to compile such a large function.
    print("SKIP")
    raise SystemExit

print("compiled ok")
