# Test that int.from_bytes() for small number of bytes generates
# small int.

try:
    import micropython

    micropython.heap_lock
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

micropython.heap_lock()
print(int.from_bytes(b"1", "little"))
print(int.from_bytes(b"12", "little"))
print(int.from_bytes(b"2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", "little"))
micropython.heap_unlock()
