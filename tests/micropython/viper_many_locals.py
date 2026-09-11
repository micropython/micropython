# test that a function with many local variables compiles
# this triggers the large stack adjustment path in asm_aarch64_entry/exit
# where stack_adjust >= 4096 (requires >= 511 local variables)
# the function is compiled but not called to avoid potential runtime issues


code = "@micropython.viper\ndef f() -> int:\n"
for i in range(520):
    code += "    v%d = %d\n" % (i, i)
code += "    return v0 + v1"
exec(code)

print("compiled ok")
