@micropython.asm_xtensa
def test_salt(a2, a3):
    salt(a2, a2, a3)


print(test_salt(-1, 1))
print(test_salt(100, -200))
print(test_salt(0, -1))


@micropython.asm_xtensa
def test_saltu(a2, a3) -> uint:
    saltu(a2, a2, a3)


print(test_saltu(-1, 1))
print(test_saltu(100, -200))
print(test_saltu(0, -1))
