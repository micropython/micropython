@micropython.asm_aarch64
def f_bl(x0):
    bl(myfunc)
    b(end)
    label(myfunc)
    mov(x0, 42)
    ret(x30)
    label(end)


print(f_bl(0))


@micropython.asm_aarch64
def f_bl_skip(x0):
    cmp(x0, 0)
    bne(skip)
    mov(x0, 99)
    b(end2)
    label(skip)
    mov(x0, 77)
    label(end2)


print(f_bl_skip(0))
print(f_bl_skip(1))
