# test bitwise operators on uint type


@micropython.viper
def shl(x: uint, y: uint) -> uint:
    return x << y


print("shl")
print(shl(1, 0))
print(shl(1, 30))
print(shl(-1, 10) & 0xFFFFFFFF)


@micropython.viper
def shr(x: uint, y: uint) -> uint:
    return x >> y


print("shr")
print(shr(1, 0))
print(shr(16, 3))
print(shr(-1, 1) in (0x7FFFFFFF, 0x7FFFFFFF_FFFFFFFF))


@micropython.viper
def and_(x: uint, y: uint):
    return x & y, y & x


print("and")
print(*and_(1, 0))
print(*and_(1, 3))
print(*and_(-1, 2))
print(*(x & 0xFFFFFFFF for x in and_(-1, -2)))


@micropython.viper
def or_(x: uint, y: uint):
    return x | y, y | x


print("or")
print(*or_(1, 0))
print(*or_(1, 2))
print(*(x & 0xFFFFFFFF for x in or_(-1, 2)))


@micropython.viper
def xor(x: uint, y: uint):
    return x ^ y, y ^ x


print("xor")
print(*xor(1, 0))
print(*xor(1, 3))
print(*(x & 0xFFFFFFFF for x in xor(-1, 3)))
print(*xor(-1, -3))
