# tests int constant folding in compiler

# negation
print(-0x3fffffff) # 32-bit edge case
print(-0x3fffffffffffffff) # 64-bit edge case
print(-(-0x3fffffff - 1)) # 32-bit edge case
print(-(-0x3fffffffffffffff - 1)) # 64-bit edge case

# 1's complement
print(~0x3fffffff) # 32-bit edge case
print(~0x3fffffffffffffff) # 64-bit edge case
print(~(-0x3fffffff - 1)) # 32-bit edge case
print(~(-0x3fffffffffffffff - 1)) # 64-bit edge case

# zero big-num on rhs
print(1 + ((1 << 65) - (1 << 65)))

# negative big-num on rhs
print(1 + (-(1 << 65)))
