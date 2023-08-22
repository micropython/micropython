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


# too many chars in []
test_re("[" + "a" * 256 + "]")

# too many groups
test_re("(a)" * 256)

# jump too big for ?
test_re("(" + "a" * 62 + ")?")

# jump too big for *
test_re("(" + "a" * 60 + ".)*")
test_re("(" + "a" * 60 + "..)*")

# jump too big for +
test_re("(" + "a" * 62 + ")+")

# jump too big for |
test_re("b" * 63 + "|a")
