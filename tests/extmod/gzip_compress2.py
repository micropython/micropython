# Test MicroPython-specific extensions to GzipFile.

try:
    # Check if gzip is available.
    import gzip
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # Check if compression is enabled.
    b = io.BytesIO()
    g = gzip.GzipFile(fileobj=b, mode="wb")
except ValueError:
    print("SKIP")
    raise SystemExit

b = io.BytesIO()
for wbits in range(0, 16):
    try:
        gzip.GzipFile(fileobj=b, mode="wb", wbits=wbits)
        print(wbits)
    except ValueError:
        print("ValueError")

gzip.GzipFile(fileobj=b, mode="wb", wbits=5, hist=bytearray(32))

try:
    gzip.GzipFile(fileobj=b, mode="wb", wbits=5, hist=bytearray(27))
except ValueError:
    print("ValueError")

gzip.GzipFile(fileobj=b, mode="wb", wbits=5, hist=bytearray(37))

# Invalid mode.
buf = io.BytesIO()
try:
    gzip.GzipFile(fileobj=buf, mode="abc")
except ValueError:
    print("ValueError")
