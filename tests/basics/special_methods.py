class Cud():

    def __init__(self):
        print("__init__ called")

    def __repr__(self):
        print("__repr__ called")
        return ""

    def __lt__(self, other):
        print("__lt__ called")

    def __le__(self, other):
        print("__le__ called")

    def __eq__(self, other):
        print("__eq__ called")

    def __ne__(self, other):
        print("__ne__ called")

    def __ge__(self, other):
        print("__ge__ called")

    def __gt__(self, other):
        print("__gt__ called")

    def __abs__(self):
        print("__abs__ called")

    def __add__(self, other):
        print("__add__ called")

    def __and__(self, other):
        print("__and__ called")

    def __floordiv__(self, other):
        print("__floordiv__ called")

    def __index__(self, other):
        print("__index__ called")

    def __inv__(self):
        print("__inv__ called")

    def __invert__(self):
        print("__invert__ called")

    def __lshift__(self, val):
        print("__lshift__ called")

    def __mod__(self, val):
        print("__mod__ called")

    def __mul__(self, other):
        print("__mul__ called")

    def __matmul__(self, other):
        print("__matmul__ called")

    def __neg__(self):
        print("__neg__ called")

    def __or__(self, other):
        print("__or__ called")

    def __pos__(self):
        print("__pos__ called")

    def __pow__(self, val):
        print("__pow__ called")

    def __rshift__(self, val):
        print("__rshift__ called")

    def __sub__(self, other):
        print("__sub__ called")

    def __truediv__(self, other):
        print("__truediv__ called")

    def __div__(self, other):
        print("__div__ called")

    def __xor__(self, other):
        print("__xor__ called")

    def __iadd__(self, other):
        print("__iadd__ called")
        return self

    def __isub__(self, other):
        print("__isub__ called")
        return self

cud1 = Cud()
cud2 = Cud()

str(cud1)
cud1 < cud2
cud1 <= cud2
cud1 == cud2
cud1 >= cud2
cud1 > cud2
cud1 + cud2
cud1 - cud2

# more in special_methods2.py
