# Tests that the pending exception state is managed correctly
# (previously failed on native emitter).

def noop_task():
    print('noop task')
    yield 1

def raise_task():
    print('raise task')
    yield 2
    print('raising')
    raise Exception

def main():
    try:
        yield from raise_task()
    except:
        print('main exception')

    yield from noop_task()

for z in main():
    print('outer iter', z)
