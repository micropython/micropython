# test that iterating doesn't use the heap
try:
    frozenset
except NameError:
    print("SKIP")
    raise SystemExit
try:
    import uarray as array
except ImportError:
    try:
        import array
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    from micropython import heap_lock, heap_unlock
except (ImportError, AttributeError):
    heap_lock = heap_unlock = lambda: 0


def do_iter(l):
    heap_lock()
    for i in l:
        print(i)
    heap_unlock()


def gen_func():
    yield 1
    yield 2


# pre-create collections to iterate over
ba = bytearray(b"123")
ar = array.array("H", (123, 456))
t = (1, 2, 3)
l = [1, 2]
d = {1: 2}
s = set((1,))
fs = frozenset((1,))
g1 = (100 + x for x in range(2))
g2 = gen_func()

# test containment (both success and failure) with the heap locked
heap_lock()
print(49 in b"123", 255 in b"123")
print(1 in t, -1 in t)
print(1 in l, -1 in l)
print(1 in d, -1 in d)
print(1 in s, -1 in s)
heap_unlock()

# test unpacking with the heap locked
unp0 = unp1 = unp2 = None  # preallocate slots for globals
heap_lock()
unp0, unp1, unp2 = t
print(unp0, unp1, unp2)
heap_unlock()

# test certain builtins with the heap locked
heap_lock()
print(all(t))
print(any(t))
print(min(t))
print(max(t))
print(sum(t))
heap_unlock()

# test iterating over collections with the heap locked
do_iter(b"123")
do_iter(ba)
do_iter(ar)
do_iter(t)
do_iter(l)
do_iter(d)
do_iter(s)
do_iter(fs)
do_iter(g1)
do_iter(g2)
