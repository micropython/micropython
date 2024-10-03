# test load/store opcodes


@micropython.asm_rv32imc
def l():
    li(a5, 4)
    addi(sp, sp, -12)
    li(a0, 0x123)
    c_swsp(a0, 0)
    addi(a1, a0, 0x111)
    c_swsp(a1, 4)
    addi(a2, a1, 0x111)
    c_swsp(a2, 8)
    mov(a4, sp)
    c_lw(a3, a4, 0)
    bne(a3, a0, END)
    addi(a5, a5, -1)
    lw(a3, a4, 4)
    bne(a3, a1, END)
    addi(a5, a5, -1)
    lhu(a3, a4, 8)
    bne(a3, a2, END)
    addi(a5, a5, -1)
    lbu(a0, a4, 8)
    addi(a0, a0, 0x300)
    bne(a0, a2, END)
    addi(a5, a5, -1)
    label(END)
    addi(sp, sp, 12)
    mov(a0, a5)


print(l())


@micropython.asm_rv32imc
def s():
    li(a5, 4)
    addi(sp, sp, -12)
    c_swsp(zero, 0)
    c_swsp(zero, 4)
    c_swsp(zero, 8)
    li(a0, 0x12345)
    mov(a4, sp)
    c_sw(a0, a4, 0)
    sh(a0, a4, 4)
    sb(a0, a4, 8)
    li(a1, 0xFFFF)
    and_(a1, a0, a1)
    andi(a2, a0, 0xFF)
    lw(a3, sp, 0)
    bne(a3, a0, END)
    addi(a5, a5, -1)
    lw(a3, sp, 4)
    bne(a3, a1, END)
    addi(a5, a5, -1)
    lw(a3, sp, 8)
    bne(a3, a2, END)
    addi(a5, a5, -1)
    label(END)
    addi(sp, sp, 12)
    mov(a0, a5)


print(s())


@micropython.asm_rv32imc
def lu():
    li(a5, 4)
    addi(sp, sp, -8)
    li(a0, 0xF1234567)
    c_swsp(a0, 0)
    c_swsp(a0, 4)
    lh(a1, sp, 0)
    blt(a1, zero, END)
    addi(a5, a5, -1)
    lb(a2, sp, 4)
    blt(a2, zero, END)
    addi(a5, a5, -1)
    label(END)
    addi(sp, sp, 8)
    mov(a0, a5)


print(lu())
