# Test weakref.ref objects that may be collected at the same time as their
# referent: any callback that runs must be passed a live weakref.ref.

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

import gc


class T:
    pass


ok = [True]


def callback(r):
    # Allocate, to reuse any memory that was freed too early.
    bytearray(64)
    if type(r) is not weakref.ref:
        ok[0] = False


def make(n):
    keep = []
    for i in range(n):
        t = T()
        keep.append(weakref.ref(t, callback))
        keep.append(bytearray(i * 37 % 300))
    return len(keep)


for _ in range(20):
    make(30)
    gc.collect()
print(ok[0])
