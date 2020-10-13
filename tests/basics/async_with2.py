# test waiting within async with enter/exit functions

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

class AContext:
    async def __aenter__(self):
        print('enter')
        print('f returned:', await Awaitable(10))
    async def __aexit__(self, exc_type, exc, tb):
        print('exit', exc_type, exc)
        print('f returned:', await Awaitable(20))

async def coro():
    async with AContext():
        print('body start')
        print('body f returned:', await Awaitable(30))
        print('body end')

o = coro()
try:
    while True:
        print('coro yielded:', o.send(None))
except StopIteration:
    print('finished')
