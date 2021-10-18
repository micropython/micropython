# Test MicroPython behaviour which is different in CPython.

try:
    import ujson as json
except ImportError:
    print("SKIP")
    raise SystemExit

# CPython accepts these by default.
# With MICROPY_PY_UJSON_FLOAT_MODE_JAVASCRIPT they're accepted as well.
special_float = [("NaN", "nan"), ("Infinity", "inf"), ("-Infinity", "-inf")]
try:
    json.loads(special_float[0][0])
    json_mode = 2
except ValueError:
    json_mode = 0

if json_mode == 0:
    for s, _ in special_float:
        try:
            json.loads(s)
        except ValueError:
            print("ValueError")
elif json_mode == 2:
    for s, r in special_float:
        if str(json.loads(s)) == r:
            print("ValueError")
