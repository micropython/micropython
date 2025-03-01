# test time.time_ns()

try:
    import time

    time.sleep_us
    time.time_ns
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


t0 = time.time_ns()
time.sleep_us(5000)
t1 = time.time_ns()

# Check that time_ns increases.
print(t0 < t1)

# Check that time_ns counts correctly, but be very lenient with the bounds (2ms to 50ms).
if 2000000 < t1 - t0 < 50000000:
    print(True)
else:
    print(t0, t1, t1 - t0)

# Check that time.time() and time.time_ns() are within a second of each other.
# Note that time.time() may return an int or float.
for _ in range(10):
    t_s, t_ns = time.time(), time.time_ns()
    print(abs(t_s * 1_000 - t_ns // 1_000_000) <= 1_000)
    time.sleep_us(100_000)
