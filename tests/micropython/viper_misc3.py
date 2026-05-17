# Miscellaneous viper tests.

import micropython


# a for loop
@micropython.viper
def viper_for(a: int, b: int) -> int:
    total = 0
    for x in range(a, b):
        total += x
    return total


print(viper_for(10, 10000))


# accessing a global
@micropython.viper
def viper_access_global():
    global gl
    gl = 1
    return gl


print(viper_access_global(), gl)


# calling print with object and int types
@micropython.viper
def viper_print(x, y: int):
    print(x, y + 1)


viper_print(1, 2)


# convert constants to objects in tuple
@micropython.viper
def viper_tuple_consts(x):
    return (x, 1, False, True)


print(viper_tuple_consts(0))


# making a tuple from an object and an int
@micropython.viper
def viper_tuple(x, y: int):
    return (x, y + 1)


print(viper_tuple(1, 2))


# making a list from an object and an int
@micropython.viper
def viper_list(x, y: int):
    return [x, y + 1]


print(viper_list(1, 2))


# making a set from an object and an int
@micropython.viper
def viper_set(x, y: int):
    return {x, y + 1}


print(sorted(list(viper_set(1, 2))))


# raising an exception
@micropython.viper
def viper_raise(x: int):
    raise OSError(x)


try:
    viper_raise(1)
except OSError as e:
    print(repr(e))


# calling GC after defining the function
@micropython.viper
def viper_gc() -> int:
    return 1


print(viper_gc())
import gc

gc.collect()
print(viper_gc())
