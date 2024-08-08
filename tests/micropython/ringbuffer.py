# check that micropython.ringbuffer works correctly.

import micropython

try:
    micropython.ringbuffer
except AttributeError:
    print("SKIP")
    raise SystemExit

rb = micropython.ringbuffer(16)
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

print(rb.write(b"\x00\x01" * 10))
print(rb.read())

try:
    # size must be int.
    micropython.ringbuffer(None)
except TypeError as ex:
    print(ex)
