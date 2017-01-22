try:
    print((10).to_bytes(1, "big"))
except Exception as e:
    print(type(e))
