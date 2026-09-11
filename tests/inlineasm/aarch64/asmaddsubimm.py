# test add/sub with immediate values


@micropython.asm_aarch64
def asm_add_imm(x0):
    add(x0, x0, 100)


print(asm_add_imm(1))
print(asm_add_imm(500))


@micropython.asm_aarch64
def asm_sub_imm(x0):
    sub(x0, x0, 50)


print(asm_sub_imm(100))
print(asm_sub_imm(500))


@micropython.asm_aarch64
def asm_add_sub_imm(x0):
    add(x0, x0, 1000)
    sub(x0, x0, 500)


print(asm_add_sub_imm(1))
