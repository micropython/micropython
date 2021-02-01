i = 123456789012345678901234567890
print(i >> 1)
print(i >> 1000)

# result needs rounding up
i = -(1 << 70)
print(i >> 80)
i = -0xffffffffffffffff
print(i >> 32)
