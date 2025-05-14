# test basic async for execution
# example taken from PEP0492


class AsyncIteratorWrapper:
    def __init__(self, obj):
        print("init")
        self._obj = obj

    def __repr__(self):
        return "AsyncIteratorWrapper-" + self._obj

    def __aiter__(self):
        print("aiter")
        return AsyncIteratorWrapperIterator(self._obj)


class AsyncIteratorWrapperIterator:
    def __init__(self, obj):
        print("init")
        self._it = iter(obj)

    async def __anext__(self):
        print("anext")
        try:
            value = next(self._it)
        except StopIteration:
            raise StopAsyncIteration
        return value


def run_coro(c):
    print("== start ==")
    try:
        c.send(None)
    except StopIteration:
        print("== finish ==")


async def coro0():
    async for letter in AsyncIteratorWrapper("abc"):
        print(letter)


run_coro(coro0())


async def coro1():
    a = AsyncIteratorWrapper("def")
    async for letter in a:
        print(letter)
    print(a)


run_coro(coro1())

a_global = AsyncIteratorWrapper("ghi")


async def coro2():
    async for letter in a_global:
        print(letter)
    print(a_global)


run_coro(coro2())


async def coro3(a):
    async for letter in a:
        print(letter)
    print(a)


run_coro(coro3(AsyncIteratorWrapper("jkl")))
