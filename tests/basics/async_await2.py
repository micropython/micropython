# test await expression

# uPy allows normal generators to be awaitables.
# CircuitPython does not.
# In CircuitPython you need to have an __await__ method on an awaitable like in CPython;
#  and like in CPython, generators do not have __await__.

class Awaitable:
    def __init__(self, value):
        self.value = value

    def __await__(self):
        print('wait value:', self.value)
        msg = yield 'message from wait({})'.format(self.value)
        print('wait got back:', msg)
        return 10

async def f():
    x = await Awaitable(1)**2
    print('x =', x)

coro = f()
print('return from send:', coro.send(None))
try:
    coro.send('message from main')
except StopIteration:
    print('got StopIteration')
