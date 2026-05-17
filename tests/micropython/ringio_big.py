# Check that micropython.RingIO works correctly.

try:
    import micropython

    micropython.RingIO
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

results = []

try:
    # The maximum possible size passed as an integer.
    micropython.RingIO(65534)

    try:
        # Size may not be too big
        micropython.RingIO(65535)
    except ValueError as ex:
        results.append(type(ex))

    # Allocate a buffer for use below.
    buf_64k = memoryview(bytearray(65536))

    # The maximum possible size passed as a buffer.
    micropython.RingIO(buf_64k[:-1])

    try:
        # Buffer may not be too big
        micropython.RingIO(buf_64k)
    except ValueError as ex:
        results.append(type(ex))

except MemoryError:
    print("SKIP")
    raise SystemExit

for result in results:
    print(result)
