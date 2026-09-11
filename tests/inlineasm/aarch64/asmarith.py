@micropython.asm_aarch64
def asm_mul(x0, x1):
    mul(x0, x0, x1)


print(asm_mul(3, 4))
print(asm_mul(5, 6))


@micropython.asm_aarch64
def asm_add(x0, x1):
    add(x0, x0, x1)


print(asm_add(10, 20))


@micropython.asm_aarch64
def asm_sub(x0, x1):
    sub(x0, x0, x1)


print(asm_sub(30, 10))
