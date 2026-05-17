# test arithmetic opcodes


@micropython.asm_rv32
def f1():
    li(a0, 0x100)
    li(a1, 1)
    add(a0, a0, a1)
    addi(a0, a0, 1)
    addi(a0, a0, -2)
    sub(a0, a0, a1)
    c_add(a0, a1)
    c_addi(a0, -1)
    c_sub(a0, a1)


print(hex(f1()))


@micropython.asm_rv32
def f2():
    li(a0, 0x10FF)
    li(a1, 1)
    and_(a2, a0, a1)
    andi(a3, a0, 0x10)
    or_(a2, a2, a3)
    ori(a2, a2, 8)
    li(a1, 0x200)
    c_or(a2, a1)
    li(a1, 0xF0)
    mv(a0, a2)
    c_and(a0, a1)
    li(a1, 0x101)
    xor(a0, a0, a1)
    xori(a0, a0, 0x101)
    c_xor(a0, a1)


print(hex(f2()))


@micropython.asm_rv32
def f3(a0, a1):
    slt(a0, a0, a1)


print(f3(0xFFFFFFF0, 0xFFFFFFF1))
print(f3(0x0, 0xFFFFFFF1))
print(f3(0xFFFFFFF1, 0xFFFFFFF1))
print(f3(0xFFFFFFF1, 0xFFFFFFF0))


@micropython.asm_rv32
def f4(a0, a1):
    sltu(a0, a0, a1)


print(f3(0xFFFFFFF0, 0xFFFFFFF1))
print(f3(0x0, 0xFFFFFFF1))
print(f3(0xFFFFFFF1, 0xFFFFFFF1))
print(f3(0xFFFFFFF1, 0xFFFFFFF0))


@micropython.asm_rv32
def f5(a0):
    slti(a0, a0, -2)


print(f5(-1))
print(f5(-3))


@micropython.asm_rv32
def f6(a0):
    sltiu(a0, a0, -2)


print(f6(-1))
print(f6(-3))
