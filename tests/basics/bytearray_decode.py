try:
    print(bytearray(b'').decode())
    print(bytearray(b'abc').decode())
except AttributeError:
    print("SKIP")
    raise SystemExit
