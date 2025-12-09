# test support for 64-bit long integers
# (some ports don't support arbitrary precision but do support these)

# this test is adapted from int_big1.py with numbers kept within 64-bit signed range

# to test arbitrary precision integers

x = 1000000000000000000
xn = -1000000000000000000
y = 2000000000000000000

# printing
print(x)
print(y)
print('%#X' % (x - x)) # print prefix
print('{:#,}'.format(x)) # print with commas

# construction
print(int(x))

# addition
print(x + 1)
print(x + y)
print(x + xn == 0)
print(bool(x + xn))

# subtraction
print(x - 1)
print(x - y)
print(y - x)
print(x - x == 0)
print(bool(x - x))

# multiplication
print(x * 2)
print(1090511627776 * 1048500)

# integer division
print(x // 2)
print(y // x)

# bit inversion
print(~x)
print(~(-x))

# left shift
print("left shift positive")
x = 0x40000000
for i in range(32):
    x = x << 1
    print(x)

# right shift
print("right shift positive")
x = 0x2000000000000000  # TODO: why can't second-tip bit be set?
for i in range(64):
    x = x >> 1
    print(x)

# left shift of a negative number
print("left shift negative")
for i in range(8):
    print(-10000000000000000 << i)
    print(-10000000000000001 << i)
    print(-10000000000000002 << i)
    print(-10000000000000003 << i)
    print(-10000000000000004 << i)


# right shift of a negative number
print("right shift negative")
for i in range(8):
    print(-1000000000000000000 >> i)
    print(-1000000000000000001 >> i)
    print(-1000000000000000002 >> i)
    print(-1000000000000000003 >> i)
    print(-1000000000000000004 >> i)

# conversion from string
print(int("1234567890123456789"))
print(int("-1234567890123456789"))
print(int("1234567890abcdef", 16))
print(int("1234567890ABCDEF", 16))
print(int("-1234567890ABCDEF", 16))
print(int("ijklmnopqrsz", 36))

# numbers close to 64-bit limits
print(int("-9111222333444555666"))
print(int("9111222333444555666"))

# numbers with preceding 0s
print(int("-00000000000000000000009111222333444555666"))
print(int("0000000000000000000000009111222333444555666"))

# invalid characters in string
try:
    print(int("1234567890abcdef"))
except ValueError:
    print('ValueError');
try:
    print(int("123456789\x01"))
except ValueError:
    print('ValueError');

# test parsing ints just on threshold of small to big
# for 32 bit archs
x = 1073741823 # small
x = -1073741823 # small
x = 1073741824 # big
x = -1073741824 # big
# for 64 bit archs
x = 4611686018427387903 # small
x = -4611686018427387903 # small
x = 4611686018427387904 # big
x = -4611686018427387904 # big

# sys.maxsize is a constant bigint, so test it's compatible with dynamic ones
import sys
if hasattr(sys, "maxsize"):
    print(sys.maxsize - 1 + 1 == sys.maxsize)
else:
    print(True)  # No maxsize property in this config

# test extraction of big int value via mp_obj_get_int_maybe
x = 1 << 62
print('a' * (x + 4 - x))

# test overflow check in mp_obj_get_int_maybe
x = 1 << 32
r = None
try:
    r = range(0, x)
except OverflowError:
    # 32-bit target, correctly handled the overflow of x
    print("ok")
if r is not None:
    if len(r) == x:
        # 64-bit target, everything is just a small-int
        print("ok")
    else:
        # 32-bit target that did not handle the overflow of x
        print("unhandled overflow")

# negative shifts are invalid
try:
    print((1 << 48) >> -4)
except ValueError as e:
    print(e)

try:
    print((1 << 48) << -6)
except ValueError as e:
    print(e)

# Test that the most extreme 64 bit integer values all parse with int()
print(int("-9223372036854775807"))
print(int("-9223372036854775808"))
print(int("9223372036854775807"))

# Test that the most negative 64 bit integer can be formed via arithmetic
print(-9223372036854775807-1)
