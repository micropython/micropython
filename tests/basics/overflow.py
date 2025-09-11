# For 64 bit platforms, this hits the overflow case in mp_compute_size_overflow
try:
    "a" * 8 * (1 << 62)
except (OverflowError, MemoryError):
    print("MemoryError")
    raise SystemExit

# For 32-bit platforms, this hits the overflow case in mp_compute_size_overflow
# We have to try this second, because if the *host* Python is 64 bits then this
# will try to allocate multiple gigabytes of memory and run for a long time.
try:
    "a" * 8 * (1 << 29)
except (OverflowError, MemoryError):
    print("MemoryError")
    raise SystemExit
