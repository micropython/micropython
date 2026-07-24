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


@micropython.viper
def posc() -> int:
    x = -1
    x1 = +x
    assert x == -1
    return x1


print(posc())


@micropython.viper
def negc() -> int:
    x = -1
    x1 = -x
    assert x == -1
    return x1


print(negc())


@micropython.viper
def invc() -> int:
    x = -1
    x1 = ~x
    assert x == -1
    return x1


print(invc())
