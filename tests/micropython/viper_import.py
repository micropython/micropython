# test import within viper function
# requires: viper


@micropython.viper
def f():
    import micropython

    print(micropython.const(1))

    from micropython import const

    print(const(2))


f()
