@micropython.asm_rv32
def f1():
    li(a0, 0)
    la(a1, END)
    c_jr(a1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    label(END)


print(f1())


@micropython.asm_rv32
def f2():
    addi(sp, sp, -4)
    c_swsp(ra, 0)
    li(ra, 0)
    li(a0, 0)
    c_jal(END)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    label(END)
    bne(ra, zero, SUCCESS)
    c_addi(a0, 2)
    label(SUCCESS)
    c_lwsp(ra, 0)
    addi(sp, sp, 4)


print(f2())


@micropython.asm_rv32
def f3():
    li(a0, 0)
    c_j(END)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    label(END)


print(f3())


@micropython.asm_rv32
def f4():
    addi(sp, sp, -4)
    c_swsp(ra, 0)
    li(ra, 0)
    li(a0, 0)
    la(a1, END)
    c_jalr(a1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    label(END)
    bne(ra, zero, SUCCESS)
    c_addi(a0, 2)
    label(SUCCESS)
    c_lwsp(ra, 0)
    addi(sp, sp, 4)


print(f4())


@micropython.asm_rv32
def f5():
    li(a0, 0)
    li(a1, 0)
    jal(a1, END)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    label(END)
    bne(a1, zero, SUCCESS)
    c_addi(a0, 2)
    label(SUCCESS)


print(f5())


@micropython.asm_rv32
def f6():
    li(a0, 0)
    la(a1, JUMP)
    li(a2, 0)
    jalr(a2, a1, 10)
    label(JUMP)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    c_addi(a0, 1)
    bne(a2, zero, SUCCESS)
    c_addi(a0, 2)
    label(SUCCESS)


print(f6())
