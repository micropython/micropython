class Base:

    def __init__(self):
        self.a = 1

    def meth(self):
        print("in Base meth", self.a)

class Sub(Base):

    def meth(self):
        print("in Sub meth")
        return super().meth()

a = Sub()
a.meth()

# printing super
class A:
    def p(self):
        print(str(super())[:18])
A().p()


# test compiler's handling of long expressions with super
class A:
    bar = 123
    def foo(self):
        print('A foo')
        return [1, 2, 3]
class B(A):
    def foo(self):
        print('B foo')
        print(super().bar) # accessing attribute after super()
        return super().foo().count(2) # calling a subsequent method
print(B().foo())
