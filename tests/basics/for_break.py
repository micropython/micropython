# Testcase for break in a for [within bunch of other code]
# https://github.com/micropython/micropython/issues/635

def foo():
    seq = [1, 2, 3]
    v = 100
    i = 5
    while i > 0:
        print(i)
        for a in seq:
            if a == 2:
                break
        i -= 1

foo()

# break from within nested for loop
def bar():
    l = [1, 2, 3]
    for e1 in l:
        print(e1)
        for e2 in l:
            print(e1, e2)
            if e2 == 2:
                break

bar()
