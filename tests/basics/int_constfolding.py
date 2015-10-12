# tests int constant folding in compiler

# positive
print(+1)
print(+100)

# negation
print(-1)
print(-(-1))
print(-0x3fffffff) # 32-bit edge case
print(-0x3fffffffffffffff) # 64-bit edge case
print(-(-0x3fffffff - 1)) # 32-bit edge case
print(-(-0x3fffffffffffffff - 1)) # 64-bit edge case

# 1's complement
print(~0)
print(~1)
print(~-1)
print(~0x3fffffff) # 32-bit edge case
print(~0x3fffffffffffffff) # 64-bit edge case
print(~(-0x3fffffff - 1)) # 32-bit edge case
print(~(-0x3fffffffffffffff - 1)) # 64-bit edge case

# addition
print(1 + 2)

# subtraction
print(1 - 2)
print(2 - 1)

# multiplication
print(1 * 2)
print(123 * 456)

# floor div and modulo
print(123 // 7, 123 % 7)
print(-123 // 7, -123 % 7)
print(123 // -7, 123 % -7)
print(-123 // -7, -123 % -7)

