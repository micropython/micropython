try:
    import uhashlib as hashlib
except ImportError:
    import hashlib


h = hashlib.sha256()
print(h.digest())

h = hashlib.sha256()
h.update(b"123")
print(h.digest())

h = hashlib.sha256()
h.update(b"abcd" * 1000)
print(h.digest())

print(hashlib.sha256(b"\xff" * 64).digest())

# TODO: running .digest() several times in row is not supported()
#h = hashlib.sha256(b'123')
#print(h.digest())
#print(h.digest())

# TODO: partial digests are not supported
#h = hashlib.sha256(b'123')
#print(h.digest())
#h.update(b'456')
#print(h.digest())
