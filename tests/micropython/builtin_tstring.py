# This tests the built-in __template__ (which is MicroPython specific).

# Test a varying number of arguments.
print(__template__(()))
print(__template__((), ()))
print(__template__((), None, None))
print(__template__((), None, None, None))
print(__template__((), None, None, None, None))
print(__template__((), None, None, None, None, None))

# Test two strings and one interpolation.
print(__template__(("Hello ", "!"), (42, "x", None, "")))

# Test not enough arguments.
try:
    print(__template__())
except TypeError as er:
    print(repr(er))

# Test two arguments with second not being a tuple/list.
try:
    print(__template__((), None))
except TypeError as er:
    print(repr(er))
