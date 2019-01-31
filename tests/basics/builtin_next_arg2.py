# test next(iter, default)

try:
    next(iter([]), 42)
except TypeError:  # 2-argument version not supported
    print('SKIP')
    raise SystemExit

print(next(iter([]), 42))
print(next(iter(range(0)), 42))
print(next((x for x in [0] if x == 1), 43))

def gen():
    yield 1
    yield 2

g = gen()
print(next(g, 42))
print(next(g, 43))
print(next(g, 44))

class Gen:
    def __init__(self):
        self.b = False

    def __next__(self):
        if self.b:
            raise StopIteration
        self.b = True
        return self.b

g = Gen()
print(next(g, 44))
print(next(g, 45))
