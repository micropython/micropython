try:
    import hashlib
except ImportError:
    # MicroPython with hashlib module disabled.
    print("SKIP")
    raise SystemExit

try:
    hashlib.md5
except AttributeError:
    # MD5 is only available on some ports
    print("SKIP")
    raise SystemExit

md5 = hashlib.md5(b"hello")
md5.update(b"world")
print(md5.digest())
