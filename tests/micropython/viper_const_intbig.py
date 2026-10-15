# check loading constants
# requires: viper


@micropython.viper
def f():
    return 123456789012345678901234567890


print(f())
