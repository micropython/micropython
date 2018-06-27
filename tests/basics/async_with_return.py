# test async with, escaped by a return

class AContext:
    async def __aenter__(self):
        print('enter')
        return 1
    async def __aexit__(self, exc_type, exc, tb):
        print('exit', exc_type, exc)

async def f1():
    async with AContext():
        print('body')
        return

o = f1()
try:
    o.send(None)
except StopIteration:
    print('finished')

async def f2():
    try:
        async with AContext():
            print('body')
            return
    finally:
        print('finally')

o = f2()
try:
    o.send(None)
except StopIteration:
    print('finished')

async def f3():
    try:
        try:
            async with AContext():
                print('body')
                return
        finally:
            print('finally inner')
    finally:
        print('finally outer')

o = f3()
try:
    o.send(None)
except StopIteration:
    print('finished')
