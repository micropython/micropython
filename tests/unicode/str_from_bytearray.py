# Test that a str/bytes created from a bytearray is an independent snapshot,
# and not a view onto the (mutable) bytearray buffer.  Mutating or resizing the
# source bytearray afterwards must not change the previously created object.

# Skip if needed via `skip_bytearray` logic in run-tests.py

# Non-interned payloads (byte literals) are used so the result isn't returned as
# an existing qstr.  Every scenario is covered with plain-ASCII data so the core
# bytearray paths are exercised directly; a couple of cases embed raw UTF-8 byte
# sequences to also cover multi-byte Unicode decoding, without relying on
# str.encode() to build the test data.

# str(bytearray, ...) then mutate the source in place.
ba = bytearray(b"the quick brown fox jumped over!")
s = str(ba, "utf-8")
ba[0] = ord("T")
print(s)
print(s[0])

# Same, but with multi-byte UTF-8 content: "café naïve 🐍 snapshot!".
ba = bytearray(b"caf\xc3\xa9 na\xc3\xafve \xf0\x9f\x90\x8d snapshot!")  # codespell:ignore caf
s = str(ba, "utf-8")
ba[0] = ord("X")
print(s)

# Overwriting every byte of the source must not change the str.
ba = bytearray(b"snapshot test one two three four!")
s = str(ba, "utf-8")
for i in range(len(ba)):
    ba[i] = ord("x")
print(s)

# Growing the bytearray reallocates its buffer; the str must stay intact.
ba = bytearray(b"grow test alpha bravo charlie delta!")
s = str(ba, "utf-8")
for _ in range(1000):
    ba.append(ord("Z"))
print(s)

# bytes(bytearray) is likewise an independent snapshot.
ba = bytearray(b"bytes snapshot alpha bravo charlie!")
b = bytes(ba)
ba[0] = ord("X")
print(b)

# Same, but with multi-byte UTF-8 content: "bytes ünicode 🐍 snakes!".
ba = bytearray(b"bytes \xc3\xbcnicode \xf0\x9f\x90\x8d snakes!")
b = bytes(ba)
ba[0] = ord("X")
print(b)
