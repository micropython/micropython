try:
    from uio import BytesIO
    import umsgpack as msgpack
except:
    try:
        from io import BytesIO
        import msgpack
    except ImportError:
        print("SKIP")
        raise SystemExit

b = BytesIO()
msgpack.pack(False, s)
print(b.getvalue())

b = BytesIO()
msgpack.pack({"a": (-1, 0, 2, [3, None], 128)}, b)
print(b.getvalue())

# pack to a small-int not allowed
try:
    msgpack.pack(123, 1)
except (AttributeError, OSError):  # CPython and uPy have different errors
    print("Exception")

# pack to an object not allowed
try:
    msgpack.pack(123, {})
except (AttributeError, OSError):  # CPython and uPy have different errors
    print("Exception")
