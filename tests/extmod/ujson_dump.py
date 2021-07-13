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

s = StringIO()
json.dump(False, s)
print(s.getvalue())

s = StringIO()
json.dump({"a": (2, [3, None])}, s)
print(s.getvalue())

# dump to a small-int not allowed
try:
    json.dump(123, 1)
except (AttributeError, OSError):  # CPython and uPy have different errors
    print("Exception")

# dump to an object not allowed
try:
    json.dump(123, {})
except (AttributeError, OSError):  # CPython and uPy have different errors
    print("Exception")
