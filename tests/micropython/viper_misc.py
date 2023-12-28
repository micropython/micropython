# Miscellaneous viper tests.

import micropython


# viper function taking and returning ints
@micropython.viper
def viper_int(x: int, y: int) -> int:
    return x + y + 3


print(viper_int(1, 2))


# viper function taking and returning objects
@micropython.viper
def viper_object(x: object, y: object) -> object:
    return x + y


print(viper_object(1, 2))


# return None as non-object (should return 0)
@micropython.viper
def viper_ret_none() -> int:
    return None


print(viper_ret_none())


# return Ellipsis as object
@micropython.viper
def viper_ret_ellipsis() -> object:
    return ...


print(viper_ret_ellipsis())


# 3 args
@micropython.viper
def viper_3args(a: int, b: int, c: int) -> int:
    return a + b + c


print(viper_3args(1, 2, 3))


# 4 args
@micropython.viper
def viper_4args(a: int, b: int, c: int, d: int) -> int:
    return a + b + c + d


print(viper_4args(1, 2, 3, 4))


# a local (should have automatic type int)
@micropython.viper
def viper_local(x: int) -> int:
    y = 4
    return x + y


print(viper_local(3))


# without type annotation, types should default to object
@micropython.viper
def viper_no_annotation(x, y):
    return x * y


print(viper_no_annotation(4, 5))
