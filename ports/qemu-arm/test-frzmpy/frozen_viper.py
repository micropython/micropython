# Test freezing viper code.

import micropython


@micropython.viper
def viper_add(x, y):
    print(x + y)
