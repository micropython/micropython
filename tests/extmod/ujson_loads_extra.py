# Test MicroPython behaviour which is different in CPython.

try:
    import ujson as json
except ImportError:
    print("SKIP")
    raise SystemExit

# CPython accepts these by default.
for s in ["NaN", "Infinity", "-Infinity"]:
    try:
        json.loads(s)
    except ValueError:
        print("ValueError")
