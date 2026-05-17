# Test throwing repeatedly into the same generator.


def gen():
    while True:
        try:
            print("gen received:", (yield "value"))
        except ValueError as exc:
            print(repr(exc))


g = gen()
for i in range(2):
    print("send, got:", g.send(None))
    print("throw, got:", g.throw(ValueError("a", i)))
    print("throw, got:", g.throw(ValueError("b", i)))
