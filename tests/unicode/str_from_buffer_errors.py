# str(buffer, encoding, errors) should apply the 'ignore'/'replace' error
# handlers to any buffer source (array.array, memoryview, ...), the same way it
# does for bytes and bytearray.  Companion to str_from_buffer_snapshot.py.

try:
    import array

    memoryview
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

# Requires the decode error handlers (MICROPY_PY_BUILTINS_BYTES_DECODE_ERRORS).
# When the feature is disabled, invalid UTF-8 raises even with 'replace'.
try:
    str(memoryview(b"\xff"), "utf-8", "replace")
except UnicodeError:
    print("SKIP")
    raise SystemExit

# array.array holding invalid UTF-8 bytes (typecode "B" keeps this endian-neutral).
a = array.array("B", b"a\xffb\x80c")
print(repr(str(a, "utf-8", "replace")))
print(repr(str(a, "utf-8", "ignore")))

# A memoryview over invalid UTF-8 (0xc3 0x28 is an invalid 2-byte sequence).
mv = memoryview(b"x\xc3\x28y")
print(repr(str(mv, "utf-8", "replace")))
print(repr(str(mv, "utf-8", "ignore")))

# Strict decoding (the default) must still raise for a buffer source.
try:
    str(memoryview(b"a\xffb"), "utf-8")
    print("UNEXPECTED")
except UnicodeError:
    print("UnicodeError")

# A valid-UTF-8 buffer passes through an error handler unchanged ("café").
print(repr(str(array.array("B", b"caf\xc3\xa9"), "utf-8", "replace")))  # codespell:ignore caf
