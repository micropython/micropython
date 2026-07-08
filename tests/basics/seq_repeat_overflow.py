# Test that repeating a sequence with a count large enough to overflow the
# internal byte-size computation raises a clean exception instead of
# wrapping around and corrupting memory. This is checked against a fixed
# .exp file (rather than comparing against CPython) because the size at
# which this overflows is a size_t vs machine-int implementation detail
# that differs between CPython (limited by Py_ssize_t, ie sys.maxsize) and
# MicroPython (limited by the C size_t used for the allocation, which is
# larger), so the two don't necessarily raise the same exception type here.
import sys

if not hasattr(sys, "maxsize"):
    print("SKIP")
    raise SystemExit


def test_repeat_overflow(seq):
    n = sys.maxsize // len(seq) + 2
    try:
        seq * n
    except (OverflowError, MemoryError):
        print(type(seq).__name__, "raises")
    else:
        print(type(seq).__name__, "no exception (unexpected)")


test_repeat_overflow(b"ab")
test_repeat_overflow("ab")
test_repeat_overflow([1, 2])
test_repeat_overflow((1, 2))


# The case above only reaches the overflow check for list/tuple: with
# 1-byte elements, len(seq) * n itself doesn't exceed size_t for an
# in-range n when len(seq) is small, so bytes/str instead just fail a
# (very) large allocation with MemoryError, never actually exercising
# their own OverflowError check. Use a longer sequence so an in-range n
# can push len(seq) * n itself past SIZE_MAX, hitting that check directly.
def test_repeat_overflow_direct(seq):
    n = sys.maxsize
    try:
        seq * n
    except OverflowError:
        print(type(seq).__name__, "raises directly")


test_repeat_overflow_direct(b"abc")
test_repeat_overflow_direct("abc")
