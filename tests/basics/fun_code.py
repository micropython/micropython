# Test function.__code__ attribute.

try:
    (lambda: 0).__code__
except AttributeError:
    print("SKIP")
    raise SystemExit


def f():
    return a


ftype = type(f)

# Test __code__ access and function constructor.
code = f.__code__
print(type(ftype(code, {})) is ftype)

# Test instantiating multiple code's with different globals dicts.
code = f.__code__
f1 = ftype(code, {"a": 1})
f2 = ftype(code, {"a": 2})
print(f1(), f2())

# Test bad first argument type.
try:
    ftype(None, {})
except TypeError:
    print("TypeError")

# Test bad second argument type.
try:
    ftype(f.__code__, None)
except TypeError:
    print("TypeError")
