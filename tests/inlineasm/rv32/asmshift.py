@micropython.asm_rv32
def lsl1(a0):
    slli(a0, a0, 1)


print(hex(lsl1(0x123)))


@micropython.asm_rv32
def lsl23(a0):
    slli(a0, a0, 23)


print(hex(lsl23(1)))


@micropython.asm_rv32
def lsr1(a0):
    srli(a0, a0, 1)


print(hex(lsr1(0x123)))


@micropython.asm_rv32
def lsr31(a0):
    srli(a0, a0, 31)


print(hex(lsr31(0x80000000)))


@micropython.asm_rv32
def asr1(a0):
    srai(a0, a0, 1)


print(hex(asr1(0x123)))


@micropython.asm_rv32
def asr31(a0):
    srai(a0, a0, 31)


print(hex(asr31(0x80000000)))


@micropython.asm_rv32
def clsl1(a0):
    c_slli(a0, 1)


print(hex(clsl1(0x123)))


@micropython.asm_rv32
def clsl23(a0):
    c_slli(a0, 23)


print(hex(clsl23(1)))


@micropython.asm_rv32
def clsr1(a0):
    c_srli(a0, 1)


print(hex(clsr1(0x123)))


@micropython.asm_rv32
def clsr31(a0):
    c_srli(a0, 31)


print(hex(clsr31(0x80000000)))


@micropython.asm_rv32
def casr1(a0):
    c_srai(a0, 1)


print(hex(casr1(0x123)))


@micropython.asm_rv32
def casr31(a0):
    c_srai(a0, 31)


print(hex(casr31(0x80000000)))


@micropython.asm_rv32
def lsl1r(a0):
    li(a1, 1)
    sll(a0, a0, a1)


print(hex(lsl1r(0x123)))


@micropython.asm_rv32
def lsr1r(a0):
    li(a1, 1)
    srl(a0, a0, a1)


print(hex(lsr1r(0x123)))


@micropython.asm_rv32
def asr1r(a0):
    li(a1, 1)
    sra(a0, a0, a1)


print(hex(asr1r(0x123)))
