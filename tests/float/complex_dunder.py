# test __complex__ function support
# requires: complex float


class TestFloat:
    def __float__(self):
        return 1.0


class TestComplex:
    def __complex__(self):
        return 1j + 10


class TestStrComplex:
    def __complex__(self):
        return "a"


class TestNonComplex:
    def __complex__(self):
        return 6


class Test:
    pass


print(complex(TestFloat()))
print(complex(TestComplex()))

try:
    print(complex(TestStrComplex()))
except TypeError:
    print("TypeError")


try:
    print(complex(TestNonComplex()))
except TypeError:
    print("TypeError")


try:
    print(complex(Test()))
except TypeError:
    print("TypeError")

print(complex(TestComplex(), 2))


class TestRaisingComplex:
    def __complex__(self):
        raise ValueError


try:
    print(complex(TestRaisingComplex(), 0))
except ValueError:
    print("ValueError")
