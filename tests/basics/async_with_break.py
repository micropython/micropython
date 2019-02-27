# test async with, escaped by a break

class AContext:
    async def __aenter__(self):
        print('enter')
        return 1
    async def __aexit__(self, exc_type, exc, tb):
        print('exit', exc_type, exc)

async def f1():
    while 1:
        async with AContext():
            print('body')
            break
            print('no 1')
        print('no 2')

o = f1()
try:
    print(o.send(None))
except StopIteration:
    print('finished')

async def f2():
    while 1:
        try:
            async with AContext():
                print('body')
                break
                print('no 1')
        finally:
            print('finally')
        print('no 2')

o = f2()
try:
    print(o.send(None))
except StopIteration:
    print('finished')

async def f3():
    while 1:
        try:
            try:
                async with AContext():
                    print('body')
                    break
                    print('no 1')
            finally:
                print('finally inner')
        finally:
            print('finally outer')
        print('no 2')

o = f3()
try:
    print(o.send(None))
except StopIteration:
    print('finished')
