# call tests for python operators
# TODO: add the sequences related operators


class cud():

    def __init__(self):
        print("__init__ called")
        pass

    def __repr__(self):
        print("__repr__ called")
        return ""

# Comparisons
    def __lt__(self, other):
        print("__lt__ called")
        pass

    def __le__(self, other):
        print("__le__ called")
        pass

    def __eq__(self, other):
        print("__eq__ called")
        pass

    def __ne__(self, other):
        print("__ne__ called")
        pass

    def __ge__(self, other):
        print("__ge__ called")
        pass

    def __gt__(self, other):
        print("__gt__ called")
        pass

# Math
    def __abs__(self):
        print("__abs__ called")
        pass

    def __add__(self, other):
        print("__add__ called")
        pass

    def __and__(self, other):
        print("__and__ called")
        pass

    def __floordiv__(self, other):
        print("__floordiv__ called")
        pass

    def __index__(self, other):
        print("__index__ called")
        pass

    def __inv__(self):
        print("__inv__ called")
        pass

    def __invert__(self):
        print("__invert__ called")
        pass

    def __lshift__(self, val):
        print("__lshift__ called")
        pass

    def __mod__(self, val):
        print("__mod__ called")
        pass

    def __mul__(self, other):
        print("__mul__ called")
        pass

    def __matmul__(self, other):
        print("__matmul__ called")
        pass

    def __neg__(self):
        print("__neg__ called")
        pass

    def __or__(self, other):
        print("__or__ called")
        pass

    def __pos__(self):
        print("__pos__ called")
        pass

    def __pow__(self, val):
        print("__pow__ called")
        pass

    def __rshift__(self, val):
        print("__rshift__ called")
        pass

    def __sub__(self, other):
        print("__sub__ called")
        pass

    def __truediv__(self, other):
        print("__truediv__ called")
        pass

    def __div__(self, other):
        print("__div__ called")
        pass

    def __xor__(self, other):
        print("__xor__ called")
        pass

cud1 = cud()
cud2 = cud()
str(cud1)
cud1 < cud2
cud1 <= cud2
cud1 == cud2
# TODO: ne is not supported, !(eq) is called instead
#cud1 != cud2
cud1 >= cud2
cud1 > cud2
cud1 + cud2
# TODO: binary and is not supported
#cud1 & cud2
cud2 // cud1
~cud1
# TODO: binary lshift is not supported
# cud1<<1
# TODO: modulus is not supported
#cud1 % 2
cud1 * cud2
cud1 * 2
-cud1
# TODO: binary or is not supported
#cud1 | cud2
+cud1
# TODO: pow is not supported
# cud1**2
# TODO: rshift is not suported
# cud1>>1
cud1 - cud2
cud1 / cud2
cud1 / 2
# TODO: xor is not supported
# cud1^cud2

# TODO: in the followin test, cpython still calls __eq__
# cud3=cud1
# cud3==cud1
