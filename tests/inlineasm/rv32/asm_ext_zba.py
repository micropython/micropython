@micropython.asm_rv32
def test_sh1add(a0, a1):
    sh1add(a0, a0, a1)


@micropython.asm_rv32
def test_sh2add(a0, a1):
    sh2add(a0, a0, a1)


@micropython.asm_rv32
def test_sh3add(a0, a1):
    sh3add(a0, a0, a1)


print(hex(test_sh1add(10, 20)))
print(hex(test_sh2add(10, 20)))
print(hex(test_sh3add(10, 20)))
