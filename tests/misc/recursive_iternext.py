# This tests that recursion with iternext doesn't lead to segfault.

try:
    x = (1, 2)
    for i in range(1000):
        x = enumerate(x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(1000):
        x = filter(None, x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(1000):
        x = map(max, x, ())
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(1000):
        x = zip(x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")
