# test uPy ujson behaviour that's not valid in CPy

try:
    import ujson
except ImportError:
    print("SKIP")
    raise SystemExit

print(ujson.dumps(b"1234"))

# By default MicroPython prints these as str(s).
# With MICROPY_PY_UJSON_FLOAT_MODE_STRICT it's a ValueError to try this.
special_float = ["nan", "inf", "-inf"]
try:
    ujson.dumps(float(special_float[0]))
    json_mode = 0
except ValueError:
    json_mode = 1

if json_mode == 0:
    for s in special_float:
        print(ujson.dumps(float(s)))
elif json_mode == 1:
    for s in special_float:
        try:
            ujson.dumps(float(s))
        except ValueError:
            print(s)
