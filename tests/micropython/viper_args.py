# test calling viper functions with different number of args

@micropython.viper
def f0():
    print(0)
f0()

@micropython.viper
def f1(x1:int):
    print(x1)
f1(1)

@micropython.viper
def f2(x1:int, x2:int):
    print(x1, x2)
f2(1, 2)

@micropython.viper
def f3(x1:int, x2:int, x3:int):
    print(x1, x2, x3)
f3(1, 2, 3)

@micropython.viper
def f4(x1:int, x2:int, x3:int, x4:int):
    print(x1, x2, x3, x4)
f4(1, 2, 3, 4)

@micropython.viper
def f5(x1:int, x2:int, x3:int, x4:int, x5:int):
    print(x1, x2, x3, x4, x5)
f5(1, 2, 3, 4, 5)

@micropython.viper
def f6(x1:int, x2:int, x3:int, x4:int, x5:int, x6:int):
    print(x1, x2, x3, x4, x5, x6)
f6(1, 2, 3, 4, 5, 6)

# test compiling *x, **x, * args (currently unsupported at runtime)
@micropython.viper
def f(*x, **y):
    pass
@micropython.viper
def f(*):
    pass
