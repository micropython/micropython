# Test adhoc extension to uheapq to support wraparound
# time (utime.ticks_ms() style) task queue.
from utime import ticks_add, ticks_diff
import uheapq as heapq

DEBUG = 0

MAX = ticks_add(0, -1)
MODULO_HALF = MAX // 2 + 1

if DEBUG:
    def dprint(*v):
        print(*v)
else:
    def dprint(*v):
        pass

# Try not to crash on invalid data
h = []
heapq.heappush(h, 1)
try:
    heapq.heappush(h, 2, True)
    assert False
except TypeError:
    pass

heapq.heappush(h, 2)
try:
    heapq.heappop(h, True)
    assert False
except TypeError:
    pass


def pop_all(h):
    l = []
    while h:
        l.append(heapq.heappop(h, True))
    dprint(l)
    return l

def add(h, v):
    heapq.heappush(h, (v, None), True)

h = []
add(h, 0)
add(h, MAX)
add(h, MAX - 1)
add(h, 101)
add(h, 100)
add(h, MAX - 2)
dprint(h)
l = pop_all(h)
for i in range(len(l) - 1):
    diff = ticks_diff(l[i + 1][0], l[i][0])
    assert diff > 0

def edge_case(edge, offset):
    h = []
    add(h, ticks_add(0, offset))
    add(h, ticks_add(edge, offset))
    dprint(h)
    l = pop_all(h)
    diff = ticks_diff(l[1][0], l[0][0])
    dprint(diff, diff > 0)
    return diff

dprint("===")
diff = edge_case(MODULO_HALF - 1, 0)
assert diff == MODULO_HALF - 1
assert edge_case(MODULO_HALF - 1, 100) == diff
assert edge_case(MODULO_HALF - 1, -100) == diff

# We expect diff to be always positive, per the definition of heappop() which should return
# the smallest value.
# This is the edge case where this invariant breaks, due to assymetry of two's-complement
# range - there's one more negative integer than positive, so heappushing values like below
# will then make ticks_diff() return the minimum negative value. We could make heappop
# return them in a different order, but ticks_diff() result would be the same. Conclusion:
# never add to a heap values where (a - b) == MODULO_HALF (and which are >= MODULO_HALF
# ticks apart in real time of course).
dprint("===")
diff = edge_case(MODULO_HALF, 0)
assert diff == -MODULO_HALF
assert edge_case(MODULO_HALF, 100) == diff
assert edge_case(MODULO_HALF, -100) == diff

dprint("===")
diff = edge_case(MODULO_HALF + 1, 0)
assert diff == MODULO_HALF - 1
assert edge_case(MODULO_HALF + 1, 100) == diff
assert edge_case(MODULO_HALF + 1, -100) == diff

print("OK")
