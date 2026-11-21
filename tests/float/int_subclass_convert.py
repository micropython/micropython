# test conversion of int subclass instances to float
# (math functions are covered by math_fun_int_subclass.py: the math
# module is not available on all targets)


class MyInt(int):
    pass


# float() constructor
print(float(MyInt(0)))
print(float(MyInt(2)))
print(float(MyInt(-3)))

# string formatting
print("%f" % MyInt(2))

# arithmetic with a float operand
print(1.5 + MyInt(2))
print(MyInt(2) + 1.5)
print(MyInt(2) * 1.5)


# multi-level subclass
class MyInt2(MyInt):
    pass


print(float(MyInt2(5)))


# a __float__ method defined by the subclass takes precedence
class MyIntFloat(int):
    def __float__(self):
        return 42.5


print(float(MyIntFloat(3)))


# a __float__ method must return a float
class MyIntBadFloat(int):
    def __float__(self):
        return 3


try:
    float(MyIntBadFloat(3))
except TypeError:
    print("TypeError")

# int() conversion is unaffected
print(int(MyInt(2)))
