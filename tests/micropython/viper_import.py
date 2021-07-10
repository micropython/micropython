# test import within viper function


@micropython.viper
def f():
    import micropython

    print(micropython.const(1))

    from micropython import const

    print(const(2))


f()
