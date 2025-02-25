# test branch instructions


@micropython.asm_xtensa
def tball(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    ball(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tball(0xFFFFFFFF, 0xFFFFFFFF))
print(tball(0xFFFEFFFF, 0xFFFFFFFF))
print(tball(0x00000000, 0xFFFFFFFF))
print(tball(0xFFFFFFFF, 0x01010101))


@micropython.asm_xtensa
def tbany(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bany(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbany(0xFFFFFFFF, 0xFFFFFFFF))
print(tbany(0xFFFEFFFF, 0xFFFFFFFF))
print(tbany(0x00000000, 0xFFFFFFFF))
print(tbany(0xFFFFFFFF, 0x01010101))


@micropython.asm_xtensa
def tbbc(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bbc(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbbc(0xFFFFFFFF, 4))
print(tbbc(0xFFFEFFFF, 16))
print(tbbc(0x00000000, 1))


BBCI_TEMPLATE = """
@micropython.asm_xtensa
def tbbci(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bbci(a3, {}, end)
    movi(a2, -1)
    label(end)

print(tbbci({}))
"""


for value, bit in ((0xFFFFFFFF, 4), (0xFFFEFFFF, 16), (0x00000000, 1)):
    try:
        exec(BBCI_TEMPLATE.format(bit, value))
    except SyntaxError as error:
        print(error)


@micropython.asm_xtensa
def tbbs(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bbs(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbbs(0x00000000, 4))
print(tbbs(0x00010000, 16))
print(tbbs(0xFFFFFFFF, 1))


BBSI_TEMPLATE = """
@micropython.asm_xtensa
def tbbsi(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bbsi(a3, {}, end)
    movi(a2, -1)
    label(end)

print(tbbsi({}))
"""


for value, bit in ((0x00000000, 4), (0x00010000, 16), (0xFFFFFFFF, 1)):
    try:
        exec(BBSI_TEMPLATE.format(bit, value))
    except SyntaxError as error:
        print(error)


@micropython.asm_xtensa
def tbeq(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    beq(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbeq(0x00000000, 0x00000000))
print(tbeq(0x00010000, 0x00000000))
print(tbeq(0xFFFFFFFF, 0xFFFFFFFF))


@micropython.asm_xtensa
def tbeqz(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    beqz(a3, end)
    movi(a2, -1)
    label(end)


print(tbeqz(0))
print(tbeqz(0x12345678))
print(tbeqz(-1))


@micropython.asm_xtensa
def tbge(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bge(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbge(0x00000000, 0x00000000))
print(tbge(0x00010000, 0x00000000))
print(tbge(0xF0000000, 0xFFFFFFFF))


@micropython.asm_xtensa
def tbgeu(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bgeu(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbgeu(0x00000000, 0x00000000))
print(tbgeu(0x00010000, 0x00000000))
print(tbgeu(0xF0000000, 0xFFFFFFFF))
print(tbgeu(0xFFFFFFFF, 0xF0000000))


@micropython.asm_xtensa
def tbgez(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bgez(a3, end)
    movi(a2, -1)
    label(end)


print(tbgez(0))
print(tbgez(0x12345678))
print(tbgez(-1))


@micropython.asm_xtensa
def tblt(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    blt(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tblt(0x00000000, 0x00000000))
print(tblt(0x00010000, 0x00000000))
print(tblt(0xF0000000, 0xFFFFFFFF))


@micropython.asm_xtensa
def tbltu(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bltu(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbltu(0x00000000, 0x00000000))
print(tbltu(0x00010000, 0x00000000))
print(tbltu(0xF0000000, 0xFFFFFFFF))
print(tbltu(0xFFFFFFFF, 0xF0000000))


@micropython.asm_xtensa
def tbltz(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bltz(a3, end)
    movi(a2, -1)
    label(end)


print(tbltz(0))
print(tbltz(0x12345678))
print(tbltz(-1))


@micropython.asm_xtensa
def tbnall(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bnall(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbnall(0xFFFFFFFF, 0xFFFFFFFF))
print(tbnall(0xFFFEFFFF, 0xFFFFFFFF))
print(tbnall(0x00000000, 0xFFFFFFFF))
print(tbnall(0xFFFFFFFF, 0x01010101))


@micropython.asm_xtensa
def tbne(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bne(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbne(0x00000000, 0x00000000))
print(tbne(0x00010000, 0x00000000))
print(tbne(0xFFFFFFFF, 0xFFFFFFFF))


@micropython.asm_xtensa
def tbnez(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bnez(a3, end)
    movi(a2, -1)
    label(end)


print(tbnez(0))
print(tbnez(0x12345678))
print(tbnez(-1))


@micropython.asm_xtensa
def tbnone(a2, a3) -> int:
    mov(a4, a2)
    movi(a2, 0)
    bnone(a4, a3, end)
    movi(a2, -1)
    label(end)


print(tbnone(0xFFFFFFFF, 0xFFFFFFFF))
print(tbnone(0xFFFEFFFF, 0xFFFFFFFF))
print(tbnone(0x00000000, 0xFFFFFFFF))
print(tbnone(0x10101010, 0x01010101))


@micropython.asm_xtensa
def tbeqz_n(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    beqz_n(a3, end)
    movi(a2, -1)
    label(end)


print(tbeqz_n(0))
print(tbeqz_n(0x12345678))
print(tbeqz_n(-1))


@micropython.asm_xtensa
def tbnez_n(a2) -> int:
    mov(a3, a2)
    movi(a2, 0)
    bnez(a3, end)
    movi(a2, -1)
    label(end)


print(tbnez_n(0))
print(tbnez_n(0x12345678))
print(tbnez_n(-1))
