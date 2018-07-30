# test use of return with try-except

def f(l, i):
    try:
        return l[i]
    except IndexError:
        print('IndexError')
        return -1

print(f([1], 0))
print(f([], 0))
