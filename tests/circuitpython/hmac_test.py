try:
    import hmac
except ImportError:
    print("SKIP")
    raise SystemExit


def hx(b):
    return "".join("%02x" % c for c in b)


# RFC 4231 Test Case 1
print(hmac.new(b"\x0b" * 20, b"Hi There", digestmod="sha256").hexdigest())
print(hmac.new(b"\x0b" * 20, b"Hi There", digestmod="sha1").hexdigest())

# RFC 4231 Test Case 2 ("Jefe")
print(hmac.new(b"Jefe", b"what do ya want for nothing?", digestmod="sha256").hexdigest())

# RFC 4231 Test Case 7 (key longer than the block size)
long_key = b"\xaa" * 131
long_data = (
    b"This is a test using a larger than block-size key and a larger than block-size "
    b"data. The key needs to be hashed before being used by the HMAC algorithm."
)
print(hmac.new(long_key, long_data, digestmod="sha256").hexdigest())

# key exactly one block, and an empty message
print(hmac.new(b"k" * 64, b"msg", digestmod="sha256").hexdigest())
print(hmac.new(b"key", b"", digestmod="sha256").hexdigest())

# incremental update matches one-shot
m = hmac.new(b"key", digestmod="sha256")
m.update(b"ab")
m.update(b"cde")
print(m.hexdigest() == hmac.new(b"key", b"abcde", digestmod="sha256").hexdigest())

# digest() finalizes: a repeated call returns the same cached bytes, but
# update() afterward is no longer allowed
x = hmac.new(b"key", b"1234", digestmod="sha256")
d1 = x.hexdigest()
d2 = x.hexdigest()
print(d1 == d2 == hmac.new(b"key", b"1234", digestmod="sha256").hexdigest())
try:
    x.update(b"more")
except RuntimeError:
    print("RuntimeError")

# copy() is not supported: PSA's multipart MAC operation can't be cloned
a = hmac.new(b"key", b"foo", digestmod="sha256")
try:
    a.copy()
except NotImplementedError:
    print("NotImplementedError")

# digest() returns bytes; hexdigest() is its hex
h = hmac.new(b"key", b"abcde", digestmod="sha256")
print(hx(h.digest()) == h.hexdigest())

# one-shot module function
one_shot = hmac.digest(b"key", b"abcde", "sha256")
incremental = hmac.new(b"key", b"abcde", digestmod="sha256").digest()
print(one_shot == incremental)

# metadata
s = hmac.new(b"k", digestmod="sha256")
print(s.digest_size, s.block_size, s.name)
s = hmac.new(b"k", digestmod="sha1")
print(s.digest_size, s.block_size, s.name)

# unsupported algorithm
try:
    hmac.new(b"k", digestmod="md5")
except ValueError:
    print("ValueError")

# compare_digest
good = hmac.new(b"key", b"msg", digestmod="sha256").digest()
bad = bytearray(good)
bad[0] ^= 1
print(hmac.compare_digest(good, bytes(good)))
print(hmac.compare_digest(good, bytes(bad)))
print(hmac.compare_digest(good, good[:-1]))
print(hmac.compare_digest(memoryview(good), bytearray(good)))
