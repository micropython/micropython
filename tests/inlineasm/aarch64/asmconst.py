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


@micropython.asm_aarch64
def c4(x0):
    # 64-bit mov: a full 32-bit unsigned constant (larger than INT32_MAX).
    mov(x0, 0xFFFFFFFF)


print(hex(c4(0)))


@micropython.asm_aarch64
def c5(x0):
    # 64-bit mov: a constant wider than 32 bits.
    mov(x0, 0x123456789)


print(hex(c5(0)))
