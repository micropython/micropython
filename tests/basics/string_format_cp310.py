# Python 3.10+ functionality test for {} format string

def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

test("{:0s}", "ab")
test("{:06s}", "ab")
test("{:<06s}", "ab")
test("{:>06s}", "ab")
