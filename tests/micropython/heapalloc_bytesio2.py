# Creating BytesIO from immutable object should not immediately
# copy its content.
try:
    import io
    import micropython

    micropython.mem_total
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


data = b"1234" * 256

before = micropython.mem_total()

buf = io.BytesIO(data)

after = micropython.mem_total()

print(after - before < len(data))
