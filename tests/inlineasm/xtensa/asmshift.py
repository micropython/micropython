@micropython.asm_xtensa
def lsl1(a2):
    slli(a2, a2, 1)


print(hex(lsl1(0x123)))


@micropython.asm_xtensa
def lsl23(a2):
    slli(a2, a2, 23)


print(hex(lsl23(1)))


@micropython.asm_xtensa
def lsr1(a2):
    srli(a2, a2, 1)


print(hex(lsr1(0x123)))


@micropython.asm_xtensa
def lsr15(a2):
    srli(a2, a2, 15)


print(hex(lsr15(0x80000000)))


@micropython.asm_xtensa
def asr1(a2):
    srai(a2, a2, 1)


print(hex(asr1(0x123)))


@micropython.asm_xtensa
def asr31(a2):
    srai(a2, a2, 31)


print(hex(asr31(0x80000000)))


@micropython.asm_xtensa
def lsl1r(a2):
    movi(a3, 1)
    ssl(a3)
    sll(a2, a2)


print(hex(lsl1r(0x123)))


@micropython.asm_xtensa
def lsr1r(a2):
    movi(a3, 1)
    ssr(a3)
    srl(a2, a2)


print(hex(lsr1r(0x123)))


@micropython.asm_xtensa
def asr1r(a2):
    movi(a3, 1)
    ssr(a3)
    sra(a2, a2)


print(hex(asr1r(0x123)))


@micropython.asm_xtensa
def f0(a2):
    ssai(9)
    sll(a2, a2)


print(hex(f0(1)))


@micropython.asm_xtensa
def f1(a2, a3):
    ssa8l(a3)
    srl(a2, a2)


print(hex(f1(0x12340000, 2)))


@micropython.asm_xtensa
def f2(a2, a3):
    ssa8b(a3)
    sll(a2, a2)


print(hex(f2(0x1234, 2)))


@micropython.asm_xtensa
def f3(a2, a3, a4):
    ssr(a4)
    src(a2, a2, a3)


print(hex(f3(0x00000001, 0x80000000, 2)))


@micropython.asm_xtensa
def f4(a2):
    srai(a2, a2, 24)


print(hex(f4(0x12345678)))
