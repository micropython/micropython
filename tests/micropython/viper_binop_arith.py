# test arithmetic operators

@micropython.viper
def add(x:int, y:int):
    print(x + y)
    print(y + x)
add(1, 2)
add(42, 3)
add(-1, 2)
add(-42, -3)

@micropython.viper
def sub(x:int, y:int):
    print(x - y)
    print(y - x)
sub(1, 2)
sub(42, 3)
sub(-1, 2)
sub(-42, -3)

@micropython.viper
def mul(x:int, y:int):
    print(x * y)
    print(y * x)
mul(0, 1)
mul(1, -1)
mul(1, 2)
mul(8, 3)
mul(-3, 4)
mul(-9, -6)

@micropython.viper
def shl(x:int, y:int):
    print(x << y)
shl(1, 0)
shl(1, 3)
shl(1, 30)
shl(42, 10)
shl(-42, 10)

@micropython.viper
def shr(x:int, y:int):
    print(x >> y)
shr(1, 0)
shr(1, 3)
shr(42, 2)
shr(-42, 2)

@micropython.viper
def and_(x:int, y:int):
    print(x & y, y & x)
and_(1, 0)
and_(1, 3)
and_(0xf0, 0x3f)
and_(-42, 6)

@micropython.viper
def or_(x:int, y:int):
    print(x | y, y | x)
or_(1, 0)
or_(1, 2)
or_(-42, 5)

@micropython.viper
def xor(x:int, y:int):
    print(x ^ y, y ^ x)
xor(1, 0)
xor(1, 2)
xor(-42, 5)
