# test for correct binding of self when accessing attr of an instance

class A:
    def __init__(self, arg):
        self.val = arg
    def __str__(self):
        return 'A.__str__ ' + str(self.val)
    def __call__(self, arg):
        return 'A.__call__', arg
    def foo(self, arg):
        return 'A.foo', self.val, arg

def make_closure(x_in):
    x = x_in
    def closure(y):
        return x, y is c
    return closure

class C:
    # these act like methods and bind self

    def f1(self, arg):
        return 'C.f1', self is c, arg
    f2 = lambda self, arg: ('C.f2', self is c, arg)
    f3 = make_closure('f3') # closure
    def f4(self, arg): # generator
        yield self is c, arg

    # these act like simple variables and don't bind self

    f5 = int # builtin type
    f6 = abs # builtin function
    f7 = A # user type
    f8 = A(8) # user instance which is callable
    f9 = A(9).foo # user bound method

c = C()
print(c.f1(1))
print(c.f2(2))
print(c.f3())
print(next(c.f4(4)))
print(c.f5(5))
#print(c.f6(-6)) not working in uPy
print(c.f7(7))
print(c.f8(8))
print(c.f9(9))

# not working in uPy
#class C(list):
#    # this acts like a method and binds self
#    f1 = list.extend
#c = C()
#c.f1([3, 1, 2])
#print(c)
