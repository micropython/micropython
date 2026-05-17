# Parse 64-bit integers from JSON payloads.
#
# This also exercises parsing integers from strings
# where the value may not be null terminated (last line)
try:
    import json
except ImportError:
    print("SKIP")
    raise SystemExit


print(json.loads("9111222333444555666"))
print(json.loads("-9111222333444555666"))
print(json.loads("9111222333444555666"))
print(json.loads("-9111222333444555666"))
print(json.loads('["9111222333444555666777",9111222333444555666]'))
