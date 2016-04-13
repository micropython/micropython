# basic functionality test for {} format string

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
