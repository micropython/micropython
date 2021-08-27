# test waiting within "async for" __anext__ function

# uPy allows normal generators to be awaitables.
# CircuitPython does not.
# In CircuitPython you need to have an __await__ method on an awaitable like in CPython;
#  and like in CPython, generators do not have __await__.

class Awaitable:
    def __init__(self, x):
        self.x = x

    def __await__(self):
        print('f start:', self.x)
        yield self.x + 1
        yield self.x + 2
        return self.x + 3

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
        print('f returned:', await Awaitable(20))
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
