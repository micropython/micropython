try:
    # If we don't expose object.__new__ (small ports), there's
    # nothing to test.
    object.__new__
except AttributeError:
    print("SKIP")
    raise SystemExit

class A:
    def __new__(cls):
        print("A.__new__")
        return super(cls, A).__new__(cls)

    def __init__(self):
        print("A.__init__")

    def meth(self):
        print('A.meth')

#print(A.__new__)
#print(A.__init__)

a = A()
a.meth()

a = A.__new__(A)
a.meth()

#print(a.meth)
#print(a.__init__)
#print(a.__new__)

# __new__ should automatically be a staticmethod, so this should work
a = a.__new__(A)
a.meth()

class B:
    def __new__(self, v1, v2):
        print("B.__new__", v1, v2)

    def __init__(self, v1, v2):
        # Should not be called in this test
        print("B.__init__", v1, v2)

print("B inst:", B(1, 2))
