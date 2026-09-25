# test cmp with immediates, including the shifted 12-bit form, and the
# boundary values of the add/sub/cmp immediate encodings


@micropython.asm_aarch64
def cmp_shift(x0):
    # cmp with a 12-bit immediate shifted left by 12: 0x1000
    mov(x1, 0)
    cmp(x0, 0x1000)
    beq(eq)
    mov(x1, 1)
    label(eq)
    mov(x0, x1)


print(cmp_shift(0x1000))
print(cmp_shift(0x1001))


@micropython.asm_aarch64
def cmp_shift_max(x0):
    # cmp with the largest shifted immediate: 0xfff000
    mov(x1, 0)
    cmp(x0, 0xFFF000)
    beq(eq)
    mov(x1, 1)
    label(eq)
    mov(x0, x1)


print(cmp_shift_max(0xFFF000))
print(cmp_shift_max(0xFFF001))


@micropython.asm_aarch64
def cmp_max_unshifted(x0):
    # cmp with the largest unshifted immediate: 0xfff
    mov(x1, 0)
    cmp(x0, 0xFFF)
    beq(eq)
    mov(x1, 1)
    label(eq)
    mov(x0, x1)


print(cmp_max_unshifted(0xFFF))
print(cmp_max_unshifted(0x1000))


@micropython.asm_aarch64
def add_imm_max(x0):
    # add with the largest shifted immediate: 0xfff000
    add(x0, x0, 0xFFF000)


print(hex(add_imm_max(0)))


@micropython.asm_aarch64
def sub_imm_max(x0):
    # sub with the largest shifted immediate: 0xfff000
    sub(x0, x0, 0xFFF000)


print(hex(sub_imm_max(0xFFF001)))
