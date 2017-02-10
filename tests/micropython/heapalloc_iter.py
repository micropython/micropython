# test that iterating doesn't use the heap

try:
    from micropython import heap_lock, heap_unlock
except (ImportError, AttributeError):
    heap_lock = heap_unlock = lambda:0
import array

def do_iter(l):
    for i in l:
        print(i)

def gen_func():
    yield 1
    yield 2

# pre-create collections to iterate over
ba = bytearray(b'123')
ar = array.array('H', (123, 456))
t = (1, 2, 3)
l = [1, 2]
d = {1:2}
s = {1}
fs = frozenset((1,))
g1 = (100 + x for x in range(2))
g2 = gen_func()

# test certain builtins with the heap locked
heap_lock()
print(all(t))
print(any(t))
print(min(t))
print(max(t))
print(sum(t))
heap_unlock()

# test iterating over collections with the heap locked
heap_lock()
do_iter(b'123')
do_iter(ba)
do_iter(ar)
do_iter(t)
do_iter(l)
do_iter(d)
do_iter(s)
do_iter(fs)
do_iter(g1)
do_iter(g2)
heap_unlock()
