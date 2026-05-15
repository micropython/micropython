# check if Xtensa inline asm supports LX7+ opcodes


@micropython.asm_xtensa
def f():
    salt(a0, a0, a0)


f()
print("lx7")
