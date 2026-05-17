@micropython.asm_rv32
def sdiv(a0, a1):
    div(a0, a0, a1)


@micropython.asm_rv32
def udiv(a0, a1):
    divu(a0, a0, a1)


@micropython.asm_rv32
def srem(a0, a1):
    rem(a0, a0, a1)


@micropython.asm_rv32
def urem(a0, a1):
    remu(a0, a0, a1)


print(sdiv(1234, 3))
print(sdiv(-1234, 3))
print(sdiv(1234, -3))
print(sdiv(-1234, -3))

print(udiv(1234, 3))
print(udiv(0xFFFFFFFF, 0x7FFFFFFF))
print(udiv(0xFFFFFFFF, 0xFFFFFFFF))

print(srem(1234, 3))
print(srem(-1234, 3))
print(srem(1234, -3))
print(srem(-1234, -3))

print(urem(1234, 3))
print(urem(0xFFFFFFFF, 0x7FFFFFFF))
print(urem(0xFFFFFFFF, 0xFFFFFFFF))


@micropython.asm_rv32
def m1(a0, a1):
    mul(a0, a0, a1)


@micropython.asm_rv32
def m2(a0, a1):
    mulh(a0, a0, a1)


@micropython.asm_rv32
def m3(a0, a1):
    mulhu(a0, a0, a1)


@micropython.asm_rv32
def m4(a0, a1):
    mulhsu(a0, a0, a1)


print(m1(0xFFFFFFFF, 2))
print(m2(0xFFFFFFFF, 0xFFFFFFF0))
print(m3(0xFFFFFFFF, 0xFFFFFFF0))
print(m4(0xFFFFFFFF, 0xFFFFFFF0))
