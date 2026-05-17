# tests transition from small to large int representation by multiplication
for rhs in range(2, 11):
    lhs = 1
    for k in range(100):
        res = lhs * rhs
        print(lhs, '*', rhs, '=', res)
        lhs = res

# below tests pos/neg combinations that overflow small int

# 31-bit overflow
i = 1 << 20
print(i * i)
print(i * -i)
print(-i * i)
print(-i * -i)

# 63-bit overflow
i = 1 << 40
print(i * i)
print(i * -i)
print(-i * i)
print(-i * -i)
