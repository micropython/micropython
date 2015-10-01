# tests transition from small to large int representation by left-shift operation
for i in range(1, 17):
    for shift in range(70):
        print(i, '<<', shift, '=', i << shift)

# test bit-shifting negative integers
for i in range(8):
    print(-100000000000000000000000000000 << i)
    print(-100000000000000000000000000001 << i)
    print(-100000000000000000000000000002 << i)
    print(-100000000000000000000000000003 << i)
    print(-100000000000000000000000000004 << i)
    print(-100000000000000000000000000000 >> i)
    print(-100000000000000000000000000001 >> i)
    print(-100000000000000000000000000002 >> i)
    print(-100000000000000000000000000003 >> i)
    print(-100000000000000000000000000004 >> i)

# shl by zero
print((1<<70) << 0)
