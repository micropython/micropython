# Test MicroPython-specific restrictions of function.__code__ attribute.

try:
    (lambda: 0).__code__
except AttributeError:
    print("SKIP")
    raise SystemExit


def f_with_children():
    def g():
        pass


# Can't access __code__ when function has children.
try:
    f_with_children.__code__
except AttributeError:
    print("AttributeError")
