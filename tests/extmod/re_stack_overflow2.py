# Test overflow in re.compile output code.

try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit


def test_re(r):
    try:
        re.compile(r)
    except:
        print("Error")


try:
    r = "(" * 65536 + ")" * 65536
except MemoryError:
    print("SKIP")
    raise SystemExit

# This happens to trigger RecursionError on current versions of CPython
# (tested with 3.13.5) as well, so no .exp file is needed.
test_re(r)
