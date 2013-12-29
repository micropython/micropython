# test when different variables are closed over by different functions

def f():
    l1 = 1
    l2 = 2
    l3 = 3

    def g():
        return l1 + l2

    def h():
        return l2 + l3
