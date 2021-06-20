# Tests that the correct nested exception handler is used when
# throwing into a generator (previously failed on native emitter).

def gen():
    try:
        yield 1
        try:
            yield 2
            try:
                yield 3
            except Exception:
                yield 4
                print(0)
            yield 5
        except Exception:
            yield 6
            print(1)
        yield 7
    except Exception:
        yield 8
        print(2)
    yield 9

for i in range(1, 10):
    g = gen()
    try:
        for _ in range(i):
            print(next(g))
        print(g.throw(ValueError))
    except ValueError:
        print('ValueError')
    except StopIteration:
        print('StopIteration')
