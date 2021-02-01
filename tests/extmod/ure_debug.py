# test printing debugging info when compiling
try:
    import ure

    ure.DEBUG
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

ure.compile("^a|b[0-9]\w$", ure.DEBUG)
