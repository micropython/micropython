# test construction of array.array from an object with an overflowing len

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit


class TooLong:
    def __len__(self):
        return 1 << 61

    def __iter__(self):
        for _ in range(4):
            yield 1.23


try:
    array("d", TooLong())
except MemoryError:
    print("MemoryError")
