# test await expression

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
def wait(value):
    print('wait value:', value)
    msg = yield 'message from wait({})'.format(value)
    print('wait got back:', msg)
    return 10

async def f():
    x = await wait(1)**2
    print('x =', x)

coro = f()
print('return from send:', coro.send(None))
try:
    coro.send('message from main')
except StopIteration:
    print('got StopIteration')
