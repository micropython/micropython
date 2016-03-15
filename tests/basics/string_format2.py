# comprehensive functionality test for {} format string

int_tests = False # these take a while, and some give wrong results
char_tests = True
str_tests = True

def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

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

if int_tests:
    int_nums = (-1234, -123, -12, -1, 0, 1, 12, 123, 1234, True, False)
    #int_nums = (-12, -1, 0, 1, 12, True, False)
    for type in ('', 'b', 'd', 'o', 'x', 'X'):
        for width in ('', '1', '3', '5', '7'):
            for alignment in ('', '<', '>', '=', '^'):
                for fill in ('', ' ', '0', '@'):
                    for sign in ('', '+', '-', ' '):
                        for prefix in ('', '#'):
                            for num in int_nums:
                                test_fmt('', fill, alignment, sign, prefix, width, '', type, num)

if char_tests:
    for width in ('', '1', '2'):
        for alignment in ('', '<', '>', '^'):
            for fill in ('', ' ', '0', '@'):
                test_fmt('', fill, alignment, '', '', width, '', 'c', 48)

if str_tests:
    for conv in ('', 'r', 's'):
        for width in ('', '1', '4', '10'):
            for alignment in ('', '<', '>', '^'):
                for fill in ('', ' ', '0', '@'):
                    for str in ('', 'a', 'bcd', 'This is a test with a longer string'):
                        test_fmt(conv, fill, alignment, '', '', width, '', 's', str)
