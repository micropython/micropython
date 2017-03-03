# tests int constant folding in compiler

# positive
print(+1)
print(+100)

# negation
print(-1)
print(-(-1))

# 1's complement
print(~0)
print(~1)
print(~-1)

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
