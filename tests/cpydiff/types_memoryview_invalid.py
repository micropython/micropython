"""
categories: Types,memoryview
description: memoryview can become invalid if its target is resized
cause: CPython prevents a ``bytearray`` or ``io.bytesIO`` object from changing size while there is a ``memoryview`` object that references it. MicroPython requires the programmer to manually ensure that an object is not resized while any ``memoryview`` references it.

In the worst case scenario, resizing an object which is the target of a memoryview can cause the memoryview(s) to reference invalid freed memory (a use-after-free bug) and corrupt the MicroPython runtime.
workaround: Do not change the size of any ``bytearray`` or ``io.bytesIO`` object that has a ``memoryview`` assigned to it.
"""
b = bytearray(b"abcdefg")
m = memoryview(b)
b.extend(b"hijklmnop")
print(b, bytes(m))
