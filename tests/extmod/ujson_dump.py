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
