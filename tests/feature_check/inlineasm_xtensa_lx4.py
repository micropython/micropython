# check if Xtensa inline asm supports LX4+ opcodes


@micropython.asm_xtensa
def f():
    minu(a0, a0, a0)


f()
print("lx4")
