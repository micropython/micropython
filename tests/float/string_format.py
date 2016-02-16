def test(fmt, *args):
    print('{:8s}'.format(fmt) + '>' +  fmt.format(*args) + '<')

test("{:10.4}", 123.456)
test("{:10.4e}", 123.456)
test("{:10.4e}", -123.456)
test("{:10.4f}", 123.456)
test("{:10.4f}", -123.456)
test("{:10.4g}", 123.456)
test("{:10.4g}", -123.456)
test("{:10.4n}", 123.456)
test("{:e}", 100)
test("{:f}", 200)
test("{:g}", 300)

test("{:10.4E}", 123.456)
test("{:10.4E}", -123.456)
test("{:10.4F}", 123.456)
test("{:10.4F}", -123.456)
test("{:10.4G}", 123.456)
test("{:10.4G}", -123.456)

test("{:06e}", float("inf"))
test("{:06e}", float("-inf"))
test("{:06e}", float("nan"))

# The following fails right now
#test("{:10.1}", 0.0)

print("%.0f" % (1.750000 % 0.08333333333))
# Below isn't compatible with single-precision float
#print("%.1f" % (1.750000 % 0.08333333333))
#print("%.2f" % (1.750000 % 0.08333333333))
#print("%.12f" % (1.750000 % 0.08333333333))

# tests for errors in format string

try:
    '{:10.1b}'.format(0.0)
except ValueError:
    print('ValueError')
