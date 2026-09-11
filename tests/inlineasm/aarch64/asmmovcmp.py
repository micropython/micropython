# test mov with register-to-register and cmp with immediate


@micropython.asm_aarch64
def asm_mov_reg(x0):
    mov(x1, x0)
    mov(x0, x1)


print(asm_mov_reg(42))


@micropython.asm_aarch64
def asm_cmp_imm(x0):
    mov(x1, 0)
    cmp(x0, 5)
    beq(eq_label)
    mov(x1, 1)
    label(eq_label)
    mov(x0, x1)


print(asm_cmp_imm(5))
print(asm_cmp_imm(10))


@micropython.asm_aarch64
def asm_mov_chain(x0):
    mov(x1, x0)
    mov(x2, x1)
    mov(x3, x2)
    mov(x0, x3)


print(asm_mov_chain(123))
