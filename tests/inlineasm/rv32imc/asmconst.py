# test constants in assembler


@micropython.asm_rv32imc
def c1():
    li(a0, 0xFFFFFFFF)
    li(a1, 0xF0000000)
    sub(a0, a0, a1)


print(hex(c1()))


@micropython.asm_rv32imc
def c2():
    lui(a0, 0x12345)
    li(a1, 0x678)
    add(a0, a0, a1)


print(hex(c2()))


@micropython.asm_rv32imc
def c3() -> uint:
    lui(a0, 0)
    addi(a0, a0, 0xFFF)


print(hex(c3()))


@micropython.asm_rv32imc
def c4():
    c_lui(a0, 1)
    c_li(a1, 1)
    c_add(a0, a1)


print(hex(c4()))
