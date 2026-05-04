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

# 32-bit: repeating a 4-byte field ~1.5e9 times wraps uint32 size_t.
try:
    struct.calcsize("1500000000I")
    print("no error")
except (ValueError, OverflowError, MemoryError):
    print("raises")
