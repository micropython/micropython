# Calling an inherited classmethod
class Base:

    @classmethod
    def foo(cls):
        print(cls.__name__)

class Sub(Base):
    pass


Sub.foo()
