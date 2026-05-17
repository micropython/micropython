def f():
    n = 0
    while True:
        n = yield n + 1
        print(n)

g = f()
try:
    g.send(1)
except TypeError:
    print("caught")

print(g.send(None))
print(g.send(100))
print(g.send(200))


def f2():
    print("entering")
    for i in range(3):
        print(i)
        yield
    print("returning 1")
    print("returning 2")

g = f2()
g.send(None)
g.send(1)
g.send(1)
try:
    g.send(1)
except StopIteration:
    print("caught")
try:
    g.send(1)
except StopIteration:
    print("caught")
