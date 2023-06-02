# test native function attributes


def f():
    pass


if not hasattr(f, "__name__"):
    print("SKIP")
    raise SystemExit


@micropython.native
def native_f():
    pass


print(type(native_f.__name__))
print(type(native_f.__globals__))
print(native_f.__globals__ is globals())

try:
    native_f.__name__ = None
except AttributeError:
    print("AttributeError")
