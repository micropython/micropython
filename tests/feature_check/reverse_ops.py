class Foo:
    def __radd__(self, other):
        pass


try:
    5 + Foo()
except TypeError:
    print("TypeError")
