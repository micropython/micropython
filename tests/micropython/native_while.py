# test native while loop


@micropython.native
def f(n):
    i = 0
    while i < n:
        print(i)
        i += 1


f(2)
f(4)
