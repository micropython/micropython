def gen():
    yield from (1, 2, 3)

def gen2():
    yield from gen()

def gen3():
    yield from (4, 5)
    yield 6

print(list(gen()))
print(list(gen2()))
print(list(gen3()))
