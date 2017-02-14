# test uPy ujson behaviour that's not valid in CPy

try:
    import ujson
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(ujson.dumps(b'1234'))
