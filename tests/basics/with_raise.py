# test with when context manager raises in __enter__/__exit__

class CtxMgr:
    def __init__(self, id):
        self.id = id

    def __enter__(self):
        print("__enter__", self.id)
        if 10 <= self.id < 20:
            raise Exception('enter', self.id)
        return self

    def __exit__(self, a, b, c):
        print("__exit__", self.id, repr(a), repr(b))
        if 15 <= self.id < 25:
            raise Exception('exit', self.id)

# no raising
try:
    with CtxMgr(1):
        pass
except Exception as e:
    print(e)

# raise in enter
try:
    with CtxMgr(10):
        pass
except Exception as e:
    print(e)

# raise in enter and exit
try:
    with CtxMgr(15):
        pass
except Exception as e:
    print(e)

# raise in exit
try:
    with CtxMgr(20):
        pass
except Exception as e:
    print(e)
