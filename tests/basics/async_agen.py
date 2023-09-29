# Test async generators

# helper
def run_task(t):
    while True:
        try:
            t.send(None)
        except Exception as er:
            print('run_task finished with', repr(er))
            break

# Test SyntaxError

try:
    exec('async def f(): yield from y')
except Exception as er:
    print(type(er))

try:
    exec('async def f(): yield; return 1')
except Exception as er:
    print(type(er))

try:
    exec('async def f(): return 2; yield')
except Exception as er:
    print(type(er))

# Test core behaviour

async def genfunc():
    yield 1
    yield 2

async def main():
    gen = genfunc()

    print(gen.__aiter__() is gen)

    print(repr(await gen.__anext__())) # should be 1
    print(repr(await gen.__anext__())) # should be 2

    try:
        await gen.__anext__()
    except Exception as er:
        print(type(er))

    try:
        await gen.__anext__()
    except Exception as er:
        print(type(er))

run_task(main())

# Test async-for iterating over an async generator

class Wait:
    def __init__(self, n):
        self.n = n
    def __await__(self):
        return self
    __iter__ = __await__ # needed for uPy
    def __next__(self):
        if self.n == 0:
            raise StopIteration
        print('wait', self.n)
        self.n -= 1

async def gen():
    for i in range(4):
        yield i
        await Wait(2)

async def main():
    g = gen()
    async for a in g:
        print('got', a)

run_task(main())
