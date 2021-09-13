try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

for sep in [
    None,
    (", ", ": "),
    (",", ": "),
    (",", ":"),
    [", ", ": "],
    [",", ": "],
    [",", ":"],
]:
    print(json.dumps(False, separators=sep))
    print(json.dumps(True, separators=sep))
    print(json.dumps(None, separators=sep))
    print(json.dumps(1, separators=sep))
    print(json.dumps("abc", separators=sep))
    print(json.dumps("\x00\x01\x7e", separators=sep))
    print(json.dumps([], separators=sep))
    print(json.dumps([1], separators=sep))
    print(json.dumps([1, 2], separators=sep))
    print(json.dumps([1, True], separators=sep))
    print(json.dumps((), separators=sep))
    print(json.dumps((1,), separators=sep))
    print(json.dumps((1, 2), separators=sep))
    print(json.dumps((1, (2, 3)), separators=sep))
    print(json.dumps({}, separators=sep))
    print(json.dumps({"a": 1}, separators=sep))
    print(json.dumps({"a": (2, [3, None])}, separators=sep))
    print(json.dumps('"quoted"', separators=sep))
    print(json.dumps("space\n\r\tspace", separators=sep))
    print(json.dumps({None: -1}, separators=sep))
    print(json.dumps({False: 0}, separators=sep))
    print(json.dumps({True: 1}, separators=sep))
    print(json.dumps({1: 2}, separators=sep))

try:
    json.dumps(False, separators={"a": 1})
except (TypeError, ValueError):  # CPython and uPy have different errors
    print("Exception")

# invalid separator types
for sep in [1, object()]:
    try:
        json.dumps(False, separators=sep)
    except TypeError:
        print("Exception")

# too many/ not enough separators
for sep in [(), (",", ":", "?"), (",",), []]:
    try:
        json.dumps(False, separators=sep)
    except ValueError:
        print("Exception")
