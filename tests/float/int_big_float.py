# test bignum operation with float/complex

i = 1 << 65

# convert bignum to float on rhs
print("%.5g" % (2.0 * i))

# negative bignum as float
print("%.5g" % float(-i))

# this should convert to float
print("%.5g" % (i / 5))

# these should delegate to float
print("%.5g" % (i * 1.2))
print("%.5g" % (i / 1.2))

# this should delegate to complex
print("%.5g" % (i * 1.2j).imag)

# negative power should produce float
print("%.5g" % (i**-1))
print("%.5g" % ((2 + i - i) ** -3))

try:
    i / 0
except ZeroDivisionError:
    print("ZeroDivisionError")
