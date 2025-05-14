# basic functionality test for {} format string using large integers


def test(fmt, *args):
    print("{:8s}".format(fmt) + ">" + fmt.format(*args) + "<")


# Separator formatter

test("{:,}", 123_456_789_012_345_678_901_234_567)
test("{:,}", 23_456_789_012_345_678_901_234_567)
test("{:,}", 3_456_789_012_345_678_901_234_567)
test("{:,}", -123_456_789_012_345_678_901_234_567)
test("{:,}", -23_456_789_012_345_678_901_234_567)
test("{:,}", -3_456_789_012_345_678_901_234_567)
