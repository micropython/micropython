# Test str.format_map()
try:
    str.format_map
except:
    print("SKIP")
    raise SystemExit

print("{name}".format_map({"name": "world"}))
print("{x} + {y} = {z}".format_map({"x": 1, "y": 2, "z": 3}))
print("{name} is {age}".format_map({"name": "Alice", "age": 30}))
print("no replacements".format_map({}))
print("{a}{b}{c}".format_map({"a": "x", "b": "y", "c": "z"}))
