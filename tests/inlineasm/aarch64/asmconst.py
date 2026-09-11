# test constants in assembler


@micropython.asm_aarch64
def c1():
    mov(x0, 0x1234)


print(hex(c1()))


@micropython.asm_aarch64
def c2():
    mov(x0, -1)


print(hex(c2()))


@micropython.asm_aarch64
def c3():
    mov(x0, 0)
    mov(x1, 0x100)
    sub(x0, x0, x1)


print(hex(c3()))
