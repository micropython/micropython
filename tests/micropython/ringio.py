# Check that micropython.RingIO works correctly.

try:
    import micropython

    micropython.RingIO
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

rb = micropython.RingIO(16)
print(rb)

print(rb.any())

rb.write(b"\x00")
print(rb.any())

rb.write(b"\x00")
print(rb.any())

print(rb.read(2))
print(rb.any())


rb.write(b"\x00\x01")
print(rb.read())

print(rb.read(1))

# Try to write more data than can fit at one go.
print(rb.write(b"\x00\x01" * 10))
print(rb.write(b"\x00"))
print(rb.read())


ba = bytearray(17)
rb = micropython.RingIO(ba)
print(rb)
print(rb.write(b"\x00\x01" * 10))
print(rb.write(b"\x00"))
print(rb.read())

try:
    # Size must be int.
    micropython.RingIO(None)
except TypeError as ex:
    print(type(ex))

try:
    # Buffer may not be empty
    micropython.RingIO(bytearray(0))
except ValueError as ex:
    print(type(ex))

try:
    # Buffer may not be too small
    micropython.RingIO(bytearray(1))
except ValueError as ex:
    print(type(ex))

try:
    # Size may not be too small
    micropython.RingIO(0)
except ValueError as ex:
    print(type(ex))
