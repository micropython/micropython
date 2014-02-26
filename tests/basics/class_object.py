class A:
    pass

class B():
    pass

class C(object):
    pass

class D(A):
    pass

a = A()
b = B()
c = C()
print(isinstance(c, object))
