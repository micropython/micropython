# tests transition from small to large int representation by multiplication
for rhs in range(2, 11):
    lhs = 1
    for k in range(100):
        res = lhs * rhs
        print(lhs, '*', rhs, '=', res)
        lhs = res

