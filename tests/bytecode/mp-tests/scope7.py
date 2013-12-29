# test order of closed over locals
# not that CPython seems to sort closed over variables (but not fast locals)

def f():
    l1 = 1
    l2 = 4
    l3 = 3
    l4 = 2
    l5 = 5

    def g():
        return l1 + l4 + l3 + l2 + l5

    def h():
        return l1 + l2 + l3 + l4 + l5
