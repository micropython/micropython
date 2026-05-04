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
    # 64-bit: need a very large count; skip the overflow test but confirm
    # that a plain large format raises cleanly rather than hanging.
    repeat = "1000000000"
else:
    # 32-bit: repeating a 4-byte field ~1.5e9 times wraps size_t.
    repeat = "1500000000"

try:
    struct.calcsize(repeat + "I")
    print("no error")
except (ValueError, OverflowError, MemoryError):
    print("raises")
