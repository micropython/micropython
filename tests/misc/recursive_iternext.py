# This tests that recursion with iternext doesn't lead to segfault.
try:
    enumerate
    filter
    map
    max
    zip
except:
    import sys
    print("SKIP")
    sys.exit()

# We need to pick an N that is large enough to hit the recursion
# limit, but not too large that we run out of heap memory.
try:
    # large stack/heap, eg unix
    [0] * 80000
    N = 2400
except:
    try:
        # medium, eg pyboard
        [0] * 10000
        N = 1000
    except:
        # small, eg esp8266
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
