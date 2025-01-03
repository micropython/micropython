# test constants in assembler


@micropython.asm_rv32
def c1():
    li(a0, 0xFFFFFFFF)
    li(a1, 0xF0000000)
    sub(a0, a0, a1)


print(hex(c1()))


@micropython.asm_rv32
def c2():
    lui(a0, 0x12345)
    li(a1, 0x678)
    add(a0, a0, a1)


print(hex(c2()))


@micropython.asm_rv32
def c3() -> uint:
    lui(a0, 0)
    addi(a0, a0, 0x7FF)


print(hex(c3()))


@micropython.asm_rv32
def c4() -> uint:
    lui(a0, 0)
    addi(a0, a0, -1)


print(hex(c4()))


@micropython.asm_rv32
def c5():
    c_lui(a0, 1)
    c_li(a1, 1)
    c_add(a0, a1)


print(hex(c5()))
