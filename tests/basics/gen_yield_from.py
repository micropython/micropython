# Case of terminating subgen using return with value
def gen():
    yield 1
    yield 2
    return 3

def gen2():
    print("here1")
    print((yield from gen()))
    print("here2")

g = gen2()
print(list(g))


# StopIteration from within a Python function, within a native iterator (map), within a yield from
def gen7(x):
    if x < 3:
        return x
    else:
        raise StopIteration(444)

def gen8():
    print((yield from map(gen7, range(100))))

g = gen8()
print(list(g))
