# Change the following to True to get a much more comprehensive set of tests
# to run, albeit, which take considerably longer.

full_tests = False


def test(fmt, *args):
    print("{:8s}".format(fmt) + ">" + fmt.format(*args) + "<")


def test_fmt(conv, fill, alignment, sign, prefix, width, precision, type, arg):
    fmt = "{"
    if conv:
        fmt += "!"
        fmt += conv
    fmt += ":"
    if alignment:
        fmt += fill
        fmt += alignment
    fmt += sign
    fmt += prefix
    fmt += width
    if precision:
        fmt += "."
        fmt += precision
    fmt += type
    fmt += "}"
    test(fmt, arg)
    if fill == "0" and alignment == "=":
        fmt = "{:"
        fmt += sign
        fmt += prefix
        fmt += width
        if precision:
            fmt += "."
            fmt += precision
        fmt += type
        fmt += "}"
        test(fmt, arg)


eg_nums = (
    0.0,
    -0.0,
    0.1,
    1.234,
    12.3459,
    1.23456789,
    123456789.0,
    -0.0,
    -0.1,
    -1.234,
    -12.3459,
    1e4,
    1e-4,
    1e5,
    1e-5,
    1e6,
    1e-6,
    1e10,
    1e37,
    -1e37,
    1e-37,
    -1e-37,
    1.23456e8,
    1.23456e7,
    1.23456e6,
    1.23456e5,
    1.23456e4,
    1.23456e3,
    1.23456e2,
    1.23456e1,
    1.23456e0,
    1.23456e-1,
    1.23456e-2,
    1.23456e-3,
    1.23456e-4,
    1.23456e-5,
    1.23456e-6,
    1.23456e-7,
    1.23456e-8,
    -1.23456e8,
    -1.23456e7,
    -1.23456e6,
    -1.23456e5,
    -1.23456e4,
    -1.23456e3,
    -1.23456e2,
    -1.23456e1,
    -1.23456e0,
    -1.23456e-1,
    -1.23456e-2,
    -1.23456e-3,
    -1.23456e-4,
    -1.23456e-5,
    -1.23456e-6,
    -1.23456e-7,
    -1.23456e-8,
)

if full_tests:
    for type in ("e", "E", "g", "G", "n"):
        for width in ("", "4", "6", "8", "10"):
            for alignment in ("", "<", ">", "=", "^"):
                for fill in ("", "@", "0", " "):
                    for sign in ("", "+", "-", " "):
                        for prec in ("", "1", "3", "6"):
                            for num in eg_nums:
                                test_fmt("", fill, alignment, sign, "", width, prec, type, num)

# Note: We use 1.23459 rather than 1.2345 because '{:3f}'.format(1.2345)
#       rounds differently than print("%.3f", 1.2345);

f_nums = (
    0.0,
    -0.0,
    0.0001,
    0.001,
    0.01,
    0.1,
    1.0,
    10.0,
    0.0012,
    0.0123,
    0.1234,
    1.23459,
    12.3456,
    -0.0001,
    -0.001,
    -0.01,
    -0.1,
    -1.0,
    -10.0,
    -0.0012,
    -0.0123,
    -0.1234,
    -1.23459,
    -12.3456,
)

if full_tests:
    for type in ("f", "F"):
        for width in ("", "4", "6", "8", "10"):
            for alignment in ("", "<", ">", "=", "^"):
                for fill in ("", " ", "0", "@"):
                    for sign in ("", "+", "-", " "):
                        # An empty precision defaults to 6, but when uPy is
                        # configured to use a float, we can only use a
                        # precision of 6 with numbers less than 10 and still
                        # get results that compare to CPython (which uses
                        # long doubles).
                        for prec in ("1", "2", "3"):
                            for num in f_nums:
                                test_fmt("", fill, alignment, sign, "", width, prec, type, num)
                        for num in int_nums2:
                            test_fmt("", fill, alignment, sign, "", width, "", type, num)

pct_nums1 = (0.1, 0.58, 0.99, -0.1, -0.58, -0.99)
pct_nums2 = (True, False, 1, 0, -1)

if full_tests:
    type = "%"
    for width in ("", "4", "6", "8", "10"):
        for alignment in ("", "<", ">", "=", "^"):
            for fill in ("", " ", "0", "@"):
                for sign in ("", "+", "-", " "):
                    # An empty precision defaults to 6, but when uPy is
                    # configured to use a float, we can only use a
                    # precision of 6 with numbers less than 10 and still
                    # get results that compare to CPython (which uses
                    # long doubles).
                    for prec in ("1", "2", "3"):
                        for num in pct_nums1:
                            test_fmt("", fill, alignment, sign, "", width, prec, type, num)
                    for num in pct_nums2:
                        test_fmt("", fill, alignment, sign, "", width, "", type, num)
else:
    for num in pct_nums1:
        test_fmt("", "", "", "", "", "", "1", "%", num)

# We don't currently test a type of '' with floats (see the detailed comment
# in  objstr.c)
