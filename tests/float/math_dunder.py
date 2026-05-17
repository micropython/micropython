# test that math functions support user classes with __float__

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit


class TestFloat:
    def __float__(self):
        return 1.0


print("%.5g" % math.exp(TestFloat()))
