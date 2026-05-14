try:
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

import sys

# On 32-bit platforms a crafted format string can wrap size_t around to a
# small value, causing struct.pack to allocate a tiny buffer and then
# write far past its end.  Verify that a ValueError is raised instead.
# The exact repeat count needed to overflow depends on platform pointer size.

if sys.maxsize > 2**31:
    # 64-bit: a 32-bit size_t overflow is not reachable; skip.
    print("SKIP")
    raise SystemExit

# 32-bit: the format count must fit within MicroPython's small-integer
# range (2**30 - 1 on 32-bit).  A single leading 'I' (4 bytes) reduces
# the remaining budget to (2**32 - 1 - 4) // 4 = 1073741822, and
# 1073741823 (= 2**30 - 1) exceeds that, so a ValueError must be raised.
try:
    struct.calcsize("1I1073741823I")
    print("no error")
except (ValueError, OverflowError, MemoryError):
    print("raises")
