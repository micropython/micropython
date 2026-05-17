try:
    bytearray.count
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b"aaaa").count(b"a"))
