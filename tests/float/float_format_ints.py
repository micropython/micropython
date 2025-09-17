# Test that integers format to exact values.

for b in [13, 123, 457, 23456]:
    for r in range(1, 10):
        e_fmt = "{:." + str(r) + "e}"
        f_fmt = "{:." + str(r) + "f}"
        g_fmt = "{:." + str(r) + "g}"
        for e in range(0, 5):
            f = b * (10**e)
            title = str(b) + " x 10^" + str(e)
            print(title, "with format", e_fmt, "gives", e_fmt.format(f))
            print(title, "with format", f_fmt, "gives", f_fmt.format(f))
            print(title, "with format", g_fmt, "gives", g_fmt.format(f))

# The tests below check border cases involving all mantissa bits.
# In case of REPR_C, where the mantissa is missing two bits, the
# the string representation for such numbers might not always be exactly
# the same but nevertheless be correct, so we must allow a few exceptions.
is_REPR_C = float("1.0000001") == float("1.0")

# 16777215 is 2^24 - 1, the largest integer that can be completely held
# in a float32.
val_str = "{:f}".format(16777215)

# When using REPR_C, 16777215.0 is the same as 16777212.0 or 16777214.4
# (depending on the implementation of pow() function, the result may differ)
if is_REPR_C and (val_str == "16777212.000000" or val_str == "16777214.400000"):
    val_str = "16777215.000000"

print(val_str)

# 4294967040 = 16777215 * 128 is the largest integer that is exactly
# represented by a float32 and that will also fit within a (signed) int32.
# The upper bound of our integer-handling code is actually double this,
# but that constant might cause trouble on systems using 32 bit ints.
val_str = "{:f}".format(2147483520)

# When using FLOAT_IMPL_FLOAT, 2147483520.0 == 2147483500.0
# Both representations are valid, the second being "simpler"
is_float32 = float("1e300") == float("inf")
if is_float32 and val_str == "2147483500.000000":
    val_str = "2147483520.000000"

# When using REPR_C, 2147483520.0 is the same as 2147483200.0
# Both representations are valid, the second being "simpler"
if is_REPR_C and val_str == "2147483200.000000":
    val_str = "2147483520.000000"

print(val_str)

# Very large positive integers can be a test for precision and resolution.
# This is a weird way to represent 1e38 (largest power of 10 for float32).
print("{:.6e}".format(float("9" * 30 + "e8")))
