# Test accuracy of `repr` conversions.
# This test also increases code coverage for corner cases.

try:
    import array, math, random
except ImportError:
    print("SKIP")
    raise SystemExit

# The largest errors come from seldom used very small numbers, near the
# limit of the representation. So we keep them out of this test to keep
# the max relative error display useful.
if float("1e-100") == 0.0:
    # single-precision
    float_type = "f"
    float_size = 4
    # testing range
    min_expo = -96  # i.e. not smaller than 1.0e-29
    # Expected results (given >=50'000 samples):
    # - MICROPY_FLTCONV_IMPL_EXACT: 100% exact conversions
    # - MICROPY_FLTCONV_IMPL_APPROX: >=98.53% exact conversions, max relative error <= 1.01e-7
    min_success = 0.980  # with only 1200 samples, the success rate is lower
    max_rel_err = 1.1e-7
    # REPR_C is typically used with FORMAT_IMPL_BASIC, which has a larger error
    is_REPR_C = float("1.0000001") == float("1.0")
    if is_REPR_C:  # REPR_C
        min_success = 0.83
        max_rel_err = 5.75e-07
else:
    # double-precision
    float_type = "d"
    float_size = 8
    # testing range
    min_expo = -845  # i.e. not smaller than 1.0e-254
    # Expected results (given >=200'000 samples):
    # - MICROPY_FLTCONV_IMPL_EXACT: 100% exact conversions
    # - MICROPY_FLTCONV_IMPL_APPROX: >=99.83% exact conversions, max relative error <= 2.7e-16
    min_success = 0.997  # with only 1200 samples, the success rate is lower
    max_rel_err = 2.7e-16


# Deterministic pseudorandom generator. Designed to be uniform
# on mantissa values and exponents, not on the represented number
def pseudo_randfloat():
    rnd_buff = bytearray(float_size)
    for _ in range(float_size):
        rnd_buff[_] = random.getrandbits(8)
    return array.array(float_type, rnd_buff)[0]


random.seed(42)
stats = 0
N = 1200
max_err = 0
for _ in range(N):
    f = pseudo_randfloat()
    while type(f) is not float or math.isinf(f) or math.isnan(f) or math.frexp(f)[1] <= min_expo:
        f = pseudo_randfloat()

    str_f = repr(f)
    f2 = float(str_f)
    if f2 == f:
        stats += 1
    else:
        error = abs((f2 - f) / f)
        if max_err < error:
            max_err = error

print(N, "values converted")
if stats / N >= min_success and max_err <= max_rel_err:
    print("float format accuracy OK")
else:
    print("FAILED: repr rate=%.3f%% max_err=%.3e" % (100 * stats / N, max_err))
