# test storing an attribute with a value of different viper types


class X:
    def __str__(self):
        return "X"


x = X()


@micropython.viper
def a():
    x.i0 = 0
    x.i7 = 7
    x.s = "hello"
    x.o = x


a()

print(x.i0)
print(x.i7)
print(x.s)
print(x.o)
