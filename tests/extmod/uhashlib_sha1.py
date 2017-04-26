import sys
try:
    import uhashlib as hashlib
except ImportError:
    try:
        import hashlib
    except ImportError:
        # This is neither uPy, nor cPy, so must be uPy with
        # uhashlib module disabled.
        print("SKIP")
        sys.exit()

try:
    hashlib.sha1
except AttributeError:
    # SHA1 is only available on some ports
    print("SKIP")
    sys.exit()

sha1 = hashlib.sha1(b'hello')
sha1.update(b'world')
print(sha1.digest())
