# This tests that printing recursive data structure doesn't lead to segfault.
try:
    import uio as io
except ImportError:
    print("SKIP")
    raise SystemExit

l = [1, 2, 3, None]
l[-1] = l
try:
    print(l, file=io.StringIO())
except RuntimeError:
    print("RuntimeError")
