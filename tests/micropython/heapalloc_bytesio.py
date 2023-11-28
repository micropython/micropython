try:
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

import micropython

data = b"1234" * 16
buf = io.BytesIO(64)

micropython.heap_lock()

buf.write(data)

micropython.heap_unlock()

print(buf.getvalue())
