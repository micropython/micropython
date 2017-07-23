# test printing debugging info when compiling
try:
    import ure
except ImportError:
    print("SKIP")
    raise SystemExit

ure.compile('^a|b[0-9]\w$', ure.DEBUG)
