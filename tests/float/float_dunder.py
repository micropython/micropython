# test __float__ function support


class TestFloat:
    def __float__(self):
        return 10.0


class TestStrFloat:
    def __float__(self):
        return "a"


class TestNonFloat:
    def __float__(self):
        return 6


class Test:
    pass


print("%.1f" % float(TestFloat()))
print("%.1f" % TestFloat())


try:
    print(float(TestStrFloat()))
except TypeError:
    print("TypeError")


try:
    print(float(TestNonFloat()))
except TypeError:
    print("TypeError")


try:
    print(float(Test()))
except TypeError:
    print("TypeError")
