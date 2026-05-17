# Test that integers format to exact values.
# This test requires at least 32-bit floats (won't work with 30-bit).

# Check that powers of 10 (that fit in float32) format correctly.
for i in range(31):
    # It works to 12 digits on all platforms *except* qemu, where
    # 10^11 comes out as 10000000820 or something.
    print(i, "{:.7g}".format(float("1e" + str(i))))
