try:
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit


class Evil:
    n_calls = 0

    def __init__(self, arr, val):
        self.arr = arr
        self.val = val

    def __lt__(self, other):
        Evil.n_calls += 1
        if Evil.n_calls == 16:
            self.arr.clear()
            gc.collect()
        return self.val < other.val


a = []
a.extend(Evil(a, i) for i in range(32))

try:
    a.sort()
except ValueError:
    print("ValueError")
