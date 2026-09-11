@micropython.asm_aarch64
def mov_reg(x0, x1):
    mov(x0, x1)


@micropython.asm_aarch64
def cmp_reg_eq(x0, x1):
    cmp(x0, x1)
    bne(ne_label)
    mov(x0, 1)
    b(end)
    label(ne_label)
    mov(x0, 0)
    label(end)


@micropython.asm_aarch64
def mvn_reg(x0):
    mvn(x0, x0)


print(mov_reg(10, 99))
print(cmp_reg_eq(5, 5))
print(cmp_reg_eq(5, 6))
print(hex(mvn_reg(0)))
print(hex(mvn_reg(0xFF)))
