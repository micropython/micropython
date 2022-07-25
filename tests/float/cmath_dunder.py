# test that cmath functions support user classes with __float__ and __complex__

try:
    import cmath
except ImportError:
    print("SKIP")
    raise SystemExit


class TestFloat:
    def __float__(self):
        return 1.0


class TestComplex:
    def __complex__(self):
        return 1j + 10


for clas in TestFloat, TestComplex:
    print("%.5g" % cmath.phase(clas()))
