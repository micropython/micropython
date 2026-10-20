# test positional-only arguments (PEP 570) with viper functions
# requires: viper


@micropython.viper
def f1(a: int, b: int, /) -> int:
    return a + b


print(f1(1, 2))


@micropython.viper
def f2(a: int, b: int, /, c: int) -> int:
    return a + b + c


print(f2(1, 2, 3))

# viper functions don't take keyword arguments
try:
    f2(1, b=2, c=3)
except TypeError:
    print("TypeError")
