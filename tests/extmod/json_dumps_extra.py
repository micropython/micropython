# test uPy json behaviour that's not valid in CPy

try:
    import json
except ImportError:
    print("SKIP")
    raise SystemExit

print(json.dumps(b"1234"))
