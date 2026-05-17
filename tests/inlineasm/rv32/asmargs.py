# test passing arguments


@micropython.asm_rv32
def arg0():
    c_li(a0, 1)


print(arg0())


@micropython.asm_rv32
def arg1(a0):
    addi(a0, a0, 1)


print(arg1(1))


@micropython.asm_rv32
def arg2(a0, a1):
    add(a0, a0, a1)


print(arg2(1, 2))


@micropython.asm_rv32
def arg3(a0, a1, a2):
    add(a0, a0, a1)
    add(a0, a0, a2)


print(arg3(1, 2, 3))


@micropython.asm_rv32
def arg4(a0, a1, a2, a3):
    add(a0, a0, a1)
    add(a0, a0, a2)
    add(a0, a0, a3)


print(arg4(1, 2, 3, 4))
