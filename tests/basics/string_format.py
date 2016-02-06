# Change the following to True to get a much more comprehensive set of tests
# to run, albeit, which take considerably longer.

full_tests = False

def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

test("}}{{")
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
test("{:.1s}", "ab")

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

test("{:>20}", "foo")
test("{:^20}", "foo")
test("{:<20}", "foo")

# nested format specifiers
print("{:{}}".format(123, '#>10'))
print("{:{}{}{}}".format(123, '#', '>', '10'))
print("{0:{1}{2}}".format(123, '#>', '10'))
print("{text:{align}{width}}".format(text="foo", align="<", width=20))
print("{text:{align}{width}}".format(text="foo", align="^", width=10))
print("{text:{align}{width}}".format(text="foo", align=">", width=30))

print("{foo}/foo".format(foo="bar"))
print("{}".format(123, foo="bar"))
print("{}-{foo}".format(123, foo="bar"))

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

# tests for errors in format string

try:
    '{0:0}'.format('zzz')
except (ValueError):
    print('ValueError')

try:
    '{1:}'.format(1)
except IndexError:
    print('IndexError')

try:
    '}'.format('zzzz')
except ValueError:
    print('ValueError')

# end of format parsing conversion specifier
try:
    '{!'.format('a')
except ValueError:
    print('ValueError')

# unknown conversion specifier
try:
    'abc{!d}'.format('1')
except ValueError:
    print('ValueError')

try:
    '{abc'.format('zzzz')
except ValueError:
    print('ValueError')

# expected ':' after specifier
try:
    '{!s :}'.format(2)
except ValueError:
    print('ValueError')

try:
    '{}{0}'.format(1, 2)
except ValueError:
    print('ValueError')

try:
    '{1:}'.format(1)
except IndexError:
    print('IndexError')

try:
    '{ 0 :*^10}'.format(12)
except KeyError:
    print('KeyError')

try:
    '{0}{}'.format(1)
except ValueError:
    print('ValueError')

try:
    '{}{}'.format(1)
except IndexError:
    print('IndexError')

try:
    '{0:+s}'.format('1')
except ValueError:
    print('ValueError')

try:
    '{0:+c}'.format(1)
except ValueError:
    print('ValueError')

try:
    '{0:s}'.format(1)
except ValueError:
    print('ValueError')

try:
    '{:*"1"}'.format('zz')
except ValueError:
    print('ValueError')

# unknown format code for str arg
try:
    '{:X}'.format('zz')
except ValueError:
    print('ValueError')
