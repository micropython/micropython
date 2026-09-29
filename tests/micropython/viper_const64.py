# Test large integer constants in viper mode.  Viper's "int" is the machine
# word, so wider constants raise ViperTypeError; compiling via exec() lets
# 32-bit targets skip instead of failing.

try:
    exec(
        """
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
    return 0x1234_5678_9ABC_DEF0


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


@micropython.viper
def v_neg1() -> int:
    # 0xffffffffffffffff: every halfword is 0xffff -> single MOVN #0.
    return -1


@micropython.viper
def v_neg64k() -> int:
    # 0xffffffffffff0000: exactly one halfword is not 0xffff -> single MOVN.
    return -65536


@micropython.viper
def v_movn_mix() -> int:
    # 0xffffffffa987edcc: general case -> MOVN base + one MOVK.
    return -0xA987EDCC


@micropython.viper
def v_movz_sparse() -> int:
    # 0x1234000056780000: MOVZ base skips the zero low halfword.
    return 0x1234000056780000


@micropython.viper
def v_u_lt(a: uint, b: uint) -> bool:
    return a < b


@micropython.viper
def v_u_ge(a: uint, b: uint) -> bool:
    return a >= b
"""
    )
except ViperTypeError:
    print("SKIP")
    raise SystemExit

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
print(hex(v_neg1()))
print(hex(v_neg64k()))
print(hex(v_movn_mix()))
print(hex(v_movz_sparse()))
# uint comparisons use small positive values: same output under bytecode fallback.
print(v_u_lt(3, 9), v_u_lt(9, 3), v_u_ge(9, 3))
