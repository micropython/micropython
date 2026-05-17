def gen():
    yield 1
    yield 2
    raise ValueError

def gen2():
    try:
        print((yield from gen()))
    except ValueError:
        print("caught ValueError from downstream")

g = gen2()
print(list(g))
