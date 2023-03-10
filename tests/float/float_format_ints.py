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

# Check that powers of 10 (that fit in float32) format correctly.
for i in range(31):
    # It works to 12 digits on all platforms *except* qemu-arm, where
    # 10^11 comes out as 10000000820 or something.
    print("{:.7g}".format(float("1e" + str(i))))

# 16777215 is 2^24 - 1, the largest integer that can be completely held
# in a float32.
print("{:f}".format(16777215))
# 4294967040 = 16777215 * 128 is the largest integer that is exactly
# represented by a float32 and that will also fit within a (signed) int32.
# The upper bound of our integer-handling code is actually double this,
# but that constant might cause trouble on systems using 32 bit ints.
print("{:f}".format(2147483520))
# Very large positive integers can be a test for precision and resolution.
# This is a weird way to represent 1e38 (largest power of 10 for float32).
print("{:.6e}".format(float("9" * 30 + "e8")))
