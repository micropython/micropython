# test all conditional branch instructions


@micropython.asm_aarch64
def f_eq(x0, x1):
    cmp(x0, x1)
    beq(eq_label)
    mov(x0, 0)
    b(end)
    label(eq_label)
    mov(x0, 1)
    label(end)


@micropython.asm_aarch64
def f_ne(x0, x1):
    cmp(x0, x1)
    bne(ne_label)
    mov(x0, 0)
    b(end)
    label(ne_label)
    mov(x0, 1)
    label(end)


@micropython.asm_aarch64
def f_lt(x0, x1):
    cmp(x0, x1)
    blt(lt_label)
    mov(x0, 0)
    b(end)
    label(lt_label)
    mov(x0, 1)
    label(end)


@micropython.asm_aarch64
def f_gt(x0, x1):
    cmp(x0, x1)
    bgt(gt_label)
    mov(x0, 0)
    b(end)
    label(gt_label)
    mov(x0, 1)
    label(end)


@micropython.asm_aarch64
def f_le(x0, x1):
    cmp(x0, x1)
    ble(le_label)
    mov(x0, 0)
    b(end)
    label(le_label)
    mov(x0, 1)
    label(end)


@micropython.asm_aarch64
def f_ge(x0, x1):
    cmp(x0, x1)
    bge(ge_label)
    mov(x0, 0)
    b(end)
    label(ge_label)
    mov(x0, 1)
    label(end)


print(f_eq(5, 5))
print(f_eq(5, 6))
print(f_ne(5, 5))
print(f_ne(5, 6))
print(f_lt(3, 5))
print(f_lt(5, 5))
print(f_gt(7, 5))
print(f_gt(5, 5))
print(f_le(5, 5))
print(f_le(7, 5))
print(f_ge(5, 5))
print(f_ge(3, 5))
