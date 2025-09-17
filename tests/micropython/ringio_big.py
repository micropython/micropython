# Check that micropython.RingIO works correctly.

import micropython

try:
    micropython.RingIO
except AttributeError:
    print("SKIP")
    raise SystemExit

try:
    # The maximum possible size
    micropython.RingIO(bytearray(65535))
    micropython.RingIO(65534)

    try:
        # Buffer may not be too big
        micropython.RingIO(bytearray(65536))
    except ValueError as ex:
        print(type(ex))

    try:
        # Size may not be too big
        micropython.RingIO(65535)
    except ValueError as ex:
        print(type(ex))
except MemoryError:
    print("SKIP")
    raise SystemExit
