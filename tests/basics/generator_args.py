# Handling of "complicated" arg forms to generators
# https://github.com/micropython/micropython/issues/397
def gen(v=5):
    for i in range(v):
        yield i

print(list(gen()))
print(list(gen(v=10)))


def g(*args, **kwargs):
    for i in args:
        yield i
    for k, v in kwargs.items():
        yield (k, v)

print(list(g(1, 2, 3, foo="bar")))
