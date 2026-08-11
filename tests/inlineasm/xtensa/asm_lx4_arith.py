@micropython.asm_xtensa
def test_max(a2, a3):
    max_(a2, a2, a3)


print(test_max(-1, 1))
print(test_max(100, -200))
print(test_max(0, -1))


@micropython.asm_xtensa
def test_maxu(a2, a3) -> uint:
    maxu(a2, a2, a3)


print(hex(test_maxu(-1, 1)))
print(hex(test_maxu(100, -200)))
print(hex(test_maxu(0, -1)))


@micropython.asm_xtensa
def test_min(a2, a3):
    min_(a2, a2, a3)


print(test_min(-1, 1))
print(test_min(100, -200))
print(test_min(0, -1))


@micropython.asm_xtensa
def test_minu(a2, a3) -> uint:
    minu(a2, a2, a3)


print(test_minu(-1, 1))
print(test_minu(100, -200))
print(test_minu(0, -1))


@micropython.asm_xtensa
def test_quos(a2, a3):
    quos(a2, a2, a3)


print(test_quos(100, 2))
print(test_quos(-100, 2))
print(test_quos(2, 100))


@micropython.asm_xtensa
def test_quou(a2, a3) -> uint:
    quou(a2, a2, a3)


print(test_quou(100, 2))
print(test_quou(-100, 2))
print(test_quou(2, 100))


@micropython.asm_xtensa
def test_rems(a2, a3):
    rems(a2, a2, a3)


print(test_rems(100, 2))
print(test_rems(-100, 2))
print(test_rems(20, 100))
print(test_rems(-20, 100))


@micropython.asm_xtensa
def test_remu(a2, a3) -> uint:
    remu(a2, a2, a3)


print(test_remu(100, 2))
print(test_remu(-100, 2))
print(test_remu(20, 100))
print(test_remu(-20, 100))


CLAMPS_TEMPLATE = """
@micropython.asm_xtensa
def test_clamps(a2):
    clamps(a2, a2, {})
print(test_clamps({}))
"""

for value, bits in (
    (12, 7),
    (-12, 7),
    (-129, 7),
    (129, 7),
    (1000, 9),
    (-1000, 9),
    (1 + (1 << 20), 20),
    (-(1 << 20) - 2, 20),
):
    try:
        exec(CLAMPS_TEMPLATE.format(bits, value))
    except SyntaxError as error:
        print(error)


SEXT_TEMPLATE = """
@micropython.asm_xtensa
def test_sext(a2):
    sext(a2, a2, {})
print(test_sext({}))
"""

for value, bits in (
    (12, 7),
    (-12, 7),
    (-129, 7),
    (129, 7),
    (1000, 9),
    (-1000, 9),
    (1 + (1 << 20), 20),
    (-(1 << 20) - 2, 20),
):
    try:
        exec(SEXT_TEMPLATE.format(bits, value))
    except SyntaxError as error:
        print(error)
