try:
    import usys as sys
except ImportError:
    import sys

try:
    sys.exc_info
except:
    print("SKIP")
    raise SystemExit


def f():
    print(sys.exc_info()[0:2])


try:
    raise ValueError("value", 123)
except:
    print(sys.exc_info()[0:2])
    f()

# Outside except block, sys.exc_info() should be back to None's
f()

# Recursive except blocks are not handled - just don't
# use exc_info() at all, use explicit variables in "except".
