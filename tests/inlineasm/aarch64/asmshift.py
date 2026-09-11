@micropython.asm_aarch64
def lsl1(x0):
    lsl(x0, x0, 1)


print(hex(lsl1(0x123)))


@micropython.asm_aarch64
def lsl23(x0):
    lsl(x0, x0, 23)


print(hex(lsl23(1)))


@micropython.asm_aarch64
def lsr1(x0):
    lsr(x0, x0, 1)


print(hex(lsr1(0x123)))


@micropython.asm_aarch64
def lsr31(x0):
    lsr(x0, x0, 31)


print(hex(lsr31(0x8000000000000000)))


@micropython.asm_aarch64
def asr1(x0):
    asr(x0, x0, 1)


print(hex(asr1(0x123)))


@micropython.asm_aarch64
def asr31(x0):
    asr(x0, x0, 31)


print(hex(asr31(0x8000000000000000)))
