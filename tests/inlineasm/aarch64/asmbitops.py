@micropython.asm_aarch64
def f1(x0, x1):
    and_(x0, x0, x1)


print(hex(f1(0x123, 0xf0)))


@micropython.asm_aarch64
def f2(x0, x1):
    orr(x0, x0, x1)


print(hex(f2(0x123, 0xf0)))


@micropython.asm_aarch64
def f3(x0, x1):
    eor(x0, x0, x1)


print(hex(f3(0x123, 0xff)))


@micropython.asm_aarch64
def f4(x0):
    mvn(x0, x0)


print(hex(f4(0x123) & 0xffffffffffffffff))
