class CtxMgr:

    def __enter__(self):
        print("__enter__")
        return self

    def __exit__(self, a, b, c):
        print("__exit__", repr(a), repr(b))

def foo():
    with CtxMgr():
        return 4

print(foo())
