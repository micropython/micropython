class A:
    def __new__(cls):
        print("A.__new__")
        return super(cls, A).__new__(cls)

    def __init__(self):
        pass

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
        None
B(1, 2)
