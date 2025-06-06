# Test accuracy of `repr` conversions.
# This test also increases code coverage for corner cases.

try:
    import array, math, binascii
except ImportError:
    print("SKIP")
    raise SystemExit


seed = 42


def pseudo_randouble():
    global seed
    ddef = []
    for _ in range(8):
        ddef.append(seed & 0xFF)
        seed = binascii.crc32(b'\0', seed)
    arr = array.array('d', bytes(ddef))
    return ddef, arr[0]


# The largest errors come from seldom used very small numbers, near the
# limit of the representation. So we keep them out of this test to keep
# the max relative error display useful.
if float('1e-100') == 0:
    # single-precision
    min_expo = -96  # i.e. not smaller than 1.0e-29
    # Expected results:
    # HIGH_QUALITY_REPR=1: 99.71% exact conversions, relative error < 1e-7
    min_success = 0.997
    max_rel_err = 1e-7
    # HIGH_QUALITY_REPR=0: 94.89% exact conversions, relative error < 1e-6
else:
    # double-precision
    min_expo = -845  # i.e. not smaller than 1.0e-254
    # Expected results:
    # HIGH_QUALITY_REPR=1: 99.83% exact conversions, relative error < 2.7e-16
    min_success = 0.998
    max_rel_err = 2e-16
    # HIGH_QUALITY_REPR=0: 64.01% exact conversions, relative error < 1.1e-15

stats = 0
N = 10000
max_err = 0
for _ in range(N):
    (ddef, f) = pseudo_randouble()
    while f == math.isinf(f) or math.isnan(f) or math.frexp(f)[1] <= min_expo:
        (ddef, f) = pseudo_randouble()

    str_f = repr(f)
    f2 = float(str_f)
    if f2 == f:
        stats += 1
    else:
        error = abs(f2 - f) / f
        if max_err < error:
            max_err = error

print(N, "values converted")
print(stats / N >= min_success)
print(max_err <= max_rel_err)
