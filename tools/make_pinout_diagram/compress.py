import sys
import zlib

data_str = sys.stdin.read()
compressed = zlib.compress(bytes(data_str, "utf-8"), wbits=10)
print(
    f"{len(data_str) / len(compressed):.1f}x smaller: uncompressed={len(data_str)}, compressed={len(compressed)}",
    file=sys.stderr,
)
print(compressed)
