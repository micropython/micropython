# test basic async for execution
# example taken from PEP0492

class AsyncIteratorWrapper:
    def __init__(self, obj):
        print('init')
        self._it = iter(obj)

    def __aiter__(self):
        print('aiter')
        return self

    async def __anext__(self):
        print('anext')
        try:
            value = next(self._it)
        except StopIteration:
            raise StopAsyncIteration
        return value

async def coro():
    async for letter in AsyncIteratorWrapper('abc'):
        print(letter)

o = coro()
try:
    o.send(None)
except StopIteration:
    print('finished')
