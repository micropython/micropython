# Test a class that behaves correctly


class Test1:
    def __float__(self):
        return 123.4

    def __complex__(self):
        return 123.4j


print(float(Test1()))
print(complex(Test1()))

# Test a class that returns the wrong type


class Test2:
    def __float__(self):
        return "Not a float"

    def __complex__(self):
        return "Not very complex"


try:
    print(float(Test2()))
except TypeError:
    print("TypeError")

try:
    print(complex(Test2()))
except TypeError:
    print("TypeError")

# Test a class that doesn't implement the conversion methods


class Test3:
    pass


try:
    print(float(Test3()))
except TypeError:
    print("TypeError")

try:
    print(complex(Test3()))
except TypeError:
    print("TypeError")


# Test a class that raises non-TypeError exception in conversions
# Also checks that the error in __complex__ does not cause a fallback
# to using __float__


class Test4:
    def __float__(self):
        return [1, 2, 3][4]

    def __complex__(self):
        return 1.0 / 0.0


try:
    print(float(Test4()))
except IndexError:
    print("IndexError")

try:
    print(complex(Test4()))
except ZeroDivisionError:
    print("ZeroDivisionError")
