class Foo:
    def __radd__(self, other):
        return "reverse"


try:
    print(5 + Foo())
except TypeError:
    print("TypeError")
