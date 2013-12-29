# closed over variable 2 deep

def f():
    x = 1
    def g():
        def h():
            return 1 + x
