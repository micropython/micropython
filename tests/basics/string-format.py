# Change the following to True to get a much more comprehensive set of tests
# to run, albeit, which take considerably longer.

full_tests = False

def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

test("{}-{}", 1, [4, 5])
test("{0}-{1}", 1, [4, 5])
test("{1}-{0}", 1, [4, 5])
test("{:x}", 1)
test("{!r}", 2)
test("{:x}", 0x10)
test("{!r}", "foo")
test("{!s}", "foo")
test("{0!r:>10s} {0!s:>10s}", "foo")

test("{:4b}", 10)
test("{:4c}", 48)
test("{:4d}", 123)
test("{:4n}", 123)
test("{:4o}", 123)
test("{:4x}", 123)
test("{:4X}", 123)

test("{:4,d}", 12345678)

test("{:#4b}", 10)
test("{:#4o}", 123)
test("{:#4x}", 123)
test("{:#4X}", 123)

test("{:#4d}", 0)
test("{:#4b}", 0)
test("{:#4o}", 0)
test("{:#4x}", 0)
test("{:#4X}", 0)

test("{:<6s}", "ab")
test("{:>6s}", "ab")
test("{:^6s}", "ab")

test("{: <6d}", 123)
test("{: <6d}", -123)
test("{:0<6d}", 123)
test("{:0<6d}", -123)
test("{:@<6d}", 123)
test("{:@<6d}", -123)

test("{:@< 6d}", 123)
test("{:@< 6d}", -123)
test("{:@<+6d}", 123)
test("{:@<+6d}", -123)
test("{:@<-6d}", 123)
test("{:@<-6d}", -123)

test("{:@>6d}",  -123)
test("{:@<6d}",  -123)
test("{:@=6d}",  -123)
test("{:06d}",  -123)

def test_fmt(conv, fill, alignment, sign, prefix, width, precision, type, arg):
    fmt = '{'
    if conv:
        fmt += '!'
        fmt += conv
    fmt += ':'
    if alignment:
        fmt += fill
        fmt += alignment
    fmt += sign
    fmt += prefix
    fmt += width
    if precision:
        fmt += '.'
        fmt += precision
    fmt += type
    fmt += '}'
    test(fmt,  arg)
    if fill == '0' and alignment == '=':
        fmt = '{:'
        fmt += sign
        fmt += prefix
        fmt += width
        if precision:
            fmt += '.'
            fmt += precision
        fmt += type
        fmt += '}'
        test(fmt, arg)

int_nums = (-1234, -123, -12, -1, 0, 1, 12, 123, 1234, True, False)
int_nums2 = (-12, -1, 0, 1, 12, True, False)

if full_tests:
    for type in ('', 'b', 'd', 'o', 'x', 'X'):
        for width in ('', '1', '3', '5', '7'):
            for alignment in ('', '<', '>', '=', '^'):
                for fill in ('', ' ', '0', '@'):
                    for sign in ('', '+', '-', ' '):
                        for prefix in ('', '#'):
                            for num in int_nums:
                                test_fmt('', fill, alignment, sign, prefix, width, '', type, num)

if full_tests:
    for width in ('', '1', '2'):
        for alignment in ('', '<', '>', '^'):
            for fill in ('', ' ', '0', '@'):
                test_fmt('', fill, alignment, '', '', width, '', 'c', 48)

if full_tests:
    for conv in ('', 'r', 's'):
        for width in ('', '1', '4', '10'):
            for alignment in ('', '<', '>', '^'):
                for fill in ('', ' ', '0', '@'):
                    for str in ('', 'a', 'bcd', 'This is a test with a longer string'):
                        test_fmt(conv, fill, alignment, '', '', width, '', 's', str)

# TODO Add tests for erroneous format strings.
