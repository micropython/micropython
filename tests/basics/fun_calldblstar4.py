# test calling a function with multiple **args


def f(a, b=None, c=None):
    print(a, b, c)


f(**{"a": 1}, **{"b": 2})
f(**{"a": 1}, **{"b": 2}, c=3)
f(**{"a": 1}, b=2, **{"c": 3})

try:
    f(1, **{"b": 2}, **{"b": 3})
except TypeError:
    print("TypeError")

# test calling a method with multiple **args


class A:
    def f(self, a, b=None, c=None):
        print(a, b, c)


a = A()
a.f(**{"a": 1}, **{"b": 2})
a.f(**{"a": 1}, **{"b": 2}, c=3)
a.f(**{"a": 1}, b=2, **{"c": 3})

try:
    a.f(1, **{"b": 2}, **{"b": 3})
except TypeError:
    print("TypeError")
