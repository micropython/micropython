# Test MicroPython-specific restrictions of function.__code__ attribute.

try:
    (lambda: 0).__code__
except AttributeError:
    print("SKIP")
    raise SystemExit

import micropython


@micropython.native
def f_native():
    pass


# Can't access __code__ when function is native code.
try:
    f_native.__code__
except AttributeError:
    print("AttributeError")
