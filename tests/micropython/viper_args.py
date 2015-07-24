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

# only up to 4 arguments currently supported
