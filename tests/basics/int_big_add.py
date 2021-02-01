# tests transition from small to large int representation by addition

# 31-bit overflow
i = 0x3fffffff
print(i + i)
print(-i + -i)

# 47-bit overflow
i = 0x3fffffffffff
print(i + i)
print(-i + -i)

# 63-bit overflow
i = 0x3fffffffffffffff
print(i + i)
print(-i + -i)
