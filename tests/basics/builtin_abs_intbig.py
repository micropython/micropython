# test builtin abs

# bignum
print(abs(123456789012345678901234567890))
print(abs(-123456789012345678901234567890))

# edge cases for 32 and 64 bit archs (small int overflow when negating)
print(abs(-0x3fffffff - 1))
print(abs(-0x3fffffffffffffff - 1))

# edge case for nan-boxing with 47-bit small int
i = -0x3fffffffffff
print(abs(i - 1))
