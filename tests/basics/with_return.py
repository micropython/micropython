class CtxMgr:
    def __init__(self, id):
        self.id = id

    def __enter__(self):
        print("__enter__", self.id)
        return self

    def __exit__(self, a, b, c):
        print("__exit__", self.id, repr(a), repr(b))

# simple case
def foo():
    with CtxMgr(1):
        return 4
print(foo())

# for loop within with (iterator needs removing upon return)
def f():
    with CtxMgr(1):
        for i in [1, 2]:
            return i
print(f())

# multiple for loops within with
def f():
    with CtxMgr(1):
        for i in [1, 2]:
            for j in [3, 4]:
                return (i, j)
print(f())

# multiple for loops within nested withs
def f():
    with CtxMgr(1):
        for i in [1, 2]:
            for j in [3, 4]:
                with CtxMgr(2):
                    for k in [5, 6]:
                        for l in [7, 8]:
                            return (i, j, k, l)
print(f())

# multiple for loops that are optimised, and nested withs
def f():
    with CtxMgr(1):
        for i in range(1, 3):
            for j in range(3, 5):
                with CtxMgr(2):
                    for k in range(5, 7):
                        for l in range(7, 9):
                            return (i, j, k, l)
print(f())
