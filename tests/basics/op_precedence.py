# see https://docs.python.org/3/reference/expressions.html#operator-precedence

# '|' is the least binding numeric operator

# '^'
# OK:  1 | (2 ^ 3) = 1 | 1 = 1
# BAD: (1 | 2) ^ 3 = 3 ^ 3 = 0
print(1 | 2 ^ 3)

# '&'
# OK:  3 ^ (2 & 1) = 3 ^ 0 = 3
# BAD: (3 ^ 2) & 1 = 1 & 1 = 1
print(3 ^ 2 & 1)

# '<<', '>>'
# OK:  2 & (3 << 1) = 2 & 6 = 2
# BAD: (2 & 3) << 1 = 2 << 1 = 4
print(2 & 3 << 1)
# OK:  6 & (4 >> 1) = 6 & 2 = 2
# BAD: (6 & 4) >> 1 = 2 >> 1 = 1
print(6 & 4 >> 1)

# '+', '-'
# OK:  1 << (1 + 1) = 1 << 2 = 4
# BAD: (1 << 1) + 1 = 2 + 1 = 3
print(1 << 1 + 1)

# '*', '/', '//', '%'
# OK:  2 + (2 * 2) = 2 + 4 = 6
# BAD: (2 + 2) * 2 = 4 * 2 = 8
print(2 + 2 * 2)

# '+x', '-x', '~x'

# '**'
# OK:  -(2**2) = -4
# BAD: (-2)**2 = 4
print(-2**2)
# OK: 2**(-1) = 0.5
print(2**-0)

# (expr...)
print((2 + 2) * 2)
