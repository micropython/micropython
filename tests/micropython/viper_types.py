# test various type conversions

import micropython

# converting incoming arg to bool
@micropython.viper
def f1(x:bool):
    print(x)
f1(0)
f1(1)
f1([])
f1([1])

# taking and returning a bool
@micropython.viper
def f2(x:bool) -> bool:
    return x
print(f2([]))
print(f2([1]))

# converting to bool within function
@micropython.viper
def f3(x) -> bool:
    return bool(x)
print(f3([]))
print(f3(-1))
