# test loading from bytes and bytearray (introduced in Python 3.6)

try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

print(json.loads(b'[1,2]'))
print(json.loads(bytearray(b'[null]')))
