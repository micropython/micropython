try:
    import uio as io
except ImportError:
    import io

# test __enter__/__exit__
with io.StringIO() as b:
    b.write("foo")
    print(b.getvalue())
