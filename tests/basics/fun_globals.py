# test the __globals__ attribute of a function


def foo():
    pass


if not hasattr(foo, "__globals__"):
    print("SKIP")
    raise SystemExit

print(type(foo.__globals__))
print(foo.__globals__ is globals())

foo.__globals__["bar"] = 123
print(bar)

try:
    foo.__globals__ = None
except AttributeError:
    print("AttributeError")
