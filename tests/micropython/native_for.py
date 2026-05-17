# test for native for loops


@micropython.native
def f1(n):
    for i in range(n):
        print(i)


f1(4)


@micropython.native
def f2(r):
    for i in r:
        print(i)


f2(range(4))
