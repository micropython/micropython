try:
    import hashlib
except ImportError:
    # This is neither MP, nor CPy, so must be MP with
    # hashlib module disabled.
    print("SKIP")
    raise SystemExit

try:
    hashlib.sha1
except AttributeError:
    # SHA1 is only available on some ports
    print("SKIP")
    raise SystemExit

sha1 = hashlib.sha1(b"hello")
sha1.update(b"world")
print(sha1.digest())
