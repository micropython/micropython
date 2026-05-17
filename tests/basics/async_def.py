# test async def

def dec(f):
    print('decorator')
    return f

# test definition with a decorator
@dec
async def foo():
    print('foo')

coro = foo()
try:
    coro.send(None)
except StopIteration:
    print('StopIteration')
