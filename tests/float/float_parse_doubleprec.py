# test parsing of floats, requiring double-precision

# very large integer part with a very negative exponent should cancel out
print(float("9" * 400 + "e-100"))
print(float("9" * 400 + "e-200"))
print(float("9" * 400 + "e-400"))

# many fractional digits
print(float("." + "9" * 400))
print(float("." + "9" * 400 + "e100"))
print(float("." + "9" * 400 + "e-100"))

# tiny fraction with large exponent
print("%.14e" % float("." + "0" * 400 + "9e100"))
print("%.14e" % float("." + "0" * 400 + "9e200"))
print("%.14e" % float("." + "0" * 400 + "9e400"))

# ensure that accuracy is retained when value is close to a subnormal
print(float("1.00000000000000000000e-307"))
print(float("10.0000000000000000000e-308"))
print(float("100.000000000000000000e-309"))

# ensure float is parsed exactly
print(float("74e46"))

# Overflow/largest double boundary:
print(float("1.7976931348623159e308"))  # (should overflow, 0x000000000000f07f )
print("%.15e" % float("1.7976931348623158e308"))  # (should yield the max double, 0xffffffffffffef7f )

"""
TODO get these working?
# Normalized/denormalized double boundary
print("%.16e" % float("2.2250738585072012e-308"))  # (should yield smallest normalized double, 0x0000000000001000 )
print("%.16e" % float("2.2250738585072011e-308"))  # (should yield largest denormalized double, 0xffffffffffff0f00 )

# Shortest (up to) 17-digit input that converts to smallest denormalized double:
print(float("5e-324"))  # (should yield smallest denormalized double, 0x0100000000000000 )

# Closest 17-digit input to the smallest denormalized double:
print(float("4.9406564584124654e-324"))  # (should yield smallest denormalized double, 0x0100000000000000 )

# The next boundary will depend on how good the ldexp implementation is on the target platform:
# Smallest denormalized double/underflow boundary:

print(float("2.4703282292062328e-324"))  # (should yield smallest denormalized double, 0x0100000000000000 )
# (Note that this value is greater than 2**-1075 and therefore should round up. 64-bit CPython 3.7.5 on win32 gets this right. Your mileage may vary, since the 54 most significant bits of the result are 0b1.00000000000000000000000000000000000000000000000000000 x 2**-1075.)
"""

print(float("2.4703282292062327e-324"))  # (should underflow to zero: 0x0000000000000000 )
# (Note that this value is less than 2**-1075 and therefore should round down to zero.)
