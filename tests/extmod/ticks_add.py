try:
    from time import ticks_diff, ticks_add
except ImportError:
    print("SKIP")
    raise SystemExit

# Maximum value returned from ticks_add, ticks_ms, etc.
TICKS_MAX = ticks_add(0, -1)
# Maximum value returned from ticks_diff.
TICKS_INTERVAL_MAX = TICKS_MAX // 2

# Invariants:
# - ticks_diff(ticks_add(T, delta), T) == delta
# - ticks_diff(T, ticks_add(T, delta)) == -delta

# Check actual values of ticks_add.
print(ticks_add(20, 12))
print(ticks_add(20, -12))

# Check invariant.
print(ticks_diff(ticks_add(100, 123), 100))
print(ticks_diff(ticks_add(100, -123), 100))
print(ticks_diff(100, ticks_add(100, 123)))
print(ticks_diff(100, ticks_add(100, -123)))

# Check limits.
for T in (0, 10, TICKS_MAX):
    for delta in (
        -TICKS_INTERVAL_MAX - 1,
        -TICKS_INTERVAL_MAX,
        0,
        TICKS_INTERVAL_MAX,
        TICKS_INTERVAL_MAX + 1,
    ):
        try:
            print(ticks_diff(ticks_add(T, delta), T) == delta)
        except OverflowError:
            print("OverflowError")
        try:
            print(ticks_diff(T, ticks_add(T, delta)) == -delta)
        except OverflowError:
            print("OverflowError")
