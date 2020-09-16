# test waiting within "async for" __anext__ function

try:
    import usys as sys
except ImportError:
    import sys
if sys.implementation.name == 'micropython':
    # uPy allows normal generators to be awaitables
    coroutine = lambda f: f
else:
    import types
    coroutine = types.coroutine

@coroutine
def f(x):
    print('f start:', x)
    yield x + 1
    yield x + 2
    return x + 3

class ARange:
    def __init__(self, high):
        print('init')
        self.cur = 0
        self.high = high

    def __aiter__(self):
        print('aiter')
        return self

    async def __anext__(self):
        print('anext')
        print('f returned:', await f(20))
        if self.cur < self.high:
            val = self.cur
            self.cur += 1
            return val
        else:
            raise StopAsyncIteration

async def coro():
    async for x in ARange(4):
        print('x', x)

o = coro()
try:
    while True:
        print('coro yielded:', o.send(None))
except StopIteration:
    print('finished')
