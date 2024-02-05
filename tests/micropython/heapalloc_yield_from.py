# Check that yield-from can work without heap allocation

import micropython


# Yielding from a function generator
def sub_gen(a):
    for i in range(a):
        yield i


def gen(g):
    yield from g


g = gen(sub_gen(4))
micropython.heap_lock()
print(next(g))
print(next(g))
micropython.heap_unlock()


# Yielding from a user iterator
class G:
    def __init__(self):
        self.value = 0

    def __iter__(self):
        return self

    def __next__(self):
        v = self.value
        self.value += 1
        return v


g = gen(G())
micropython.heap_lock()
print(next(g))
print(next(g))
micropython.heap_unlock()
