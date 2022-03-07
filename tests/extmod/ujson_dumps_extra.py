# test uPy ujson behaviour that's not valid in CPy

try:
    import ujson
except ImportError:
    print("SKIP")
    raise SystemExit

print(ujson.dumps(b"1234"))

# By default MicroPython prints these as str(s).
# With MICROPY_PY_UJSON_FLOAT_MODE_STRICT it's a ValueError to try this.
# With MICROPY_PY_UJSON_FLOAT_MODE_JAVASCRIPT they're dumped JS-style.
special_float = [("nan", "NaN"), ("inf", "Infinity"), ("-inf", "-Infinity")]
try:
    n = ujson.dumps(float(special_float[0][0]))
    json_mode = 2 if n[0] == "N" else 0
except ValueError:
    json_mode = 1

if json_mode == 0:
    for s, _ in special_float:
        print(ujson.dumps(float(s)))
elif json_mode == 1:
    for s, _ in special_float:
        try:
            ujson.dumps(float(s))
        except ValueError:
            print(s)
elif json_mode == 2:
    for s, r in special_float:
        if ujson.dumps(float(s)) == r:
            print(s)
