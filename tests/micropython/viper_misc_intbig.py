import micropython


# unsigned ints
@micropython.viper
def viper_uint() -> uint:
    return uint(-1)


import usys

print(viper_uint() == (usys.maxsize << 1 | 1))
