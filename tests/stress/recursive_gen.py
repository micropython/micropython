# test deeply recursive generators

def gen():
    yield from gen()

try:
    list(gen())
except RuntimeError:
    print('RuntimeError')
