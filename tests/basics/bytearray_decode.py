try:
    print(bytearray(b'').decode())
    print(bytearray(b'abc').decode())
except AttributeError:
    print("SKIP")
    raise SystemExit

print(bytearray(b'').decode('utf-8'))
print(bytearray(b'abc').decode('ascii'))

try:
    print(bytearray(b'abc').decode('InvalidEncoding'))
except ValueError:
    print("SKIP")
    raise SystemExit
