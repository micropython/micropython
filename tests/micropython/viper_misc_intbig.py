# requires: viper

import sys


# unsigned ints
@micropython.viper
def viper_uint() -> uint:
    return uint(-1)


print(viper_uint() == (sys.maxsize << 1 | 1))
