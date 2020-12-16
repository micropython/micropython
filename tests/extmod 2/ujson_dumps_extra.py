# test uPy ujson behaviour that's not valid in CPy

try:
    import ujson
except ImportError:
    print("SKIP")
    raise SystemExit

print(ujson.dumps(b'1234'))
