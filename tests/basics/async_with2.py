# test waiting within async with enter/exit functions

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

class AContext:
    async def __aenter__(self):
        print('enter')
        print('f returned:', await f(10))
    async def __aexit__(self, exc_type, exc, tb):
        print('exit', exc_type, exc)
        print('f returned:', await f(20))

async def coro():
    async with AContext():
        print('body start')
        print('body f returned:', await f(30))
        print('body end')

o = coro()
try:
    while True:
        print('coro yielded:', o.send(None))
except StopIteration:
    print('finished')
