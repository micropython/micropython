def gen():
    i = 0
    while 1:
        yield i
        i += 1

g = gen()

try:
    g.pend_throw
except AttributeError:
    print("SKIP")
    raise SystemExit


print(next(g))
print(next(g))
g.pend_throw(ValueError())

v = None
try:
    v = next(g)
except Exception as e:
    print("raised", repr(e))

print("ret was:", v)

try:
    gen().pend_throw(ValueError())
except TypeError:
    print("TypeError")
