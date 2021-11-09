try:
    import uio
except ImportError:
    print("SKIP")
    raise SystemExit

from src import micropython

data = b"1234" * 16
buf = uio.BytesIO(64)

micropython.heap_lock()

buf.write(data)

micropython.heap_unlock()

print(buf.getvalue())
