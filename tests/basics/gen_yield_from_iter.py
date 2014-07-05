def gen():
    yield from (1, 2, 3)

def gen2():
    yield from gen()

print(list(gen()))
print(list(gen2()))
