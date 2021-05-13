# test arithmetic operators with uint type


@micropython.viper
def add(x: uint, y: uint):
    return x + y, y + x


print("add")
print(*add(1, 2))
print(*(x & 0xFFFFFFFF for x in add(-1, -2)))


@micropython.viper
def sub(x: uint, y: uint):
    return x - y, y - x


print("sub")
print(*(x & 0xFFFFFFFF for x in sub(1, 2)))
print(*(x & 0xFFFFFFFF for x in sub(-1, -2)))


@micropython.viper
def mul(x: uint, y: uint):
    return x * y, y * x


print("mul")
print(*mul(2, 3))
print(*(x & 0xFFFFFFFF for x in mul(2, -3)))
print(*mul(-2, -3))
