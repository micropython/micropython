# On ports with 64-bit integer support, this exercises
# parsing integers from strings where the value may not
# be null terminated
try:
    import json
except ImportError:
    print("SKIP")
    raise SystemExit


print(json.loads("9111222333444555666"))
print(json.loads("-9111222333444555666"))
print(json.loads("9111222333444555666"))
print(json.loads("-9111222333444555666"))
print(json.loads("[\"9111222333444555666777\",9111222333444555666]"))
