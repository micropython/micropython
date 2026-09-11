@micropython.viper
def v_add(a: int, b: int) -> int:
    return a + b


@micropython.viper
def v_sub(a: int, b: int) -> int:
    return a - b


@micropython.viper
def v_mul(a: int, b: int) -> int:
    return a * b


@micropython.viper
def v_large_const() -> int:
    return 0x1234_5678_9abc_def0


@micropython.viper
def v_neg_large() -> int:
    return -0x7FFF_FFFF_FFFF


@micropython.viper
def v_shift_left(a: int, b: int) -> int:
    return a << b


@micropython.viper
def v_shift_right(a: int, b: int) -> int:
    return a >> b


@micropython.viper
def v_and(a: int, b: int) -> int:
    return a & b


@micropython.viper
def v_or(a: int, b: int) -> int:
    return a | b


@micropython.viper
def v_xor(a: int, b: int) -> int:
    return a ^ b


@micropython.viper
def v_compare(a: int, b: int) -> bool:
    return a > b


@micropython.viper
def v_many_params(a: int, b: int, c: int, d: int) -> int:
    return a + b + c + d


print(v_add(100, 200))
print(v_sub(500, 300))
print(v_mul(7, 8))
print(hex(v_large_const()))
print(hex(v_neg_large()))
print(hex(v_shift_left(1, 40)))
print(hex(v_shift_right(0xFF00, 8)))
print(hex(v_and(0xFF, 0x0F)))
print(hex(v_or(0xF0, 0x0F)))
print(hex(v_xor(0xAA, 0x55)))
print(v_compare(10, 5))
print(v_compare(5, 10))
print(v_many_params(1, 2, 3, 4))
