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


# Verify that an injected exception will be raised from next().
print(next(g))
print(next(g))
g.pend_throw(ValueError())

v = None
try:
    v = next(g)
except Exception as e:
    print("raised", repr(e))

print("ret was:", v)

# Verify that you can't resume the coroutine from within the running coroutine.
def gen_next():
    next(g)
    yield 1

g = gen_next()

try:
    next(g)
except Exception as e:
    print("raised", repr(e))


# Verify that you can't pend_throw from within the running coroutine.
def gen_pend_throw():
    g.pend_throw(ValueError())
    yield 1

g = gen_pend_throw()

try:
    next(g)
except Exception as e:
    print("raised", repr(e))


# Verify that the pend_throw exception can be ignored.
class CancelledError(Exception):
    pass

def gen_cancelled():
    for i in range(5):
        try:
            yield i
        except CancelledError:
            print('ignore CancelledError')

g = gen_cancelled()
print(next(g))
g.pend_throw(CancelledError())
print(next(g))
# pend_throw() is not allowed on generators that have not started yet
g = gen_cancelled()
try:
     g.pend_throw(CancelledError())
except TypeError:
     print("TypeError")

# Verify that calling pend_throw returns the previous exception.
g = gen()
next(g)
print(repr(g.pend_throw(CancelledError())))
print(repr(g.pend_throw(OSError)))


# Verify that you can pend_throw(None) to cancel a previous pend_throw.
g = gen()
next(g)
g.pend_throw(CancelledError())
print(repr(g.pend_throw(None)))
print(next(g))
