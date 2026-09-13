# test _thread lock with timeout exceeding maximum

import math
import _thread

try:
    import utime as time
except ImportError:
    import time

if not hasattr(_thread, "TIMEOUT_MAX"):
    print("SKIP")
    raise SystemExit

# check timeout is greater than zero
print(_thread.TIMEOUT_MAX > 0)

# check timeout is not infinite (on platforms where it's an integer it cannot be infinity)
print(isinstance(_thread.TIMEOUT_MAX, int) or not math.isinf(_thread.TIMEOUT_MAX))

lock = _thread.allocate_lock()
try:
    # note: 4096 is the step to the next representable value after float(2**64-1)
    # this ensures the resulting float is actually different from the maximum
    lock.acquire(1, _thread.TIMEOUT_MAX + 4096)
except OverflowError:
    print("can't specify a timeout greater than TIMEOUT_MAX")
