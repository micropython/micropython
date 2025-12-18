@micropython.asm_xtensa
def f1(a2):
    abs_(a2, a2)


for value in (10, -10, 0):
    print(f1(value))


ADDMI_TEMPLATE = """
@micropython.asm_xtensa
def f1(a2) -> int:
    addmi(a2, a2, {})
print(f1(0))
"""

for value in (-32768, -32767, 32512, 32513, 0):
    try:
        exec(ADDMI_TEMPLATE.format(value))
    except SyntaxError as error:
        print(error)


@micropython.asm_xtensa
def a2(a2, a3) -> int:
    addx2(a2, a2, a3)


@micropython.asm_xtensa
def a4(a2, a3) -> int:
    addx4(a2, a2, a3)


@micropython.asm_xtensa
def a8(a2, a3) -> int:
    addx8(a2, a2, a3)


@micropython.asm_xtensa
def s2(a2, a3) -> int:
    subx2(a2, a2, a3)


@micropython.asm_xtensa
def s4(a2, a3) -> int:
    subx4(a2, a2, a3)


@micropython.asm_xtensa
def s8(a2, a3) -> int:
    subx8(a2, a2, a3)


for first, second in ((100, 100), (-100, 100), (-100, -100), (100, -100)):
    print("a2", a2(first, second))
    print("a4", a4(first, second))
    print("a8", a8(first, second))
    print("s2", s2(first, second))
    print("s4", s4(first, second))
    print("s8", s8(first, second))


@micropython.asm_xtensa
def f5(a2) -> int:
    neg(a2, a2)


for value in (0, -100, 100):
    print(f5(value))


@micropython.asm_xtensa
def f6():
    movi(a2, 0x100)
    movi(a3, 1)
    add(a2, a2, a3)
    addi(a2, a2, 1)
    addi(a2, a2, -2)
    sub(a2, a2, a3)


print(hex(f6()))


@micropython.asm_xtensa
def f7():
    movi(a2, 0x10FF)
    movi(a3, 1)
    and_(a4, a2, a3)
    or_(a4, a4, a3)
    movi(a3, 0x200)
    xor(a2, a4, a3)


print(hex(f7()))


@micropython.asm_xtensa
def f8(a2, a3):
    add_n(a2, a2, a3)


print(f8(100, 200))


@micropython.asm_xtensa
def f9(a2):
    addi_n(a2, a2, 1)


print(f9(100))


@micropython.asm_xtensa
def f10(a2, a3) -> uint:
    mull(a2, a2, a3)


print(hex(f10(0xC0000000, 2)))
