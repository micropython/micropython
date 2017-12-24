try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

data = bytearray(b'01234567')

print(uctypes.bytes_at(uctypes.addressof(data), 4)) 
print(uctypes.bytearray_at(uctypes.addressof(data), 4)) 
