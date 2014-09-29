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
