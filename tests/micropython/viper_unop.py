# test unary operators


@micropython.viper
def pos(x: int) -> int:
    return +x


print(pos(0))
print(pos(1))
print(pos(-2))


@micropython.viper
def neg(x: int) -> int:
    return -x


print(neg(0))
print(neg(1))
print(neg(-2))


@micropython.viper
def inv(x: int) -> int:
    return ~x


print(inv(0))
print(inv(1))
print(inv(-2))
