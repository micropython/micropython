# tests transition from small to large int representation by left-shift operation
for i in range(1, 17):
    for shift in range(70):
        print(i, '<<', shift, '=', i << shift)
