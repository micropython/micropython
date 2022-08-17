try:
    bytearray.partition
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b"asdsf").partition(b"s"))
print(bytearray(b"asdsf").rpartition(b"s"))
