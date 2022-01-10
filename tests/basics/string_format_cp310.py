# Python 3.10+ functionality test for {} format string

def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

test("{:06s}", "ab0000")
