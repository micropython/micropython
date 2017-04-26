# test simple async with execution

class AContext:
    async def __aenter__(self):
        print('enter')
        return 1
    async def __aexit__(self, exc_type, exc, tb):
        print('exit', exc_type, exc)

async def f():
    async with AContext():
        print('body')

o = f()
try:
    o.send(None)
except StopIteration:
    print('finished')

async def g():
    async with AContext() as ac:
        print(ac)
        raise ValueError('error')

o = g()
try:
    o.send(None)
except ValueError:
    print('ValueError')
