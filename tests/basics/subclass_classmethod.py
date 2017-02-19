# Calling an inherited classmethod
class Base:

    @classmethod
    def foo(cls):
        print(cls.__name__)

try:
    Base.__name__
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

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
