# This tests that recursion with iternext doesn't lead to segfault.

try:
    [0] * 10000
    N = 1000
except:
    N = 100

try:
    x = (1, 2)
    for i in range(N):
        x = enumerate(x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(N):
        x = filter(None, x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(N):
        x = map(max, x, ())
    tuple(x)
except RuntimeError:
    print("RuntimeError")

try:
    x = (1, 2)
    for i in range(N):
        x = zip(x)
    tuple(x)
except RuntimeError:
    print("RuntimeError")
