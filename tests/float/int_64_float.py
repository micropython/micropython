# test int64 operation with float/complex

i = 1 << 40

# convert int64 to float on rhs
print("%.5g" % (2.0 * i))

# negative int64 as float
print("%.5g" % float(-i))

# this should convert to float
print("%.5g" % (i / 5))

# these should delegate to float
print("%.5g" % (i * 1.2))
print("%.5g" % (i / 1.2))

# negative power should produce float
print("%.5g" % (i**-1))
print("%.5g" % ((2 + i - i) ** -3))

try:
    i / 0
except ZeroDivisionError:
    print("ZeroDivisionError")
