"""Test user-class __del__ raise behavior."""

try:
    import micropython
    import gc
    import sys

    gc.is_finalized
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


if sys.platform == "webassembly" and sys.implementation._build == "pyscript":
    # GC cannot run in the middle of python bytecode on this platform.
    print("SKIP")
    raise SystemExit


try:
    micropython.alloc_emergency_exception_buf(256)
except AttributeError:
    pass


n = 0


class Raiser:
    def __init__(self, i):
        self.i = i

    def __del__(self):
        global n
        if n < 5:
            n += 1
            raise Exception(n)


for i in range(10):
    obj = Raiser(i)

del obj

gc.collect()
try:
    raise Exception  # nlr.ret_val seems to be a major culprit for spurious reachability
except Exception:
    pass
gc.collect()
