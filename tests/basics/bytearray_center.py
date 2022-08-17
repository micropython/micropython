try:
    bytearray.center
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b"foo").center(6))
print(type(bytearray(b"foo").center(6)))
