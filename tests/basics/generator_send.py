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
