# test stack manipulation


@micropython.asm_rv32
def l():
    li(a2, 3)
    addi(sp, sp, -8)
    li(a0, 0x123)
    sw(a0, 0(sp))
    addi(a1, a0, 0x111)
    sw(a1, 4(sp))
    c_lwsp(a3, 0)
    bne(a3, a0, END)
    addi(a2, a2, -1)
    c_lwsp(a3, 4)
    bne(a3, a1, END)
    addi(a2, a2, -1)
    label(END)
    addi(sp, sp, 8)
    mv(a0, a2)


print(l())


@micropython.asm_rv32
def s():
    li(a2, 3)
    addi(sp, sp, -8)
    li(a0, 0x123)
    c_swsp(a0, 0)
    addi(a1, a0, 0x111)
    c_swsp(a1, 4)
    lw(a3, 0(sp))
    bne(a3, a0, END)
    addi(a2, a2, -1)
    lw(a3, 4(sp))
    bne(a3, a1, END)
    addi(a2, a2, -1)
    label(END)
    addi(sp, sp, 8)
    mv(a0, a2)


print(s())


@micropython.asm_rv32
def a():
    li(a2, 3)
    addi(sp, sp, -8)
    li(a0, 0x123)
    sw(a0, 0(sp))
    addi(a1, a0, 0x111)
    sw(a1, 4(sp))
    c_addi4spn(a3, 4)
    lw(a3, -4(a3))
    bne(a3, a0, END)
    addi(a2, a2, -1)
    label(END)
    addi(sp, sp, 8)
    mv(a0, a2)


print(a())
