# test uPy ujson behaviour that's not valid in CPy

try:
    import ujson
except ImportError:
    print("SKIP")
    raise SystemExit

print(ujson.dumps(b"1234"))

# By default MicroPython prints these as str(s).
for s in ["nan", "inf", "-inf"]:
    print(ujson.dumps(float(s)))
