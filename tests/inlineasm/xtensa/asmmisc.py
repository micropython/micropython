@micropython.asm_xtensa
def tnop(a2, a3, a4, a5):
    nop()


out2 = tnop(0x100, 0x200, 0x300, 0x400)
print(out2 == 0x100)


@micropython.asm_xtensa
def tnop_n(a2, a3, a4, a5):
    nop_n()


out2 = tnop_n(0x100, 0x200, 0x300, 0x400)
print(out2 == 0x100)


@micropython.asm_xtensa
def tmov_n(a2, a3):
    mov_n(a4, a3)
    add(a2, a4, a3)


print(tmov_n(0, 1))
