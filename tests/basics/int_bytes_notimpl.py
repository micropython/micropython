try:
    print((10).to_bytes(1, "big"))
except Exception as e:
    print(type(e))

try:
    print(int.from_bytes(b"\0", "big"))
except Exception as e:
    print(type(e))
