try:
    import uhashlib as hashlib
except ImportError:
    try:
        import hashlib
    except ImportError:
        # This is neither uPy, nor cPy, so must be uPy with
        # uhashlib module disabled.
        print("SKIP")
        raise SystemExit

try:
    h = hashlib.sha256()
    h.digest_size
except AttributeError:
    # SHA256/Extra attributes not enabled.
    print("SKIP")
    raise SystemExit


print(h.block_size)
print(h.digest_size)

h.update(b"12343")
h2 = h.copy()

h.update(b"5")
h2.update(b"6")

print(h.digest())
print(h2.digest())
