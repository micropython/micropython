# Test throwing repeatedly into the same generator, where that generator
# is yielding from another generator.


def yielder():
    yield 4
    yield 5


def gen():
    while True:
        try:
            print("gen received:", (yield from yielder()))
        except ValueError as exc:
            print(repr(exc))


g = gen()
for i in range(2):
    print("send, got:", g.send(None))
    print("throw, got:", g.throw(ValueError("a", i)))
    print("throw, got:", g.throw(ValueError("b", i)))
