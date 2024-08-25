# test branch instructions


@micropython.asm_rv32
def tbeq(a0):
    mv(a1, a0)

    li(a0, 10)
    li(a2, 1)
    beq(a1, a2, end)

    li(a0, 20)
    li(a2, 2)
    beq(a1, a2, end)

    li(a0, 30)
    li(a2, 3)
    beq(a1, a2, end)

    li(a0, 0)

    label(end)


print(tbeq(0))
print(tbeq(1))
print(tbeq(2))
print(tbeq(3))


@micropython.asm_rv32
def tbne(a0):
    mv(a1, a0)

    li(a0, 10)
    li(a2, 1)
    bne(a1, a2, end)

    li(a0, 20)
    li(a2, 2)
    bne(a1, a2, end)

    li(a0, 30)
    li(a2, 3)
    bne(a1, a2, end)

    li(a0, 0)

    label(end)


print(tbne(0))
print(tbne(1))
print(tbne(2))
print(tbne(3))


@micropython.asm_rv32
def tbgeu(a0):
    mv(a1, a0)

    li(a0, 1)
    li(a2, 2)
    bgeu(a1, a2, end)
    li(a0, 0)

    label(end)


print(tbgeu(0))
print(tbgeu(1))
print(tbgeu(2))
print(tbgeu(3))


@micropython.asm_rv32
def tbltu(a0):
    mv(a1, a0)

    li(a0, 1)
    li(a2, 2)
    bltu(a1, a2, end)
    li(a0, 0)

    label(end)


print(tbltu(0))
print(tbltu(1))
print(tbltu(2))
print(tbltu(3))


@micropython.asm_rv32
def tbge(a0):
    mv(a1, a0)

    li(a0, 1)
    li(a2, -2)
    bge(a1, a2, end)
    li(a0, 0)

    label(end)


print(tbge(-3))
print(tbge(-2))
print(tbge(-1))
print(tbge(0))


@micropython.asm_rv32
def tblt(a0):
    mv(a1, a0)

    li(a0, 1)
    li(a2, -2)
    blt(a1, a2, end)
    li(a0, 0)

    label(end)


print(tblt(-3))
print(tblt(-2))
print(tblt(-1))
print(tblt(0))


@micropython.asm_rv32
def tcbeqz(a0):
    mv(a1, a0)

    li(a0, 1)
    c_beqz(a1, end)
    li(a0, 0)

    label(end)


print(tcbeqz(0))
print(tcbeqz(1))
print(tcbeqz(2))
print(tcbeqz(3))


@micropython.asm_rv32
def tcbnez(a0):
    mv(a1, a0)

    li(a0, 1)
    c_bnez(a1, end)
    li(a0, 0)

    label(end)


print(tcbnez(0))
print(tcbnez(1))
print(tcbnez(2))
print(tcbnez(3))
