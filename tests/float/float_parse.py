# test parsing of floats

inf = float("inf")

# it shouldn't matter where the decimal point is if the exponent balances the value
print(float("1234") - float("0.1234e4"))
print(float("1.015625") - float("1015625e-6"))

# very large integer part with a very negative exponent should cancel out
print("%.4e" % float("9" * 60 + "e-60"))
print("%.4e" % float("9" * 60 + "e-40"))

# many fractional digits
print(float("." + "9" * 70))
print(float("." + "9" * 70 + "e20"))
print(float("." + "9" * 70 + "e-50") == float("1e-50"))

# tiny fraction with large exponent
print(float("." + "0" * 60 + "1e10") == float("1e-51"))
print(float("." + "0" * 60 + "9e25") == float("9e-36"))
print(float("." + "0" * 60 + "9e40") == float("9e-21"))

# ensure that accuracy is retained when value is close to a subnormal
print(float("1.00000000000000000000e-37"))
print(float("10.0000000000000000000e-38"))
print(float("100.000000000000000000e-39"))

# very large exponent literal
print(float("1e4294967301"))
print(float("1e-4294967301"))
print(float("1e18446744073709551621"))
print(float("1e-18446744073709551621"))

# mantissa overflow while processing deferred trailing zeros
print(float("10000000000000000000001"))

# check small decimals are as close to their true value as possible
for n in range(1, 10):
    print(float("0.%u" % n) == n / 10)
