# test simple async with execution

class AContext:
    async def __aenter__(self):
        print('enter')
    async def __aexit__(self, exc_type, exc, tb):
        print('exit')

async def f():
    async with AContext():
        print('body')

o = f()
try:
    o.send(None)
except StopIteration:
    print('finished')
