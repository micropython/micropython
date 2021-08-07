# Yielding from stopped generator is ok and results in None

def gen():
    return 1
    # This yield is just to make this a generator
    yield

f = gen()

def run():
    print((yield from f))
    print((yield from f))
    print((yield from f))

try:
    next(run())
except StopIteration:
    print("StopIteration")


# Where "f" is a native generator
def run():
    print((yield from f))


f = zip()
try:
    next(run())
except StopIteration:
    print("StopIteration")
