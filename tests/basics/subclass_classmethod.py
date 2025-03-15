# Calling an inherited classmethod
class Base:

    @classmethod
    def foo(cls):
        print(cls.__name__)

try:
    Base.__name__
except AttributeError:
    print("SKIP")
    raise SystemExit

class Sub(Base):
    pass


Sub.foo()

# overriding a member and accessing it via a classmethod

class A(object):
    foo = 0

    @classmethod
    def bar(cls):
        print(cls.foo)

    def baz(self):
        print(self.foo)

class B(A):
    foo = 1

B.bar() # class calling classmethod
B().bar() # instance calling classmethod
B().baz() # instance calling normal method

# super inside a classmethod
# ensure the argument of the super method that is called is the child type


class C:
    @classmethod
    def f(cls):
        print("C.f", cls.__name__)  # cls should be D

    @classmethod
    def g(cls):
        print("C.g", cls.__name__)  # cls should be D


class D(C):
    @classmethod
    def f(cls):
        print("D.f", cls.__name__)
        super().f()

    @classmethod
    def g(cls):
        print("D.g", cls.__name__)
        super(D, cls).g()


D.f()
D.g()
D().f()
D().g()
