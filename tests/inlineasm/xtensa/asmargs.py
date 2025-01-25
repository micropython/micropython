# test passing arguments


@micropython.asm_xtensa
def arg0():
    movi(a2, 1)


print(arg0())


@micropython.asm_xtensa
def arg1(a2):
    addi(a2, a2, 1)


print(arg1(1))


@micropython.asm_xtensa
def arg2(a2, a3):
    add(a2, a2, a3)


print(arg2(1, 2))


@micropython.asm_xtensa
def arg3(a2, a3, a4):
    add(a2, a2, a3)
    add(a2, a2, a4)


print(arg3(1, 2, 3))


@micropython.asm_xtensa
def arg4(a2, a3, a4, a5):
    add(a2, a2, a3)
    add(a2, a2, a4)
    add(a2, a2, a5)


print(arg4(1, 2, 3, 4))
