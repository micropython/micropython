class Meta(type):
    def __init__(cls, name, bases, dct):
        print("Meta.__init__", name, bases, dct)
        super().__init__(name, bases, dct)
        print("dct=", dct)
        cls.attr = 100

        def iter(self):
            self.i = 0
            return self

        def next(self):
            self.i += 1
            if self.i > 10:
                raise StopIteration
            return self.i

        cls.__iter__ = iter
        cls.__next__ = next
        print("done")


class Foo(metaclass=Meta):
    pass


print("create Foo2...")


class Foo2(Foo):
    x = 1
    y = 2


print(Foo2, type(Foo2))
print(list(Foo2()))
print(Foo)
print(type(Foo))
print(Foo.attr)
