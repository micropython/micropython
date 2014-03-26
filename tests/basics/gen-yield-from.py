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


# Like above, but terminate subgen using StopIteration
def gen3():
    yield 1
    yield 2
    raise StopIteration

def gen4():
    print("here1")
    print((yield from gen3()))
    print("here2")

g = gen4()
print(list(g))

# Like above, but terminate subgen using StopIteration with value
def gen5():
    yield 1
    yield 2
    raise StopIteration(123)

def gen6():
    print("here1")
    print((yield from gen5()))
    print("here2")

g = gen6()
print(list(g))
