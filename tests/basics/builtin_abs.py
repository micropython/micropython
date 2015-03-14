# test builtin abs

print(abs(False))
print(abs(True))
print(abs(1))
print(abs(-1))

# bignum
print(abs(123456789012345678901234567890))
print(abs(-123456789012345678901234567890))

# edge cases for 32 and 64 bit archs (small int overflow when negating)
print(abs(-0x3fffffff - 1))
print(abs(-0x3fffffffffffffff - 1))
