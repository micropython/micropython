# Companion to str_from_bytearray.py: str()/bytes() built from other mutable
# buffer sources (array.array and a writable memoryview) must also be independent
# snapshots, not views onto the source buffer.
try:
    import array

    _ = memoryview
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

# Non-interned byte-literal payloads (with raw UTF-8 sequences) avoid relying on
# str.encode() to build the test data.

# array.array is a mutable buffer source; str()/bytes() must snapshot the data.
# Content decodes to "array ünicode 🐍 target!".
a = array.array("B", b"array \xc3\xbcnicode \xf0\x9f\x90\x8d target!")
s = str(a, "utf-8")
b = bytes(a)
a[0] = ord("X")
print(s)
print(b)

# A writable memoryview (here over an array) is also a mutable source.
# Content decodes to "memoryview över 🐍 python!".
a = array.array("B", b"memoryview \xc3\xb6ver \xf0\x9f\x90\x8d python!")
mv = memoryview(a)
s = str(mv, "utf-8")
a[0] = ord("X")
print(s)
