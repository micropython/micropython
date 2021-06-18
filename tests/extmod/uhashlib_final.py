try:
    import uhashlib
except ImportError:
    print("SKIP")
    raise SystemExit


for algo_name in ("md5", "sha1", "sha256"):
    algo = getattr(uhashlib, algo_name, None)
    if not algo:
        continue

    # Running .digest() several times in row is not supported.
    h = algo(b"123")
    h.digest()
    try:
        h.digest()
        print("fail")
    except ValueError:
        # Expected path, don't print anything so test output is the
        # same even if the algorithm is not implemented on the port.
        pass

    # Partial digests are not supported.
    h = algo(b"123")
    h.digest()
    try:
        h.update(b"456")
        print("fail")
    except ValueError:
        # Expected path, don't print anything so test output is the
        # same even if the algorithm is not implemented on the port.
        pass

print("done")
