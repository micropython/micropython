def f(x):
    print('a')
    y = x
    print('b')
    while y > 0:
        print('c')
        y -= 1
        print('d')
        yield y
        print('e')
    print('f')
    return None

for val in f(3):
    print(val)

#gen = f(3)
#print(gen)
#print(gen.__next__())
#print(gen.__next__())
#print(gen.__next__())
#print(gen.__next__())
