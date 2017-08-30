# test for-else statement

# test optimised range with simple else
for i in range(2):
    print(i)
else:
    print('else')

# test optimised range with break over else
for i in range(2):
    print(i)
    break
else:
    print('else')

# test nested optimised range with continue in the else
for i in range(4):
    print(i)
    for j in range(4):
        pass
    else:
        continue
    break

# test optimised range with non-constant end value
N = 2
for i in range(N):
    print(i)
else:
    print('else')

# test generic iterator with simple else
for i in [0, 1]:
    print(i)
else:
    print('else')

# test generic iterator with break over else
for i in [0, 1]:
    print(i)
    break
else:
    print('else')
