try:
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

# test __enter__/__exit__
with io.StringIO() as b:
    b.write("foo")
    print(b.getvalue())
