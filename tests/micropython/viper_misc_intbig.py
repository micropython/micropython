import micropython


# unsigned ints
@micropython.viper
def viper_uint() -> uint:
    return uint(-1)


import sys

print(viper_uint() == (sys.maxsize << 1 | 1))
