try:
    from uio import StringIO
    import ujson as json
except:
    try:
        from io import StringIO
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
    s = StringIO()
    json.dump(False, s, separators=sep)
    print(s.getvalue())

    s = StringIO()
    json.dump({"a": (2, [3, None])}, s, separators=sep)
    print(s.getvalue())

    # dump to a small-int not allowed
    try:
        json.dump(123, 1, separators=sep)
    except (AttributeError, OSError):  # CPython and uPy have different errors
        print("Exception")

    # dump to an object not allowed
    try:
        json.dump(123, {}, separators=sep)
    except (AttributeError, OSError):  # CPython and uPy have different errors
        print("Exception")


try:
    s = StringIO()
    json.dump(False, s, separators={"a": 1})
except (TypeError, ValueError):  # CPython and uPy have different errors
    print("Exception")

# invalid separator types
for sep in [1, object()]:
    try:
        s = StringIO()
        json.dump(False, s, separators=sep)
    except TypeError:
        print("Exception")

# too many/ not enough separators
for sep in [(), (",", ":", "?"), (",",), []]:
    try:
        s = StringIO()
        json.dump(False, s, separators=sep)
    except ValueError:
        print("Exception")
