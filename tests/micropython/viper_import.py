# test import within viper function


@micropython.viper
def f():
    from src import micropython

    print(micropython.const(1))

    from src.micropython import const

    print(const(2))


f()
